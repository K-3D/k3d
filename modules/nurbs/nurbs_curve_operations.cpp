#include "nurbs_curve_operations.h"

namespace module
{
	namespace nurbs
	{
		int selected_curve(k3d::mesh& Output)
		{
			int my_curve=-1;
		
			const size_t group_begin = 0;
			const size_t group_end = group_begin + (*Output.nurbs_curve_groups->first_curves).size();
			for(size_t group = group_begin; group != group_end; ++group)
			{
				const size_t curve_begin = (*Output.nurbs_curve_groups->first_curves)[group];
				const size_t curve_end = curve_begin + (*Output.nurbs_curve_groups->curve_counts)[group];
				for(size_t curve = curve_begin; curve != curve_end; ++curve)
				{
					if( (*Output.nurbs_curve_groups->curve_selection)[curve]>0.0)
						if(my_curve>=0)
						{
							return -1;
						}
						else
						{
							my_curve=curve;
						}
				}
			}
			return my_curve;
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
			const size_t curve_knots_end = curve_knots_begin + (curve_point_end - curve_point_begin) + (*groups.curve_orders)[curve];
			
			k3d::log() << debug << "Get last knot within " << curve_knots_begin << " and " << curve_knots_end << " with size " << knots.size() << std::endl;
			double last = knots[curve_knots_end - 1];

			k3d::mesh::knots_t new_knots;

			//flip knot vector
			for(int knot = curve_knots_end - 1; (knot >= curve_knots_begin) && (knot >= 0); knot--)
			{
				k3d::log() << debug << "Flip knot vector (knot = " << knot << "), last was " << last  << std::endl;
				new_knots.push_back(last - knots[knot]);
			}
			
			k3d::mesh::knots_t::iterator knot_iter = knots.begin() + curve_knots_begin;
			
			k3d::log() << debug << "erase old knots" << std::endl;
			for(size_t i = curve_knots_begin; i < curve_knots_end; i++)
				knots.erase(knot_iter);

			k3d::log() << debug << "insert new ones" << std::endl;
			knots.insert(knot_iter, new_knots.begin(), new_knots.end());
			
			print_knot_vector(groups, knots, curve);
			
			//flip point indices
			k3d::mesh::indices_t::iterator curve_points_begin_iter = point_indices.begin() + curve_point_begin;
			k3d::mesh::indices_t::iterator curve_points_end_iter = point_indices.begin() + curve_point_end;

			k3d::log() << debug << "reverse points" << std::endl;
			std::reverse(curve_points_begin_iter, curve_points_end_iter);

			//flip weights
			k3d::mesh::weights_t& curve_point_weights = *k3d::make_unique(groups.curve_point_weights);

			k3d::mesh::weights_t::iterator point_weights_begin_iter = curve_point_weights.begin() + curve_point_begin;
			k3d::mesh::weights_t::iterator point_weights_end_iter = curve_point_weights.begin() + curve_point_end;
			k3d::log() << debug << "reverse weights" << std::endl;
			std::reverse(point_weights_begin_iter, point_weights_end_iter);
			k3d::log() << debug << "finished flipping\n" << std::endl;
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
			
			int point_offset = curve_points_end - curve_points_begin;
			int knot_offset = curve_knots_end - curve_knots_begin;
			
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
			
			k3d::log() << debug << "Erasing knots" << std::endl;
			k3d::mesh::knots_t::iterator knot_pos = curve_knots.begin() + curve_first_knots[curve];
			for( size_t i = 0; i < knot_offset; i++ )
				curve_knots.erase(knot_pos);
			
			k3d::log() << debug << "Erasing points & weights" << std::endl;
			k3d::mesh::indices_t::iterator point_pos = curve_points.begin() + curve_first_points[curve];
			k3d::mesh::weights_t::iterator weight_pos = curve_point_weights.begin() + curve_first_points[curve];
			for( size_t i = 0; i < point_offset; i++ )
			{
				curve_points.erase(point_pos);
				curve_point_weights.erase(weight_pos);
			}
			
			k3d::log() << debug << "Erasing point counts" << std::endl;
			k3d::mesh::counts_t::iterator point_count_pos = curve_point_counts.begin() + curve;
			curve_point_counts.erase(point_count_pos);
			
			k3d::log() << debug << "Erasing orders" << std::endl;
			k3d::mesh::orders_t::iterator order_pos = curve_orders.begin() + curve;
			curve_orders.erase(order_pos);
			
			k3d::log() << debug << "Erasing first_points" << std::endl;
			k3d::mesh::indices_t::iterator first_points_pos = curve_first_points.begin() + curve;
			curve_first_points.erase(first_points_pos);
			
			k3d::log() << debug << "Erasing first knots" << std::endl;
			k3d::mesh::indices_t::iterator first_knots_pos = curve_first_knots.begin() + curve;
			curve_first_knots.erase(first_knots_pos);
			
			k3d::log() << debug << "Erasing selection" << std::endl;
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
			else if( point1 == curve_points_end[0] - 1 && point2 == curve_points_begin[1] )
			{
				//no changes
			}
			else
			{
				k3d::log() << error << "You need to select and end-point of each curve!" << std::endl;
				return;
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
			
			for( size_t curr_curve = 0; curr_curve < count_all_curves_in_groups(groups); curr_curve++)
			{
				if(curve_first_points[curr_curve] > curve_first_points[use_curve1])
					curve_first_points[curr_curve] -= point_offset;
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
			k3d::mesh::knots_t::iterator knots2_begin = knots.begin();
			k3d::mesh::knots_t::iterator knots2_end = knots.begin();
			
			if(curve_knots_begin[use2] > curve_knots_begin[use1])
			{
				knots2_begin +=  curve_knots_begin[use2] - offset1;
				knots2_end +=  curve_knots_end[use2] - offset1;
				k3d::log() << debug << "With offset " << std::distance(knots2_begin, knots2_end) << std::endl;
			}
			else
			{
				knots2_begin +=  curve_knots_begin[use2];
				knots2_end +=  curve_knots_end[use2];
				k3d::log() << debug << "Without offset " << std::distance(knots2_begin, knots2_end) << std::endl;
			}
			
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
			knots2_begin += offset2 - 1;
			
			k3d::log() << debug << "Going to insert at pos " << std::distance(knots.begin(), knots1_end) << " with size " << knots.size() << std::endl;
			knots.insert(knots1_end, 1.0);
			knots1_end++;
			knots.insert(knots1_end, 1.0 + knot_inc);
			knots1_end++;
			knots.insert(knots1_end, knots2_begin, knots2_end);
			k3d::log() << debug << "Inserted Knots" << std::endl;
			
			int knot_offset = curve_knots_end[use2] - curve_knots_begin[use2] - offset1 - offset2 + 3;
			
			k3d::log() << debug << "Offset for first_knots is " << knot_offset << std::endl;
			
			for( size_t curr_curve = 0; curr_curve < count_all_curves_in_groups(groups); curr_curve++)
			{
				if(curve_first_knots[curr_curve] > curve_first_knots[use_curve1])
					curve_first_knots[curr_curve] -= knot_offset;
			}
			
			k3d::log() << debug << "Deleting 2nd curve.." << std::endl;
			//delete the 2nd curve
			delete_curve(groups,use_curve2);
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
							
				for( size_t curr_curve = 0; curr_curve < count_all_curves_in_groups(groups); curr_curve++)
				{				
					if(curve_first_knots[curr_curve] > curve_first_knots[curve])
						curve_first_knots[curr_curve] -= knot_offset;
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
				
				for( size_t curr_curve = 0; curr_curve < count_all_curves_in_groups(groups); curr_curve++)
				{
					if(curve_first_points[curr_curve] > curve_first_points[curve])
						curve_first_points[curr_curve] -= point_offset;
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
		
	}//nurbs
}//module
