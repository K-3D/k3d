// K-3D
// Copyright (c) 1995-2005, Timothy M. Shead
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

/** \file Paint edges using a VBO
 */

#include "colored_selection_painter_gl.h"
#include "normal_cache.h"
#include "utility.h"
#include "vbo.h"

#include <k3d-i18n-config.h>
#include <k3dsdk/document_plugin_factory.h>
#include <k3dsdk/extension_gl.h>
#include <k3dsdk/gl.h>
#include <k3dsdk/hints.h>
#include <k3dsdk/imesh_painter_gl.h>
#include <k3dsdk/node.h>
#include <k3dsdk/painter_render_state_gl.h>
#include <k3dsdk/painter_selection_state_gl.h>
#include <k3dsdk/polyhedron.h>
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
// vbo_edge_painter

class vbo_edge_painter :
	public colored_selection_painter
{
	typedef colored_selection_painter base;
public:
	vbo_edge_painter(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document)
	{
	}
	
	void on_paint_mesh(const k3d::mesh& Mesh, const k3d::gl::painter_render_state& RenderState)
	{
		return_if_fail(k3d::gl::extension::query_vbo());

		if(!has_non_empty_polyhedra(Mesh))
			return;
		
		const color_t color = RenderState.node_selection ? selected_mesh_color() : unselected_mesh_color(RenderState.parent_selection);
		const color_t selected_color = RenderState.show_component_selection ? selected_component_color() : color;

		k3d::gl::store_attributes attributes;
		glDisable(GL_LIGHTING);
		
		enable_blending();
		
		clean_vbo_state();
		
		edge_selection& selected_edges = get_data<edge_selection>(&Mesh, this);
		
		get_data<point_vbo>(&Mesh, this).bind();
		
		for(k3d::mesh::primitives_t::const_iterator primitive = Mesh.primitives.begin(); primitive != Mesh.primitives.end(); ++primitive)
		{
			boost::scoped_ptr<k3d::polyhedron::const_primitive> polyhedron(k3d::polyhedron::validate(**primitive));
			if(!polyhedron.get())
				continue;
		
			get_data<edge_vbo>(&Mesh, this).bind(primitive->get());
			
			const k3d::uint_t edge_count = polyhedron->edge_points.size();
			const selection_records_t& edge_selection_records = selected_edges.records(primitive->get());
			if (!edge_selection_records.empty())
			{
				for (selection_records_t::const_iterator record = edge_selection_records.begin(); record != edge_selection_records.end(); ++record)
				{
					color4d(record->weight ? selected_color : color);
					size_t start = record->begin * 2;
					size_t end = record->end;
					end = end > edge_count ? edge_count : end;
					end *= 2;
					size_t count = end - start;
					glDrawElements(GL_LINES, count, GL_UNSIGNED_INT, static_cast<GLuint*>(0) + start);
				}
			}
			else
			{
				color4d(color);
				glDrawElements(GL_LINES, polyhedron->edge_points.size() * 2, GL_UNSIGNED_INT, 0);
			}
		}
		
		clean_vbo_state();
		disable_blending();
	}
	
	void on_select_mesh(const k3d::mesh& Mesh, const k3d::gl::painter_render_state& RenderState, const k3d::gl::painter_selection_state& SelectionState)
	{
		return_if_fail(k3d::gl::extension::query_vbo());

		if(!has_non_empty_polyhedra(Mesh))
			return;
			
		if (!SelectionState.select_split_edges)
			return;
		
		k3d::gl::store_attributes attributes;
		glDisable(GL_LIGHTING);

		clean_vbo_state();
		
		get_data<point_vbo>(&Mesh, this).bind();
		
		for(k3d::mesh::primitives_t::const_iterator primitive = Mesh.primitives.begin(); primitive != Mesh.primitives.end(); ++primitive)
		{
			boost::scoped_ptr<k3d::polyhedron::const_primitive> polyhedron(k3d::polyhedron::validate(**primitive));
			if(!polyhedron.get())
		
			get_data<edge_vbo>(&Mesh, this).bind(primitive->get());
			
			const k3d::mesh::indices_t& edge_points = polyhedron->edge_points;
			const k3d::mesh::indices_t& clockwise_edges = polyhedron->clockwise_edges;
			const k3d::mesh::points_t& points = *Mesh.points;
			
			const size_t edge_count = polyhedron->edge_points.size();
			for(size_t edge = 0; edge < edge_count; ++edge)
			{
				if (SelectionState.select_backfacing || 
						(!SelectionState.select_backfacing && 
								!backfacing(points[edge_points[edge]] * RenderState.matrix, RenderState.camera, get_data<normal_cache>(&Mesh, this).point_normals(this).at(edge_points[edge]))
								&& !backfacing(points[edge_points[clockwise_edges[edge]]] * RenderState.matrix, RenderState.camera, get_data<normal_cache>(&Mesh, this).point_normals(this).at(edge_points[clockwise_edges[edge]]))))
				{
					k3d::gl::push_selection_token(k3d::selection::SPLIT_EDGE, edge);
		
					glDrawElements(GL_LINES, 2, GL_UNSIGNED_INT, static_cast<GLuint*>(0) + 2*edge);
		
					k3d::gl::pop_selection_token();
				}
			}
		}
		
		clean_vbo_state();
	}
	
	void on_mesh_changed(const k3d::mesh& Mesh, k3d::ihint* Hint)
	{
		return_if_fail(k3d::gl::extension::query_vbo());
		
		if(!has_non_empty_polyhedra(Mesh))
			return;
		
		schedule_data<point_vbo>(&Mesh, Hint, this);
		schedule_data<edge_vbo>(&Mesh, Hint, this);
		schedule_data<edge_selection>(&Mesh, Hint, this);
		schedule_data<normal_cache>(&Mesh, Hint, this);
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<vbo_edge_painter, k3d::interface_list<k3d::gl::imesh_painter > > factory(
				k3d::uuid(0x7fca3f65, 0x531b4db6, 0xa11dac8a, 0x0b9f17b1),
		"OpenGLVBOEdgePainter",
		_("Renders mesh edges (OpenGL VBOs)"),
		"OpenGL Painter",
		k3d::iplugin_factory::EXPERIMENTAL);

		return factory;
	}
};

	/////////////////////////////////////////////////////////////////////////////
// vbo_edge_painter_factory

	k3d::iplugin_factory& vbo_edge_painter_factory()
	{
		return vbo_edge_painter::get_factory();
	}

} // namespace painters

} // namespace opengl

} // namespace module


