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

namespace module
{

	namespace nurbs
	{
		void curve_knot_insertion(k3d::mesh& input, size_t curve, double u, size_t r);
		extern void normalize_knot_vector(k3d::mesh::nurbs_curve_groups_t& groups, k3d::mesh::knots_t& knots, size_t curve);
		extern int get_curve_group(k3d::mesh::nurbs_curve_groups_t& groups, size_t curve);
		extern int count_all_curves_in_groups(k3d::mesh::nurbs_curve_groups_t& groups);
		extern int selected_curve(k3d::mesh& Output);
		k3d::point4 curve_point(k3d::mesh& input, size_t curve, double u);
		bool point3_float_equal(const k3d::point3& p1, const k3d::point3& p2, float threshold=0.000001);
		
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
					if( point3_float_equal(mesh_points[ curve_points[point] ],p3) )
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
		
		
		bool point3_float_equal(const k3d::point3& p1, const k3d::point3& p2, float threshold)
		{
			float point1[3], point2[3];
			
			point1[0] = static_cast<float>(p1[0]);
			point1[1] = static_cast<float>(p1[1]);
			point1[2] = static_cast<float>(p1[2]);
			
			point2[0] = static_cast<float>(p2[0]);
			point2[1] = static_cast<float>(p2[1]);
			point2[2] = static_cast<float>(p2[2]);
			
			if( fabs(point1[0] - point2[0]) < threshold
				&& fabs(point1[1] - point2[1]) < threshold
				&& fabs(point1[2] - point2[2]) < threshold)
				return true;
			return false;
		}
		
		int find_span(k3d::mesh& input, size_t curve, double u)
		{
			k3d::log() << debug << "FindSpan of " << u << " in curve " << curve << std::endl;
			
			k3d::mesh::nurbs_curve_groups_t& groups = *k3d::make_unique(input.nurbs_curve_groups);
			k3d::mesh::knots_t& curve_knots = *k3d::make_unique(groups.curve_knots);
			
			size_t order = (*groups.curve_orders)[curve];
			size_t nr_points = (*groups.curve_point_counts)[curve];
			
			size_t curve_knots_begin = (*groups.curve_first_knots)[curve];
			size_t curve_knots_end = curve_knots_begin + nr_points + order;
			
			size_t n = curve_knots_end - curve_knots_begin - (order - 1);
			
			if( u == curve_knots.at(curve_knots_begin + n) )
			{
				k3d::log() << debug << "Span is " << n - 1 << std::endl;
				return n - 1;
			}
			
			int low = (order - 1);
			int high = n;
			int mid = (low + high) / 2;
			
			while( u < curve_knots.at( mid ) || u >= curve_knots.at( curve_knots_begin + mid + 1 ) )
			{
				if( u < curve_knots.at( mid ) )
					high = mid;
				else
					low = mid;
				mid = (low + high) / 2;
			}
			
			k3d::log() << debug << "Span is " << mid << std::endl;
			return mid;
		}
		
		std::vector<double> basis_functions(k3d::mesh& input, size_t curve, double u, size_t span)
		{
			k3d::log() << debug << "Getting basis functions" << std::endl;
			k3d::mesh::nurbs_curve_groups_t& groups = *k3d::make_unique(input.nurbs_curve_groups);
			k3d::mesh::knots_t& curve_knots = *k3d::make_unique(groups.curve_knots);
			
			size_t order = (*groups.curve_orders)[curve];
			size_t nr_points = (*groups.curve_point_counts)[curve];
			
			size_t curve_knots_begin = (*groups.curve_first_knots)[curve];
			size_t curve_knots_end = curve_knots_begin + nr_points + order;
			
			std::vector<double> bases(order, 0.0);
			bases.at(0) = 1.0;
			
			std::vector<double> left(order, 0.0);
			std::vector<double> right(order, 0.0);
			
			for( int j = 1; j < order; j++)
			{
				left.at(j) = u - curve_knots.at(curve_knots_begin + span + 1 - j);
				right.at(j) = curve_knots.at(curve_knots_begin + span + j) - u;
				
				k3d::log() << debug << "j = " << j << " | left = " << left.at(j) << " | right = " << right.at(j) << std::endl; 
				
				double saved = 0.0;
				
				for(int r = 0; r < j; r++)
				{
					float temp = bases.at(r) / (right.at(r+1) + left.at(j-r));
					bases.at(r) = saved + right.at(r + 1)*temp;
					saved = left.at(j-r)*temp;
				}
				bases.at(j) = saved;
			}
			return bases;
		}
		
		k3d::point4 curve_point(k3d::mesh& input, size_t curve, double u)
		{
			k3d::log() << debug << "Evaluating point on curve " << curve << " with knot " << u << std::endl;
			k3d::mesh::nurbs_curve_groups_t& groups = *k3d::make_unique(input.nurbs_curve_groups);
			k3d::mesh::knots_t& curve_knots = *k3d::make_unique(groups.curve_knots);
			k3d::mesh::indices_t& curve_points = *k3d::make_unique(groups.curve_points);
			k3d::mesh::weights_t& curve_point_weights = *k3d::make_unique(groups.curve_point_weights);
			
			size_t order = (*groups.curve_orders)[curve];
			size_t nr_points = (*groups.curve_point_counts)[curve];
			
			size_t curve_points_begin = (*groups.curve_first_points)[curve];
			size_t curve_points_end = curve_points_begin + nr_points;
			
			size_t curve_knots_begin = (*groups.curve_first_knots)[curve];
			size_t curve_knots_end = curve_knots_begin + nr_points + order;
			
			size_t span = find_span(input, curve, u);
			std::vector<double> bases = basis_functions(input, curve, u, span);
			
			k3d::point4 cv;
			for(size_t i=0; i <= order - 1; i++)
			{
				k3d::point3 p = input.points->at(curve_points.at(curve_points_begin + span - order + 1 + i));
				double w = curve_point_weights.at(curve_points_begin + span - order + 1 + i);
				cv = cv + bases.at(i)*k3d::point4(p[0]*w,p[1]*w,p[2]*w, w);
			}
			return cv;
		}
		
		void curve_knot_insertion(k3d::mesh& input, size_t curve, double u, size_t r)
		{
			try{
				//*******************************************************************
				//Set up and find multiplicity of "u"
				//*******************************************************************
			
				k3d::mesh::nurbs_curve_groups_t& groups = *k3d::make_unique(input.nurbs_curve_groups);
				k3d::mesh::knots_t& curve_knots = *k3d::make_unique(groups.curve_knots);
				k3d::mesh::points_t& mesh_points = *k3d::make_unique(input.points);
				k3d::mesh::selection_t& point_selection = *k3d::make_unique(input.point_selection);
				k3d::mesh::indices_t& curve_points = *k3d::make_unique(groups.curve_points);
				k3d::mesh::weights_t& curve_point_weights = *k3d::make_unique(groups.curve_point_weights);				
			
				size_t order = (*groups.curve_orders)[curve];
				size_t nr_points = (*groups.curve_point_counts)[curve];
			
				size_t curve_points_begin = (*groups.curve_first_points)[curve];
				size_t curve_points_end = curve_points_begin + nr_points;
			
				size_t curve_knots_begin = (*groups.curve_first_knots)[curve];
				size_t curve_knots_end = curve_knots_begin + nr_points + order;
			
				int k=-1;
				size_t s=0;
			
				//look where to insert the new knot, check multiplicity(s) of the knot at u
				int i = curve_knots_begin;
			
				k3d::log() << debug << "Starting while loop" << std::endl;
				
				
				while( (i < (int)curve_knots_end) && (curve_knots[i] - u) < 0.000001 )
				{
					i++;
				}
			
				k3d::log() << debug << "While loop stopped with i=" << i << std::endl;			
				//we found the first knot greater than u or we're at the end so thats our k now
				k = --i;
				if( i < 0 )
					i++;
				if ( i < curve_knots_end)
				{
					//we go back to see how often we have this knot
					k3d::log() << debug << "Starting while loop" << std::endl;
					
					while( (i >= (int)curve_knots_begin) && fabs(curve_knots[i] - u) < 0.000001 )
					{
						i--;
						s++;
					}
					k3d::log() << debug << "While loop stopped with i=" << i << ", s=" << s << " and u=" << u << std::endl;
				}
			
				if( s + r > order - 1)
				{
					k3d::log() << error << "Cannot insert knot r=" << r << " times, multiplicity would be greater than degree " << order - 1 << std::endl;
					int new_r = order - 1 - s;
					if(new_r <= 0)
					{
						//we had a knot that was already inserted too often, so dont insert it again
						new_r = 0;
						k3d::log() << error << "We're not going to insert the knot at all" << std::endl;
						return;
					}
					r = new_r;
					k3d::log() << debug << "Reducing r to the maximum possible value " << r << std::endl;			
				}
			
				//*******************************************************************
				//Algorithm from "The NURBS book" page 151
				//*******************************************************************
			
				size_t mp = curve_knots_end - curve_knots_begin;
				size_t nq = nr_points + r;
				
				k3d::log() << debug << "Inserting knot " << u << " with multiplicity " << r << " into curve" << std::endl;
 				
				k3d::mesh::knots_t knots;
			
				for( size_t i = 0; i <= k; i++ )
				{
					knots.push_back(curve_knots.at(i + curve_knots_begin));
				}
				
			
				for( size_t i = 1; i <= r; i++ )
				{
					knots.push_back(u);
				}
			
				for( size_t i = k + 1; i < mp; i++ )
				{
					knots.push_back(curve_knots.at(i + curve_knots_begin));
				}
				
				k3d::point3 p = mesh_points.at( (curve_points.at( curve_points_begin )) ); 
				std::vector<k3d::point4> points(nq, k3d::point4(p[0],p[1],p[2], curve_point_weights.at(curve_points_begin)) );
				
				k3d::log() << debug << "Points array has size " << points.size() << std::endl;
				k3d::log() << debug << "Loading control points part 1" << std::endl;
				
				for( size_t i = 0; i <= k + 1 - order; i++ )
				{
					p = mesh_points.at( (curve_points.at( curve_points_begin + i )) );
					points.at(i) = k3d::point4(p[0],p[1],p[2], curve_point_weights.at(curve_points_begin + i));
				}
				
				k3d::log() << debug << "Control points part 2" << std::endl;
				
				for( size_t i = k - s; i < nr_points; i++ )
				{
					p = mesh_points.at( (curve_points.at( curve_points_begin + i )) ); 
					points.at(i + r) = k3d::point4(p[0],p[1],p[2], curve_point_weights.at(curve_points_begin + i));
				}
				
				
				std::vector<k3d::point4> tmp;
				
				k3d::log() << debug << "Filling temp array with weighted control points" << std::endl;
				
				for( size_t i = 0; i<= order - 1 - s; i++ )
				{
					p = mesh_points.at( curve_points.at( curve_points_begin + i + k - (order - 1)) );
					const k3d::double_t w = curve_point_weights.at( curve_points_begin + i + k - (order - 1)); 
					tmp.push_back( k3d::point4(w*p[0],w*p[1],w*p[2], w) );
				}
				size_t L=0;
				
				k3d::log() << debug << "Starting calculation loop" << std::endl;
				
				for( size_t j = 1; j <= r; j++ )
				{
					L = k - (order - 1) + j;
					double alpha=0.0;
					for( size_t i = 0; i <= order - 1 - j - s; i++ )
					{
						alpha = (u - curve_knots.at(curve_knots_begin + L + i)) / (curve_knots.at(curve_knots_begin + i + k + 1) - curve_knots.at(curve_knots_begin + L + i));
						tmp[i] = alpha * tmp.at(i + 1) + (1.0 - alpha) * tmp.at(i);
					}
					const k3d::point4& t1 = tmp[0];
					double w = t1[3];
					points[L] = k3d::point4(t1[0]/w, t1[1]/w, t1[2]/w, w);
					const k3d::point4& t2 = tmp[order - 1 - j - s];
					w = t2[3];
					points[k + r - j - s] = k3d::point4(t2[0]/w, t2[1]/w, t2[2]/w, w);
				}
			
				for( size_t i = L + 1; i < k - s; i++ )
				{
					const k3d::point4& t = tmp[i - L];
					double w = t[3];
					points[i] = k3d::point4(t[0]/w, t[1]/w, t[2]/w, w);
				}
				
				//*******************************************************************
				//Insert new points and knot vector into the mesh
				//*******************************************************************
			
				//insert knots
				k3d::log() << debug << "Inserting knots" << std::endl;
				const size_t knot_offset = knots.size() - mp;
				k3d::mesh::indices_t& curve_first_knots = *k3d::make_unique( groups.curve_first_knots );
				k3d::mesh::knots_t::iterator knot = curve_knots.begin();
				knot += curve_first_knots[curve];
				//remove old knot vector
				for( size_t i = 0; i < mp; i++)
					curve_knots.erase(knot);
				//insert new one
				curve_knots.insert(knot, knots.begin(), knots.end());
							
				for( size_t curr_curve = 0; curr_curve < count_all_curves_in_groups(groups); curr_curve++ )
				{
					if(curve_first_knots[curr_curve] > curve_first_knots[curve])
						curve_first_knots[curr_curve] += knot_offset;
				}
			
				k3d::log() << debug << "Inserting points" << std::endl;
				//new points
				k3d::mesh::indices_t new_curve_points;
				
				k3d::mesh::weights_t new_curve_point_weights;
				
				for( size_t i = 0; i < points.size(); i++ )
				{
					p = k3d::point3(points[i][0],points[i][1],points[i][2]);
					
					k3d::mesh::points_t::iterator index = find(mesh_points.begin(), mesh_points.end(), p);
					if( index != mesh_points.end())
					{
						new_curve_points.push_back(distance(mesh_points.begin(),index));
						k3d::log() << debug << "Point already there, adding index " << new_curve_points[i] << " to curve_points" << std::endl;
						k3d::log() << debug << "Point: " << points.at(i).n[0] << " x " << points.at(i).n[1] << " x " << points.at(i).n[2] << " x " << points.at(i).n[3] << std::endl;
					}
					else
					{
						k3d::log() << debug << "Need to add point:" << std::endl;
						k3d::log() << debug << "Point: " << points.at(i).n[0] << " x " << points.at(i).n[1] << " x " << points.at(i).n[2] << " x " << points.at(i).n[3] << std::endl;
						mesh_points.push_back(p);
						point_selection.push_back(0.0);
						new_curve_points.push_back(mesh_points.size() - 1);
					}
					//insert curve_point_weight
					new_curve_point_weights.push_back( points[i][3] );
				}
				
				k3d::mesh::weights_t::iterator point_weight = curve_point_weights.begin() + curve_points_begin;
				for( size_t i = 0; i < curve_points_end - curve_points_begin; i++ )
				{
					curve_point_weights.erase(point_weight);
				}
				
				curve_point_weights.insert(point_weight, new_curve_point_weights.begin(), new_curve_point_weights.end());

				k3d::mesh::indices_t& curve_first_points = *k3d::make_unique( groups.curve_first_points );
				
				k3d::mesh::indices_t::iterator point = curve_points.begin();
				point += curve_first_points[curve];
				for(size_t i = 0; i < nr_points; i++)
					curve_points.erase(point);
				curve_points.insert(point,new_curve_points.begin(),new_curve_points.end());

				const size_t point_offset = new_curve_points.size() - (curve_points_end - curve_points_begin);
			
				for( size_t curr_curve = 0; curr_curve < count_all_curves_in_groups(groups); curr_curve++ )
				{
					if(curve_first_points[curr_curve] > curve_first_points[curve])
						curve_first_points[curr_curve] += point_offset;
				}
				
				k3d::mesh::counts_t& curve_point_counts = *k3d::make_unique(groups.curve_point_counts);
				curve_point_counts[curve] += point_offset;
				
				k3d::log() << debug << "Inserted " << knot_offset << " knots and " << point_offset << " points!" << std::endl;
			}
			catch(...)
			{
				k3d::log() << error << "Tried to access nonexisting value in a std::vector" <<std::endl;
			}
		}
		
	}//namespace nurbs
} //namespace module
