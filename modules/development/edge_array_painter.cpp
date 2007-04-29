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

#include <k3dsdk/document_plugin_factory.h>
#include <k3dsdk/extension_gl.h>
#include <k3dsdk/gl.h>
#include <k3dsdk/hints.h>
#include <k3dsdk/i18n.h>
#include <k3dsdk/imesh_painter_gl.h>
#include <k3dsdk/new_mesh.h>
#include <k3dsdk/node.h>
#include <k3dsdk/painter_render_state_gl.h>
#include <k3dsdk/painter_selection_state_gl.h>
#include <k3dsdk/persistent.h>
#include <k3dsdk/selection.h>
#include <k3dsdk/utility_gl.h>

#include "colored_selection_painter_gl.h"
#include "vbo.h"

namespace libk3ddevelopment
{

	/////////////////////////////////////////////////////////////////////////////
// edge_array_painter

class edge_array_painter :
	public colored_selection_painter
{
	typedef colored_selection_painter base;
public:
	edge_array_painter(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_points_cache(painter_cache<boost::shared_ptr<const k3d::dev::mesh::points_t>, vbo>::instance(Document)),
		m_edges_cache(painter_cache<boost::shared_ptr<const k3d::dev::mesh::indices_t>, vbo>::instance(Document)),
		m_selection_cache(painter_cache<boost::shared_ptr<const k3d::dev::mesh::indices_t>, selection_records_t>::instance(Document)),
		m_hint_cache(painter_cache<boost::shared_ptr<const k3d::dev::mesh::points_t>, k3d::hint::mesh_geometry_changed_t>::instance(Document))
	{
	}
	
	~edge_array_painter()
	{
		m_points_cache.remove_painter(this);
		m_edges_cache.remove_painter(this);
		m_selection_cache.remove_painter(this);
	}
	
	/// Updates the vertex buffer object.
	void update_buffer(const k3d::dev::mesh& Mesh)
	{
		if(!Mesh.points || Mesh.points->empty())
			return;
		if(!Mesh.polyhedra)
			return;
		if(!Mesh.polyhedra->edge_points)
			return;
		if(!Mesh.polyhedra->clockwise_edges)
			return;
				
		// Fill vertex buffer
		create_point_vbo(Mesh.points, m_points_cache);
		k3d::hint::mesh_geometry_changed_t* changed_hint = m_hint_cache.get_data(Mesh.points);
		if (changed_hint)
		{
			update_point_vbo(Mesh.points, m_points_cache, changed_hint->changed_points);
// 			m_hint_cache.remove_data(Mesh.points);
		}
		
		//VBO for edge indices
		vbo* edge_buffer = m_edges_cache.get_data(Mesh.polyhedra->edge_points);
		if (!edge_buffer)
		{
			const k3d::dev::mesh::indices_t& edge_points = *Mesh.polyhedra->edge_points;
			const k3d::dev::mesh::indices_t& clockwise_edges = *Mesh.polyhedra->clockwise_edges;
			return_if_fail(edge_points.size() == clockwise_edges.size());
			edge_buffer = m_edges_cache.create_data(Mesh.polyhedra->edge_points);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, *edge_buffer);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * edge_points.size() * 2, 0, GL_STATIC_DRAW); // left uninited
			// init index buffer with indices from edge_points and clockwise_edges
			GLuint* indices = static_cast<GLuint*>(glMapBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_READ_WRITE)); // map buffer memory in indices
			for (size_t i = 0; i < edge_points.size(); ++i)
			{
				indices[2*i] = static_cast<GLuint>(edge_points[i]);
				indices[2*i + 1] = static_cast<GLuint>(edge_points[clockwise_edges[i]]);
			}
			glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER); // release indices
		}

		// selection data
		selection_records_t* selection = m_selection_cache.get_data(Mesh.polyhedra->edge_points);
		if (!selection)
		{
			selection = m_selection_cache.create_data(Mesh.polyhedra->edge_points);
			array_to_selection(*Mesh.polyhedra->edge_selection, *selection);
		}
	}
	
	void paint_mesh(const k3d::dev::mesh& Mesh, const k3d::gl::painter_render_state& RenderState)
	{
		return_if_fail(k3d::gl::extension::query_vbo());

		if(!Mesh.points || Mesh.points->empty())
			return;
		if(!Mesh.polyhedra)
			return;
		if(!Mesh.polyhedra->edge_points)
			return;
		if(!Mesh.polyhedra->clockwise_edges)
			return;

		update_buffer(Mesh);
		
		const k3d::color color = RenderState.node_selection ? selected_mesh_color() : unselected_mesh_color();
		const k3d::color selected_color = RenderState.show_component_selection ? selected_component_color() : color;

		k3d::gl::store_attributes attributes;
		glDisable(GL_LIGHTING);

		vbo* const point_buffer = m_points_cache.get_data(Mesh.points);
		vbo* const edge_buffer = m_edges_cache.get_data(Mesh.polyhedra->edge_points);
		
		return_if_fail(glIsBuffer(*point_buffer));
		return_if_fail(glIsBuffer(*edge_buffer));
		
		clean_vbo_state();
		
		glBindBuffer(GL_ARRAY_BUFFER, *point_buffer);
		glVertexPointer(3, GL_DOUBLE, 0, 0);
		glEnableClientState(GL_VERTEX_ARRAY);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, *edge_buffer);
		
		size_t edge_count = Mesh.polyhedra->edge_points->size();
		const selection_records_t* edge_selection = m_selection_cache.get_data(Mesh.polyhedra->edge_points);
		return_if_fail(edge_selection);
		if (!edge_selection->empty())
		{
			for (selection_records_t::const_iterator record = edge_selection->begin(); record != edge_selection->end(); ++record)
			{
				k3d::gl::color3d(record->weight ? selected_color : color);
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
			k3d::gl::color3d(color);
			glDrawElements(GL_LINES, Mesh.polyhedra->edge_points->size() * 2, GL_UNSIGNED_INT, 0);
		}
		
		clean_vbo_state();
	}
	
	void select_mesh(const k3d::dev::mesh& Mesh, const k3d::gl::painter_render_state& RenderState, const k3d::gl::painter_selection_state& SelectionState)
	{
		return_if_fail(k3d::gl::extension::query_vbo());

		if(!Mesh.points || Mesh.points->empty())
			return;
		if(!Mesh.polyhedra)
			return;
		if(!Mesh.polyhedra->edge_points)
			return;
		if(!Mesh.polyhedra->clockwise_edges)
			return;

		update_buffer(Mesh);
		
		k3d::gl::store_attributes attributes;
		glDisable(GL_LIGHTING);
		
		vbo* const point_buffer = m_points_cache.get_data(Mesh.points);
		vbo* const edge_buffer = m_edges_cache.get_data(Mesh.polyhedra->edge_points);
		
		return_if_fail(glIsBuffer(*point_buffer));
		return_if_fail(glIsBuffer(*edge_buffer));
		
		clean_vbo_state();
		
		glBindBuffer(GL_ARRAY_BUFFER, *point_buffer);
		glVertexPointer(3, GL_DOUBLE, 0, 0);
		glEnableClientState(GL_VERTEX_ARRAY);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, *edge_buffer);
		
		const size_t edge_count = Mesh.polyhedra->edge_points->size();
		for(size_t edge = 0; edge < edge_count; ++edge)
		{
			k3d::gl::push_selection_token(k3d::selection::ABSOLUTE_SPLIT_EDGE, edge);

			glDrawElements(GL_LINES, 2, GL_UNSIGNED_INT, static_cast<GLuint*>(0) + 2*edge);

			k3d::gl::pop_selection_token();
		}
		
		clean_vbo_state();
	}
	
	void mesh_changed(const k3d::dev::mesh& Mesh, k3d::iunknown* Hint)
	{
		return_if_fail(k3d::gl::extension::query_vbo());

		if(!Mesh.points || Mesh.points->empty())
			return;
		if(!Mesh.polyhedra)
			return;
		if(!Mesh.polyhedra->edge_points)
			return;
		if(!Mesh.polyhedra->clockwise_edges)
			return;
		
		m_points_cache.register_painter(Mesh.points, this);
		m_edges_cache.register_painter(Mesh.polyhedra->edge_points, this);
		m_selection_cache.register_painter(Mesh.polyhedra->edge_points, this);
		
		k3d::hint::mesh_geometry_changed_t* changed_hint = dynamic_cast<k3d::hint::mesh_geometry_changed_t*>(Hint);
		k3d::hint::selection_changed_t* selection_hint = dynamic_cast<k3d::hint::selection_changed_t*>(Hint);
		if (selection_hint)
		{
			m_selection_cache.remove_data(Mesh.polyhedra->edge_points);
		}
		else if (changed_hint && !changed_hint->changed_points.empty())
		{
			k3d::hint::mesh_geometry_changed_t& stored_changed_hint = *(m_hint_cache.create_data(Mesh.points));
			stored_changed_hint = *changed_hint;
		}
		else
		{
			k3d::log() << debug << "removing edge data" << std::endl;
			m_points_cache.remove_data(Mesh.points);
			m_edges_cache.remove_data(Mesh.polyhedra->edge_points);
			m_selection_cache.remove_data(Mesh.polyhedra->edge_points);
		}
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<edge_array_painter, k3d::interface_list<k3d::gl::imesh_painter > > factory(
				k3d::uuid(0x7fca3f65, 0x531b4db6, 0xa11dac8a, 0x0b9f17b1),
		"VBOEdgePainter",
		_("Renders mesh edges"),
		"Development",
		k3d::iplugin_factory::EXPERIMENTAL);

		return factory;
	}
	
private:
	painter_cache<boost::shared_ptr<const k3d::dev::mesh::points_t>, vbo>& m_points_cache;
	painter_cache<boost::shared_ptr<const k3d::dev::mesh::indices_t>, vbo>& m_edges_cache;
	painter_cache<boost::shared_ptr<const k3d::dev::mesh::indices_t>, selection_records_t>& m_selection_cache;
	painter_cache<boost::shared_ptr<const k3d::dev::mesh::points_t>, k3d::hint::mesh_geometry_changed_t>& m_hint_cache;
};

	/////////////////////////////////////////////////////////////////////////////
// edge_array_painter_factory

	k3d::iplugin_factory& edge_array_painter_factory()
	{
		return edge_array_painter::get_factory();
	}

} // namespace libk3ddevelopment

