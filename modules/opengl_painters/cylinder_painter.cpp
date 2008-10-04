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
#include <k3dsdk/mesh.h>
#include <k3dsdk/mesh_painter_gl.h>
#include <k3dsdk/nurbs.h>
#include <k3dsdk/painter_render_state_gl.h>
#include <k3dsdk/cylinder.h>
#include <k3dsdk/utility_gl.h>

#include <boost/scoped_ptr.hpp>

namespace module
{

namespace opengl
{

namespace painters
{

/////////////////////////////////////////////////////////////////////////////
// cylinder_painter

class cylinder_painter :
	public k3d::gl::mesh_painter
{
	typedef k3d::gl::mesh_painter base;

public:
	cylinder_painter(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document)
	{
	}

	void on_paint_mesh(const k3d::mesh& Mesh, const k3d::gl::painter_render_state& RenderState)
	{
		for(k3d::mesh::primitives_t::const_iterator primitive = Mesh.primitives.begin(); primitive != Mesh.primitives.end(); ++primitive)
		{
			boost::scoped_ptr<k3d::cylinder::const_primitive> cylinder(k3d::cylinder::validate(**primitive));
			if(!cylinder)
				continue;

			glPolygonOffset(1.0, 1.0);
			glEnable(GL_POLYGON_OFFSET_FILL);
			glEnable(GL_LIGHTING);

			glColor3d(0.8, 0.8, 0.8);

			glMatrixMode(GL_MODELVIEW);
			for(k3d::uint_t i = 0; i != cylinder->matrices.size(); ++i)
			{
				glPushMatrix();
				k3d::gl::push_matrix(cylinder->matrices[i]);
				draw_solid(RenderState, cylinder->radii[i], cylinder->z_min[i], cylinder->z_max[i], cylinder->sweep_angles[i]);
				glPopMatrix();
			}

/*
			if(RenderState.node_selection)
			{
				glDisable(GL_LIGHTING);
				glColor3d(1, 1, 1);

				for(k3d::uint_t i = 0; i != cylinder->matrices.size(); ++i)
				{
					glPushMatrix();
					k3d::gl::push_matrix(cylinder->matrices[i]);
					glCallList(get_wireframe_display_list());
					glPopMatrix();
				}
			}
*/
		}
	}

	void on_select_mesh(const k3d::mesh& Mesh, const k3d::gl::painter_render_state& RenderState, const k3d::gl::painter_selection_state& SelectionState)
	{
		for(k3d::mesh::primitives_t::const_iterator primitive = Mesh.primitives.begin(); primitive != Mesh.primitives.end(); ++primitive)
		{
			boost::scoped_ptr<k3d::cylinder::const_primitive> cylinder(k3d::cylinder::validate(**primitive));
			if(!cylinder)
				continue;

			glDisable(GL_LIGHTING);

			glMatrixMode(GL_MODELVIEW);
			for(k3d::uint_t i = 0; i != cylinder->matrices.size(); ++i)
			{
				glPushMatrix();
				k3d::gl::push_matrix(cylinder->matrices[i]);
				draw_solid(RenderState, cylinder->radii[i], cylinder->z_min[i], cylinder->z_max[i], cylinder->sweep_angles[i]);
				glPopMatrix();
			}

		}
	}

	void on_mesh_changed(const k3d::mesh& Mesh, k3d::ihint* Hint)
	{
	}

	void draw_solid(const k3d::gl::render_state& State, const k3d::double_t Radius, const k3d::double_t ZMin, const k3d::double_t ZMax, const k3d::double_t SweepAngle)
	{
		if(!Radius)
			return;

		std::vector<GLfloat> incomplete_gl_u_knot_vector;
		std::vector<GLfloat> incomplete_gl_v_knot_vector;
		std::vector<GLfloat> incomplete_gl_control_points;

		std::vector<k3d::double_t> weights;
		std::vector<k3d::point3> arc_points;
		k3d::nurbs::circular_arc(k3d::vector3(1, 0, 0), k3d::vector3(0, 1, 0), 0, SweepAngle, 4, incomplete_gl_u_knot_vector, weights, arc_points);

		incomplete_gl_v_knot_vector.insert(incomplete_gl_v_knot_vector.end(), 2, 0);
		incomplete_gl_v_knot_vector.insert(incomplete_gl_v_knot_vector.end(), 1);
		incomplete_gl_v_knot_vector.insert(incomplete_gl_v_knot_vector.end(), 2, 2);

		for(unsigned long i = 0; i <= 2; ++i)
		{
			const k3d::point3 offset = k3d::mix(ZMin, ZMax, static_cast<double>(i) / static_cast<double>(2)) * k3d::point3(0, 0, 1);

			for(unsigned long j = 0; j != arc_points.size(); ++j)
			{
				incomplete_gl_control_points.push_back(weights[j] * (Radius * arc_points[j][0] + offset[0]));
				incomplete_gl_control_points.push_back(weights[j] * (Radius * arc_points[j][1] + offset[1]));
				incomplete_gl_control_points.push_back(weights[j] * (Radius * arc_points[j][2] + offset[2]));
				incomplete_gl_control_points.push_back(weights[j]);
			}
		}

		GLUnurbsObj* const nurbs_renderer = gluNewNurbsRenderer();

		// Important!  We load our own matrices for efficiency (saves round-trips to the server) and to prevent problems with selection
		gluNurbsProperty(nurbs_renderer, GLU_AUTO_LOAD_MATRIX, GL_FALSE);
		gluNurbsProperty(nurbs_renderer, GLU_CULLING, GL_TRUE);
		GLfloat gl_modelview_matrix[16];
		glGetFloatv(GL_MODELVIEW_MATRIX, gl_modelview_matrix);
		gluLoadSamplingMatrices(nurbs_renderer, gl_modelview_matrix, State.gl_projection_matrix, State.gl_viewport);

		gluBeginSurface(nurbs_renderer);
		gluNurbsSurface(nurbs_renderer, incomplete_gl_u_knot_vector.size(), &incomplete_gl_u_knot_vector[0], incomplete_gl_v_knot_vector.size(), &incomplete_gl_v_knot_vector[0], 4, 36, &incomplete_gl_control_points[0], 3, 2, GL_MAP2_VERTEX_4);
		gluEndSurface(nurbs_renderer);

		gluDeleteNurbsRenderer(nurbs_renderer);
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<cylinder_painter, k3d::interface_list<k3d::gl::imesh_painter> > factory(
			k3d::uuid(0x311d3fbd, 0x6a46e7b0, 0xccf9af8e, 0xf2daf44a),
			"OpenGLCylinderPainter",
			_("Renders cylinder primitives using OpoenGL"),
			"OpenGL Painter",
			k3d::iplugin_factory::EXPERIMENTAL);

		return factory;
	}
};

/////////////////////////////////////////////////////////////////////////////
// cylinder_painter_factory

k3d::iplugin_factory& cylinder_painter_factory()
{
	return cylinder_painter::get_factory();
}

} // namespace painters

} // namespace opengl

} // namespace module

