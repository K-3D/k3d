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
#include <k3dsdk/hints.h>
#include <k3d-i18n-config.h>
#include <k3dsdk/mesh_painter_gl.h>
#include <k3dsdk/mesh_operations.h>
#include <k3dsdk/painter_render_state_gl.h>
#include <k3dsdk/painter_selection_state_gl.h>
#include <k3dsdk/persistent.h>
#include <k3dsdk/selection.h>

#include "colored_selection_painter_gl.h"

namespace module
{

namespace opengl
{

namespace painters
{

/////////////////////////////////////////////////////////////////////////////
// point_painter

class point_painter :
	public colored_selection_painter
{
	typedef colored_selection_painter base;

public:
	point_painter(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document)
	{
	}

	void on_paint_mesh(const k3d::mesh& Mesh, const k3d::gl::painter_render_state& RenderState)
	{
		if(!Mesh.points)
			return;
		if(!Mesh.point_selection)
			return;

		const k3d::mesh::points_t& points = *Mesh.points;
		const k3d::mesh::selection_t& point_selection = *Mesh.point_selection;
		
		glEnableClientState(GL_VERTEX_ARRAY);
		glVertexPointer(3, GL_DOUBLE, 0, &points[0]);

		k3d::gl::store_attributes attributes;
		glDisable(GL_LIGHTING);

		const color_t color = RenderState.node_selection ? selected_mesh_color() : unselected_mesh_color(RenderState.parent_selection);
		const color_t selected_color = RenderState.show_component_selection ? selected_component_color() : color;
	
		enable_blending();
		
		glBegin(GL_POINTS);
		const size_t point_begin = 0;
		const size_t point_end = points.size();
		for(size_t point = point_begin; point != point_end; ++point)
		{
			color4d(point_selection[point] ? selected_color : color);
			glArrayElement(point);
		}
		glEnd();
		
		disable_blending();
	}
	
	void on_select_mesh(const k3d::mesh& Mesh, const k3d::gl::painter_render_state& RenderState, const k3d::gl::painter_selection_state& SelectionState)
	{
		if(!SelectionState.select_points)
			return;

		if(!Mesh.points)
			return;

		const k3d::mesh::points_t& points = *Mesh.points;

		glEnableClientState(GL_VERTEX_ARRAY);
		glVertexPointer(3, GL_DOUBLE, 0, &points[0]);

		k3d::gl::store_attributes attributes;
		glDisable(GL_LIGHTING);

		const size_t point_begin = 0;
		const size_t point_end = points.size();
		for(size_t point = point_begin; point != point_end; ++point)
		{
			k3d::gl::push_selection_token(k3d::selection::ABSOLUTE_POINT, point);

			glBegin(GL_POINTS);
			glArrayElement(point);
			glEnd();

			k3d::gl::pop_selection_token(); // ABSOLUTE_POINT
		}
	}
	
	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<point_painter, k3d::interface_list<k3d::gl::imesh_painter > > factory(
			k3d::uuid(0xad24a631, 0xacbd4af1, 0xbd6d6362, 0x75c3d5d7),
			"OpenGLPointPainter",
			_("Renders mesh points"),
			"OpenGL Painters",
			k3d::iplugin_factory::EXPERIMENTAL);

		return factory;
	}
};

/////////////////////////////////////////////////////////////////////////////
// point_painter_factory

k3d::iplugin_factory& point_painter_factory()
{
	return point_painter::get_factory();
}

} // namespace painters

} // namespace opengl

} // namespace module


