// K-3D
// Copyright (c) 1995-2007, Timothy M. Shead
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
	\author Timothy M. Shead (tshead@k-3d.com)
*/

#include <k3d-i18n-config.h>
#include <k3d-platform-config.h>
#include <k3dsdk/document_plugin_factory.h>
#include <k3dsdk/mesh_painter_gl.h>
#include <k3dsdk/nurbs_patch.h>
#include <k3dsdk/painter_render_state_gl.h>
#include <k3dsdk/painter_selection_state_gl.h>
#include <k3dsdk/selection.h>

#include <boost/scoped_ptr.hpp>

#if defined K3D_API_DARWIN
	#define GLU_NURBS_CALLBACK(callback) (GLvoid(*)())callback
#elif defined K3D_API_WIN32
	#define GLU_NURBS_CALLBACK(callback) (_GLUfuncptr)callback
#else
	#define GLU_NURBS_CALLBACK(callback) (void(*)())callback
#endif // !K3D_API_DARWIN

namespace module
{

namespace opengl
{

namespace painters
{

static void on_nurbs_error(GLenum ErrorCode)
{
		k3d::log() << debug << "NURBS patch error: " << gluErrorString(ErrorCode) << std::endl;
}

/////////////////////////////////////////////////////////////////////////////
// nurbs_patch_painter

class nurbs_patch_painter :
	public k3d::gl::mesh_painter
{
	typedef k3d::gl::mesh_painter base;

public:
	nurbs_patch_painter(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		nurbs_renderer(gluNewNurbsRenderer())
	{
		// Important!  We load our own matrices for efficiency (saves round-trips to the server) and to prevent problems with selection
		gluNurbsProperty(nurbs_renderer, GLU_AUTO_LOAD_MATRIX, GL_FALSE);
		gluNurbsProperty(nurbs_renderer, GLU_CULLING, GL_TRUE);
		gluNurbsProperty(nurbs_renderer, GLU_DISPLAY_MODE, GLU_FILL);
		gluNurbsProperty(nurbs_renderer, GLU_SAMPLING_TOLERANCE, 50);
		gluNurbsCallback(nurbs_renderer, GLU_ERROR, GLU_NURBS_CALLBACK(on_nurbs_error));
	}

	~nurbs_patch_painter()
	{
		if(nurbs_renderer)
                        gluDeleteNurbsRenderer(nurbs_renderer);
	}

	void on_paint_mesh(const k3d::mesh& Mesh, const k3d::gl::painter_render_state& RenderState)
	{
		for(k3d::mesh::primitives_t::const_iterator primitive = Mesh.primitives.begin(); primitive != Mesh.primitives.end(); ++primitive)
		{
			boost::scoped_ptr<k3d::nurbs_patch::const_primitive> nurbs(k3d::nurbs_patch::validate(**primitive));
			if(!nurbs)
				return;

			const k3d::mesh::points_t& points = *Mesh.points;

			k3d::gl::store_attributes attributes;
			glEnable(GL_LIGHTING);
			glEnable(GL_AUTO_NORMAL);
			glDisable(GL_CULL_FACE);

			GLfloat gl_modelview_matrix[16];
			glGetFloatv(GL_MODELVIEW_MATRIX, gl_modelview_matrix);
			gluLoadSamplingMatrices(nurbs_renderer, gl_modelview_matrix, RenderState.gl_projection_matrix, RenderState.gl_viewport);

			const k3d::color color = k3d::color(0.8, 0.8, 0.8);
			const k3d::color selected_color = RenderState.show_component_selection ? k3d::color(1, 0, 0) : color;

			const k3d::uint_t patch_begin = 0;
			const k3d::uint_t patch_end = patch_begin + nurbs->patch_first_points.size();
			for(k3d::uint_t patch = patch_begin; patch != patch_end; ++patch)
			{
				const k3d::uint_t patch_u_point_count = nurbs->patch_u_point_counts[patch];
				const k3d::uint_t patch_v_point_count = nurbs->patch_v_point_counts[patch];
				const k3d::uint_t patch_point_begin = nurbs->patch_first_points[patch];
				const k3d::uint_t patch_point_end = patch_point_begin + (patch_u_point_count * patch_v_point_count);
				const k3d::uint_t patch_u_order = nurbs->patch_u_orders[patch];
				const k3d::uint_t patch_v_order = nurbs->patch_v_orders[patch];
				const k3d::uint_t patch_u_first_knot = nurbs->patch_u_first_knots[patch];
				const k3d::uint_t patch_v_first_knot = nurbs->patch_v_first_knots[patch];

				k3d::gl::material(GL_FRONT_AND_BACK, GL_DIFFUSE, nurbs->patch_selections[patch] ? selected_color : color);

				std::vector<GLfloat> gl_u_knot_vector(&nurbs->patch_u_knots[patch_u_first_knot], &nurbs->patch_u_knots[patch_u_first_knot + patch_u_point_count + patch_u_order]);
				std::vector<GLfloat> gl_v_knot_vector(&nurbs->patch_v_knots[patch_v_first_knot], &nurbs->patch_v_knots[patch_v_first_knot + patch_v_point_count + patch_v_order]);

				const GLint gl_u_stride = 4;
				const GLint gl_v_stride = gl_u_stride * patch_u_point_count;

				std::vector<GLfloat> gl_control_points;
				gl_control_points.reserve(4 * patch_u_point_count * patch_v_point_count);
				for(k3d::uint_t patch_point = patch_point_begin; patch_point != patch_point_end; ++patch_point)
				{
					const double patch_point_weight = nurbs->patch_point_weights[patch_point];
					const k3d::point3 weighted_patch_point = patch_point_weight * points[nurbs->patch_points[patch_point]];

					gl_control_points.push_back(static_cast<GLfloat>(weighted_patch_point[0]));
					gl_control_points.push_back(static_cast<GLfloat>(weighted_patch_point[1]));
					gl_control_points.push_back(static_cast<GLfloat>(weighted_patch_point[2]));
					gl_control_points.push_back(static_cast<GLfloat>(patch_point_weight));
				}

				gluBeginSurface(nurbs_renderer);
				//k3d::log() << debug << "Painting patch with knot sizes: " << gl_u_knot_vector.size() << ", " << gl_v_knot_vector.size() << " and orders " << patch_u_order << ", " << patch_v_order  << " with " << gl_control_points.size() << " control points. " << std::endl;
				gluNurbsSurface(nurbs_renderer, gl_u_knot_vector.size(), &gl_u_knot_vector[0], gl_v_knot_vector.size(), &gl_v_knot_vector[0], gl_u_stride, gl_v_stride, &gl_control_points[0], patch_u_order, patch_v_order, GL_MAP2_VERTEX_4);

				if(nurbs->patch_trim_loop_counts[patch])
				{
					k3d::uint_t loops_start = nurbs->patch_first_trim_loops[patch];
					k3d::uint_t loops_end = loops_start + nurbs->patch_trim_loop_counts[patch];
					for (k3d::uint_t loop_index = loops_start; loop_index != loops_end; ++loop_index)
					{
						//k3d::log() << debug << "  drawing loop " << loop_index << std::endl;
						gluBeginTrim(nurbs_renderer);
						k3d::uint_t curves_start = nurbs->trim_loop_first_curves[loop_index];
						k3d::uint_t curves_end = curves_start + nurbs->trim_loop_curve_counts[loop_index];
						for (k3d::uint_t curve = curves_start; curve != curves_end; ++curve)
						{
							//k3d::log() << debug << "    drawing curve " << curve << std::endl;
							k3d::uint_t knots_end = nurbs->curve_first_knots[curve] + nurbs->curve_point_counts[curve] + nurbs->curve_orders[curve];
							return_if_fail(knots_end <= nurbs->curve_knots.size());
							std::vector<GLfloat> gl_trim_knot_vector(&nurbs->curve_knots[nurbs->curve_first_knots[curve]], &nurbs->curve_knots[knots_end]);
							//k3d::log() << debug << "      knot vector:" << std::endl;
							//for (k3d::uint_t knot = 0; knot != gl_trim_knot_vector.size(); ++knot)
								//k3d::log() << debug << "                   " << gl_trim_knot_vector[knot] << std::endl;
							std::vector<GLfloat> gl_trim_control_points;
							k3d::uint_t points_start = nurbs->curve_first_points[curve];
							k3d::uint_t points_end = points_start + nurbs->curve_point_counts[curve];
							return_if_fail(points_end <= nurbs->curve_points.size());
							for (k3d::uint_t point = points_start; point != points_end; ++point)
							{
								k3d::point2 control_point = nurbs->points[nurbs->curve_points[point]];
								double weight = nurbs->curve_point_weights[point];
								//k3d::log() << debug << "        drawing point " << control_point << " with weight " << weight << std::endl;
								gl_trim_control_points.push_back(static_cast<GLfloat>(control_point[0] * weight));
								gl_trim_control_points.push_back(static_cast<GLfloat>(control_point[1] * weight));
								gl_trim_control_points.push_back(static_cast<GLfloat>(weight));
							}
							gluNurbsCurve(nurbs_renderer, gl_trim_knot_vector.size(), &gl_trim_knot_vector[0], 3, &gl_trim_control_points[0], static_cast<GLint>(nurbs->curve_orders[curve]), GLU_MAP1_TRIM_3);
						}
						gluEndTrim(nurbs_renderer);
					}
				}
				gluEndSurface(nurbs_renderer);
			}
		}
	}

	void on_select_mesh(const k3d::mesh& Mesh, const k3d::gl::painter_render_state& RenderState, const k3d::gl::painter_selection_state& SelectionState)
	{
		if(!SelectionState.select_uniform)
			return;

		k3d::uint_t primitive_index = 0;
		for(k3d::mesh::primitives_t::const_iterator primitive = Mesh.primitives.begin(); primitive != Mesh.primitives.end(); ++primitive, ++primitive_index)
		{
			boost::scoped_ptr<k3d::nurbs_patch::const_primitive> nurbs(k3d::nurbs_patch::validate(**primitive));
			if(!nurbs)
				return;

			const k3d::mesh::points_t& points = *Mesh.points;

			k3d::gl::store_attributes attributes;
			glDisable(GL_LIGHTING);
			glDisable(GL_AUTO_NORMAL);
			glDisable(GL_CULL_FACE);

			k3d::gl::push_selection_token(k3d::selection::PRIMITIVE, primitive_index);

			GLfloat gl_modelview_matrix[16];
			glGetFloatv(GL_MODELVIEW_MATRIX, gl_modelview_matrix);
			gluLoadSamplingMatrices(nurbs_renderer, gl_modelview_matrix, RenderState.gl_projection_matrix, RenderState.gl_viewport);

			const k3d::uint_t patch_begin = 0;
			const k3d::uint_t patch_end = patch_begin + nurbs->patch_first_points.size();
			for(k3d::uint_t patch = patch_begin; patch != patch_end; ++patch)
			{
				const k3d::uint_t patch_u_point_count = nurbs->patch_u_point_counts[patch];
				const k3d::uint_t patch_v_point_count = nurbs->patch_v_point_counts[patch];
				const k3d::uint_t patch_point_begin = nurbs->patch_first_points[patch];
				const k3d::uint_t patch_point_end = patch_point_begin + (patch_u_point_count * patch_v_point_count);
				const k3d::uint_t patch_u_order = nurbs->patch_u_orders[patch];
				const k3d::uint_t patch_v_order = nurbs->patch_v_orders[patch];
				const k3d::uint_t patch_u_first_knot = nurbs->patch_u_first_knots[patch];
				const k3d::uint_t patch_v_first_knot = nurbs->patch_v_first_knots[patch];

				std::vector<GLfloat> gl_u_knot_vector(&nurbs->patch_u_knots[patch_u_first_knot], &nurbs->patch_u_knots[patch_u_first_knot + patch_u_point_count + patch_u_order]);
				std::vector<GLfloat> gl_v_knot_vector(&nurbs->patch_v_knots[patch_v_first_knot], &nurbs->patch_v_knots[patch_v_first_knot + patch_v_point_count + patch_v_order]);

				const GLint gl_u_stride = 4;
				const GLint gl_v_stride = gl_u_stride * patch_u_point_count;

				std::vector<GLfloat> gl_control_points;
				gl_control_points.reserve(4 * patch_u_point_count * patch_v_point_count);
				for(k3d::uint_t patch_point = patch_point_begin; patch_point != patch_point_end; ++patch_point)
				{
					const double patch_point_weight = nurbs->patch_point_weights[patch_point];
					const k3d::point3 weighted_patch_point = patch_point_weight * points[nurbs->patch_points[patch_point]];

					gl_control_points.push_back(static_cast<GLfloat>(weighted_patch_point[0]));
					gl_control_points.push_back(static_cast<GLfloat>(weighted_patch_point[1]));
					gl_control_points.push_back(static_cast<GLfloat>(weighted_patch_point[2]));
					gl_control_points.push_back(static_cast<GLfloat>(patch_point_weight));
				}

				k3d::gl::push_selection_token(k3d::selection::UNIFORM, patch);

				gluBeginSurface(nurbs_renderer);
				gluNurbsSurface(nurbs_renderer, gl_u_knot_vector.size(), &gl_u_knot_vector[0], gl_v_knot_vector.size(), &gl_v_knot_vector[0], gl_u_stride, gl_v_stride, &gl_control_points[0], patch_u_order, patch_v_order, GL_MAP2_VERTEX_4);

				if(nurbs->patch_trim_loop_counts[patch])
				{
					k3d::uint_t loops_start = nurbs->patch_first_trim_loops[patch];
					k3d::uint_t loops_end = loops_start + nurbs->patch_trim_loop_counts[patch];
					for (k3d::uint_t loop_index = loops_start; loop_index != loops_end; ++loop_index)
					{
						gluBeginTrim(nurbs_renderer);
						k3d::uint_t curves_start = nurbs->trim_loop_first_curves[loop_index];
						k3d::uint_t curves_end = curves_start + nurbs->trim_loop_curve_counts[loop_index];
						for (k3d::uint_t curve = curves_start; curve != curves_end; ++curve)
						{
							std::vector<GLfloat> gl_trim_knot_vector(&nurbs->curve_knots[nurbs->curve_first_knots[curve]], &nurbs->curve_knots[nurbs->curve_first_knots[curve] + nurbs->curve_point_counts[curve] + nurbs->curve_orders[curve]]);
							std::vector<GLfloat> gl_trim_control_points;
							k3d::uint_t points_start = nurbs->curve_first_points[curve];
							k3d::uint_t points_end = points_start + nurbs->curve_point_counts[curve];
							for (k3d::uint_t point = points_start; point != points_end; ++point)
							{
								k3d::point2 control_point = nurbs->points[nurbs->curve_points[point]];
								double weight = nurbs->curve_point_weights[point];
								gl_trim_control_points.push_back(static_cast<GLfloat>(control_point[0] * weight));
								gl_trim_control_points.push_back(static_cast<GLfloat>(control_point[1] * weight));
								gl_trim_control_points.push_back(static_cast<GLfloat>(weight));
							}
							gluNurbsCurve(nurbs_renderer, gl_trim_knot_vector.size(), &gl_trim_knot_vector[0], 3, &gl_trim_control_points[0], static_cast<GLint>(nurbs->curve_orders[curve]), GLU_MAP1_TRIM_3);
						}
						gluEndTrim(nurbs_renderer);
					}
				}
				gluEndSurface(nurbs_renderer);

				k3d::gl::pop_selection_token(); // UNIFORM
			}

			k3d::gl::pop_selection_token(); // PRIMITIVE
		}
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<nurbs_patch_painter, k3d::interface_list<k3d::gl::imesh_painter > > factory(
			k3d::uuid(0x8881a190, 0x9d1c4e6f, 0x8b8c0523, 0x2dc43fda),
			"OpenGLNURBSPatchPainter",
			_("Renders NURBS patches"),
			"OpenGL Painter",
			k3d::iplugin_factory::EXPERIMENTAL);

		return factory;
	}

private:
	GLUnurbsObj* const nurbs_renderer;
};

/////////////////////////////////////////////////////////////////////////////
// nurbs_patch_painter_factory

k3d::iplugin_factory& nurbs_patch_painter_factory()
{
	return nurbs_patch_painter::get_factory();
}

} // namespace painters

} // namespace opengl

} // namespace module


