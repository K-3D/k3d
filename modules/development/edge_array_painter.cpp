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
#include <k3dsdk/mesh.h>
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
	public colored_selection_painter,
	public hint_processor
{
	typedef colored_selection_painter base;
public:
	edge_array_painter(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_points_cache(painter_cache<boost::shared_ptr<const k3d::mesh::points_t>, point_vbo>::instance(Document)),
		m_edges_cache(painter_cache<boost::shared_ptr<const k3d::mesh::indices_t>, edge_vbo>::instance(Document)),
		m_selection_cache(painter_cache<boost::shared_ptr<const k3d::mesh::indices_t>, edge_selection>::instance(Document))
	{
	}
	
	~edge_array_painter()
	{
		m_points_cache.remove_painter(this);
		m_edges_cache.remove_painter(this);
		m_selection_cache.remove_painter(this);
	}
	
	void on_paint_mesh(const k3d::mesh& Mesh, const k3d::gl::painter_render_state& RenderState)
	{
		return_if_fail(k3d::gl::extension::query_vbo());

		if(!k3d::validate_polyhedra(Mesh))
			return;
		
		const k3d::color color = RenderState.node_selection ? selected_mesh_color() : unselected_mesh_color();
		const k3d::color selected_color = RenderState.show_component_selection ? selected_component_color() : color;

		k3d::gl::store_attributes attributes;
		glDisable(GL_LIGHTING);
		
		clean_vbo_state();
		
		point_vbo* const point_buffer = m_points_cache.create_data(Mesh.points);
		edge_vbo* const edge_buffer = m_edges_cache.create_data(Mesh.polyhedra->edge_points);
		
		point_buffer->execute(Mesh);
		point_buffer->bind();
		
		edge_buffer->execute(Mesh);
		edge_buffer->bind();
		
		edge_selection* selected_edges = m_selection_cache.create_data(Mesh.polyhedra->edge_points);
		selected_edges->execute(Mesh);
		
		size_t edge_count = Mesh.polyhedra->edge_points->size();
		const selection_records_t& edge_selection_records = selected_edges->records();
		if (!edge_selection_records.empty())
		{
			for (selection_records_t::const_iterator record = edge_selection_records.begin(); record != edge_selection_records.end(); ++record)
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
	
	void on_select_mesh(const k3d::mesh& Mesh, const k3d::gl::painter_render_state& RenderState, const k3d::gl::painter_selection_state& SelectionState)
	{
		return_if_fail(k3d::gl::extension::query_vbo());

		if(!k3d::validate_polyhedra(Mesh))
			return;
			
		if (!SelectionState.select_edges)
			return;
		
		k3d::gl::store_attributes attributes;
		glDisable(GL_LIGHTING);

		clean_vbo_state();
		
		point_vbo* const point_buffer = m_points_cache.create_data(Mesh.points);
		edge_vbo* const edge_buffer = m_edges_cache.create_data(Mesh.polyhedra->edge_points);
		
		point_buffer->execute(Mesh);
		point_buffer->bind();
		
		edge_buffer->execute(Mesh);
		edge_buffer->bind();
		
		const size_t edge_count = Mesh.polyhedra->edge_points->size();
		for(size_t edge = 0; edge < edge_count; ++edge)
		{
			k3d::gl::push_selection_token(k3d::selection::ABSOLUTE_SPLIT_EDGE, edge);

			glDrawElements(GL_LINES, 2, GL_UNSIGNED_INT, static_cast<GLuint*>(0) + 2*edge);

			k3d::gl::pop_selection_token();
		}
		
		clean_vbo_state();
	}
	
	void on_mesh_changed(const k3d::mesh& Mesh, k3d::iunknown* Hint)
	{
		return_if_fail(k3d::gl::extension::query_vbo());

		if (!k3d::validate_polyhedra(Mesh))
			return;
		
		m_points_cache.register_painter(Mesh.points, this);
		m_edges_cache.register_painter(Mesh.polyhedra->edge_points, this);
		m_selection_cache.register_painter(Mesh.polyhedra->edge_points, this);
		
		process(Mesh, Hint);
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
	
protected:

	///////
	// hint processor implementation
	///////
	
	virtual void on_geometry_changed(const k3d::mesh& Mesh, k3d::iunknown* Hint)
	{
		m_points_cache.create_data(Mesh.points)->schedule(Mesh, Hint);
	}
	
	virtual void on_selection_changed(const k3d::mesh& Mesh, k3d::iunknown* Hint)
	{
		m_selection_cache.create_data(Mesh.polyhedra->edge_points)->schedule(Mesh, Hint);
	}
	
	virtual void on_topology_changed(const k3d::mesh& Mesh, k3d::iunknown* Hint)
	{
		on_mesh_deleted(Mesh, Hint);
	}
	
	virtual void on_address_changed(const k3d::mesh& Mesh, k3d::iunknown* Hint)
	{
		k3d::hint::mesh_address_changed_t* address_hint = dynamic_cast<k3d::hint::mesh_address_changed_t*>(Hint);
		return_if_fail(address_hint);
		m_points_cache.switch_key(address_hint->old_points_address, Mesh.points);
		m_edges_cache.switch_key(address_hint->old_edge_points_address, Mesh.polyhedra->edge_points);
		m_selection_cache.switch_key(address_hint->old_edge_points_address, Mesh.polyhedra->edge_points);
	}
	
	virtual void on_mesh_deleted(const k3d::mesh& Mesh, k3d::iunknown* Hint)
	{
		m_points_cache.remove_data(Mesh.points);
		m_edges_cache.remove_data(Mesh.polyhedra->edge_points);
		m_selection_cache.remove_data(Mesh.polyhedra->edge_points);
	}
	
private:
	painter_cache<boost::shared_ptr<const k3d::mesh::points_t>, point_vbo>& m_points_cache;
	painter_cache<boost::shared_ptr<const k3d::mesh::indices_t>, edge_vbo>& m_edges_cache;
	painter_cache<boost::shared_ptr<const k3d::mesh::indices_t>, edge_selection>& m_selection_cache;
};

	/////////////////////////////////////////////////////////////////////////////
// edge_array_painter_factory

	k3d::iplugin_factory& edge_array_painter_factory()
	{
		return edge_array_painter::get_factory();
	}

} // namespace libk3ddevelopment

