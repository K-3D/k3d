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
		void connect_at_points(k3d::mesh& Mesh, size_t curve1, size_t curve2, size_t point1, size_t point2, bool continuous);


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
				Output = Input;
			}

			void on_update_mesh(const k3d::mesh& Input, k3d::mesh& Output)
			{
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

				if( curves.size() != 2 || points.size() != 2 || curves[0] == curves[1])
				{
					k3d::log() << error << "You need to select exactly 2 points on 2 different curves!\n"<<"Selected: "<<points.size()<<" points on "<<curves.size()<<" curves" << std::endl;
				}

				connect_at_points(Output, curves[0], curves[1], points[0], points[1], make_continuous);

				assert_warning(k3d::validate_nurbs_curve_groups(Output));
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

		void print_knot_vector(k3d::mesh::nurbs_curve_groups_t& groups, k3d::mesh::knots_t& knots, size_t curve)
		{
			const size_t curve_knots_begin = (*groups.curve_first_knots)[curve];
			const size_t curve_knots_end = curve_knots_begin + (*groups.curve_point_counts)[curve] + (*groups.curve_orders)[curve];

			std::stringstream str;
			str << "Knot vector:";
			for(size_t knot = curve_knots_begin; knot < curve_knots_end; knot++)
			{
				str << " " << knots[knot];
			}
			k3d::log() << debug << str.str() << std::endl;
		}

		void replace_point(k3d::mesh::nurbs_curve_groups_t& groups, k3d::mesh::indices_t& indices, k3d::mesh::knots_t& knots, size_t newIndex, size_t curve, size_t point, bool continuous)
		{
			const size_t curve_point_begin = (*groups.curve_first_points)[curve];
			const size_t curve_point_end = curve_point_begin + (*groups.curve_point_counts)[curve];

			for(size_t points = curve_point_begin; points < curve_point_end; ++points)
			{
				if(indices[points] == point)
				{
					//we found the index pointing to point1
					indices[points] = newIndex;
					if(continuous)
					{

						const size_t curve_knots_begin = (*groups.curve_first_knots)[curve];
						const size_t curve_knots_end = curve_knots_begin + curve_point_end - curve_point_begin + (*groups.curve_orders)[curve];

						const size_t order = (*groups.curve_orders)[curve];
						const size_t half_order = static_cast<unsigned int> (floor(0.5 * order));
						const size_t pos = half_order + (points - curve_point_begin) + curve_knots_begin;
						float knot_at_pos = knots[ points - curve_point_begin + curve_knots_begin + half_order ];

						if( pos - curve_knots_begin < order )
						{
							for( size_t x = curve_knots_begin; x < order + curve_knots_begin; ++x )
								knots[x] = knot_at_pos;
							size_t knot_pos = order + curve_knots_begin;
							while( (knots[knot_pos + 1] - knots[knot_pos] > 2) && (knot_pos < curve_knots_end -1) )
							{
								knots[knot_pos + 1] = knots[knot_pos] + 2;
								knot_pos++;
							}
						}
						else if( pos - curve_knots_begin + order < curve_knots_end )
						{
							for( size_t x = curve_knots_end - 1; x > curve_knots_end - order; --x )
								knots[x] = knot_at_pos;
							size_t knot_pos = curve_knots_end - order;
							while( (knots[knot_pos] - knots[knot_pos - 1] > 2) && (knot_pos < curve_knots_begin) )
							{
								knots[knot_pos - 1] = knots[knot_pos] - 2;
								knot_pos--;
							}
						}
						else
							k3d::log() << debug << "Should split up the curve here" << std::endl;
					}
				}
			}
		}

		void flip_curve(k3d::mesh::nurbs_curve_groups_t& groups, k3d::mesh::indices_t& point_indices, k3d::mesh::knots_t& knots, size_t curve)
		{
			k3d::log() << debug << "Flipping curve " << curve << std::endl;
			
			const size_t curve_point_begin = (*groups.curve_first_points)[curve];
			const size_t curve_point_end = curve_point_begin + (*groups.curve_point_counts)[curve];

			const size_t curve_knots_begin = (*groups.curve_first_knots)[curve];
			const size_t curve_knots_end = curve_knots_begin + curve_point_end - curve_point_begin + (*groups.curve_orders)[curve];

			double last = knots[curve_knots_end];

			k3d::mesh::knots_t new_knots;

			//flip knot vector
			for(size_t knot = curve_knots_end - 1; knot >= curve_knots_begin; knot--)
			{
				new_knots.push_back(last - knots[knot]);
				last = knots[knot];
			}

			//flip point indices
			k3d::mesh::indices_t::iterator curve_points_begin_iter = point_indices.begin() + curve_point_begin;
			k3d::mesh::indices_t::iterator curve_points_end_iter = point_indices.begin() + curve_point_end;

			std::reverse(curve_points_begin_iter, curve_points_end_iter);

			//flip weights
			k3d::mesh::weights_t& curve_point_weights = *k3d::make_unique(groups.curve_point_weights);

			k3d::mesh::weights_t::iterator point_weights_begin_iter = curve_point_weights.begin() + curve_point_begin;
			k3d::mesh::weights_t::iterator point_weights_end_iter = curve_point_weights.begin() + curve_point_end;

			std::reverse(point_weights_begin_iter, point_weights_end_iter);
		}

		void normalize_knot_vector(k3d::mesh::nurbs_curve_groups_t& groups, k3d::mesh::knots_t& knots, size_t curve)
		{
			k3d::log() << debug << "Normalizing knot vector of curve" << curve << std::endl;
			print_knot_vector(groups,knots,curve);
			const size_t curve_knots_begin = (*groups.curve_first_knots)[curve];
			const size_t curve_knots_end = curve_knots_begin + (*groups.curve_point_counts)[curve] + (*groups.curve_orders)[curve];

			double max = knots[curve_knots_end-1];

			for(size_t knot = curve_knots_begin; knot < curve_knots_end; knot++)
			{
				knots[knot] /= max;
			}
			print_knot_vector(groups,knots,curve);
		}
		
		int count_all_curves_in_groups(k3d::mesh::nurbs_curve_groups_t& groups)
		{
			size_t curve_count = 0;
			const size_t group_begin = 0;
			const size_t group_end = group_begin + (*groups.first_curves).size();
			
			for(size_t group = group_begin; group != group_end; ++group)
			{
				curve_count  += (*groups.curve_counts)[group];
			}
			
			return curve_count;
		}
		
		int get_curve_group(k3d::mesh::nurbs_curve_groups_t& groups, size_t curve)
		{
			const size_t group_begin = 0;
			const size_t group_end = group_begin + (*groups.first_curves).size();
			
			for(size_t group = group_begin; group != group_end; ++group)
			{
				if(curve >= (*groups.first_curves)[group] && curve < (*groups.first_curves)[group] + (*groups.curve_counts)[group])
					return group;
			}
			
			k3d::log() << error << "Curve not found" << std::endl;
			return -1;
		}
		
		void remove_empty_groups(k3d::mesh::nurbs_curve_groups_t& groups)
		{
			k3d::mesh::indices_t& first_curves = *k3d::make_unique(groups.first_curves);
			k3d::mesh::counts_t& curve_counts = *k3d::make_unique(groups.curve_counts);
			k3d::mesh::materials_t& materials = *k3d::make_unique(groups.materials);
			
			const size_t group_begin = 0;
			const size_t group_end = group_begin + (*groups.first_curves).size();
			
			std::vector<size_t> to_delete;
			
			for(size_t group = group_begin; group != group_end; ++group)
			{
				if(curve_counts[group] == 0)
					to_delete.push_back(group);
			}
			
			for(size_t i = 0; i < to_delete.size(); i++)
			{
				k3d::mesh::indices_t::iterator first_iter = first_curves.begin() + to_delete[i];
				k3d::mesh::counts_t::iterator counts_iter = curve_counts.begin() + to_delete[i];
				k3d::mesh::materials_t::iterator mat_iter = materials.begin() + to_delete[i];
				
				first_curves.erase(first_iter);
				curve_counts.erase(counts_iter);
				materials.erase(mat_iter);
			}
		}
		
		void delete_curve(k3d::mesh::nurbs_curve_groups_t& groups, size_t curve)
		{
			k3d::mesh::knots_t& curve_knots = *k3d::make_unique(groups.curve_knots);
			k3d::mesh::indices_t& curve_points = *k3d::make_unique(groups.curve_points);
			k3d::mesh::weights_t& curve_point_weights = *k3d::make_unique(groups.curve_point_weights);
			k3d::mesh::counts_t& curve_counts = *k3d::make_unique(groups.curve_counts);
			k3d::mesh::orders_t& curve_orders = *k3d::make_unique(groups.curve_orders);
			k3d::mesh::counts_t& curve_point_counts = *k3d::make_unique(groups.curve_point_counts);
			k3d::mesh::indices_t& curve_first_points = *k3d::make_unique(groups.curve_first_points);
			k3d::mesh::indices_t& curve_first_knots = *k3d::make_unique(groups.curve_first_knots);
			k3d::mesh::indices_t& first_curves = *k3d::make_unique(groups.first_curves);
			k3d::mesh::selection_t& curve_selection = *k3d::make_unique(groups.curve_selection);
			
			const size_t curve_points_begin = (*groups.curve_first_points)[curve];
			const size_t curve_points_end = curve_points_begin + (*groups.curve_point_counts)[curve];
			
			const size_t curve_knots_begin = (*groups.curve_first_knots)[curve];
			const size_t curve_knots_end = curve_knots_begin + (*groups.curve_point_counts)[curve] + (*groups.curve_orders)[curve];
			
			k3d::log() << debug << "Deleting curve " << curve << " with points " << curve_points_begin << " to " << curve_points_end << " and knots " << curve_knots_begin << " to " << curve_knots_end << std::endl;
			
			const size_t point_offset = curve_points_end - curve_points_begin;
			const size_t knot_offset = curve_knots_end - curve_knots_begin;
			
			//offset all following points and knots
			for( size_t curr_curve = 0; curr_curve < count_all_curves_in_groups(groups); curr_curve++)
			{
				if(curve_first_points[curr_curve] > curve_first_points[curve])
					curve_first_points[curr_curve] -= point_offset;
				if(curve_first_knots[curr_curve] > curve_first_knots[curve])
					curve_first_knots[curr_curve] -= knot_offset;
			}
			
			int curr_group = get_curve_group(groups,curve);
			k3d::log() << debug << "Decreasing curve count of group " << curr_group << std::endl;
			if(curr_group >= 0)
				curve_counts[curr_group]--;
			
			const size_t group_begin = 0;
			const size_t group_end = group_begin + (*groups.first_curves).size();
			
			for(size_t group = group_begin; group < group_end; group++)
			{
				if(first_curves[group] > first_curves[curr_group])
					first_curves[group]--;
			}
			
			//remove points, weights, knots, point_count and order
			k3d::mesh::knots_t::iterator knot_pos = curve_knots.begin() + curve_first_knots[curve];
			for( size_t i = 0; i < knot_offset; i++ )
				curve_knots.erase(knot_pos);
			
			k3d::mesh::indices_t::iterator point_pos = curve_points.begin() + curve_first_points[curve];
			k3d::mesh::weights_t::iterator weight_pos = curve_point_weights.begin() + curve_first_points[curve];
			for( size_t i = 0; i < point_offset; i++ )
			{
				curve_points.erase(point_pos);
				curve_point_weights.erase(weight_pos);
			}
			
			k3d::mesh::counts_t::iterator point_count_pos = curve_point_counts.begin() + curve;
			curve_point_counts.erase(point_count_pos);
			
			k3d::mesh::orders_t::iterator order_pos = curve_orders.begin() + curve;
			curve_orders.erase(order_pos);
			
			k3d::mesh::indices_t::iterator first_points_pos = curve_first_points.begin() + curve;
			curve_first_points.erase(first_points_pos);
			
			k3d::mesh::indices_t::iterator first_knots_pos = curve_first_knots.begin() + curve;
			curve_first_knots.erase(first_knots_pos);
			
			k3d::mesh::selection_t::iterator selection_pos = curve_selection.begin() + curve;
			curve_selection.erase(selection_pos);
			
			remove_empty_groups(groups);
		} 
		

		//assumes that the 2 curves share one end point (the one with new_index)
		void join_curves(k3d::mesh::nurbs_curve_groups_t& groups, k3d::mesh::indices_t& indices, k3d::mesh::knots_t& knots, size_t point1, size_t curve1, size_t point2, size_t curve2)
		{

			k3d::log() << debug << "Join curves " << curve1 << " and " << curve2 << std::endl;
			//	if point is the first or last point of both curves, flip one of them
			//	->choose them in a way that they can be joined
			//	->normalize knot vectors
			//	->add 1 to all knots in the 2nd curve
			//	->put all points, weights and knots from 2nd into 1st curve
			//	->delete 2nd curve

			k3d::mesh::indices_t& curve_first_points = *k3d::make_unique( groups.curve_first_points);
			k3d::mesh::indices_t& curve_first_knots = *k3d::make_unique( groups.curve_first_knots);
			
			const size_t curve_points_begin[2] = {curve_first_points[curve1], curve_first_points[curve2]};
			const size_t curve_points_end[2] = { curve_points_begin[0] + (*groups.curve_point_counts)[curve1], curve_points_begin[1] + (*groups.curve_point_counts)[curve2] };

			const size_t curve_knots_begin[2] = { curve_first_knots[curve1], curve_first_knots[curve2]};
			const size_t curve_knots_end[2] = { curve_knots_begin[0] + (*groups.curve_orders)[curve1] + (*groups.curve_point_counts)[curve1], curve_knots_begin[1] + (*groups.curve_orders)[curve2] + (*groups.curve_point_counts)[curve2]};

			//we might need to use curve 2 as curve 1 so we dont have to flip
			size_t use_curve1 = curve1;
			size_t use_curve2 = curve2;
			
			size_t use1 = 0;
			size_t use2 = 1;

			if( point1 == curve_points_begin[0] && point2 == curve_points_begin[1] )
				flip_curve(groups, indices, knots, curve1);
			else if( point1 == curve_points_end[0]-1 && point2 == curve_points_end[1]-1 )
				flip_curve(groups, indices, knots, curve2);
			else if( point1 == curve_points_begin[0] && point2 == curve_points_end[1]-1 )
			{
				use_curve1 = curve2;
				use_curve2 = curve1;
				use1 = 1;
				use2 = 0;
			}

			normalize_knot_vector(groups, knots, curve1);
			normalize_knot_vector(groups, knots, curve2);

			k3d::log() << debug << "Copy Points" << std::endl;
			//copy points
			k3d::mesh::indices_t::iterator points_begin = indices.begin() + curve_points_begin[use2];
			k3d::mesh::indices_t::iterator points_end = indices.begin() + curve_points_end[use2];

			k3d::mesh::indices_t::iterator points_insert_at = indices.begin() + curve_points_end[use1];
			indices.insert(points_insert_at, points_begin, points_end);

			const size_t point_offset = curve_points_end[use2] - curve_points_begin[use2];
			
			k3d::log() << debug << "PointOffset " << point_offset << std::endl;
			
			k3d::mesh::counts_t& curve_point_counts = *k3d::make_unique(groups.curve_point_counts);
			curve_point_counts[use_curve1] += point_offset;
			
			for( size_t curr_curve = 0; curr_curve < count_all_curves_in_groups(groups); curr_curve++ )
			{
				if(curve_first_points[curr_curve] > curve_first_points[use_curve1])
					curve_first_points[curr_curve] += point_offset;
			}

			k3d::log() << debug << "Copy Weights" << std::endl;
			//copy weights
			k3d::mesh::weights_t& weights = *k3d::make_unique(groups.curve_point_weights);
			
			k3d::mesh::weights_t::iterator weights_begin = weights.begin() + curve_points_begin[use2];
			k3d::mesh::weights_t::iterator weights_end = weights.begin() + curve_points_end[use2];

			k3d::mesh::weights_t::iterator weights_insert_at = weights.begin() + curve_points_end[use1];
			weights.insert(weights_insert_at, weights_begin, weights_end);

			k3d::log() << debug << "Copy Knots" << std::endl;
			//copy knots
			k3d::mesh::knots_t::iterator knots1_begin = knots.begin() + curve_knots_begin[use1];
			k3d::mesh::knots_t::iterator knots1_end = knots.begin() + curve_knots_end[use1];
			
			k3d::log() << debug << "Found where to insert new knots" << std::endl;
			
			//only delete within the 2 curves
			size_t offset1 = std::count(knots1_begin, knots1_end, 1.0); //Get the number that need deleting
			
			k3d::log() << debug << "Found " << offset1 << " knots with value 1.0 in curve 1" << std::endl;
			
			knots.erase(std::remove(knots1_begin,knots1_end,1.0), knots1_end);
			
			//if curve_knots_begin[use_curve1] < curve_knots_begin[use_curve2] do nothing, else subtract offset1
			knots1_end = knots.begin() + curve_knots_end[use1] - offset1;
			k3d::mesh::knots_t::iterator knots2_begin = knots.begin() + curve_knots_begin[use2];
			k3d::mesh::knots_t::iterator knots2_end = knots.begin() + curve_knots_end[use2];
			
			if(curve_knots_begin[use2] > curve_knots_begin[use1])
			{
				knots2_begin -= offset1;
				knots2_end -= offset1;
				curve_first_knots[use_curve2] -= offset1;
				k3d::log() << debug << "With offset " << std::distance(knots2_begin, knots2_end) << std::endl;
			}
			else
				k3d::log() << debug << "Without offset " << std::distance(knots2_begin, knots2_end) << std::endl;
			
			knots1_end--;
			double knot_inc = (1.0 - *knots1_end);
			k3d::log() << debug << "Adding " << 1 + 2 * knot_inc << " to each knot of 2nd curve" << std::endl;
			
			std::stringstream str;
			str << "Knot vector:";
			
			for(k3d::mesh::knots_t::iterator i = knots2_begin; i < knots2_end; ++i)
			{
				*i += 1 + 2 * knot_inc;
				str << " " << *i;
			}
			k3d::log() << debug << str.str() << std::endl;
			
			double find_first = *knots2_begin;
			size_t offset2 = std::count(knots2_begin, knots2_end, find_first);
			k3d::log() << debug << "Found " << offset2 << " knots with value " << find_first << " in curve 2" << std::endl;
			
			knots1_end = knots.begin() + curve_knots_end[use1] - offset1;
			knots2_begin = knots.begin() + curve_knots_begin[use2] + offset2;
			knots2_end = knots.begin() + curve_knots_end[use2];
			
			if(curve_knots_begin[use1] < curve_knots_begin[use2])
			{
				knots2_begin -= offset1;
				knots2_end -= offset1;
			}
			
			k3d::log() << debug << "Going to insert at pos " << std::distance(knots.begin(), knots1_end) << " with size " << knots.size() << std::endl;
			knots.insert(knots1_end, 1.0);
			knots1_end++;
			knots.insert(knots1_end, 1.0 + knot_inc);
			knots1_end++;
			knots.insert(knots1_end, knots2_begin, knots2_end);
			k3d::log() << debug << "Inserted Knots" << std::endl;
			
			int knot_offset = curve_knots_end[use2] - curve_knots_begin[use2] - offset1 - offset2 + 3;
			
			k3d::log() << debug << "Offset for first_knots is " << knot_offset << std::endl;
			
			for( size_t curr_curve = 0; curr_curve < count_all_curves_in_groups(groups); curr_curve++ )
			{
				if(curve_first_knots[curr_curve] > curve_first_knots[use_curve1])
					curve_first_knots[curr_curve] += knot_offset;
			}
			
			k3d::log() << debug << "Deleting 2nd curve.." << std::endl;
			//delete the 2nd curve
			delete_curve(groups,use_curve2);
		}

		void connect_at_points(k3d::mesh& Mesh, size_t curve1, size_t curve2, size_t point1, size_t point2, bool continuous)
		{
			k3d::mesh::nurbs_curve_groups_t& groups = *k3d::make_unique(Mesh.nurbs_curve_groups);
			k3d::mesh::indices_t& curve_points = *k3d::make_unique(groups.curve_points);
			k3d::mesh::knots_t& curve_knots = *k3d::make_unique(groups.curve_knots);
	
			k3d::mesh::points_t& mesh_points = *k3d::make_unique(Mesh.points);
			k3d::mesh::selection_t& point_selection = *k3d::make_unique(Mesh.point_selection);

			//now join the 2 curves
			join_curves(groups, curve_points, curve_knots, point1, curve1, point2, curve2);
		}

	}//namespace nurbs
} //namespace module
