#include "nurbs_curve_modifier.h"

namespace module
{
    namespace nurbs
    {
        nurbs_curve_modifier::nurbs_curve_modifier(k3d::mesh& input)
        {
            m_instance = &input;
            if (m_instance->nurbs_curve_groups == NULL)
            {
                groups = new k3d::mesh::nurbs_curve_groups_t();
            }
            else
            {
                groups = k3d::make_unique(input.nurbs_curve_groups);
            }

            curve_knots = k3d::make_unique(groups->curve_knots);
            curve_points = k3d::make_unique(groups->curve_points);
            curve_point_weights = k3d::make_unique(groups->curve_point_weights);
            curve_counts = k3d::make_unique(groups->curve_counts);
            curve_orders = k3d::make_unique(groups->curve_orders);
            curve_point_counts = k3d::make_unique(groups->curve_point_counts);
            curve_first_points = k3d::make_unique(groups->curve_first_points);
            curve_first_knots = k3d::make_unique(groups->curve_first_knots);
            first_curves = k3d::make_unique(groups->first_curves);
            curve_selection = k3d::make_unique(groups->curve_selection);
            point_selection = k3d::make_unique(input.point_selection);
            mesh_points = k3d::make_unique(input.points);
            materials = k3d::make_unique(groups->materials);
        }

        int nurbs_curve_modifier::add_curve(nurbs_curve& curve, bool shared)
        {
            try
            {
                MY_DEBUG << "ADD_CURVE" << std::endl;
                if(curve_counts->size() > 0)
                {
                    curve_counts->back()++;
                }
                else
                {
                    first_curves->push_back(0);
                    curve_counts->push_back(1);
                }

                curve_first_points->push_back(curve_points->size());
                curve_first_knots->push_back(curve_knots->size());
                curve_orders->push_back(curve.curve_knots.size() - curve.control_points.size());
                curve_point_counts->push_back(curve.control_points.size());
                curve_knots->insert(curve_knots->end(), curve.curve_knots.begin(), curve.curve_knots.end());
                curve_point_weights->insert(curve_point_weights->end(), curve.curve_point_weights.begin(), curve.curve_point_weights.end());
                curve_selection->push_back(0.0);

                k3d::mesh::indices_t points;

                for (int i = 0; i < curve.control_points.size(); i++)
                {
                    points.push_back(insert_point(curve.control_points.at(i),shared));
                }

                curve_points->insert(curve_points->end(), points.begin(), points.end());

                return count_all_curves_in_groups() - 1;
            }
            catch(...)
            {
                k3d::log() << error << nurbs_debug << "Error in addCurve" << std::endl;
                return -1;
            }
        }

        size_t nurbs_curve_modifier::insert_point(k3d::point3& point, bool shared)
        {
            try
            {
                int found = -1;

                for (int j = 0; j < mesh_points->size(); j++)
                {
                    if (point3_float_equal(mesh_points->at(j),point,0.000001))
                    {
                        found = j;
                    }
                }

                if (found < 0 || !shared)
                {
                    //we need to insert the point
                    mesh_points->push_back(point);
                    point_selection->push_back(0.0);
                    return mesh_points->size() - 1;
                }
                else return found;
            }
            catch (std::exception& e)
            {
                MY_DEBUG << "Error in InsertPoint: " << e.what() << std::endl;
                return 0;
            }
            catch (...)
            {
                MY_DEBUG << "Error in InsertPoint" << std::endl;
                return 0;
            }
        }

        bool nurbs_curve_modifier::find_point_inside(k3d::mesh::indices_t *points, size_t index)
        {
            if (points == NULL)
                return false;

            for (size_t j = 0; j < points->size(); j++)
            {
                if (points->at(j) == index)
                    return true;
            }
            return false;
        }

        void nurbs_curve_modifier::offset_all_after(k3d::mesh::indices_t *points, size_t first, int offset)
        {
            if (points == NULL)
                return;

            try
            {
                for (k3d::mesh::indices_t::iterator i = points->begin(); i != points->end(); ++i)
                {

                    if (*i > first)
                    {
                        if (offset > 0)
                        {
                            *i += offset;
                        }
                        else
                        {
                            *i -= abs(offset);
                        }
                    }
                }
            }
            catch (...)
            {
                k3d::log() << error << nurbs_debug << " Tried to access nonexisting value from 'offset_all_after'" << std::endl;
            }
        }

        void nurbs_curve_modifier::remove_unused_points()
        {
            try
            {
                k3d::mesh::point_groups_t *point_groups = NULL;
                k3d::mesh::indices_t *point_group_points = NULL;

                k3d::mesh::linear_curve_groups_t *linear_curve_groups = NULL;
                k3d::mesh::indices_t *linear_curve_points = NULL;

                k3d::mesh::cubic_curve_groups_t *cubic_curve_groups = NULL;
                k3d::mesh::indices_t *cubic_curve_points = NULL;

                k3d::mesh::bilinear_patches_t *bilinear_patches = NULL;
                k3d::mesh::indices_t *bilinear_patch_points = NULL;

                k3d::mesh::bicubic_patches_t *bicubic_patches = NULL;
                k3d::mesh::indices_t *bicubic_patch_points = NULL;

                k3d::mesh::nurbs_patches_t *nurbs_patches = NULL;
                k3d::mesh::indices_t *nurbs_patch_points = NULL;

                k3d::mesh::polyhedra_t *polyhedra = NULL;
                k3d::mesh::indices_t *polyhedra_edge_points = NULL;

                if (m_instance->point_groups != NULL)
                {
                    point_groups = k3d::make_unique(m_instance->point_groups);
                    point_group_points = k3d::make_unique(point_groups->points);
                }

                if (m_instance->linear_curve_groups != NULL)
                {
                    linear_curve_groups = k3d::make_unique(m_instance->linear_curve_groups);
                    linear_curve_points = k3d::make_unique(linear_curve_groups->curve_points);
                }

                if (m_instance->cubic_curve_groups != NULL)
                {
                    cubic_curve_groups = k3d::make_unique(m_instance->cubic_curve_groups);
                    cubic_curve_points = k3d::make_unique(cubic_curve_groups->curve_points);
                }

                if (m_instance->bilinear_patches != NULL)
                {
                    bilinear_patches = k3d::make_unique(m_instance->bilinear_patches);
                    bilinear_patch_points = k3d::make_unique(bilinear_patches->patch_points);
                }

                if (m_instance->bicubic_patches != NULL)
                {
                    bicubic_patches = k3d::make_unique(m_instance->bicubic_patches);
                    bicubic_patch_points = k3d::make_unique(bicubic_patches->patch_points);
                }

                if (m_instance->nurbs_patches != NULL)
                {
                    nurbs_patches = k3d::make_unique(m_instance->nurbs_patches);
                    nurbs_patch_points = k3d::make_unique(nurbs_patches->patch_points);
                }

                if (m_instance->polyhedra != NULL)
                {
                    polyhedra = k3d::make_unique(m_instance->polyhedra);
                    polyhedra_edge_points = k3d::make_unique(polyhedra->edge_points);
                }

                std::vector<bool> is_used(mesh_points->size(),false);

                //look for unused points
                for (size_t i = 0; i < mesh_points->size(); i++)
                {
                    is_used.at(i) = find_point_inside(point_group_points,i) || find_point_inside(linear_curve_points,i) || find_point_inside(cubic_curve_points,i) ||
                                    find_point_inside(bilinear_patch_points,i) || find_point_inside(bicubic_patch_points,i) || find_point_inside(nurbs_patch_points,i) ||
                                    find_point_inside(polyhedra_edge_points,i) || find_point_inside(curve_points,i);
                }


                for (int i = mesh_points->size() - 1; i >= 0; i--)
                {
                    if (!is_used.at(i))
                    {
                        MY_DEBUG << "Going to remove point: " << i << std::endl;
                        k3d::mesh::points_t::iterator point_loc = mesh_points->begin() + i;
                        mesh_points->erase(point_loc);
                        k3d::mesh::selection_t::iterator sel_loc = point_selection->begin() + i;
                        point_selection->erase(sel_loc);

                        offset_all_after(point_group_points, i, -1);
                        offset_all_after(linear_curve_points, i, -1);
                        offset_all_after(cubic_curve_points, i, -1);
                        offset_all_after(bilinear_patch_points, i, -1);
                        offset_all_after(bicubic_patch_points, i, -1);
                        offset_all_after(nurbs_patch_points, i, -1);
                        offset_all_after(polyhedra_edge_points, i, -1);
                        offset_all_after(curve_points, i, -1);
                    }
                }
            }
            catch (...)
            {
                k3d::log() << error << nurbs_debug << "Tried to access nonexisting value from 'remove_unused_points'" << std::endl;
            }
        }

        int nurbs_curve_modifier::selected_curve()
        {
            int my_curve=-1;

            const size_t group_begin = 0;
            const size_t group_end = group_begin + first_curves->size();
            for (size_t group = group_begin; group != group_end; ++group)
            {
                const size_t curve_begin = first_curves->at(group);
                const size_t curve_end = curve_begin + curve_counts->at(group);
                for (size_t curve = curve_begin; curve != curve_end; ++curve)
                {
                    if ( curve_selection->at(curve)>0.0)
                        if (my_curve>=0)
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

        void nurbs_curve_modifier::print_knot_vector(size_t curve)
        {
            const size_t curve_knots_begin = curve_first_knots->at(curve);
            const size_t curve_knots_end = curve_knots_begin + curve_point_counts->at(curve) + curve_orders->at(curve);

            std::stringstream str;
            str << "Knot vector:";
            for (size_t knot = curve_knots_begin; knot < curve_knots_end; knot++)
            {
                str << " " << curve_knots->at(knot);
            }
            if (MODULE_NURBS_DEBUG) k3d::log() << debug << nurbs_debug << str.str() << std::endl;
        }

        void nurbs_curve_modifier::replace_point(size_t newIndex, size_t curve, size_t point, bool continuous)
        {
            if (MODULE_NURBS_DEBUG) k3d::log() << debug << nurbs_debug << "Replace Point started on curve " << curve << " and point " << point << std::endl;
            const size_t curve_point_begin = curve_first_points->at(curve);
            const size_t curve_point_end = curve_point_begin + curve_point_counts->at(curve);

            for (size_t points = curve_point_begin; points < curve_point_end; ++points)
            {
                if (curve_points->at(points) == point)
                {
                    if (MODULE_NURBS_DEBUG) k3d::log() << debug << nurbs_debug << "Found point, replacing by" << newIndex << std::endl;
                    //we found the index pointing to point1
                    curve_points->at(points) = newIndex;
                    if (continuous)
                    {

                        const size_t curve_knots_begin = curve_first_knots->at(curve);
                        const size_t curve_knots_end = curve_knots_begin + curve_point_end - curve_point_begin + curve_orders->at(curve);

                        const size_t order = curve_orders->at(curve);
                        const size_t half_order = static_cast<unsigned int> (floor(0.5 * order));
                        const size_t pos = half_order + (points - curve_point_begin) + curve_knots_begin;
                        float knot_at_pos = curve_knots->at( points - curve_point_begin + curve_knots_begin + half_order );

                        if ( pos - curve_knots_begin < order )
                        {
                            for ( size_t x = curve_knots_begin; x < order + curve_knots_begin; ++x )
                                curve_knots->at(x) = knot_at_pos;
                            size_t knot_pos = order + curve_knots_begin;
                            while ( (curve_knots->at(knot_pos + 1) - curve_knots->at(knot_pos) > 2) && (knot_pos < curve_knots_end -1) )
                            {
                                curve_knots->at(knot_pos + 1) = curve_knots->at(knot_pos) + 2;
                                knot_pos++;
                            }
                        }
                        else if ( pos - curve_knots_begin + order < curve_knots_end )
                        {
                            for ( size_t x = curve_knots_end - 1; x > curve_knots_end - order; --x )
                                curve_knots->at(x) = knot_at_pos;
                            size_t knot_pos = curve_knots_end - order;
                            while ( (curve_knots->at(knot_pos) - curve_knots->at(knot_pos - 1) > 2) && (knot_pos < curve_knots_begin) )
                            {
                                curve_knots->at(knot_pos - 1) = curve_knots->at(knot_pos) - 2;
                                knot_pos--;
                            }
                        }
                        else
                            if (MODULE_NURBS_DEBUG) k3d::log() << debug << nurbs_debug << "Should split up the curve here" << std::endl;
                    }
                }
            }
        }

        void nurbs_curve_modifier::flip_curve(size_t curve)
        {
            if (MODULE_NURBS_DEBUG) k3d::log() << debug << nurbs_debug << "Flipping curve " << curve << std::endl;

            const size_t curve_point_begin = curve_first_points->at(curve);
            const size_t curve_point_end = curve_point_begin + curve_point_counts->at(curve);

            const size_t curve_knots_begin = curve_first_knots->at(curve);
            const size_t curve_knots_end = curve_knots_begin + (curve_point_end - curve_point_begin) + curve_orders->at(curve);

            if (MODULE_NURBS_DEBUG) k3d::log() << debug << nurbs_debug << "Get last knot within " << curve_knots_begin << " and " << curve_knots_end << " with size " << curve_knots->size() << std::endl;
            double last = curve_knots->at(curve_knots_end - 1);

            k3d::mesh::knots_t new_knots;

            //flip knot vector
            for (int knot = curve_knots_end - 1; (knot >= curve_knots_begin) && (knot >= 0); knot--)
            {
                if (MODULE_NURBS_DEBUG) k3d::log() << debug << nurbs_debug << "Flip knot vector (knot = " << knot << "), last was " << last  << std::endl;
                new_knots.push_back(last - curve_knots->at(knot));
            }

            k3d::mesh::knots_t::iterator knot_iter = curve_knots->begin() + curve_knots_begin;

            if (MODULE_NURBS_DEBUG) k3d::log() << debug << nurbs_debug << "erase old knots" << std::endl;
            for (size_t i = curve_knots_begin; i < curve_knots_end; i++)
                curve_knots->erase(knot_iter);

            if (MODULE_NURBS_DEBUG) k3d::log() << debug << nurbs_debug << "insert new ones" << std::endl;
            curve_knots->insert(knot_iter, new_knots.begin(), new_knots.end());

            print_knot_vector(curve);

            //flip point indices
            k3d::mesh::indices_t::iterator curve_points_begin_iter = curve_points->begin() + curve_point_begin;
            k3d::mesh::indices_t::iterator curve_points_end_iter = curve_points->begin() + curve_point_end;

            if (MODULE_NURBS_DEBUG) k3d::log() << debug << nurbs_debug << "reverse points" << std::endl;
            std::reverse(curve_points_begin_iter, curve_points_end_iter);

            //flip weights
            k3d::mesh::weights_t::iterator point_weights_begin_iter = curve_point_weights->begin() + curve_point_begin;
            k3d::mesh::weights_t::iterator point_weights_end_iter = curve_point_weights->begin() + curve_point_end;
            if (MODULE_NURBS_DEBUG) k3d::log() << debug << nurbs_debug << "reverse weights" << std::endl;
            std::reverse(point_weights_begin_iter, point_weights_end_iter);
            if (MODULE_NURBS_DEBUG) k3d::log() << debug << nurbs_debug << "finished flipping\n" << std::endl;
        }

        void nurbs_curve_modifier::normalize_knot_vector(size_t curve)
        {
            if (MODULE_NURBS_DEBUG) k3d::log() << debug << nurbs_debug << "Normalizing knot vector of curve" << curve << std::endl;
            print_knot_vector(curve);

            const size_t curve_point_begin = curve_first_points->at(curve);
            const size_t curve_point_end = curve_point_begin + curve_point_counts->at(curve);

            const size_t curve_knots_begin = curve_first_knots->at(curve);
            const size_t curve_knots_end = curve_knots_begin + (curve_point_end - curve_point_begin) + curve_orders->at(curve);

            double min = curve_knots->at(curve_knots_begin);
            double max = curve_knots->at(curve_knots_end-1) - min;

            for (size_t knot = curve_knots_begin; knot < curve_knots_end; knot++)
            {
                curve_knots->at(knot) = (curve_knots->at(knot) - min) / max;
            }
            print_knot_vector(curve);
        }

        int nurbs_curve_modifier::count_all_curves_in_groups()
        {
            size_t curve_count = 0;
            const size_t group_begin = 0;
            const size_t group_end = group_begin + first_curves->size();

            for (size_t group = group_begin; group < group_end; ++group)
            {
                curve_count  += curve_counts->at(group);
            }

            return curve_count;
        }

        int nurbs_curve_modifier::get_curve_group(size_t curve)
        {
            const size_t group_begin = 0;
            const size_t group_end = group_begin + first_curves->size();

            for (size_t group = group_begin; group != group_end; ++group)
            {
                if (curve >= first_curves->at(group) && curve < first_curves->at(group) + curve_counts->at(group))
                    return group;
            }

            k3d::log() << error << "Curve not found" << std::endl;
            return -1;
        }

        void nurbs_curve_modifier::remove_empty_groups()
        {

            const size_t group_begin = 0;
            const size_t group_end = group_begin + first_curves->size();

            std::vector<size_t> to_delete;

            for (size_t group = group_begin; group < group_end; ++group)
            {
                if (curve_counts->at(group) == 0)
                    to_delete.push_back(group);
            }

            for (size_t i = 0; i < to_delete.size(); i++)
            {
                k3d::mesh::indices_t::iterator first_iter = first_curves->begin() + to_delete[i];
                k3d::mesh::counts_t::iterator counts_iter = curve_counts->begin() + to_delete[i];
                k3d::mesh::materials_t::iterator mat_iter = materials->begin() + to_delete[i];

                first_curves->erase(first_iter);
                curve_counts->erase(counts_iter);
                materials->erase(mat_iter);
            }
        }

        void nurbs_curve_modifier::delete_curve(size_t curve)
        {
            const size_t curve_points_begin = curve_first_points->at(curve);
            const size_t curve_points_end = curve_points_begin + curve_point_counts->at(curve);

            const size_t curve_knots_begin = curve_first_knots->at(curve);
            const size_t curve_knots_end = curve_knots_begin + (curve_points_end - curve_points_begin) + curve_orders->at(curve);

            MY_DEBUG << "Deleting curve " << curve << " with points " << curve_points_begin << " to " << curve_points_end << " and knots " << curve_knots_begin << " to " << curve_knots_end << std::endl;

            int point_offset = curve_points_end - curve_points_begin;
            int knot_offset = curve_knots_end - curve_knots_begin;

            //offset all following points and knots
            for ( size_t curr_curve = 0; curr_curve < count_all_curves_in_groups(); curr_curve++)
            {
                if (curve_first_points->at(curr_curve) > curve_first_points->at(curve))
                    curve_first_points->at(curr_curve) -= point_offset;
                if (curve_first_knots->at(curr_curve) > curve_first_knots->at(curve))
                    curve_first_knots->at(curr_curve) -= knot_offset;
            }

            int curr_group = get_curve_group(curve);

            if (curr_group >= 0)
            {
                curve_counts->at(curr_group)--;
                MY_DEBUG << "Decreasing curve count of group " << curr_group <<  " to " << curve_counts->at(curr_group) << std::endl;
            }

            const size_t group_begin = 0;
            const size_t group_end = group_begin + first_curves->size();

            for (size_t group = group_begin; group < group_end; group++)
            {
                if (first_curves->at(group) > first_curves->at(curr_group))
                {
                    first_curves->at(group)--;
                    MY_DEBUG << "Decreasing first_curves of group " << group <<  " to " << first_curves->at(group) << std::endl;
                }
            }

            //remove points, weights, knots, point_count and order

            if (MODULE_NURBS_DEBUG) k3d::log() << debug << nurbs_debug << "Erasing knots" << std::endl;
            k3d::mesh::knots_t::iterator knot_pos = curve_knots->begin() + curve_first_knots->at(curve);
            for ( size_t i = 0; i < knot_offset; i++ )
                curve_knots->erase(knot_pos);

            if (MODULE_NURBS_DEBUG) k3d::log() << debug << nurbs_debug << "Erasing points & weights" << std::endl;
            k3d::mesh::indices_t::iterator point_pos = curve_points->begin() + curve_first_points->at(curve);
            k3d::mesh::weights_t::iterator weight_pos = curve_point_weights->begin() + curve_first_points->at(curve);
            for ( size_t i = 0; i < point_offset; i++ )
            {
                curve_points->erase(point_pos);
                curve_point_weights->erase(weight_pos);
            }

            if (MODULE_NURBS_DEBUG) k3d::log() << debug << nurbs_debug << "Erasing point counts" << std::endl;
            k3d::mesh::counts_t::iterator point_count_pos = curve_point_counts->begin() + curve;
            curve_point_counts->erase(point_count_pos);

            if (MODULE_NURBS_DEBUG) k3d::log() << debug << nurbs_debug << "Erasing orders" << std::endl;
            k3d::mesh::orders_t::iterator order_pos = curve_orders->begin() + curve;
            curve_orders->erase(order_pos);

            if (MODULE_NURBS_DEBUG) k3d::log() << debug << nurbs_debug << "Erasing first_points" << std::endl;
            k3d::mesh::indices_t::iterator first_points_pos = curve_first_points->begin() + curve;
            curve_first_points->erase(first_points_pos);

            if (MODULE_NURBS_DEBUG) k3d::log() << debug << nurbs_debug << "Erasing first knots" << std::endl;
            k3d::mesh::indices_t::iterator first_knots_pos = curve_first_knots->begin() + curve;
            curve_first_knots->erase(first_knots_pos);

            if (MODULE_NURBS_DEBUG) k3d::log() << debug << nurbs_debug << "Erasing selection" << std::endl;
            k3d::mesh::selection_t::iterator selection_pos = curve_selection->begin() + curve;
            curve_selection->erase(selection_pos);

            remove_empty_groups();
            remove_unused_points();
        }

        void nurbs_curve_modifier::join_curves(size_t point1, size_t curve1, size_t point2, size_t curve2)
        {
            if (MODULE_NURBS_DEBUG) k3d::log() << debug << nurbs_debug << "Join curves " << curve1 << " and " << curve2 << std::endl;
            //	if point is the first or last point of both curves, flip one of them
            //	.choose them in a way that they can be joined
            //	.normalize knot vectors
            //	.add 1 to all knots in the 2nd curve
            //	.put all points, weights and knots from 2nd into 1st curve
            //	.delete 2nd curve

            if (curve_orders->at(curve1) < curve_orders->at(curve2))
            {
                int dif = curve_orders->at(curve2) - curve_orders->at(curve1);
                for (int i = 0; i < dif; i++)
                    curve_degree_elevate(curve1);
            }
            else if (curve_orders->at(curve2) < curve_orders->at(curve1))
            {
                int dif = curve_orders->at(curve1) - curve_orders->at(curve2);
                for (int i = 0; i < dif; i++)
                    curve_degree_elevate(curve2);
            }


            const size_t curve_points_begin[2] = {curve_first_points->at(curve1), curve_first_points->at(curve2)};
            const size_t curve_points_end[2] = { curve_points_begin[0] + curve_point_counts->at(curve1), curve_points_begin[1] + curve_point_counts->at(curve2) };

            const size_t curve_knots_begin[2] = { curve_first_knots->at(curve1), curve_first_knots->at(curve2)};
            const size_t curve_knots_end[2] = { curve_knots_begin[0] + curve_orders->at(curve1) + curve_point_counts->at(curve1), curve_knots_begin[1] + curve_orders->at(curve2) + curve_point_counts->at(curve2)};

            //we might need to use curve 2 as curve 1 so we dont have to flip
            size_t use_curve1 = curve1;
            size_t use_curve2 = curve2;

            size_t use1 = 0;
            size_t use2 = 1;

            if ( point1 == curve_points_begin[0] && point2 == curve_points_begin[1] )
                flip_curve(curve1);
            else if ( point1 == curve_points_end[0]-1 && point2 == curve_points_end[1]-1 )
                flip_curve(curve2);
            else if ( point1 == curve_points_begin[0] && point2 == curve_points_end[1]-1 )
            {
                use_curve1 = curve2;
                use_curve2 = curve1;
                use1 = 1;
                use2 = 0;
            }
            else if ( point1 == curve_points_end[0] - 1 && point2 == curve_points_begin[1] )
            {
                //no changes
            }
            else
            {
                k3d::log() << error << "You need to select and end-point of each curve!" << std::endl;
                return;
            }


            normalize_knot_vector(curve1);
            normalize_knot_vector(curve2);

            if (MODULE_NURBS_DEBUG) k3d::log() << debug << nurbs_debug << "Copy Points" << std::endl;
            //copy points
            k3d::mesh::indices_t::iterator points_begin = curve_points->begin() + curve_points_begin[use2];
            k3d::mesh::indices_t::iterator points_end = curve_points->begin() + curve_points_end[use2];

            k3d::mesh::indices_t::iterator points_insert_at = curve_points->begin() + curve_points_end[use1];
            curve_points->insert(points_insert_at, points_begin, points_end);

            const size_t point_offset = curve_points_end[use2] - curve_points_begin[use2];

            if (MODULE_NURBS_DEBUG) k3d::log() << debug << nurbs_debug << "PointOffset " << point_offset << std::endl;

            curve_point_counts->at(use_curve1) += point_offset;

            for ( size_t curr_curve = 0; curr_curve < count_all_curves_in_groups(); curr_curve++)
            {
                if (curve_first_points->at(curr_curve) > curve_first_points->at(use_curve1))
                    curve_first_points->at(curr_curve) += point_offset;
            }

            if (MODULE_NURBS_DEBUG) k3d::log() << debug << nurbs_debug << "Copy Weights" << std::endl;
            //copy weights

            k3d::mesh::weights_t::iterator weights_begin = curve_point_weights->begin() + curve_points_begin[use2];
            k3d::mesh::weights_t::iterator weights_end = curve_point_weights->begin() + curve_points_end[use2];

            k3d::mesh::weights_t::iterator weights_insert_at = curve_point_weights->begin() + curve_points_end[use1];
            curve_point_weights->insert(weights_insert_at, weights_begin, weights_end);

            if (MODULE_NURBS_DEBUG) k3d::log() << debug << nurbs_debug << "Copy Knots" << std::endl;
            //copy knots
            k3d::mesh::knots_t::iterator knots1_begin = curve_knots->begin() + curve_knots_begin[use1];
            k3d::mesh::knots_t::iterator knots1_end = curve_knots->begin() + curve_knots_end[use1];

            if (MODULE_NURBS_DEBUG) k3d::log() << debug << nurbs_debug << "Found where to insert new knots" << std::endl;

            //only delete within the 1st curve
            size_t offset1 = std::count(knots1_begin, knots1_end, 1.0); //Get the number that need deleting

            if (MODULE_NURBS_DEBUG) k3d::log() << debug << nurbs_debug << "Found " << offset1 << " knots with value 1.0 in curve 1" << std::endl;

            curve_knots->erase(std::remove(knots1_begin,knots1_end,1.0), knots1_end);

            //if curve_knots_begin[use_curve1] < curve_knots_begin[use_curve2] do nothing, else subtract offset1
            knots1_end = curve_knots->begin() + curve_knots_end[use1] - offset1;
            k3d::mesh::knots_t::iterator knots2_begin = curve_knots->begin();
            k3d::mesh::knots_t::iterator knots2_end = curve_knots->begin();

            if (curve_knots_begin[use2] > curve_knots_begin[use1])
            {
                knots2_begin +=  curve_knots_begin[use2] - offset1;
                knots2_end +=  curve_knots_end[use2] - offset1;
                if (MODULE_NURBS_DEBUG) k3d::log() << debug << nurbs_debug << "With offset " << std::distance(knots2_begin, knots2_end) << std::endl;
            }
            else
            {
                knots2_begin +=  curve_knots_begin[use2];
                knots2_end +=  curve_knots_end[use2];
                if (MODULE_NURBS_DEBUG) k3d::log() << debug << nurbs_debug << "Without offset " << std::distance(knots2_begin, knots2_end) << std::endl;
            }

            knots1_end--;

            if (MODULE_NURBS_DEBUG) k3d::log() << debug << nurbs_debug << "Adding " << 1 << " to each knot of 2nd curve" << std::endl;

            std::stringstream str;
            str << "Knot vector:";

            for (k3d::mesh::knots_t::iterator i = knots2_begin; i < knots2_end; ++i)
            {
                *i += 1;
                str << " " << *i;
            }
            if (MODULE_NURBS_DEBUG) k3d::log() << debug << nurbs_debug << str.str() << std::endl;

            double find_first = *knots2_begin;
            size_t offset2 = std::count(knots2_begin, knots2_end, find_first);
            if (MODULE_NURBS_DEBUG) k3d::log() << debug << nurbs_debug << "Found " << offset2 << " knots with value " << find_first << " in curve 2" << std::endl;

            knots1_end = curve_knots->begin() + curve_knots_end[use1] - offset1 - 1;
            knots2_begin += offset2;

            k3d::mesh::knots_t new_knots;
            double a = *knots1_end;
            double step = (*knots2_begin - a) / (curve_orders->at(use_curve1) + 1);

            for (int i = 0; i < curve_orders->at(use_curve1) + 2; i++)
            {
                new_knots.push_back(a + step*i);
                MY_DEBUG << new_knots.back() << std::endl;
            }

            MY_DEBUG << "Built new knot vector with size " << new_knots.size() << " a=" << a << " step=" << step << std::endl;

            if (MODULE_NURBS_DEBUG) k3d::log() << debug << nurbs_debug << "Going to insert at pos " << std::distance(curve_knots->begin(), knots1_end) << " with size " << curve_knots->size() << std::endl;
            knots1_end++;
            new_knots.erase(new_knots.begin());
            curve_knots->insert(knots1_end, new_knots.begin(), new_knots.end());

            knots1_end = curve_knots->begin() + curve_knots_end[use1] - offset1 + new_knots.size();
            knots2_begin = curve_knots->begin() + curve_knots_begin[use2] + offset2 + 1;
            knots2_end = curve_knots->begin() + curve_knots_end[use2];

            if (curve_knots_begin[use2] > curve_knots_begin[use1])
            {
                knots2_begin += new_knots.size() - offset1;
                knots2_end += new_knots.size() - offset1;
            }

            if (MODULE_NURBS_DEBUG) k3d::log() << debug << nurbs_debug << "Inserting : " << std::distance(knots2_begin, knots2_end) << " knots from " << std::distance(curve_knots->begin(), knots2_begin) << " ( is " << *knots2_begin << ") to " << std::distance(curve_knots->begin(), knots2_end) << " ( is " << *knots2_end << ")" << std::endl;

            new_knots.clear();
            new_knots.insert(new_knots.end(), knots2_begin, knots2_end);

            for (k3d::mesh::knots_t::iterator i = new_knots.begin(); i != new_knots.end(); ++i)
            {
                MY_DEBUG << *i << std::endl;
            }

            curve_knots->insert(knots1_end, new_knots.begin(), new_knots.end());
            if (MODULE_NURBS_DEBUG) k3d::log() << debug << nurbs_debug << "Inserted Knots" << std::endl;

            int knot_offset = curve_knots_end[use2] - curve_knots_begin[use2] - offset1 - offset2 + curve_orders->at(use_curve1);

            if (MODULE_NURBS_DEBUG) k3d::log() << debug << nurbs_debug << "Offset for first_knots is " << knot_offset << std::endl;

            for ( size_t curr_curve = 0; curr_curve < count_all_curves_in_groups(); curr_curve++)
            {
                if (curve_first_knots->at(curr_curve) > curve_first_knots->at(use_curve1))
                    curve_first_knots->at(curr_curve) += knot_offset;
            }

            if (MODULE_NURBS_DEBUG) k3d::log() << debug << nurbs_debug << "Deleting 2nd curve.." << std::endl;
            //delete the 2nd curve
            delete_curve(use_curve2);
        }

        void nurbs_curve_modifier::close_curve(size_t curve, bool keep_ends)
        {
            if (MODULE_NURBS_DEBUG) k3d::log() << debug << nurbs_debug << "NurbsCloseCurve.." << std::endl;
            /*
                - get both end points
                if(keep_ends)
                    . add the middle of the points as new one and use the old ends as CV
                else . replace them by their arithmetical middle
            */
            const size_t curve_points_begin = curve_first_points->at(curve);
            const size_t curve_points_end = curve_points_begin + curve_point_counts->at(curve);

            const size_t curve_knots_begin = curve_first_knots->at(curve);
            const size_t curve_knots_end = curve_knots_begin + (curve_points_end - curve_points_begin) + curve_orders->at(curve);

            k3d::point3 new_point = (mesh_points->at( curve_points->at(curve_points_begin) ) + mesh_points->at( curve_points->at(curve_points_end - 1) )) * 0.5;
            mesh_points->push_back(new_point);
            point_selection->push_back(0.0);
            if (MODULE_NURBS_DEBUG) k3d::log() << debug << nurbs_debug << "New point inserted" << std::endl;

            size_t new_index = mesh_points->size() - 1;

            if (!keep_ends)
            {
                if (MODULE_NURBS_DEBUG) k3d::log() << debug << nurbs_debug << "Simple way:" << std::endl;
                replace_point(new_index, curve, curve_points->at(curve_points_begin), true);
                replace_point(new_index, curve, curve_points->at(curve_points_end - 1), true);
            }
            else
            {
                if (MODULE_NURBS_DEBUG) k3d::log() << debug << nurbs_debug << "We're going the hard way:" << std::endl;
                normalize_knot_vector(curve);
                size_t order = (*groups->curve_orders)[curve];
                //take the first knot different from 0 and the last one different from 1.0, double the
                size_t first = curve_knots_begin;

                while ( curve_knots->at(first) == 0.0 && first < curve_knots_end)
                {
                    first++;
                }
                if (MODULE_NURBS_DEBUG) k3d::log() << debug << nurbs_debug << "We need to insert 0.0 at " << first << std::endl;

                size_t last = curve_knots_end - 1;

                while ( curve_knots->at(last) == 1.0 && last >= curve_knots_begin )
                {
                    last--;
                }
                if (MODULE_NURBS_DEBUG) k3d::log() << debug << nurbs_debug << "and 1.0 at " << last << std::endl;
                if (MODULE_NURBS_DEBUG) k3d::log() << debug << nurbs_debug << "Applying offsets" << std::endl;
                //insert knot at the end
                double diff = 1.0 - curve_knots->at(last);
                if (MODULE_NURBS_DEBUG) k3d::log() << debug << nurbs_debug << "Creating iterator" << std::endl;

                if (MODULE_NURBS_DEBUG) k3d::log() << debug << nurbs_debug << "looping through knots and applying diff=" << diff << std::endl;
                for ( size_t i = last + 1; i < curve_knots_end; i++)
                    curve_knots->at(i) += diff;

                //insert knot at the beginning
                diff = curve_knots->at(first);
                if (MODULE_NURBS_DEBUG) k3d::log() << debug << nurbs_debug << "iterator" << std::endl;

                if (MODULE_NURBS_DEBUG) k3d::log() << debug << nurbs_debug << "loop with diff=" << diff << std::endl;
                for ( int i = first - 1; i >= static_cast<int>(curve_knots_begin); i--)
                {
                    k3d::log() << i << std::endl;
                    curve_knots->at(i) -= diff;
                }

                if (MODULE_NURBS_DEBUG) k3d::log() << debug << nurbs_debug << "Inserting knots 1" << std::endl;
                k3d::mesh::knots_t::iterator last_iter = curve_knots->begin() + last + 1;
                curve_knots->insert(last_iter, 1.0);
                if (MODULE_NURBS_DEBUG) k3d::log() << debug << nurbs_debug << "Inserting knots 2" << std::endl;
                k3d::mesh::knots_t::iterator first_iter = curve_knots->begin() + first;
                curve_knots->insert(first_iter, 0.0);

                if (MODULE_NURBS_DEBUG) if (MODULE_NURBS_DEBUG) k3d::log() << debug << nurbs_debug << "Inserting points" << std::endl;
                //insert point indices to the new point
                k3d::mesh::indices_t::iterator point_iter = curve_points->begin() + curve_points_begin;
                curve_points->insert(point_iter, new_index);

                point_iter = curve_points->begin() + curve_points_end + 1; //because we've already inserted one point
                curve_points->insert(point_iter, new_index);

                if (MODULE_NURBS_DEBUG) k3d::log() << debug << nurbs_debug << "Inserting weights" << std::endl;
                k3d::mesh::weights_t::iterator weights_iter = curve_point_weights->begin() + curve_points_begin;
                curve_point_weights->insert(weights_iter, 1.0);
                weights_iter = curve_point_weights->begin() + curve_points_end;
                curve_point_weights->insert(weights_iter, 1.0);

                curve_point_counts->at(curve) += 2;

                for ( size_t curr_curve = 0; curr_curve < count_all_curves_in_groups(); curr_curve++)
                {
                    if (curve_first_points->at(curve) < curve_first_points->at(curr_curve))
                        curve_first_points->at(curr_curve) += 2;
                    if (curve_first_knots->at(curve) < curve_first_knots->at(curr_curve))
                        curve_first_knots->at(curr_curve) += 2;
                }
                if (MODULE_NURBS_DEBUG) k3d::log() << debug << nurbs_debug << "Normalizing knot vector" << std::endl;
                normalize_knot_vector(curve);
            }
            remove_unused_points();
        }

        bool nurbs_curve_modifier::point3_float_equal(const k3d::point3& p1, const k3d::point3& p2, float threshold)
        {
            float point1[3], point2[3];

            point1[0] = static_cast<float>(p1[0]);
            point1[1] = static_cast<float>(p1[1]);
            point1[2] = static_cast<float>(p1[2]);

            point2[0] = static_cast<float>(p2[0]);
            point2[1] = static_cast<float>(p2[1]);
            point2[2] = static_cast<float>(p2[2]);

            if ( fabs(point1[0] - point2[0]) < threshold
                    && fabs(point1[1] - point2[1]) < threshold
                    && fabs(point1[2] - point2[2]) < threshold)
                return true;
            return false;
        }

        int nurbs_curve_modifier::find_span(size_t curve, double u)
        {
            if (MODULE_NURBS_DEBUG) k3d::log() << debug << nurbs_debug << "FindSpan of " << u << " in curve " << curve << std::endl;

            size_t order = curve_orders->at(curve);
            size_t nr_points = curve_point_counts->at(curve);

            const size_t curve_point_begin = curve_first_points->at(curve);
            const size_t curve_point_end = curve_point_begin + curve_point_counts->at(curve);

            const size_t curve_knots_begin = curve_first_knots->at(curve);
            const size_t curve_knots_end = curve_knots_begin + (curve_point_end - curve_point_begin) + curve_orders->at(curve);

            size_t n = curve_knots_end - curve_knots_begin - (order - 1);

            if ( u == curve_knots->at(curve_knots_begin + n) )
            {
                if (MODULE_NURBS_DEBUG) k3d::log() << debug << nurbs_debug << "Span is " << n - 1 << std::endl;
                return n - 1;
            }

            int low = (order - 1);
            int high = n;
            int mid = (low + high) / 2;

            while ( u < curve_knots->at( mid ) || u >= curve_knots->at( curve_knots_begin + mid + 1 ) )
            {
                if ( u < curve_knots->at( mid ) )
                    high = mid;
                else
                    low = mid;
                mid = (low + high) / 2;
            }

            if (MODULE_NURBS_DEBUG) k3d::log() << debug << nurbs_debug << "Span is " << mid << std::endl;
            return mid;
        }

        std::vector<double> nurbs_curve_modifier::basis_functions(size_t curve, double u, size_t span)
        {
            if (MODULE_NURBS_DEBUG) k3d::log() << debug << nurbs_debug << "Getting basis functions" << std::endl;

            size_t order = curve_orders->at(curve);
            size_t nr_points = curve_point_counts->at(curve);

            const size_t curve_points_begin = curve_first_points->at(curve);
            const size_t curve_points_end = curve_points_begin + curve_point_counts->at(curve);

            const size_t curve_knots_begin = curve_first_knots->at(curve);
            const size_t curve_knots_end = curve_knots_begin + (curve_points_end - curve_points_begin) + curve_orders->at(curve);

            std::vector<double> bases(order, 0.0);
            bases.at(0) = 1.0;

            std::vector<double> left(order, 0.0);
            std::vector<double> right(order, 0.0);

            for ( int j = 1; j < order; j++)
            {
                left.at(j) = u - curve_knots->at(curve_knots_begin + span + 1 - j);
                right.at(j) = curve_knots->at(curve_knots_begin + span + j) - u;

                if (MODULE_NURBS_DEBUG) k3d::log() << debug << nurbs_debug << "j = " << j << " | left = " << left.at(j) << " | right = " << right.at(j) << std::endl;

                double saved = 0.0;

                for (int r = 0; r < j; r++)
                {
                    float temp = bases.at(r) / (right.at(r+1) + left.at(j-r));
                    bases.at(r) = saved + right.at(r + 1)*temp;
                    saved = left.at(j-r)*temp;
                }
                bases.at(j) = saved;
            }
            return bases;
        }

        k3d::point4 nurbs_curve_modifier::curve_point(size_t curve, double u)
        {
            if (MODULE_NURBS_DEBUG) k3d::log() << debug << nurbs_debug << "Evaluating point on curve " << curve << " with knot " << u << std::endl;


            size_t order = curve_orders->at(curve);
            size_t nr_points = curve_point_counts->at(curve);

            const size_t curve_points_begin = curve_first_points->at(curve);
            const size_t curve_points_end = curve_points_begin + curve_point_counts->at(curve);

            const size_t curve_knots_begin = curve_first_knots->at(curve);
            const size_t curve_knots_end = curve_knots_begin + (curve_points_end - curve_points_begin) + curve_orders->at(curve);

            size_t span = find_span(curve, u);
            std::vector<double> bases = basis_functions(curve, u, span);

            k3d::point4 cv;
            for (size_t i=0; i <= order - 1; i++)
            {
                k3d::point3 p = mesh_points->at(curve_points->at(curve_points_begin + span - order + 1 + i));
                double w = curve_point_weights->at(curve_points_begin + span - order + 1 + i);
                cv = cv + bases.at(i)*k3d::point4(p[0]*w,p[1]*w,p[2]*w, w);
            }
            return cv;
        }

        /*
            Implementation of Algorithm "CurveKnotInsertion" from
            "The NURBS book", page 151, by Piegl and Tiller
        */
        void nurbs_curve_modifier::curve_knot_insertion(size_t curve, double u, size_t r)
        {
            try
            {
                //*******************************************************************
                //Set up and find multiplicity of "u"
                //*******************************************************************

                size_t order = curve_orders->at(curve);
                size_t nr_points = curve_point_counts->at(curve);

                const size_t curve_points_begin = curve_first_points->at(curve);
                const size_t curve_points_end = curve_points_begin + curve_point_counts->at(curve);

                const size_t curve_knots_begin = curve_first_knots->at(curve);
                const size_t curve_knots_end = curve_knots_begin + (curve_points_end - curve_points_begin) + curve_orders->at(curve);

                MY_DEBUG << "Curve knot insertion called: curve=" << curve << " u=" << u << " r=" << r << std::endl;
                print_knot_vector(curve);

                int k=-1;
                size_t s=0;

                //look where to insert the new knot, check multiplicity(s) of the knot at u
                int i = curve_knots_begin;

                if (MODULE_NURBS_DEBUG) k3d::log() << debug << nurbs_debug << "Starting while loop" << std::endl;


                while ( (i < (int)curve_knots_end) && curve_knots->at(i) - u < 0.000001 )
                {
                    i++;
                }

                if (MODULE_NURBS_DEBUG) k3d::log() << debug << nurbs_debug << "While loop stopped with i=" << i << std::endl;
                //we found the first knot greater than u or we're at the end so thats our k now
                k = --i - curve_knots_begin;
                if ( i < 0 )
                    i++;
                if ( i < curve_knots_end)
                {
                    //we go back to see how often we have this knot
                    if (MODULE_NURBS_DEBUG) k3d::log() << debug << nurbs_debug << "Starting while loop" << std::endl;

                    while ( (i >= (int)curve_knots_begin) && fabs(curve_knots->at(i) - u) < 0.000001 )
                    {
                        i--;
                        s++;
                    }
                    if (MODULE_NURBS_DEBUG) k3d::log() << debug << nurbs_debug << "While loop stopped with i=" << i << ", s=" << s << " and u=" << u << std::endl;
                }

                if ( s + r > order - 1)
                {
                    k3d::log() << error << "Cannot insert knot r=" << r << " times, multiplicity would be greater than degree " << order - 1 << std::endl;
                    int new_r = order - 1 - s;
                    if (new_r <= 0)
                    {
                        //we had a knot that was already inserted too often, so dont insert it again
                        new_r = 0;
                        k3d::log() << error << "We're not going to insert the knot at all" << std::endl;
                        return;
                    }
                    r = new_r;
                    if (MODULE_NURBS_DEBUG) k3d::log() << debug << nurbs_debug << "Reducing r to the maximum possible value " << r << std::endl;
                }

                //*******************************************************************
                //Algorithm from "The NURBS book" page 151
                //*******************************************************************

                size_t mp = curve_knots_end - curve_knots_begin;
                size_t nq = nr_points + r;

                if (MODULE_NURBS_DEBUG) k3d::log() << debug << nurbs_debug << "Inserting knot " << u << " with multiplicity " << r << " into curve" << std::endl;

                k3d::mesh::knots_t knots;

                for ( size_t i = 0; i <= k; i++ )
                {
                    knots.push_back(curve_knots->at(i + curve_knots_begin));
                }


                for ( size_t i = 1; i <= r; i++ )
                {
                    knots.push_back(u);
                }

                for ( size_t i = k + 1; i < mp; i++ )
                {
                    knots.push_back(curve_knots->at(i + curve_knots_begin));
                }

                k3d::point3 p = mesh_points->at( (curve_points->at( curve_points_begin )) );
                std::vector<k3d::point4> points(nq, k3d::point4(p[0],p[1],p[2], curve_point_weights->at(curve_points_begin)) );

                if (MODULE_NURBS_DEBUG) k3d::log() << debug << nurbs_debug << "Points array has size " << points.size() << std::endl;
                if (MODULE_NURBS_DEBUG) k3d::log() << debug << nurbs_debug << "Loading control points part 1" << std::endl;

                for ( size_t i = 0; i <= k + 1 - order; i++ )
                {
                    p = mesh_points->at( (curve_points->at( curve_points_begin + i )) );
                    points.at(i) = k3d::point4(p[0],p[1],p[2], curve_point_weights->at(curve_points_begin + i));
                }

                if (MODULE_NURBS_DEBUG) k3d::log() << debug << nurbs_debug << "Control points part 2" << std::endl;

                for ( size_t i = k - s; i < nr_points; i++ )
                {
                    p = mesh_points->at( (curve_points->at( curve_points_begin + i )) );
                    points.at(i + r) = k3d::point4(p[0],p[1],p[2], curve_point_weights->at(curve_points_begin + i));
                }


                std::vector<k3d::point4> tmp;

                if (MODULE_NURBS_DEBUG) k3d::log() << debug << nurbs_debug << "Filling temp array with weighted control points" << std::endl;

                for ( size_t i = 0; i<= order - 1 - s; i++ )
                {
                    p = mesh_points->at( curve_points->at( curve_points_begin + i + k - (order - 1)) );
                    const k3d::double_t w = curve_point_weights->at( curve_points_begin + i + k - (order - 1));
                    tmp.push_back( k3d::point4(w*p[0],w*p[1],w*p[2], w) );
                }
                size_t L=0;

                if (MODULE_NURBS_DEBUG) k3d::log() << debug << nurbs_debug << "Starting calculation loop" << std::endl;

                for ( size_t j = 1; j <= r; j++ )
                {
                    L = k - (order - 1) + j;
                    double alpha=0.0;
                    for ( size_t i = 0; i <= order - 1 - j - s; i++ )
                    {
                        alpha = (u - curve_knots->at(curve_knots_begin + L + i)) / (curve_knots->at(curve_knots_begin + i + k + 1) - curve_knots->at(curve_knots_begin + L + i));
                        tmp[i] = alpha * tmp.at(i + 1) + (1.0 - alpha) * tmp.at(i);
                    }
                    const k3d::point4& t1 = tmp[0];
                    double w = t1[3];
                    points[L] = k3d::point4(t1[0]/w, t1[1]/w, t1[2]/w, w);
                    const k3d::point4& t2 = tmp[order - 1 - j - s];
                    w = t2[3];
                    points[k + r - j - s] = k3d::point4(t2[0]/w, t2[1]/w, t2[2]/w, w);
                }

                for ( size_t i = L + 1; i < k - s; i++ )
                {
                    const k3d::point4& t = tmp[i - L];
                    double w = t[3];
                    points[i] = k3d::point4(t[0]/w, t[1]/w, t[2]/w, w);
                }

                //*******************************************************************
                //Insert new points and knot vector into the mesh
                //*******************************************************************

                //insert knots
                if (MODULE_NURBS_DEBUG) k3d::log() << debug << nurbs_debug << "Inserting knots" << std::endl;
                const size_t knot_offset = knots.size() - mp;
                k3d::mesh::knots_t::iterator knot = curve_knots->begin();
                knot += curve_first_knots->at(curve);
                //remove old knot vector
                for ( size_t i = 0; i < mp; i++)
                    curve_knots->erase(knot);
                //insert new one
                curve_knots->insert(knot, knots.begin(), knots.end());

                for ( size_t curr_curve = 0; curr_curve < count_all_curves_in_groups(); curr_curve++)
                {
                    if (curve_first_knots->at(curr_curve) > curve_first_knots->at(curve))
                        curve_first_knots->at(curr_curve) -= knot_offset;
                }

                if (MODULE_NURBS_DEBUG) k3d::log() << debug << nurbs_debug << "Inserting points" << std::endl;
                //new points
                k3d::mesh::indices_t new_curve_points;

                k3d::mesh::weights_t new_curve_point_weights;

                for ( size_t i = 0; i < points.size(); i++ )
                {
                    p = k3d::point3(points[i][0],points[i][1],points[i][2]);

                    k3d::mesh::points_t::iterator index = std::find(mesh_points->begin(), mesh_points->end(), p);
                    if ( index != mesh_points->end())
                    {
                        new_curve_points.push_back(distance(mesh_points->begin(),index));
                        if (MODULE_NURBS_DEBUG) k3d::log() << debug << nurbs_debug << "Point already there, adding index " << new_curve_points[i] << " to curve_points" << std::endl;
                        if (MODULE_NURBS_DEBUG) k3d::log() << debug << nurbs_debug << "Point: " << points.at(i).n[0] << " x " << points.at(i).n[1] << " x " << points.at(i).n[2] << " x " << points.at(i).n[3] << std::endl;
                    }
                    else
                    {
                        if (MODULE_NURBS_DEBUG) k3d::log() << debug << nurbs_debug << "Need to add point:" << std::endl;
                        if (MODULE_NURBS_DEBUG) k3d::log() << debug << nurbs_debug << "Point: " << points.at(i).n[0] << " x " << points.at(i).n[1] << " x " << points.at(i).n[2] << " x " << points.at(i).n[3] << std::endl;
                        mesh_points->push_back(p);
                        point_selection->push_back(0.0);
                        new_curve_points.push_back(mesh_points->size() - 1);
                    }
                    //insert curve_point_weight
                    new_curve_point_weights.push_back( points[i][3] );
                }

                k3d::mesh::weights_t::iterator point_weight = curve_point_weights->begin() + curve_points_begin;
                for ( size_t i = 0; i < curve_points_end - curve_points_begin; i++ )
                {
                    curve_point_weights->erase(point_weight);
                }

                curve_point_weights->insert(point_weight, new_curve_point_weights.begin(), new_curve_point_weights.end());


                k3d::mesh::indices_t::iterator point = curve_points->begin();
                point += curve_first_points->at(curve);
                for (size_t i = 0; i < nr_points; i++)
                    curve_points->erase(point);
                curve_points->insert(point,new_curve_points.begin(),new_curve_points.end());

                const size_t point_offset = new_curve_points.size() - (curve_points_end - curve_points_begin);

                for ( size_t curr_curve = 0; curr_curve < count_all_curves_in_groups(); curr_curve++)
                {
                    if (curve_first_points->at(curr_curve) > curve_first_points->at(curve))
                        curve_first_points->at(curr_curve) -= point_offset;
                }

                curve_point_counts->at(curve) += point_offset;

                remove_unused_points();

                if (MODULE_NURBS_DEBUG) k3d::log() << debug << nurbs_debug << "Inserted " << knot_offset << " knots and " << point_offset << " points!" << std::endl;
            }
            catch (...)
            {
                k3d::log() << error << "Tried to access nonexisting value in a std::vector" <<std::endl;
            }
        }

        int nurbs_curve_modifier::factorial(int n)
        {
            if (n == 0) return 1;
            if (n < 3) return n;
            return factorial(n-1)*n;
        }

        double nurbs_curve_modifier::binomial_coefficient(int n, int k)
        {
            if ( n-k >= 0 )
            {
                double result = factorial(n);
                result /= factorial(k) * factorial(n-k);
                MY_DEBUG << n << " over " << k << " = " << result << std::endl;
                return result;
            }
            else
            {
                MY_DEBUG << "n - k has to be greater than zero!" << std::endl;
                return 0.0;
            }
        }

        int nurbs_curve_modifier::Min(int a, int b)
        {
            if (a < b)
                return a;
            else
                return b;
        }

        int nurbs_curve_modifier::Max(int a, int b)
        {
            if (a > b)
                return a;
            else
                return b;
        }

        void nurbs_curve_modifier::fill_bezalfs(std::vector<std::vector<double> >& bezalfs, int power, int t)
        {
            try
            {
                int ph = power + t;
                int ph2 = ph/2;

                bezalfs.at(0).at(0) = 1.0;
                bezalfs.at(ph).at(power) = 1.0;

                for (int i = 1; i <= ph2; i++)
                {
                    double inv = 1.0 / binomial_coefficient(ph, i);
                    int mpi = Min(power,i);

                    for ( int j = Max(0, i-t); j <= mpi; j++)
                    {
                        bezalfs.at(i).at(j) = inv * binomial_coefficient(power, j) * binomial_coefficient(t, i-j);
                        MY_DEBUG << "bezalfs["<<i<<"]["<<j<<"] = "<< bezalfs.at(i).at(j) << std::endl;
                    }
                }

                for (int i = ph2 + 1; i <= ph - 1; i++)
                {
                    int mpi = Min(power, i);
                    for (int j = Max(0, i-t); j <= mpi; j++)
                    {
                        bezalfs.at(i).at(j) = bezalfs.at(ph-i).at(power-j);
                        MY_DEBUG << "bezalfs["<<i<<"]["<<j<<"] = "<< bezalfs.at(i).at(j) << std::endl;
                    }
                }
            }
            catch (...)
            {
                k3d::log() << error << nurbs_debug << " Error in fill bezalfs" << std::endl;
            }
        }

        k3d::point4 nurbs_curve_modifier::get_homogenous_point(size_t point)
        {
            try
            {
                k3d::point3 p = mesh_points->at( curve_points->at(point) );
                double w = curve_point_weights->at(point);
                return k3d::point4(p[0]*w, p[1]*w, p[2]*w, w);
            }
            catch (...)
            {
                k3d::log() << error << nurbs_debug << "Error while getting homogenous point " << point << std::endl;
                return k3d::point4();
            }
        }

        std::string nurbs_curve_modifier::output_point(const k3d::point4& point)
        {
            std::stringstream str;
            str << point[0] << " x " << point[1] << " x " << point[2] << " x " << point[3];
            return str.str();
        }

        std::string nurbs_curve_modifier::output_point(const k3d::point3& point)
        {
            std::stringstream str;
            str << point[0] << " x " << point[1] << " x " << point[2];
            return str.str();
        }

        k3d::mesh::indices_t nurbs_curve_modifier::create_curve_points(std::vector<k3d::point4>& points)
        {
            k3d::mesh::indices_t new_curve_points;

            for (int i = 0; i < points.size(); i++)
            {
                double w = points.at(i)[3];
                k3d::point3 p(points.at(i)[0]/w, points.at(i)[1]/w, points.at(i)[2]/w);
                int found = -1;

                for (int j = 0; j < mesh_points->size(); j++)
                {
                    if (point3_float_equal(mesh_points->at(j),p,0.000001))
                    {
                        found = j;
                    }
                }

                if (found < 0)
                {
                    //we need to insert the point
                    mesh_points->push_back(p);
                    point_selection->push_back(0.0);
                    new_curve_points.push_back(mesh_points->size() - 1);
                }
                else new_curve_points.push_back(found);
            }

            return new_curve_points;
        }

        k3d::mesh::weights_t nurbs_curve_modifier::create_curve_point_weights(std::vector<k3d::point4>& points)
        {
            k3d::mesh::weights_t new_point_weights;

            for (int i = 0; i < points.size(); i++)
            {
                new_point_weights.push_back( points.at(i)[3] );
            }

            return new_point_weights;
        }
        /*
            Implementation of Algorithm A5.9 "DegreeElevateCurve" from
            "The NURBS book", page 206, by Piegl and Tiller
        */
        int nurbs_curve_modifier::curve_degree_elevate(size_t curve)
        {
            try
            {
                MY_DEBUG << "DegreeElevation started on curve " << curve << std::endl;

                const size_t t = 1;
                const size_t order = curve_orders->at(curve);
                size_t power = order - 1;
                const size_t nr_points = curve_point_counts->at(curve);

                const size_t curve_points_begin = curve_first_points->at(curve);
                const size_t curve_points_end = curve_points_begin + curve_point_counts->at(curve);

                const size_t curve_knots_begin = curve_first_knots->at(curve);
                const size_t curve_knots_end = curve_knots_begin + (curve_points_end - curve_points_begin) + curve_orders->at(curve);

                std::vector<std::vector<double> > bezalfs(power + t + 1,std::vector<double>(power + 1,1.0));
                std::vector<k3d::point4> bpts(power + 1, k3d::point4(0.0,0.0,0.0,0.0));
                std::vector<k3d::point4> ebpts(power + t + 1, k3d::point4(0.0,0.0,0.0,0.0));
                std::vector<k3d::point4> next_bpts(power - 1,k3d::point4(0.0,0.0,0.0,0.0));
                std::vector<double> alphas(power - 1, 0.0);

                k3d::mesh::knots_t new_knots(2* (nr_points + order + t), 0.0);
                std::vector<k3d::point4> new_points(2* (nr_points + t), k3d::point4(0.0,0.0,0.0,0.0));

                MY_DEBUG << "Arrays created, filling bezalfs" << std::endl;

                fill_bezalfs(bezalfs, power, t);

                int m = nr_points + power;
                int mh = power + t;
                int kind = mh + 1;
                int r = -1;
                int a = power;
                int b = order;
                int cind = 1;
                double ua = curve_knots->at(curve_knots_begin);

                new_points.at(0) = get_homogenous_point(curve_points_begin);
                MY_DEBUG << "new_points.at(0) = " << output_point(new_points.at(0)) << std::endl;

                MY_DEBUG << "Pushing back initial value for knot vector: " << ua << std::endl;
                for (int i=0; i <= power + t; i++)
                    new_knots.push_back(ua);

                MY_DEBUG << "Initialize first bezier segment" << std::endl;
                //initialize first bezier segment
                for (int i = 0; i <= power; i++)
                {
                    bpts.at(i) = get_homogenous_point(curve_points_begin + i);
                    MY_DEBUG << "Loading bpts[" << i << "] = " << output_point(bpts.at(i)) << std::endl;
                }

                while (b < m)//big loop through knot vector
                {
                    MY_DEBUG << "Looping through knot vector.. b = " << b << std::endl;
                    int i = b;
                    MY_DEBUG << "Looking for knot " << curve_knots->at(curve_knots_begin + b) << std::endl;
                    while (b < m && fabs(curve_knots->at(curve_knots_begin + b) - curve_knots->at(curve_knots_begin + b + 1)) < 0.00001)
                        b++;
                    int mul = b - i + 1;
                    MY_DEBUG << "Found knot " << mul << " times in" << std::endl;
                    print_knot_vector(curve);
                    mh = mh + mul + t;
                    double ub = curve_knots->at(curve_knots_begin + b);
                    int oldr = r;
                    r = power - mul;
                    //insert knot ub r times
                    int lbz, rbz;
                    if (oldr > 0)
                        lbz = (oldr + 2) / 2;
                    else
                        lbz = 1;

                    if (r > 0)
                        rbz = power + t - ((r + 1) / 2);
                    else
                        rbz = power + t;

                    if (r > 0)
                    {
                        //insert knot to get bezier segment
                        double numer = ub - ua;
                        MY_DEBUG << "Inserting knot " << ub << " " << r << " times" << std::endl;
                        for (int k = power; k > mul; k--)
                        {
                            alphas.at(k-mul-1) = numer / (curve_knots->at(curve_knots_begin + a + k) - ua);
                            MY_DEBUG << "alpha[" << k-mul-1 << "] = " << alphas.at(k-mul-1) << std::endl;
                        }
                        MY_DEBUG << "Calculated Alphas" << std::endl;
                        for (int j = 1; j <= r; j++)
                        {
                            int save = r-j;
                            int s = mul + j;
                            for (int k = power; k >= s; k--)
                            {
                                bpts.at(k) = alphas.at(k-s) * bpts.at(k) + (1.0 - alphas.at(k-s)) * bpts.at(k-1);
                                MY_DEBUG << "Calculated bpts[" << k << "] = " << output_point(bpts.at(k)) << std::endl;
                            }
                            next_bpts.at(save) = bpts.at(power);
                        }
                    }//end of "insert knot"
                    else
                        MY_DEBUG << "Didnt have to insert knot " << ub << " because r = " << r << std::endl;
                    //degree elevate bezier
                    for (int i=lbz; i <= power + t; i++)
                    {
                        //only points lbz, ... , power+t are used here
                        ebpts.at(i) = k3d::point4(0.0,0.0,0.0,0.0);
                        int mpi = Min(power, i);
                        for (int j = Max(0, i-t); j <= mpi; j++)
                        {
                            ebpts.at(i) += bezalfs.at(i).at(j) * bpts.at(j);
                            MY_DEBUG << "ebpts[" << i << "] = " << output_point(ebpts.at(i)) << " = " << bezalfs.at(i).at(j) << " * " << output_point(bpts.at(j)) << std::endl;
                        }
                    }
                    MY_DEBUG << "ebpts calculation done" << std::endl;

                    if (oldr > 1)
                    {
                        //must remove knot ua oldr times
                        MY_DEBUG << "we have to remove our knot "<< oldr << "times" << std::endl;
                        int first = kind - 2;
                        int last = kind;
                        double den = ub - ua;
                        double bet = (ub - new_knots.at(kind - 1)) / den;
                        //knot removal loop
                        for (int tr = 1; tr < oldr; tr++)
                        {
                            int i = first;
                            int j = last;
                            int kj = j - kind + 1;
                            while (j - i > tr)//loop and compute the new control points for one removal step
                            {
                                if (i < cind)
                                {
                                    double alf = (ub - new_knots.at(i)) / (ua - new_knots.at(i));
                                    new_points.at(i) = alf * new_points.at(i) + (1.0 - alf) * new_points.at(i-1);
                                    MY_DEBUG << "Calculated new_point[" << i << "] = " << output_point(new_points.at(i)) << " with alf = " << alf << std::endl;
                                }
                                if (j >= lbz)
                                {
                                    if (j - tr <= kind - power - t + oldr)
                                    {
                                        double gam = (ub - new_knots.at(j - tr))/den;
                                        ebpts.at(kj) = gam * ebpts.at(kj) + (1.0 - gam)*ebpts.at(kj+1);
                                    }
                                    else
                                        ebpts.at(kj) = bet * ebpts.at(kj) + (1.0 - bet)*ebpts.at(kj+1);
                                }
                                i++;
                                j--;
                                kj--;
                            }
                            first--;
                            last++;
                        }
                    }//end of removing knot ua
                    MY_DEBUG << "Either we finished removing the knot or we didnt have to" << std::endl;
                    if (a != power)
                    {
                        MY_DEBUG << a << " != " << power << std::endl;
                        for (int i = 0; i < power + t - oldr; i++)
                        {
                            new_knots.at(kind) = ua;
                            MY_DEBUG << "new_knots[" << kind << "] = " << ua << std::endl;
                            kind++;
                        }
                    }
                    MY_DEBUG << "We're going to add the points: " << lbz << " to " << rbz << " starting at " << cind << std::endl;
                    MY_DEBUG << "Points array has size " << new_points.size() << std::endl;
                    for (int j = lbz; j <= rbz; j++)
                    {
                        new_points.at(cind) = ebpts.at(j);
                        MY_DEBUG << "new_points["<<cind<<"] = " << output_point(ebpts.at(j)) << std::endl;
                        cind++;

                    }
                    MY_DEBUG << "Done adding points" << std::endl;

                    if (b < m)
                    {
                        MY_DEBUG << b << " < " << m << std::endl;
                        for (int j = 0; j < r; j++)
                            bpts.at(j) = next_bpts.at(j);
                        for (int j = r; j <= power; j++)
                        {
                            bpts.at(j) = get_homogenous_point(curve_points_begin + b - power + j);
                            MY_DEBUG << "loading further mesh points: " << output_point(bpts.at(j)) << std::endl;
                        }
                        a = b;
                        b++;
                        ua = ub;
                    }
                    else//end knot
                    {
                        MY_DEBUG << b << " >= " << m << std::endl;
                        for (int i = 0; i <= power + t; i++)
                        {
                            new_knots.at(kind+i) = ub;
                        }
                    }
                }//end while loop (b < m)

                int new_n = mh - power - t;
                MY_DEBUG << "new number of points = " << new_n << std::endl;
                MY_DEBUG << "new_points.size() = " << new_points.size() << std::endl;
                for (int i = 0; i < new_points.size(); i++)
                    MY_DEBUG << output_point(new_points.at(i)) << std::endl;
                MY_DEBUG << "new_knots.size() = " << new_knots.size() << std::endl;
                for (int i = 0; i < new_knots.size(); i++)
                    MY_DEBUG << new_knots.at(i) << std::endl;

                MY_DEBUG << "Algorithm finished, going to insert knots and points into the curve" << std::endl;


                new_points.at(new_n - 1) = get_homogenous_point(curve_points_end - 1);
                new_points.resize(new_n);
                new_knots.resize(new_n + order + t);
                k3d::mesh::weights_t new_weights = create_curve_point_weights(new_points);
                k3d::mesh::indices_t new_curve_points = create_curve_points(new_points);

                k3d::mesh::indices_t::iterator curve_points_begin_iter = curve_points->begin() + curve_points_begin;
                k3d::mesh::indices_t::iterator curve_points_end_iter = curve_points->begin() + curve_points_end;

                curve_points->erase(curve_points_begin_iter, curve_points_end_iter);
                curve_points->insert(curve_points_begin_iter, new_curve_points.begin(), new_curve_points.end());

                k3d::mesh::knots_t::iterator curve_knots_begin_iter = curve_knots->begin() + curve_knots_begin;
                k3d::mesh::knots_t::iterator curve_knots_end_iter = curve_knots->begin() + curve_knots_end;

                curve_knots->erase(curve_knots_begin_iter, curve_knots_end_iter);
                curve_knots->insert(curve_knots_begin_iter, new_knots.begin(), new_knots.end());

                k3d::mesh::weights_t::iterator curve_point_weights_begin_iter = curve_point_weights->begin() + curve_points_begin;
                k3d::mesh::weights_t::iterator curve_point_weights_end_iter = curve_point_weights->begin() + curve_points_end;

                curve_point_weights->erase(curve_point_weights_begin_iter, curve_point_weights_end_iter);
                curve_point_weights->insert(curve_point_weights_begin_iter, new_weights.begin(), new_weights.end());

                int point_offset = new_points.size() - (curve_points_end - curve_points_begin);
                int knot_offset = new_knots.size() - (curve_knots_end - curve_knots_begin);

                for ( size_t curr_curve = 0; curr_curve < count_all_curves_in_groups(); curr_curve++)
                {
                    if (curve_first_points->at(curr_curve) > curve_first_points->at(curve))
                        curve_first_points->at(curr_curve) += point_offset;
                    if (curve_first_knots->at(curr_curve) > curve_first_knots->at(curve))
                        curve_first_knots->at(curr_curve) += knot_offset;
                }

                curve_point_counts->at(curve) += point_offset;
                curve_orders->at(curve)++;

                remove_unused_points();

                MY_DEBUG << "DegreeElevation ended, added curve!" << std::endl;
            }
            catch (...)
            {
                k3d::log() << error << nurbs_debug << "Tried to access nonexisting values" << std::endl;
            }
        }

        void nurbs_curve_modifier::split_curve_at(size_t curve, double u)
        {
            try
            {
                normalize_knot_vector(curve);
                //prepare curve for splitting
                curve_knot_insertion(curve, u, curve_orders->at(curve) - 1);

                //double the point at knot "u"
                k3d::point4 p = curve_point(curve, u);
                double w = p[3];
                k3d::point3 p3(p[0]/w, p[1]/w, p[2]/w);
                MY_DEBUG << "Point: " << p[0] << " x " << p[1] << " x " << p[2] << " x " << p[3] << std::endl;

                //search the point in the actual curve
                size_t curve_points_begin = curve_first_points->at(curve);
                size_t curve_points_end = curve_points_begin + curve_point_counts->at(curve);
                int curve_point_index = -1;

                for ( size_t point = curve_points_begin; point < curve_points_end; point++)
                {
                    if ( point3_float_equal(mesh_points->at( curve_points->at(point) ),p3, 0.000001) )
                    {
                        if ( curve_point_index < 0)
                            curve_point_index = point;
                        else
                        {
                            k3d::log() << error << "Curve contains this point several times, don't know where to split" << std::endl;
                            return;
                        }
                    }
                }

                if (curve_point_index < 0)
                {
                    k3d::log() << error << "Curve does not contain this point" << std::endl;
                    return;
                }
                MY_DEBUG  << "Found in curve at index: " << curve_point_index << std::endl;

                //double points and weights
                k3d::mesh::indices_t::iterator point_iter = curve_points->begin();
                point_iter += curve_point_index + 1;
                mesh_points->push_back(p3);
                point_selection->push_back(0.0);
                curve_points->insert(point_iter, mesh_points->size() - 1);

                k3d::mesh::weights_t::iterator weight_iter = curve_point_weights->begin();
                weight_iter += curve_point_index;
                curve_point_weights->insert(weight_iter, w);

                //insert knots
                size_t curve_knots_begin = curve_first_knots->at(curve);
                size_t curve_knots_end = curve_knots_begin + curve_point_counts->at(curve) + curve_orders->at(curve);
                int knot_index = -1;

                for ( size_t knot = curve_knots_begin; knot < curve_knots_end; knot++)
                {
                    if ( fabs(curve_knots->at(knot) - u) < 0.000001 && knot_index < 0)
                        knot_index = knot;
                }

                if (knot_index < 0)
                {
                    k3d::log() << error << "Curve does not contain knot " << u << std::endl;
                    return;
                }
                MY_DEBUG  << "First occurrence of knot " << u << " is " << knot_index << std::endl;

                k3d::mesh::knots_t::iterator knot = curve_knots->begin() + knot_index;
                curve_knots->insert(knot, curve_orders->at(curve) + 1, u);

                //offset all first_knots and first_points
                for ( size_t curr_curve = 0; curr_curve < count_all_curves_in_groups(); curr_curve++ )
                {
                    if ( curve_first_points->at(curr_curve) > curve_first_points->at(curve) )
                        curve_first_points->at(curr_curve)++;

                    if ( curve_first_knots->at(curr_curve) > curve_first_knots->at(curve) )
                        curve_first_knots->at(curr_curve) += curve_orders->at(curve) + 1;
                }

                MY_DEBUG  << "Inserting new curve" << std::endl;
                //insert new curve
                curve_point_counts->at(curve) -= curve_points_end - curve_point_index - 1;

                k3d::mesh::indices_t::iterator cfp = curve_first_points->begin() + curve + 1;
                curve_first_points->insert(cfp, curve_point_index + 1);

                k3d::mesh::indices_t::iterator cfk = curve_first_knots->begin() + curve + 1;
                curve_first_knots->insert(cfk, knot_index + curve_orders->at(curve));

                k3d::mesh::orders_t::iterator co = curve_orders->begin() + curve + 1;
                curve_orders->insert(co, curve_orders->at(curve));

                k3d::mesh::counts_t::iterator cpc = curve_point_counts->begin() + curve + 1;
                curve_point_counts->insert(cpc, curve_points_end - curve_point_index);

                k3d::mesh::selection_t::iterator cs = curve_selection->begin() + curve + 1;
                curve_selection->insert(cs, 0.0);

                MY_DEBUG  << "Increasing curve_counts" << std::endl;

                //increase curve_counts, offset first_curves
                int my_group = get_curve_group(curve);

                curve_counts->at(my_group)++;

                const size_t group_begin = 0;
                const size_t group_end = group_begin + first_curves->size();

                for (size_t group = group_begin; group < group_end; ++group)
                {
                    if (first_curves->at(group) > first_curves->at(my_group))
                        first_curves->at(group)++;
                }

                remove_unused_points();
            }
            catch (std::exception& e)
            {
                MY_DEBUG << "Error in SplitCurve: " << e.what() << std::endl;
            }
            catch (...)
            {
                MY_DEBUG << "Error in SplitCurve" << std::endl;
            }
        }

        void nurbs_curve_modifier::traverse_curve(size_t curve1, size_t curve2)
        {
            //move the 1st curve along the 2nd

            const size_t curve_points_begin[2] = {curve_first_points->at(curve1), curve_first_points->at(curve2)};
            const size_t curve_points_end[2] = { curve_points_begin[0] + curve_point_counts->at(curve1), curve_points_begin[1] + curve_point_counts->at(curve2) };

            const size_t curve_knots_begin[2] = { curve_first_knots->at(curve1), curve_first_knots->at(curve2)};
            const size_t curve_knots_end[2] = { curve_knots_begin[0] + curve_orders->at(curve1) + curve_point_counts->at(curve1), curve_knots_begin[1] + curve_orders->at(curve2) + curve_point_counts->at(curve2)};

            k3d::mesh::indices_t new_points;
            k3d::mesh::weights_t new_weights;

            k3d::mesh::knots_t u_knots;
            u_knots.insert(u_knots.end(), curve_knots->begin() + curve_knots_begin[0], curve_knots->begin() + curve_knots_end[0]);

            k3d::mesh::knots_t v_knots;
            v_knots.insert(v_knots.end(), curve_knots->begin() + curve_knots_begin[1], curve_knots->begin() + curve_knots_end[1]);

            size_t u_order = curve_orders->at(curve1);
            size_t v_order = curve_orders->at(curve2);

            const size_t point_count = curve_point_counts->at(curve1);

            for (int i = 0; i < curve_point_counts->at(curve2); i++)
            {
                k3d::point3 delta_u = mesh_points->at( curve_points->at(curve_points_begin[1] + i) ) + (-mesh_points->at( curve_points->at(curve_points_begin[1]) ));
                double w_u = curve_point_weights->at(curve_points_begin[1] + i);

                for (int j = 0; j < point_count; j++)
                {
                    k3d::point3 p_v = mesh_points->at( curve_points->at(curve_points_begin[0] + j) );
                    double w_v = curve_point_weights->at(curve_points_begin[0] + j);

                    mesh_points->push_back(p_v + delta_u);
                    point_selection->push_back(0.0);

                    new_points.push_back(mesh_points->size()-1);
                    new_weights.push_back(w_u * w_v);
                }
            }


            MY_DEBUG << "Adding patch using gprim_factory" << std::endl;
            k3d::gprim_factory fac(*m_instance);
            fac.add_nurbs_patch(u_order, v_order, new_points, u_knots, v_knots, new_weights);
        }

        void nurbs_curve_modifier::revolve_curve(size_t curve, double angle, int segments)
        {
            //revolve this curve to a certain angle

            const size_t curve_points_begin = curve_first_points->at(curve);
            const size_t curve_points_end = curve_points_begin + curve_point_counts->at(curve);

            const size_t curve_knots_begin = curve_first_knots->at(curve);
            const size_t curve_knots_end = curve_knots_begin + (curve_points_end - curve_points_begin) + curve_orders->at(curve);

            //create a circle with the given angle
            k3d::mesh::knots_t u_knots;
            std::vector<double> weights;
            std::vector<k3d::point3> control_points;
            k3d::nurbs::circular_arc(k3d::point3(1, 0, 0), k3d::point3(0, 1, 0), 0, angle, segments, u_knots, weights, control_points);

            for (int i = 0; i < control_points.size(); i++)
            {
                MY_DEBUG << output_point(control_points.at(i)) << " x " << weights.at(i) << std::endl;
            }

            //we're going to scale this arc to the size of the distance of each point of the curve
            k3d::mesh::indices_t new_points;
            k3d::mesh::weights_t new_weights;

            k3d::mesh::knots_t v_knots;
            v_knots.insert(v_knots.end(), curve_knots->begin() + curve_knots_begin, curve_knots->begin() + curve_knots_end);

            size_t v_order = curve_orders->at(curve);
            size_t u_order = 3;//the circle is of degree 2

            for (int i = 0; i < curve_point_counts->at(curve); i++)
            {
                k3d::point3 p = mesh_points->at(curve_points->at(curve_points_begin + i));
                double w = curve_point_weights->at(curve_points_begin + i);
                double distance = sqrt((p[0] * p[0]) + (p[1] * p[1])); // we want the distance to the z axis
                MY_DEBUG << "Working with point: " << output_point(p) << " and weight " << w << "Creating circle with radius: " << distance << std::endl;

                for (int j = 0; j < control_points.size(); j++)
                {
                    k3d::point3 p_u(control_points.at(j)[0] * distance, control_points.at(j)[1] * distance, p[2]);
                    mesh_points->push_back(p_u);
                    point_selection->push_back(0.0);

                    new_points.push_back(mesh_points->size()-1);
                    new_weights.push_back(w * weights.at(j));
                }
            }

            MY_DEBUG << "Adding patch using gprim_factory" << std::endl;
            k3d::gprim_factory fac(*m_instance);
            fac.add_nurbs_patch(u_order, v_order, new_points, u_knots, v_knots, new_weights);
        }

        nurbs_curve nurbs_curve_modifier::extract_curve(size_t curve)
        {
            try
            {
                nurbs_curve result;

                const size_t curve_points_begin = curve_first_points->at(curve);
                const size_t curve_points_end = curve_points_begin + curve_point_counts->at(curve);

                const size_t curve_knots_begin = curve_first_knots->at(curve);
                const size_t curve_knots_end = curve_knots_begin + (curve_points_end - curve_points_begin) + curve_orders->at(curve);

                k3d::mesh::indices_t::iterator point_end = curve_points->begin() + curve_points_end;
                for (k3d::mesh::indices_t::iterator i = curve_points->begin() + curve_points_begin; i != point_end; ++i)
                {
                    result.control_points.push_back( mesh_points->at(*i) );
                }

                k3d::mesh::weights_t::iterator weights_end = (curve_point_weights->begin() + curve_points_end);
                for (k3d::mesh::weights_t::iterator i = curve_point_weights->begin() + curve_points_begin; i != weights_end; ++i)
                {
                    result.curve_point_weights.push_back(*i);
                }

                k3d::mesh::knots_t::iterator knots_end = (curve_knots->begin() + curve_knots_end);
                for (k3d::mesh::knots_t::iterator i = curve_knots->begin() + curve_knots_begin; i != knots_end; ++i)
                {
                    result.curve_knots.push_back(*i);
                }

                return result;
            }
            catch(...)
            {
                k3d::log() << error << nurbs_debug << "Error in extractCurve" << std::endl;
            }
        }

        void nurbs_curve_modifier::knot_vector_adaption(std::vector<size_t> curves)
        {
            try
            {
                MY_DEBUG << "KnotVectorAdaption" << std::endl;

                k3d::mesh::knots_t new_knot_vector;

                for (int i = 0; i < curves.size(); i++)
                {
                    k3d::mesh::knots_t::iterator position = new_knot_vector.begin();

                    const size_t curve_points_begin = curve_first_points->at(curves.at(i));
                    const size_t curve_points_end = curve_points_begin + curve_point_counts->at(curves.at(i));

                    const size_t curve_knots_begin = curve_first_knots->at(curves.at(i));
                    const size_t curve_knots_end = curve_knots_begin + (curve_points_end - curve_points_begin) + curve_orders->at(curves.at(i));

                    MY_DEBUG << "Checking curve " << curves.at(i) << " for new knots" << std::endl;

                    for (k3d::mesh::knots_t::iterator j = curve_knots->begin() + curve_knots_begin; j != curve_knots->begin() + curve_knots_end; ++j)
                    {
                        //add all new knots to new_knot_vector
                        if (position == new_knot_vector.end())
                        {
                            MY_DEBUG << "Found new knot at the end: " << *j << std::endl;
                            new_knot_vector.push_back(*j);
                            position = new_knot_vector.end();
                        }
                        else if (*position > *j)
                        {
                            MY_DEBUG << "Found new knot: " << *j << std::endl;
                            new_knot_vector.insert(position, *j);
                        }
                        else
                        {
                            position++;
                        }
                    }
                }

                std::stringstream str;
                for (int i = 0; i < new_knot_vector.size(); i++)
                {
                    str << new_knot_vector.at(i) << " ";
                }
                MY_DEBUG << "Found shared knot vector: " << str.str() << std::endl;

                //now we need to do the same thing but do knot insertion for each knot which is missing..
                for (int i = 0; i < curves.size(); i++)
                {
                    const size_t curve_points_begin = curve_first_points->at(curves.at(i));
                    const size_t curve_points_end = curve_points_begin + curve_point_counts->at(curves.at(i));

                    const size_t curve_knots_begin = curve_first_knots->at(curves.at(i));
                    const size_t curve_knots_end = curve_knots_begin + (curve_points_end - curve_points_begin) + curve_orders->at(curves.at(i));

                    int k = 0;

                    while (k < new_knot_vector.size())
                    {
                        if ( fabs(curve_knots->at(curve_knots_begin + k) - new_knot_vector.at(k)) < 0.000001 )
                        {
                            //we dont need to insert anything
                            k++;
                        }
                        else
                        {
                            //insert the missing knot
                            curve_knot_insertion(curves.at(i), new_knot_vector.at(k), 1);
                            k=0; //start looking again as knot vector changed?
                        }

                    }
                }
            }
            catch (...)
            {
                MY_DEBUG << "Access violation in knot_vector_adaption" << std::endl;
            }
        }

        void nurbs_curve_modifier::ruled_surface(size_t curve1, size_t curve2)
        {
            try
            {
                MY_DEBUG << "Ruled Surface between " << curve1 << " and " << curve2 << std::endl;
                int order = Max(curve_orders->at(curve1), curve_orders->at(curve2));
                int diff = order - curve_orders->at(curve1);

                for (int i = 0; i < diff; i++)
                {
                    curve_degree_elevate(curve1);
                }

                diff = order - curve_orders->at(curve2);
                for (int i = 0; i < diff; i++)
                {
                    curve_degree_elevate(curve1);
                }

                std::vector<size_t> curves;
                curves.push_back(curve1);
                curves.push_back(curve2);

                normalize_knot_vector(curve1);
                normalize_knot_vector(curve2);

                knot_vector_adaption(curves);

                k3d::mesh::indices_t patch_points;
                k3d::mesh::weights_t patch_point_weights;

                k3d::mesh::knots_t u_knots;
                u_knots.push_back(0);
                u_knots.push_back(0);
                u_knots.push_back(1);
                u_knots.push_back(1);

                k3d::mesh::knots_t v_knots;

                const size_t curve_points_begin = curve_first_points->at(curve1);
                const size_t curve_points_end = curve_points_begin + curve_point_counts->at(curve1);

                const size_t curve2_points_begin = curve_first_points->at(curve2);
                const size_t curve2_points_end = curve2_points_begin + curve_point_counts->at(curve2);

                const size_t curve_knots_begin = curve_first_knots->at(curve1);
                const size_t curve_knots_end = curve_knots_begin + (curve_points_end - curve_points_begin) + curve_orders->at(curve1);

                //copy knots
                for (int i = curve_knots_begin; i < curve_knots_end; i++)
                {
                    v_knots.push_back( curve_knots->at(i) );
                }

                size_t v_order = curve_orders->at(curve1);

                for (int i = 0; i < curve_point_counts->at(curve1); i++)
                {
                    patch_points.push_back( curve_points->at(curve_points_begin + i) );
                    patch_points.push_back( curve_points->at(curve2_points_begin + i) );

                    patch_point_weights.push_back( curve_point_weights->at(curve_points_begin + i) );
                    patch_point_weights.push_back( curve_point_weights->at(curve2_points_begin + i) );
                }

                k3d::gprim_factory fac(*m_instance);
                fac.add_nurbs_patch(2, v_order, patch_points, u_knots, v_knots, patch_point_weights);
            }
            catch (std::exception& e)
            {
                MY_DEBUG << "Error in RuledSurface: " << e.what() << std::endl;
            }
            catch (...)
            {
                MY_DEBUG << "Error in RuledSurface" << std::endl;
            }
        }

        bool nurbs_curve_modifier::create_cap(size_t curve)
        {
            try
            {
                MY_DEBUG << "Create Cap" << std::endl;

                //check whether curve is a loop
                const size_t curve_points_begin = curve_first_points->at(curve);
                const size_t curve_points_end = curve_points_begin + curve_point_counts->at(curve);

                if (curve_points->at(curve_points_begin) == curve_points->at(curve_points_end-1) || point3_float_equal(mesh_points->at(curve_points->at(curve_points_begin)), mesh_points->at(curve_points->at(curve_points_end-1)), 0.000001))
                {
                    MY_DEBUG << "Found a loop" << std::endl;
                    k3d::mesh t;
                    nurbs_curve_modifier mod(t);
                    nurbs_curve tmp = extract_curve(curve);
                    mod.add_curve(tmp,false);

                    MY_DEBUG << "Curve extracted" << std::endl;

                    //split it up at the middle
                    mod.normalize_knot_vector(0);
                    mod.split_curve_at(0, 0.5);
                    mod.flip_curve(0);
                    //->ruled surface
                    mod.ruled_surface(0, 1);

                    //now extract the surface and add it to this mesh
                    MY_DEBUG << "Extracting patch" << std::endl;
                    nurbs_patch_modifier mod2(*mod.m_instance);
                    nurbs_patch p = mod2.extract_patch(mod2.get_patch_count() - 1);

                    //insert with shared points!!
                    MY_DEBUG << "Insert patch into this mesh" << std::endl;
                    nurbs_patch_modifier mod3(*m_instance);
                    mod3.insert_patch(p, true);

                    return true;
                }
                return false;
            }
            catch (std::exception& e)
            {
                MY_DEBUG << "Error in CreateCap: " << e.what() << std::endl;
                return false;
            }
            catch (...)
            {
                MY_DEBUG << "Error in CreateCap" << std::endl;
                return false;
            }
        }

        void nurbs_curve_modifier::skinned_surface(std::vector<size_t> curves)
        {
            MY_DEBUG << "Skinned Surface" << std::endl;

            if(curves.size() < 2)
            {
                k3d::log() << error << nurbs_debug << "Too few curves selected for skinning" << std::endl;
                return;
            }
            //get maximum degree
            int max_degree = -1;

            for (int i = 0; i < curves.size(); i++)
            {
                max_degree = Max(max_degree, curve_orders->at(curves.at(i)));
            }

            //degree elevate all which are below this one
            for (int i = 0; i < curves.size(); i++)
            {
                int dif = max_degree - curve_orders->at(curves.at(i));

                for(int j = 0; j < dif; j++)
                {
                    curve_degree_elevate(curves.at(i));
                }
            }

            //adapt knot vectors
            knot_vector_adaption(curves);

            //calculate center for each curve
            std::vector<k3d::vector3> centers;

            for (int i = 0; i < curves.size(); i++)
            {
                k3d::point3 c;
                const size_t curve_points_begin = curve_first_points->at(curves.at(i));
                const size_t curve_points_end = curve_points_begin + curve_point_counts->at(curves.at(i));
                for(int j = curve_points_begin; j < curve_points_end; j++)
                {
                    c = c + mesh_points->at(curve_points->at(j));
                }
                c = (1.0 / curve_point_counts->at(curves.at(i))) * c;
                centers.push_back(k3d::vector3(c[0],c[1],c[2]));
            }

            //order by distance
            std::vector<std::vector<double> > distance_matrix;
            std::vector<size_t> ordered_by_distance;

            for(int i = 0; i < curves.size(); i++)
            {
                std::vector<double> distances;
                for(int j = 0; j < curves.size(); j++)
                {
                    distances.push_back(k3d::vector3(centers.at(i) - centers.at(j)).length());
                }

                distance_matrix.push_back(distances);
            }

            for(int i = 0; i < curves.size(); i++)
            {
                int shortest = 0;
                for(int j = 0; j < curves.size(); j++)
                {
                    if(distance_matrix.at(i).at(j) < distance_matrix.at(i).at(shortest) || std::find(ordered_by_distance.begin(),ordered_by_distance.end(),shortest) != ordered_by_distance.end())
                    {
                        shortest = j;
                    }
                }
                ordered_by_distance.push_back(shortest);
            }

            //create surface
            nurbs_patch skin;
            std::vector<nurbs_curve> rips;

            for(int i = 0; i < ordered_by_distance.size(); i++)
            {
                rips.push_back(extract_curve(ordered_by_distance.at(i)));
            }

            skin.u_order = rips.front().curve_knots.size() - rips.front().control_points.size();
            skin.u_knots = rips.front().curve_knots;
            skin.v_order = 3;

            skin.v_knots.push_back(0);
            skin.v_knots.push_back(0);
            for(int i = 0; i < ordered_by_distance.size() - 1; i++)
            {
                skin.v_knots.push_back(i);
            }
            skin.v_knots.push_back(skin.v_knots.back());
            skin.v_knots.push_back(skin.v_knots.back());

            for(std::vector<nurbs_curve>::iterator i = rips.begin(); i != rips.end(); ++i)
            {
                skin.point_weights.insert(skin.point_weights.end(),(*i).curve_point_weights.begin(),(*i).curve_point_weights.end());
                skin.control_points.insert(skin.control_points.end(),(*i).control_points.begin(),(*i).control_points.end());
            }

            nurbs_patch_modifier mod(*m_instance);
            mod.insert_patch(skin,true);
        }

    }//nurbs
}//module
