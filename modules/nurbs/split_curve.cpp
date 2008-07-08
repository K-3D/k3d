// K-3D
// Copyright (c) 1995-2004, Timothy M. Shead
//
// Contact: tshead@k-3d.com
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public
// License as published by the Free Software Foundation; either
// version 2 of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Public License for more details.
//
// You should have received a copy of the GNU General Public
// License along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

/** \file
		\author Carsten Haubold (CarstenHaubold@web.de)
*/

#include <k3dsdk/document_plugin_factory.h>
#include <k3dsdk/log.h>
#include <k3dsdk/module.h>
#include <k3dsdk/node.h>
#include <k3dsdk/mesh.h>
#include <k3dsdk/mesh_source.h>
#include <k3dsdk/material_sink.h>
#include <k3dsdk/mesh_operations.h>
#include <k3dsdk/nurbs.h>
#include <k3dsdk/measurement.h>
#include <k3dsdk/selection.h>
#include <k3dsdk/data.h>
#include <k3dsdk/point3.h>
#include <k3dsdk/point4.h>
#include <k3dsdk/mesh_modifier.h>
#include <k3dsdk/mesh_selection_sink.h>
#include <k3dsdk/shared_pointer.h>

#include <iostream>
#include <vector>

#include "nurbs_curve_operations.h"

namespace module
{

	namespace nurbs
	{
		class split_curve :
			public k3d::mesh_selection_sink<k3d::mesh_modifier<k3d::node > >
		{
			typedef k3d::mesh_selection_sink<k3d::mesh_modifier<k3d::node > > base;
		public:
			split_curve(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
				base(Factory, Document),
				m_u_value(init_owner(*this) + init_name(_("u_value")) + init_label(_("u_value")) + init_description(_("Split Curve at u in ]0,1[")) + init_step_increment(0.01)+ init_units(typeid(k3d::measurement::scalar)) + init_constraint(constraint::minimum(0.0 , constraint::maximum(1.0))) + init_value(0.5) )
			{
				m_mesh_selection.changed_signal().connect(make_update_mesh_slot());
				m_u_value.changed_signal().connect(make_update_mesh_slot());
			}

			void on_create_mesh(const k3d::mesh& Input, k3d::mesh& Output) 
			{
				Output = Input;
			}

			void on_update_mesh(const k3d::mesh& Input, k3d::mesh& Output)
			{
				Output = Input;
								
				if(!k3d::validate_nurbs_curve_groups(Output))
					return;

				merge_selection(m_mesh_selection.pipeline_value(), Output);
				
				int curve = selected_curve(Output);
				
				if( curve < 0 )
				{
					k3d::log() << error << "You need to select a curve!" << std::endl;
					return;
				}
				
				double u = m_u_value.pipeline_value();
				
				if(u == 0.0 || u == 1.0)
				{
					k3d::log() << error << "Cannot split a curve at its end point" << std::endl;
					return;
				}
				
				split_curve_at(Output, curve, u);
				
				assert_warning(k3d::validate_nurbs_curve_groups(Output));
			}

			static k3d::iplugin_factory& get_factory()
			{
				static k3d::document_plugin_factory<split_curve, k3d::interface_list<k3d::imesh_source, k3d::interface_list<k3d::imesh_sink > > > factory(
				k3d::uuid(0x9f5aa916, 0x8248cdec, 0x6cfbab9d, 0xd7f5f46f),
					"NurbsSplitCurve",
					_("Breaks a NURBS curve at the selected point"),
					"NURBS",
					k3d::iplugin_factory::EXPERIMENTAL);

				return factory;
			}
		private:
			k3d_data(k3d::double_t, immutable_name, change_signal, with_undo, local_storage, with_constraint, measurement_property, with_serialization) m_u_value;
			
			void split_curve_at(k3d::mesh& Output, size_t curve, double u)
			{
				k3d::mesh::nurbs_curve_groups_t& groups = *k3d::make_unique(Output.nurbs_curve_groups);
				k3d::mesh::orders_t& curve_orders = *k3d::make_unique(groups.curve_orders);
				k3d::mesh::knots_t& curve_knots = *k3d::make_unique(groups.curve_knots);
				
				normalize_knot_vector(groups,curve_knots,curve);
				//prepare curve for splitting
				curve_knot_insertion(Output, curve, u, curve_orders[curve] - 1);
				
				
				groups = *k3d::make_unique(Output.nurbs_curve_groups);
				curve_knots = *k3d::make_unique(groups.curve_knots);
				curve_orders = *k3d::make_unique(groups.curve_orders);
							
				k3d::mesh::indices_t& curve_first_points = *k3d::make_unique(groups.curve_first_points);
				k3d::mesh::indices_t& first_curves = *k3d::make_unique(groups.first_curves);
				k3d::mesh::indices_t& curve_first_knots = *k3d::make_unique(groups.curve_first_knots);
				k3d::mesh::counts_t& curve_counts = *k3d::make_unique(groups.curve_counts);
				k3d::mesh::selection_t& curve_selection = *k3d::make_unique(groups.curve_selection);
				k3d::mesh::counts_t& curve_point_counts = *k3d::make_unique(groups.curve_point_counts);
				k3d::mesh::weights_t& curve_point_weights = *k3d::make_unique(groups.curve_point_weights);
				k3d::mesh::indices_t& curve_points = *k3d::make_unique(groups.curve_points);
				k3d::mesh::points_t& mesh_points = *k3d::make_unique(Output.points);
				k3d::mesh::selection_t& point_selection = *k3d::make_unique(Output.point_selection);
				
				//double the point at knot "u"
				k3d::point4 p = curve_point(Output, curve, u);
				double w = p[3];
				k3d::point3 p3(p[0]/w, p[1]/w, p[2]/w);
				k3d::log() << debug << "Point: " << p[0] << " x " << p[1] << " x " << p[2] << " x " << p[3] << std::endl;
				
				//search the point in the actual curve
				size_t curve_points_begin = curve_first_points[curve];
				size_t curve_points_end = curve_points_begin + curve_point_counts[curve];
				int curve_point_index = -1;
				
				for( size_t point = curve_points_begin; point < curve_points_end; point++)
				{
					if( point3_float_equal(mesh_points[ curve_points[point] ],p3, 0.000001) )
					{
						if( curve_point_index < 0)
							curve_point_index = point;
						else 
						{
							k3d::log() << error << "Curve contains this point several times, don't know where to split" << std::endl;
							return;
						}
					}
				}
				
				if(curve_point_index < 0)
				{
					k3d::log() << error << "Curve does not contain this point" << std::endl;
					return;
				}
				k3d::log() << debug << "Found in curve at index: " << curve_point_index << std::endl;
				
				//double points and weights
				k3d::mesh::indices_t::iterator point_iter = curve_points.begin();
				point_iter += curve_point_index + 1;
				mesh_points.push_back(p3);	
				point_selection.push_back(0.0);
				curve_points.insert(point_iter, mesh_points.size() - 1);
				
				k3d::mesh::weights_t::iterator weight_iter = curve_point_weights.begin();
				weight_iter += curve_point_index;
				curve_point_weights.insert(weight_iter, w);				
				
				//insert knots
				size_t curve_knots_begin = curve_first_knots[curve];
				size_t curve_knots_end = curve_knots_begin + curve_point_counts[curve] + curve_orders[curve];
				int knot_index = -1;
				
				for( size_t knot = curve_knots_begin; knot < curve_knots_end; knot++)
				{
					if( fabs(curve_knots[knot] - u) < 0.000001 && knot_index < 0)
						knot_index = knot;						
				}
				
				if(knot_index < 0)
				{
					k3d::log() << error << "Curve does not contain knot " << u << std::endl;
					return;
				}
				k3d::log() << debug << "First occurrence of knot " << u << " is " << knot_index << std::endl;
				
				k3d::mesh::knots_t::iterator knot = curve_knots.begin() + knot_index;
				curve_knots.insert(knot, curve_orders[curve] + 1, u);
				
				//offset all first_knots and first_points
				for( size_t curr_curve = 0; curr_curve < count_all_curves_in_groups(groups); curr_curve++ )
				{
					if( curve_first_points[curr_curve] > curve_first_points[curve] )
						curve_first_points[curr_curve]++;
					
					if( curve_first_knots[curr_curve] > curve_first_knots[curve] )
						curve_first_knots[curr_curve] += curve_orders[curve] + 1;
				}
				
				//insert new curve
				curve_point_counts[curve] -= curve_points_end - curve_point_index - 1;
				
				k3d::mesh::indices_t::iterator cfp = curve_first_points.begin() + curve + 1;
				curve_first_points.insert(cfp, curve_point_index + 1);
				
				k3d::mesh::indices_t::iterator cfk = curve_first_knots.begin() + curve + 1;
				curve_first_knots.insert(cfk, knot_index + curve_orders[curve]);
				
				k3d::mesh::orders_t::iterator co = curve_orders.begin() + curve + 1;
				curve_orders.insert(co, curve_orders[curve]);
				
				k3d::mesh::counts_t::iterator cpc = curve_point_counts.begin() + curve + 1;
				curve_point_counts.insert(cpc, curve_points_end - curve_point_index);
				
				k3d::mesh::selection_t::iterator cs = curve_selection.begin() + curve + 1;
				curve_selection.insert(cs, 0.0);
				
				//increase curve_counts, offset first_curves
				int my_group = get_curve_group(groups, curve);
				
				curve_counts[my_group]++;
				
				const size_t group_begin = 0;
				const size_t group_end = group_begin + (*groups.first_curves).size();
				
				for(size_t group = group_begin; group != group_end; ++group)
				{
					if(first_curves[group] > first_curves[my_group])
						first_curves[group]++;
				}
			}
		};

		//Create connect_curve factory
		k3d::iplugin_factory& split_curve_factory()
		{
			return split_curve::get_factory();
		}
		
	}//namespace nurbs
} //namespace module
