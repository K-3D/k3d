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
#include <k3dsdk/mesh_modifier.h>
#include <k3dsdk/mesh_selection_sink.h>
#include <k3dsdk/shared_pointer.h>

#include <iostream>
#include <vector>

namespace module
{

	namespace nurbs
	{
		void connectAtPoints(k3d::mesh& Mesh, size_t curve1, size_t curve2, size_t point1, size_t point2, bool continuous);


		class connect_curves :
			public k3d::mesh_selection_sink<k3d::mesh_modifier<k3d::node > >
		{
			typedef k3d::mesh_selection_sink<k3d::mesh_modifier<k3d::node > > base;
		public:
			connect_curves(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
				base(Factory, Document),
				m_make_continuous(init_owner(*this) + init_name("make_continuous") + init_label(_("make_continuous")) + init_description(_("Connect Continuous? Resets the Knot-Vector!")) + init_value(false) )
			{
				m_mesh_selection.changed_signal().connect(make_update_mesh_slot());
				m_make_continuous.changed_signal().connect(make_update_mesh_slot());
			}

			void on_create_mesh(const k3d::mesh& Input, k3d::mesh& Output) 
			{
				/*const k3d::iproperty_collection::properties_t properties = k3d::user_properties(*static_cast<k3d::iproperty_collection*>(this));

				//bool *endPointUsed = new bool[properties.size()]; //needs shared_ptr
				boost::shared_ptr<vector<unsigned char> > endPointUsed(new vector<unsigned_char>(properties.size());
				
				//list of these: std::vector<double> distanceList;
				
				for(k3d::iproperty_collection::properties_t::const_iterator prop = properties.begin(); prop != properties.end(); ++prop)
				{
					k3d::iproperty& property = **prop;
					if(property.property_type() == typeid(k3d::mesh*))
					{
						const k3d::mesh* const mesh = boost::any_cast<k3d::mesh*>(k3d::property::pipeline_value(property));
						return_if_fail(mesh);
						
						//grade nochmal die selben schleifen anlegen
						
						//Pseudocode here:
						//if(properties.size()>=2)
						//	lege für jeden end-kontrollpunkt eine liste mit entfernungen zu andren kontrollpunkten an
						//	
						//	für jeden knoten die verbindung mit der kleinsten Entfernung (sortiert nach entfernungen das ganze durchführen!) so anlegen, dass der übergang stetig diffbar wird
						//	(2 ctrl points von jeder seite transformieren)
						
					}
				}*/

				Output = Input;
				k3d::bool_t make_continuous = m_make_continuous.internal_value();
				
				if(!k3d::validate_nurbs_curve_groups(Output))
					return;

				merge_selection(m_mesh_selection.pipeline_value(), Output);

				std::vector<size_t> curves;
				std::vector<size_t> points;
				
				const size_t group_begin = 0;
				const size_t group_end = group_begin + (*Output.nurbs_curve_groups->first_curves).size();
				for(size_t group = group_begin; group != group_end; ++group)
				{
					const size_t curve_begin = (*Output.nurbs_curve_groups->first_curves)[group];
					const size_t curve_end = curve_begin + (*Output.nurbs_curve_groups->curve_counts)[group];
					for(size_t curve = curve_begin; curve != curve_end; ++curve)
					{
						const size_t curve_point_begin = (*Output.nurbs_curve_groups->curve_first_points)[curve];
						const size_t curve_point_end = curve_point_begin + (*Output.nurbs_curve_groups->curve_point_counts)[curve];

						const k3d::mesh::weights_t& orig_weights = *Output.nurbs_curve_groups->curve_point_weights;
						
						boost::shared_ptr<k3d::mesh::weights_t> curve_point_weights ( new k3d::mesh::weights_t() );
						
						for(size_t curve_point = curve_point_begin; curve_point != curve_point_end; ++curve_point)
						{
							if((*Output.point_selection)[(*Output.nurbs_curve_groups->curve_points)[curve_point]])
							{
								curves.push_back(curve);
								points.push_back(curve_point);
							}
						}
					}
				}
				
				if( curves.size() != 2 || points.size() != 2)
				{
					k3d::log() << error << "You need to select exactly 2 points!\n"<<"Selected: "<<points.size()<<" points on "<<curves.size()<<" curves" << std::endl;
				}

				connectAtPoints(Output, curves[0], curves[1], points[0], points[1], make_continuous);

				assert_warning(k3d::validate_nurbs_curve_groups(Output));
			}

			void on_update_mesh(const k3d::mesh& Input, k3d::mesh& Output)
			{
				on_create_mesh(Input,Output);
			}

			static k3d::iplugin_factory& get_factory()
			{
				static k3d::document_plugin_factory<connect_curves, k3d::interface_list<k3d::imesh_source, k3d::interface_list<k3d::imesh_sink > > > factory(
				k3d::uuid(0x959eb84b, 0x544d0672, 0xd53d899b, 0x6a568e86),
					"NurbsConnectCurves",
					_("Connects a set of NURBS curves"),
					"NURBS",
					k3d::iplugin_factory::EXPERIMENTAL);

				return factory;
			}
			
		private:
			k3d_data(k3d::bool_t, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_make_continuous;
		};

		//Create connect_curve factory
		k3d::iplugin_factory& connect_curves_factory()
		{
			return connect_curves::get_factory();
		}
		
		void connectAtPoints(k3d::mesh& Mesh, size_t curve1, size_t curve2, size_t point1, size_t point2, bool continuous)
		{
						
			//Add the new point
			k3d::mesh::points_t& mesh_points = *k3d::make_unique(Mesh.points);
			k3d::mesh::selection_t& point_selection = *k3d::make_unique(Mesh.point_selection);

			//merge the 2 points into one
			k3d::point3 p1 = mesh_points[point1];
			k3d::point3 p2 = mesh_points[point2];
			
			k3d::point3 new_point = (p1 + p2); //middle of the 2 points
			new_point*=0.5;
			
			size_t newIndex = mesh_points.size();
			mesh_points.push_back(new_point);
			point_selection.push_back(0.0);
			
			//ToDo: remove the points if no longer used
			
			//loop through curve point indices to find the given ones and change them so they point to "new"
			k3d::mesh::nurbs_curve_groups_t& groups = *k3d::make_unique(Mesh.nurbs_curve_groups);
			
			k3d::mesh::indices_t& indices = *k3d::make_unique(groups.curve_points);
			k3d::mesh::knots_t& knots = *k3d::make_unique(groups.curve_knots);
			
			//curve1 - point 1
			const size_t curve_point_begin = (*groups.curve_first_points)[curve1];
			const size_t curve_point_end = curve_point_begin + (*groups.curve_point_counts)[curve1];
						
			for(size_t points = curve_point_begin; points < curve_point_end; ++points)
			{
				if(indices[points] == point1)
				{
					//we found the index pointing to point1
					indices[points] = newIndex;
					if(continuous)
					{
						const size_t curve_knots_begin = (*groups.curve_first_knots)[curve1];
						const size_t curve_knots_end = curve_knots_begin + curve_point_end - curve_point_begin + (*groups.curve_orders)[curve1];

						std::stringstream knot_stream;
				
						for(size_t i = curve_knots_begin; i < curve_knots_end; ++i)
						{
							knot_stream << knots[i] << " ";
						}
						k3d::log() << debug << "Knot vector before: " << knot_stream.str() << std::endl;
						size_t knots_to_insert = (*groups.curve_orders)[curve1];
						const size_t order = knots_to_insert;
						const size_t half_order = static_cast<unsigned int> (floor(0.5 * knots_to_insert));
						const size_t pos = half_order + (points - curve_point_begin) + curve_point_begin;
						float x=0.0;
						k3d::log() << debug << "Pos: " << pos << " Order: " << order << " KnotsBegin " << curve_knots_begin << " KnotsEnd " << curve_knots_end << " KnotVector: ";
						for(size_t i = curve_knots_begin; i < curve_knots_end; ++i)
						{
							if( abs(pos - i) < half_order || i < order || curve_knots_end - i < order)// || knots_to_insert >= curve_point_end - curve_point_begin - i || (pos <= 2 * order && i <= order) )
							{
								knots_to_insert --;
								knots[i] = x;
							}
							else knots[i] = x++;
							k3d::log() << x;
						}
						k3d::log() << std::endl;
					}
				}
			}
			
			
			//curve1 - point2
			const size_t curve_point_begin2 = (*groups.curve_first_points)[curve2];
			const size_t curve_point_end2 = curve_point_begin2 + (*groups.curve_point_counts)[curve2];
						
			for(size_t points = curve_point_begin2; points < curve_point_end2; ++points)
			{
				if(indices[points] == point2)
				{
					//we found the index pointing to point2
					indices[points] = newIndex;
				}
			}
		}

	}//namespace nurbs
} //namespace module
