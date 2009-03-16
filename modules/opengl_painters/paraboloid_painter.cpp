// K-3D
// Copyright (c) 1995-2008, Timothy M. Shead
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
	\author Tim Shead (tshead@k-3d.com)
*/

#include <k3d-i18n-config.h>
#include <k3dsdk/document_plugin_factory.h>
#include <k3dsdk/geometric_operations.h>
#include <k3dsdk/mesh.h>
#include <k3dsdk/mesh_painter_gl.h>
#include <k3dsdk/nurbs_curve.h>
#include <k3dsdk/painter_render_state_gl.h>
#include <k3dsdk/painter_selection_state_gl.h>
#include <k3dsdk/paraboloid.h>
#include <k3dsdk/selection.h>
#include <k3dsdk/utility_gl.h>

#include <boost/scoped_ptr.hpp>

namespace module
{

namespace opengl
{

namespace painters
{

/////////////////////////////////////////////////////////////////////////////
// paraboloid_painter

class paraboloid_painter :
	public k3d::gl::mesh_painter
{
	typedef k3d::gl::mesh_painter base;

public:
	paraboloid_painter(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document)
	{
	}

	void on_paint_mesh(const k3d::mesh& Mesh, const k3d::gl::painter_render_state& RenderState)
	{
		const k3d::color color = RenderState.node_selection ? k3d::color(1, 1, 1) : k3d::color(0.8, 0.8, 0.8);
		const k3d::color selected_color = RenderState.show_component_selection ? k3d::color(1, 0, 0) : color;

		for(k3d::mesh::primitives_t::const_iterator primitive = Mesh.primitives.begin(); primitive != Mesh.primitives.end(); ++primitive)
		{
			boost::scoped_ptr<k3d::paraboloid::const_primitive> paraboloid(k3d::paraboloid::validate(**primitive));
			if(!paraboloid)
				continue;

			glPolygonOffset(1.0, 1.0);
			glEnable(GL_POLYGON_OFFSET_FILL);
			glEnable(GL_LIGHTING);

			glMatrixMode(GL_MODELVIEW);
			for(k3d::uint_t i = 0; i != paraboloid->matrices.size(); ++i)
			{
				k3d::gl::material(GL_FRONT_AND_BACK, GL_DIFFUSE, paraboloid->selections[i] ? selected_color : color);

				glPushMatrix();
				k3d::gl::push_matrix(paraboloid->matrices[i]);
				draw_solid(RenderState, paraboloid->radii[i], paraboloid->z_min[i], paraboloid->z_max[i], paraboloid->sweep_angles[i]);
				glPopMatrix();
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
			boost::scoped_ptr<k3d::paraboloid::const_primitive> paraboloid(k3d::paraboloid::validate(**primitive));
			if(!paraboloid)
				continue;

			k3d::gl::push_selection_token(k3d::selection::PRIMITIVE, primitive_index);

			glDisable(GL_LIGHTING);

			glMatrixMode(GL_MODELVIEW);
			for(k3d::uint_t i = 0; i != paraboloid->matrices.size(); ++i)
			{
				k3d::gl::push_selection_token(k3d::selection::UNIFORM, i);

				glPushMatrix();
				k3d::gl::push_matrix(paraboloid->matrices[i]);
				draw_solid(RenderState, paraboloid->radii[i], paraboloid->z_min[i], paraboloid->z_max[i], paraboloid->sweep_angles[i]);
				glPopMatrix();

				k3d::gl::pop_selection_token(); // UNIFORM
			}

			k3d::gl::pop_selection_token(); // PRIMITIVE
		}
	}

	void on_mesh_changed(const k3d::mesh& Mesh, k3d::ihint* Hint)
	{
	}

	void draw_solid(const k3d::gl::render_state& State, const k3d::double_t Radius, const k3d::double_t ZMin, const k3d::double_t ZMax, const k3d::double_t SweepAngle)
	{
		if(!Radius)
			return;

		const k3d::double_t z1 = ZMax ? ZMin / ZMax : 0.0;
		const k3d::double_t z3 = 1.0;

		const k3d::double_t r1 = std::sqrt(z1);
		const k3d::double_t r3 = 1.0;

		const k3d::point3 p1(r1, z1, 0);
		const k3d::point3 p3(r3, z3, 0);

		const k3d::vector3 t1(1, 2 * std::sqrt(z1), 0);
		const k3d::vector3 t3(1, 2, 0);

		k3d::point3 p2;
		k3d::intersect_lines(p1, t1, p3, t3, p2);

		const k3d::double_t z2 = p2[1];
		const k3d::double_t r2 = p2[0];

		k3d::mesh::knots_t knots;
		k3d::mesh::weights_t weights;
		k3d::mesh::points_t arc_points;
		k3d::nurbs_curve::circular_arc(k3d::vector3(1, 0, 0), k3d::vector3(0, 1, 0), 0, SweepAngle, 4, knots, weights, arc_points);

		std::vector<GLfloat> gl_u_knot_vector(knots.begin(), knots.end());
		std::vector<GLfloat> gl_v_knot_vector;
		std::vector<GLfloat> gl_control_points;

		gl_v_knot_vector.insert(gl_v_knot_vector.end(), 3, 0);
		gl_v_knot_vector.insert(gl_v_knot_vector.end(), 3, 1);

		for(k3d::uint_t j = 0; j != arc_points.size(); ++j)
		{
			gl_control_points.push_back(weights[j] * r1 * Radius * arc_points[j][0]);
			gl_control_points.push_back(weights[j] * r1 * Radius * arc_points[j][1]);
			gl_control_points.push_back(weights[j] * z1 * ZMax);
			gl_control_points.push_back(weights[j]);
		}

		for(k3d::uint_t j = 0; j != arc_points.size(); ++j)
		{
			gl_control_points.push_back(weights[j] * r2 * Radius * arc_points[j][0]);
			gl_control_points.push_back(weights[j] * r2 * Radius * arc_points[j][1]);
			gl_control_points.push_back(weights[j] * z2 * ZMax);
			gl_control_points.push_back(weights[j]);
		}

		for(k3d::uint_t j = 0; j != arc_points.size(); ++j)
		{
			gl_control_points.push_back(weights[j] * r3 * Radius * arc_points[j][0]);
			gl_control_points.push_back(weights[j] * r3 * Radius * arc_points[j][1]);
			gl_control_points.push_back(weights[j] * z3 * ZMax);
			gl_control_points.push_back(weights[j]);
		}

		GLUnurbsObj* const nurbs_renderer = gluNewNurbsRenderer();

		// Important!  We load our own matrices for efficiency (saves round-trips to the server) and to prevent problems with selection
		gluNurbsProperty(nurbs_renderer, GLU_AUTO_LOAD_MATRIX, GL_FALSE);
		gluNurbsProperty(nurbs_renderer, GLU_CULLING, GL_TRUE);
		GLfloat gl_modelview_matrix[16];
		glGetFloatv(GL_MODELVIEW_MATRIX, gl_modelview_matrix);
		gluLoadSamplingMatrices(nurbs_renderer, gl_modelview_matrix, State.gl_projection_matrix, State.gl_viewport);

		gluBeginSurface(nurbs_renderer);
		gluNurbsSurface(nurbs_renderer, gl_u_knot_vector.size(), &gl_u_knot_vector[0], gl_v_knot_vector.size(), &gl_v_knot_vector[0], 4, 36, &gl_control_points[0], 3, 3, GL_MAP2_VERTEX_4);
		gluEndSurface(nurbs_renderer);

		gluDeleteNurbsRenderer(nurbs_renderer);
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<paraboloid_painter, k3d::interface_list<k3d::gl::imesh_painter> > factory(
			k3d::uuid(0xa4e66aa9, 0xc34a22ca, 0xc5d3b19e, 0x83c5d0f4),
			"OpenGLParaboloidPainter",
			_("Renders paraboloid primitives using OpoenGL"),
			"OpenGL Painter",
			k3d::iplugin_factory::EXPERIMENTAL);

		return factory;
	}
};

/////////////////////////////////////////////////////////////////////////////
// paraboloid_painter_factory

k3d::iplugin_factory& paraboloid_painter_factory()
{
	return paraboloid_painter::get_factory();
}

} // namespace painters

} // namespace opengl

} // namespace module

