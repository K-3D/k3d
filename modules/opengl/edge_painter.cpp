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
#include <k3d-i18n-config.h>
#include <k3dsdk/mesh_painter_gl.h>
#include <k3dsdk/mesh_operations.h>
#include <k3dsdk/painter_render_state_gl.h>
#include <k3dsdk/painter_selection_state_gl.h>
#include <k3dsdk/persistent.h>
#include <k3dsdk/selection.h>

namespace libk3dopengl
{

/////////////////////////////////////////////////////////////////////////////
// edge_painter

class edge_painter :
	public k3d::gl::mesh_painter
{
	typedef k3d::gl::mesh_painter base;

public:
	edge_painter(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document)
	{
	}

	void on_paint_mesh(const k3d::mesh& Mesh, const k3d::gl::painter_render_state& RenderState)
	{
		if(!k3d::validate_polyhedra(Mesh))
			return;

		const k3d::mesh::indices_t& edge_points = *Mesh.polyhedra->edge_points;
		const k3d::mesh::indices_t& clockwise_edges = *Mesh.polyhedra->clockwise_edges;
		const k3d::mesh::selection_t& edge_selection = *Mesh.polyhedra->edge_selection;
		const k3d::mesh::points_t& points = *Mesh.points;
		
		k3d::gl::store_attributes attributes;
		glDisable(GL_LIGHTING);

		const k3d::color color = RenderState.node_selection ? k3d::color(1, 1, 1) : k3d::color(0, 0, 0);
		const k3d::color selected_color = RenderState.show_component_selection ? k3d::color(1, 0, 0) : color;

		glBegin(GL_LINES);
		const size_t edge_count = edge_points.size();
		for(size_t edge = 0; edge != edge_count; ++edge)
		{
			k3d::gl::color3d(edge_selection[edge] ? selected_color : color);
			k3d::gl::vertex3d(points[edge_points[edge]]);
			k3d::gl::vertex3d(points[edge_points[clockwise_edges[edge]]]);
		}
		glEnd();
	}
	
	void on_select_mesh(const k3d::mesh& Mesh, const k3d::gl::painter_render_state& RenderState, const k3d::gl::painter_selection_state& SelectionState)
	{
		if(!SelectionState.select_edges)
			return;

		if(!k3d::validate_polyhedra(Mesh))
			return;

		const k3d::mesh::indices_t& edge_points = *Mesh.polyhedra->edge_points;
		const k3d::mesh::indices_t& clockwise_edges = *Mesh.polyhedra->clockwise_edges;
		const k3d::mesh::points_t& points = *Mesh.points;
		
		k3d::gl::store_attributes attributes;
		glDisable(GL_LIGHTING);

		const size_t edge_count = edge_points.size();
		for(size_t edge = 0; edge != edge_count; ++edge)
		{
			k3d::gl::push_selection_token(k3d::selection::ABSOLUTE_SPLIT_EDGE, edge);

			glBegin(GL_LINES);
			k3d::gl::vertex3d(points[edge_points[edge]]);
			k3d::gl::vertex3d(points[edge_points[clockwise_edges[edge]]]);
			glEnd();

			k3d::gl::pop_selection_token(); // ABSOLUTE_SPLIT_EDGE
		}
	}
	
	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<edge_painter, k3d::interface_list<k3d::gl::imesh_painter > > factory(
			k3d::uuid(0xb1260f93, 0xe16e4ab2, 0xbd6a7cbd, 0x85ddca8b),
			"OpenGLEdgePainter",
			_("Renders mesh edges"),
			"OpenGL Painters",
			k3d::iplugin_factory::EXPERIMENTAL);

		return factory;
	}
};

/////////////////////////////////////////////////////////////////////////////
// edge_painter_factory

k3d::iplugin_factory& edge_painter_factory()
{
	return edge_painter::get_factory();
}

} // namespace libk3dopengl

