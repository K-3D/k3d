#include "nurbs_patch_modifier.h"

namespace module
{
	namespace nurbs
	{
		nurbs_patch_modifier::nurbs_patch_modifier(k3d::mesh& input)
		{
			m_instance = &input;
			if(m_instance->nurbs_patches != NULL)
			{
			    m_nurbs_patches = k3d::make_unique(m_instance->nurbs_patches);
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
			}
			else
			{
			    k3d::log() << error << nurbs_debug << "NurbsPatch not initialized, program might crash" << std::endl;
			    m_nurbs_patches = NULL;
                m_patch_first_points = NULL;
				m_patch_u_point_counts = NULL;
				m_patch_v_point_counts = NULL;
				m_patch_u_orders = NULL;
				m_patch_v_orders = NULL;
				m_patch_u_first_knots = NULL;
				m_patch_v_first_knots = NULL;
				m_patch_selection = NULL;
				m_patch_materials = NULL;
				m_patch_points = NULL;
				m_patch_point_weights = NULL;
				m_patch_u_knots = NULL;
				m_patch_v_knots = NULL;
				m_patch_trim_curve_loop_counts = NULL;
                m_patch_first_trim_curve_loops = NULL;
                m_trim_points = NULL;
                m_trim_point_selection = NULL;
                m_first_trim_curves = NULL;
                m_trim_curve_counts = NULL;
                m_trim_curve_loop_selection = NULL;
                m_trim_curve_first_points = NULL;
                m_trim_curve_point_counts = NULL;
                m_trim_curve_orders = NULL;
                m_trim_curve_first_knots = NULL;
                m_trim_curve_selection = NULL;
                m_trim_curve_points = NULL;
                m_trim_curve_point_weights = NULL;
                m_trim_curve_knots = NULL;
			}
		}

		void nurbs_patch_modifier::add_patch(const size_t u_order, const size_t v_order, const k3d::mesh::indices_t& points, const k3d::mesh::knots_t& u_knots, const k3d::mesh::knots_t v_knots, const k3d::mesh::weights_t& weights)
		{

		}
	}
}
