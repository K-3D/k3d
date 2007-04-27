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

#include <k3dsdk/document_plugin_factory.h>
#include <k3dsdk/i18n.h>
#include <k3dsdk/mesh_painter_gl.h>
#include <k3dsdk/new_mesh.h>
#include <k3dsdk/painter_render_state_gl.h>
#include <k3dsdk/painter_selection_state_gl.h>
#include <k3dsdk/persistent.h>
#include <k3dsdk/selection.h>

namespace libk3dopengl
{

/////////////////////////////////////////////////////////////////////////////
// bilinear_patch_painter

class bilinear_patch_painter :
	public k3d::gl::mesh_painter
{
	typedef k3d::gl::mesh_painter base;

public:
	bilinear_patch_painter(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document)
	{
	}

	void on_paint_mesh(const k3d::dev::mesh& Mesh, const k3d::gl::painter_render_state& RenderState)
	{
		if(!k3d::dev::validate_bilinear_patches(Mesh))
			return;

		const k3d::dev::mesh::selection_t& patch_selection = *Mesh.bilinear_patches->patch_selection;
		const k3d::dev::mesh::indices_t& patch_points = *Mesh.bilinear_patches->patch_points;
		const k3d::dev::mesh::points_t& points = *Mesh.points;

		k3d::gl::store_attributes attributes;
		glEnable(GL_LIGHTING);

		const k3d::color color = k3d::color(0.8, 0.8, 0.8);
		const k3d::color selected_color = RenderState.show_component_selection ? k3d::color(1, 0, 0) : color;

		glFrontFace(GL_CCW);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		k3d::gl::set(GL_CULL_FACE, RenderState.draw_two_sided);

		const unsigned int u_count = 10;
		const unsigned int v_count = 10;
		const GLint u_order = 2;
		const GLint v_order = 2;
		const GLint u_stride = 3;
		const GLint v_stride = 2 * u_stride;

		glEnable(GL_MAP2_VERTEX_3);
		glEnable(GL_AUTO_NORMAL);
		glMapGrid2d(u_count, 0.0, 1.0, v_count, 0.0, 1.0);

		GLdouble gl_patch_points[2 * 2 * 3];
		const size_t patch_begin = 0;
		const size_t patch_end = patch_begin + (patch_points.size() / 4);
		for(size_t patch = patch_begin; patch != patch_end; ++patch)
		{
			k3d::gl::material(GL_FRONT_AND_BACK, GL_DIFFUSE, patch_selection[patch] ? selected_color : color);

			GLdouble* gl_patch_point = gl_patch_points;
			
			const size_t point_begin = patch * 4;
			const size_t point_end = point_begin + 4;
			for(size_t point = point_begin; point != point_end; ++point)
			{
				*gl_patch_point++ = points[patch_points[point]][0];
				*gl_patch_point++ = points[patch_points[point]][1];
				*gl_patch_point++ = points[patch_points[point]][2];
			}

			glMap2d(GL_MAP2_VERTEX_3, 0, 1, u_stride, u_order, 0, 1, v_stride, v_order, &gl_patch_points[0]);
			glEvalMesh2(GL_FILL, 0, u_count, 0, v_count);
		}
	}
	
	void on_select_mesh(const k3d::dev::mesh& Mesh, const k3d::gl::painter_render_state& RenderState, const k3d::gl::painter_selection_state& SelectionState)
	{
		if(!SelectionState.select_bilinear_patches)
			return;

		if(!k3d::dev::validate_bilinear_patches(Mesh))
			return;

		const k3d::dev::mesh::indices_t& patch_points = *Mesh.bilinear_patches->patch_points;
		const k3d::dev::mesh::points_t& points = *Mesh.points;

		k3d::gl::store_attributes attributes;

		glFrontFace(GL_CCW);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		k3d::gl::set(GL_CULL_FACE, RenderState.draw_two_sided);

		const unsigned int u_count = 10;
		const unsigned int v_count = 10;
		const GLint u_order = 2;
		const GLint v_order = 2;
		const GLint u_stride = 3;
		const GLint v_stride = 2 * u_stride;

		glEnable(GL_MAP2_VERTEX_3);
		glEnable(GL_AUTO_NORMAL);
		glMapGrid2d(u_count, 0.0, 1.0, v_count, 0.0, 1.0);

		GLdouble gl_patch_points[2 * 2 * 3];
		const size_t patch_begin = 0;
		const size_t patch_end = patch_begin + (patch_points.size() / 4);
		for(size_t patch = patch_begin; patch != patch_end; ++patch)
		{
			GLdouble* gl_patch_point = gl_patch_points;
			
			const size_t point_begin = patch * 4;
			const size_t point_end = point_begin + 4;
			for(size_t point = point_begin; point != point_end; ++point)
			{
				*gl_patch_point++ = points[patch_points[point]][0];
				*gl_patch_point++ = points[patch_points[point]][1];
				*gl_patch_point++ = points[patch_points[point]][2];
			}

			k3d::gl::push_selection_token(k3d::selection::ABSOLUTE_BILINEAR_PATCH, patch);

			glMap2d(GL_MAP2_VERTEX_3, 0, 1, u_stride, u_order, 0, 1, v_stride, v_order, &gl_patch_points[0]);
			glEvalMesh2(GL_FILL, 0, u_count, 0, v_count);

			k3d::gl::pop_selection_token(); // ABSOLUTE_BILINEAR_PATCH
		}
	}
	
	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<bilinear_patch_painter, k3d::interface_list<k3d::gl::imesh_painter > > factory(
			k3d::uuid(0xf172b866, 0xfce74aa8, 0xa9e792ff, 0x630653f3),
			"OpenGLBilinearPatchPainter",
			_("Renders bilinear patches"),
			"OpenGL Painters",
			k3d::iplugin_factory::EXPERIMENTAL);

		return factory;
	}
};

/////////////////////////////////////////////////////////////////////////////
// bilinear_patch_painter_factory

k3d::iplugin_factory& bilinear_patch_painter_factory()
{
	return bilinear_patch_painter::get_factory();
}

} // namespace libk3dopengl

