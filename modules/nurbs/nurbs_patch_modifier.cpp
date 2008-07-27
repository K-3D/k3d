#include "nurbs_patch_modifier.h"

namespace module
{
    namespace nurbs
    {
        nurbs_patch_modifier::nurbs_patch_modifier(k3d::mesh& input)
        {
            m_instance = &input;

            if (m_instance->nurbs_patches == NULL)
            {
                m_nurbs_patches = new k3d::mesh::nurbs_patches_t();
            }
            else
            {
                m_nurbs_patches = k3d::make_unique(m_instance->nurbs_patches);
            }

            m_patch_first_points = k3d::make_unique(m_nurbs_patches->patch_first_points);
            m_patch_u_point_counts = k3d::make_unique(m_nurbs_patches->patch_u_point_counts);
            m_patch_v_point_counts = k3d::make_unique(m_nurbs_patches->patch_v_point_counts);
            m_patch_u_orders = k3d::make_unique(m_nurbs_patches->patch_u_orders);
            m_patch_v_orders = k3d::make_unique(m_nurbs_patches->patch_v_orders);
            m_patch_u_first_knots = k3d::make_unique(m_nurbs_patches->patch_u_first_knots);
            m_patch_v_first_knots = k3d::make_unique(m_nurbs_patches->patch_v_first_knots);
            m_patch_selection = k3d::make_unique(m_nurbs_patches->patch_selection);
            m_patch_materials = k3d::make_unique(m_nurbs_patches->patch_materials);
            m_patch_points = k3d::make_unique(m_nurbs_patches->patch_points);
            m_patch_point_weights = k3d::make_unique(m_nurbs_patches->patch_point_weights);
            m_patch_u_knots = k3d::make_unique(m_nurbs_patches->patch_u_knots);
            m_patch_v_knots = k3d::make_unique(m_nurbs_patches->patch_v_knots);
            m_patch_trim_curve_loop_counts = k3d::make_unique(m_nurbs_patches->patch_trim_curve_loop_counts);
            m_patch_first_trim_curve_loops = k3d::make_unique(m_nurbs_patches->patch_first_trim_curve_loops);
            m_trim_points = k3d::make_unique(m_nurbs_patches->trim_points);
            m_trim_point_selection = k3d::make_unique(m_nurbs_patches->trim_point_selection);
            m_first_trim_curves = k3d::make_unique(m_nurbs_patches->first_trim_curves);
            m_trim_curve_counts = k3d::make_unique(m_nurbs_patches->trim_curve_counts);
            m_trim_curve_loop_selection = k3d::make_unique(m_nurbs_patches->trim_curve_loop_selection);
            m_trim_curve_first_points = k3d::make_unique(m_nurbs_patches->trim_curve_first_points);
            m_trim_curve_point_counts = k3d::make_unique(m_nurbs_patches->trim_curve_point_counts);
            m_trim_curve_orders = k3d::make_unique(m_nurbs_patches->trim_curve_orders);
            m_trim_curve_first_knots = k3d::make_unique(m_nurbs_patches->trim_curve_first_knots);
            m_trim_curve_selection = k3d::make_unique(m_nurbs_patches->trim_curve_selection);
            m_trim_curve_points = k3d::make_unique(m_nurbs_patches->trim_curve_points);
            m_trim_curve_point_weights = k3d::make_unique(m_nurbs_patches->trim_curve_point_weights);
            m_trim_curve_knots = k3d::make_unique(m_nurbs_patches->trim_curve_knots);
            m_mesh_points = k3d::make_unique(m_instance->points);
            m_point_selections = k3d::make_unique(m_instance->point_selection);

        }

        nurbs_curve nurbs_patch_modifier::extract_v_curve(size_t patch, size_t u)
        {
            try
            {

                nurbs_curve result;

                const size_t points_begin = m_patch_first_points->at(patch) + u;
                const size_t point_step =  m_patch_u_point_counts->at(patch);
                const size_t points_end = points_begin + m_patch_v_point_counts->at(patch);

                const size_t knot_count_v = m_patch_v_point_counts->at(patch) + m_patch_v_orders->at(patch);

                const size_t knots_begin = m_patch_v_first_knots->at(patch);

                for (int i = knots_begin; i < knots_begin + knot_count_v; i++)
                {
                    result.curve_knots.push_back(m_patch_v_knots->at(i));
                }

                for (int i = points_begin; i < points_end; i += point_step)
                {
                    result.control_points.push_back(m_mesh_points->at( m_patch_points->at(i) ));
                    result.curve_point_weights.push_back( m_patch_point_weights->at(i) );
                }

                return result;
            }
            catch (std::exception& e)
            {
                MY_DEBUG << "Error in Extract V Curve: " << e.what() << std::endl;
                return nurbs_curve();
            }
            catch (...)
            {
                MY_DEBUG << "Error in Extract V Curve" << std::endl;
                return nurbs_curve();
            }
        }

        nurbs_curve nurbs_patch_modifier::extract_u_curve(size_t patch, size_t v)
        {
            try
            {
                nurbs_curve result;

                const size_t points_begin = m_patch_first_points->at(patch) + v * m_patch_v_point_counts->at(patch);
                const size_t point_step = 1;
                const size_t points_end = points_begin + m_patch_u_point_counts->at(patch);

                const size_t knot_count_u = m_patch_u_point_counts->at(patch) + m_patch_u_orders->at(patch);

                const size_t knots_begin = m_patch_u_first_knots->at(patch);

                for (int i = knots_begin; i < knots_begin + knot_count_u; i++)
                {
                    result.curve_knots.push_back(m_patch_u_knots->at(i));
                }

                for (int i = points_begin; i < points_end; i += point_step)
                {
                    result.control_points.push_back(m_mesh_points->at( m_patch_points->at(i) ));
                    result.curve_point_weights.push_back( m_patch_point_weights->at(i) );
                }

                return result;
            }
            catch (std::exception& e)
            {
                MY_DEBUG << "Error in Extract U Curve: " << e.what() << std::endl;
                return nurbs_curve();
            }
            catch (...)
            {
                MY_DEBUG << "Error in Extract U Curve" << std::endl;
                return nurbs_curve();
            }
        }

        nurbs_patch nurbs_patch_modifier::extract_patch(size_t patch)
        {
            try
            {
                MY_DEBUG << "Extracting curves" << std::endl;
                nurbs_curve u = extract_u_curve(patch,0);
                nurbs_curve v = extract_v_curve(patch,0);

                nurbs_patch result;
                result.u_knots = u.curve_knots;
                result.v_knots = v.curve_knots;

                result.u_order = m_patch_u_orders->at(patch);
                result.v_order = m_patch_v_orders->at(patch);

                MY_DEBUG << "Copying points & weights" << std::endl;
                for (int i = 0; i < m_patch_v_point_counts->at(patch) * m_patch_u_point_counts->at(patch); i++)
                {
                    result.control_points.push_back( m_mesh_points->at( m_patch_points->at(m_patch_first_points->at(patch) + i)) );
                    result.point_weights.push_back( m_patch_point_weights->at(m_patch_first_points->at(patch) + i) );
                }

                return result;
            }
            catch (std::exception& e)
            {
                MY_DEBUG << "Error in ExtractPatch: " << e.what() << std::endl;
                return nurbs_patch();
            }
            catch (...)
            {
                MY_DEBUG << "Error in ExtractPatch" << std::endl;
                return nurbs_patch();
            }
        }

        int nurbs_patch_modifier::get_patch_count()
        {
            return m_patch_first_points->size();
        }

        void nurbs_patch_modifier::insert_patch(nurbs_patch& patch, bool share_points)
        {
            k3d::mesh::indices_t patch_points;

            for (int i = 0; i < patch.control_points.size(); i++)
            {
                patch_points.push_back(insert_point(patch.control_points.at(i),share_points));
            }

            k3d::gprim_factory fac(*m_instance);
            fac.add_nurbs_patch(patch.u_order, patch.v_order, patch_points, patch.u_knots, patch.v_knots, patch.point_weights);
        }

        size_t nurbs_patch_modifier::insert_point(k3d::point3& point, bool shared)
        {
            try
            {
                int found = -1;

                for (int j = 0; j < m_mesh_points->size(); j++)
                {
                    if (nurbs_curve_modifier::point3_float_equal(m_mesh_points->at(j),point,0.000001))
                    {
                        found = j;
                    }
                }

                if (found < 0 || !shared)
                {
                    //we need to insert the point
                    m_mesh_points->push_back(point);
                    m_point_selections->push_back(0.0);
                    return m_mesh_points->size() - 1;
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
    }
}
