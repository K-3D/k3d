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

/** \file SDS painters
 */

#include <k3d-i18n-config.h>
#include <k3dsdk/document_plugin_factory.h>
#include <k3dsdk/extension_gl.h>
#include <k3dsdk/gl.h>
#include <k3dsdk/hints.h>
#include <k3dsdk/imesh_painter_gl.h>
#include <k3dsdk/measurement.h>
#include <k3dsdk/mesh_operations.h>
#include <k3dsdk/node.h>
#include <k3dsdk/painter_cache.h>
#include <k3dsdk/painter_render_state_gl.h>
#include <k3dsdk/painter_selection_state_gl.h>
#include <k3dsdk/persistent.h>
#include <k3dsdk/selection.h>
#include <k3dsdk/subdivision_surface/k3d_sds_binding.h>
#include <k3dsdk/utility_gl.h>

#include "colored_selection_painter_gl.h"
#include "sds_cache.h"
#include "selection_cache.h"
#include "vbo.h"

namespace module
{

namespace development
{

	/////////////////////////////////////////////////////////////////////////////
// sds_vbo_painter

template <typename selection_t, typename vbo_t>
class sds_vbo_painter :
	public colored_selection_painter,
	public k3d::hint::hint_processor
{
	typedef colored_selection_painter base;
	/// Defines the set of sds caches associated with this painter
	typedef std::set<boost::shared_ptr<const k3d::mesh::points_t> > sds_cache_set_t;
	/// Cache key type that makes the number of levels part of the key
	typedef std::pair<k3d::uint_t, boost::shared_ptr<const k3d::mesh::points_t> > key_t;

public:
	sds_vbo_painter(k3d::iplugin_factory& Factory, k3d::idocument& Document, const k3d::color Unselected = k3d::color(0.2,0.2,0.2), const k3d::color Selected = k3d::color(0.6,0.6,0.6)) :
		base(Factory, Document, Unselected, Selected),
		m_sds_cache(k3d::painter_cache<boost::shared_ptr<const k3d::mesh::points_t>, sds_cache>::instance(Document)),
		m_levels(init_owner(*this) + init_name("levels") + init_label(_("Levels")) + init_description(_("Number of SDS levels")) + init_value(2) + init_constraint(constraint::minimum(2L)) + init_step_increment(1) + init_units(typeid(k3d::measurement::scalar))),
		m_selection_cache(k3d::painter_cache<boost::shared_ptr<const k3d::mesh::indices_t>, selection_t>::instance(Document)),
		m_vbo_cache(k3d::painter_cache<key_t, vbo_t>::instance(Document))
	{
		m_levels.changed_signal().connect(sigc::mem_fun(*this, &sds_vbo_painter<selection_t, vbo_t>::on_levels_changed));
		m_old_level = m_levels.pipeline_value();
	}
	
	virtual ~sds_vbo_painter()
	{
		m_sds_cache.remove_painter(this);
		m_selection_cache.remove_painter(this);
		m_vbo_cache.remove_painter(this);
	}
	
	void on_levels_changed(k3d::iunknown* Hint)
	{
		for (sds_cache_set_t::iterator points = m_sds_cache_set.begin(); points != m_sds_cache_set.end(); ++points) {
			sds_cache* cache = m_sds_cache.get_data(*points);
			if (cache)
				cache->level_changed();
			m_vbo_cache.remove_data(std::make_pair(m_old_level, *points));
		}
		m_vbo_cache.remove_painter(this);
		m_old_level = m_levels.pipeline_value();
		k3d::gl::redraw_all(document(), k3d::gl::irender_engine::ASYNCHRONOUS);
	}
	
	void on_paint_mesh(const k3d::mesh& Mesh, const k3d::gl::painter_render_state& RenderState)
	{
		if(!k3d::validate_polyhedra(Mesh))
			return;

		if (!k3d::is_sds(Mesh))
			return;
		
		sds_cache* cache = m_sds_cache.get_data(Mesh.points);
		if (!cache)
		{
			cache = m_sds_cache.create_data(Mesh.points);
			cache->cache.set_input(&Mesh);
		}
		
		m_sds_cache_set.insert(Mesh.points);
		cache->register_property(&m_levels);
		cache->execute(Mesh);
		
		selection_t* selection = m_selection_cache.create_data(Mesh.polyhedra->face_first_loops);
		selection->execute(Mesh);
		
		vbo_t* vbo_cache = m_vbo_cache.create_data(std::make_pair(m_levels.pipeline_value(), Mesh.points));
		vbo_cache->update(Mesh, m_levels.pipeline_value(), cache->cache);
		
		k3d::gl::store_attributes attributes;
		
		clean_vbo_state();
		vbo_cache->bind();
		draw(*vbo_cache, *selection, RenderState);
		clean_vbo_state();
	}
	
	void on_select_mesh(const k3d::mesh& Mesh, const k3d::gl::painter_render_state& RenderState, const k3d::gl::painter_selection_state& SelectionState)
	{
		if(!k3d::validate_polyhedra(Mesh))
			return;
			
		if (!k3d::is_sds(Mesh))
			return;
		
		sds_cache* cache = m_sds_cache.get_data(Mesh.points);
		if (!cache)
		{
			cache = m_sds_cache.create_data(Mesh.points);
			cache->cache.set_input(&Mesh);
		}
		
		m_sds_cache_set.insert(Mesh.points);
		cache->register_property(&m_levels);
		cache->execute(Mesh);
		
		vbo_t* vbo_cache = m_vbo_cache.create_data(std::make_pair(m_levels.pipeline_value(), Mesh.points));
		vbo_cache->update(Mesh, m_levels.pipeline_value(), cache->cache);
		
		k3d::gl::store_attributes attributes;
		
		clean_vbo_state();
		vbo_cache->bind();
		select(*vbo_cache, SelectionState);
		clean_vbo_state();
	}
	
	void on_mesh_changed(const k3d::mesh& Mesh, k3d::iunknown* Hint)
	{
		if(!k3d::validate_polyhedra(Mesh))
			return;
			
		if (!k3d::is_sds(Mesh))
			return;

		process(Mesh, Hint);
		
		sds_cache* cache = m_sds_cache.get_data(Mesh.points);
		if (cache)
			cache->schedule(Mesh, Hint);
		
		m_sds_cache.register_painter(Mesh.points, this);
		m_selection_cache.register_painter(Mesh.polyhedra->face_first_loops, this);
		m_vbo_cache.register_painter(std::make_pair(m_levels.pipeline_value(), Mesh.points), this);
	}

protected:
	k3d::painter_cache<boost::shared_ptr<const k3d::mesh::points_t>, sds_cache>& m_sds_cache;
	k3d_data(long, immutable_name, change_signal, with_undo, local_storage, with_constraint, measurement_property, with_serialization) m_levels;
	
	/// Hint processor implementation
	void on_geometry_changed(const k3d::mesh& Mesh, k3d::iunknown* Hint)
	{
		m_sds_cache.create_data(Mesh.points)->update = true;
		k3d::hint::mesh_geometry_changed_t* geo_hint = dynamic_cast<k3d::hint::mesh_geometry_changed_t*>(Hint);
		if (geo_hint)
			m_sds_cache.create_data(Mesh.points)->indices = geo_hint->changed_points;
		m_vbo_cache.create_data(std::make_pair(m_levels.pipeline_value(), Mesh.points))->need_update = true;
	}
	
	void on_selection_changed(const k3d::mesh& Mesh, k3d::iunknown* Hint)
	{
		m_selection_cache.create_data(Mesh.polyhedra->face_first_loops)->schedule(Mesh, Hint);
		m_sds_cache.create_data(Mesh.points)->cache.clear_modified_faces();
	}
	
	void on_topology_changed(const k3d::mesh& Mesh, k3d::iunknown* Hint)
	{
		on_mesh_deleted(Mesh, Hint);
	}
	
	void on_address_changed(const k3d::mesh& Mesh, k3d::iunknown* Hint)
	{
		k3d::hint::mesh_address_changed_t* address_hint = dynamic_cast<k3d::hint::mesh_address_changed_t*>(Hint);
		return_if_fail(address_hint);
		m_sds_cache.switch_key(address_hint->old_points_address, Mesh.points);
		m_sds_cache_set.erase(address_hint->old_points_address);
		m_selection_cache.switch_key(address_hint->old_face_first_loops_address, Mesh.polyhedra->face_first_loops);
		k3d::uint_t levels = m_levels.pipeline_value();
		m_vbo_cache.switch_key(std::make_pair(levels,address_hint->old_points_address), std::make_pair(levels, Mesh.points));
	}
	
	virtual void on_mesh_deleted(const k3d::mesh& Mesh, k3d::iunknown* Hint)
	{
		m_sds_cache_set.erase(Mesh.points);
		m_sds_cache.remove_data(Mesh.points);
		m_selection_cache.remove_data(Mesh.polyhedra->face_first_loops);
		m_vbo_cache.remove_data(std::make_pair(m_levels.pipeline_value(), Mesh.points));
	}
	
	// override to choose drawing mode
	virtual void draw(vbo_t& Cache, selection_t& Selection, const k3d::gl::painter_render_state& RenderState) = 0;
	
	// override to choose selection mode
	virtual void select(vbo_t& Cache, const k3d::gl::painter_selection_state& SelectionState) = 0;
	
private:
	sds_cache_set_t m_sds_cache_set;
	k3d::painter_cache<boost::shared_ptr<const k3d::mesh::indices_t>, selection_t>& m_selection_cache;
	k3d::painter_cache<key_t, vbo_t>& m_vbo_cache;
	k3d::uint_t m_old_level;
};

////////////////////////////////:
// sds_vbo_face_painter

class sds_vbo_face_painter : public sds_vbo_painter<face_selection, sds_face_vbo>
{
	typedef sds_vbo_painter<face_selection, sds_face_vbo> base;
	typedef face_selection selection_t;
	typedef sds_face_vbo vbo_t;
public:
	sds_vbo_face_painter(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document, k3d::color(0.2,0.2,0.2),k3d::color(0.6,0.6,0.6))
	{}
	
	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<sds_vbo_face_painter, k3d::interface_list<k3d::gl::imesh_painter > > factory(
				k3d::uuid(0x6db5cbd0, 0x0c4c42eb, 0x81142404, 0x3c25889e),
		"SDSVBOFacePainter",
		_("Renders mesh as SDS faces using OpenGL VBOs"),
		"Development",
		k3d::iplugin_factory::EXPERIMENTAL);

		return factory;
	}
protected:
	virtual void draw(vbo_t& Cache, selection_t& Selection, const k3d::gl::painter_render_state& RenderState)
	{
		glFrontFace(RenderState.inside_out ? GL_CCW : GL_CW);
		glEnable(GL_CULL_FACE);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glEnable(GL_POLYGON_OFFSET_FILL);
		glPolygonOffset(1.0, 1.0);
		glEnable(GL_LIGHTING);
		
		const color_t color = RenderState.node_selection ? selected_mesh_color() : unselected_mesh_color(RenderState.parent_selection);
		const color_t selected_color = RenderState.show_component_selection ? selected_component_color() : color;

		k3d::uint_t face_count = Cache.face_starts.size();
		const selection_records_t& face_selection_records = Selection.records();
		if (!face_selection_records.empty())
		{
			for (selection_records_t::const_iterator record = face_selection_records.begin(); record != face_selection_records.end() && record->begin < face_count; ++record)
			{ // color by selection
				const color_t& face_color = record->weight ? selected_color : color;
				k3d::gl::material(GL_FRONT_AND_BACK, GL_DIFFUSE, k3d::color(face_color.red, face_color.green, face_color.blue), face_color.alpha);
				k3d::uint_t start = record->begin;
				k3d::uint_t end = record->end;
				end = end > face_count ? face_count : end;
				k3d::uint_t start_index = Cache.face_starts[start];
				k3d::uint_t end_index = end == face_count ? Cache.index_size : Cache.face_starts[end];
				glDrawElements(GL_QUADS, end_index-start_index, GL_UNSIGNED_INT, static_cast<GLuint*>(0) + start_index);
			}
		}
		else
		{ // empty selection, everything has the same color
			color4d(color);
			glDrawElements(GL_QUADS, Cache.index_size, GL_UNSIGNED_INT, static_cast<GLuint*>(0));
		}
	}
	virtual void select(vbo_t& Cache, const k3d::gl::painter_selection_state& SelectionState)
	{
		if (!SelectionState.select_faces)
			return;
		
		k3d::gl::store_attributes attributes;
				
		glDisable(GL_LIGHTING);

		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		glEnable(GL_POLYGON_OFFSET_FILL);
		glPolygonOffset(1.0, 1.0);
		
		for (k3d::uint_t face = 0; face != Cache.face_starts.size(); ++face)
		{
			k3d::gl::push_selection_token(k3d::selection::ABSOLUTE_FACE, face);
			
			k3d::uint_t start_index = Cache.face_starts[face];
			k3d::uint_t end_index = face == (Cache.face_starts.size()-1) ? Cache.index_size : Cache.face_starts[face+1]; 
			glDrawElements(GL_QUADS, end_index-start_index, GL_UNSIGNED_INT, static_cast<GLuint*>(0) + start_index);
			
			k3d::gl::pop_selection_token(); // ABSOLUTE_FACE
		}
	}
};

k3d::iplugin_factory& sds_vbo_face_painter_factory()
{
	return sds_vbo_face_painter::get_factory();
}
	
////////////
// sds_vbo_edge_painter
////////////
	
class sds_vbo_edge_painter : public sds_vbo_painter<edge_selection, sds_edge_vbo>
{
	typedef sds_vbo_painter<edge_selection, sds_edge_vbo> base;
	typedef edge_selection selection_t;
	typedef sds_edge_vbo vbo_t;
public:
	sds_vbo_edge_painter(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document, k3d::color(0.0,0.0,0.0),k3d::color(1.0,1.0,1.0))
	{}
	
	k3d::iplugin_factory& factory()
	{
		return get_factory();
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<sds_vbo_edge_painter, k3d::interface_list<k3d::gl::imesh_painter > > factory(
				k3d::uuid(0x4282a7b2, 0xb34a138f, 0xcda9df91, 0xa9c9c8ab),
		"SDSVBOEdgePainter",
		_("Renders mesh as SDS patch borders using OpenGL VBOs"),
		"Development",
		k3d::iplugin_factory::EXPERIMENTAL);

		return factory;
	}
	
private:
	virtual void draw(vbo_t& Cache, selection_t& Selection, const k3d::gl::painter_render_state& RenderState)
	{
		glDisable(GL_LIGHTING);
		
		const color_t color = RenderState.node_selection ? selected_mesh_color() : unselected_mesh_color(RenderState.parent_selection);
		const color_t selected_color = RenderState.show_component_selection ? selected_component_color() : color;
		
		k3d::uint_t edge_count = Cache.edge_starts.size();
		
		const selection_records_t& edge_selection_records = Selection.records();
		if (!edge_selection_records.empty())
		{
			for (selection_records_t::const_iterator record = edge_selection_records.begin(); record != edge_selection_records.end() && record->begin < edge_count; ++record)
			{ // color by selection
				color4d(record->weight ? selected_color : color);
				k3d::uint_t start = record->begin;
				k3d::uint_t end = record->end;
				end = end > edge_count ? edge_count : end;
				k3d::uint_t start_index = Cache.edge_starts[start];
				k3d::uint_t end_index = end == edge_count ? Cache.index_size : Cache.edge_starts[end];
				glDrawArrays(GL_LINES, start_index, end_index-start_index);
			}
		}
		else
		{ // empty selection, everything has the same color
			color4d(color);
			glDrawArrays(GL_LINES, 0, edge_count);
		}
	}
	
	virtual void select(vbo_t& Cache, const k3d::gl::painter_selection_state& SelectionState)
	{
		if (!SelectionState.select_edges)
					return;

		glDisable(GL_LIGHTING);
		
		k3d::uint_t edge_count = Cache.edge_starts.size();
		
		for (k3d::uint_t edge = 0; edge != edge_count; ++edge)
		{
			k3d::gl::push_selection_token(k3d::selection::ABSOLUTE_SPLIT_EDGE, edge);
			
			k3d::uint_t start_index = Cache.edge_starts[edge];
			k3d::uint_t end_index = edge == (edge_count-1) ? Cache.index_size : Cache.edge_starts[edge+1];
			glDrawArrays(GL_LINES, start_index, end_index-start_index);
			
			k3d::gl::pop_selection_token(); // ABSOLUTE_SPLIT_EDGE
		}
	}
};

/////////////////////////////////////////////////////////////////////////////
// sds_vbo_edge_painter_factory

k3d::iplugin_factory& sds_vbo_edge_painter_factory()
{
	return sds_vbo_edge_painter::get_factory();
}

////////////
// sds_vbo_point_painter
////////////
	
class sds_vbo_point_painter : public sds_vbo_painter<point_selection, sds_point_vbo>
{
	typedef sds_vbo_painter<point_selection, sds_point_vbo> base;
	typedef point_selection selection_t;
	typedef sds_point_vbo vbo_t;
public:
	sds_vbo_point_painter(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document, k3d::color(0.0,0.0,0.0),k3d::color(1.0,1.0,1.0))
	{}
	
	k3d::iplugin_factory& factory()
	{
		return get_factory();
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<sds_vbo_point_painter, k3d::interface_list<k3d::gl::imesh_painter > > factory(
				k3d::uuid(0x44621bcb, 0x404a76ea, 0x1a256ebd, 0xb119ad07),
		"SDSVBOPointPainter",
		_("Renders mesh as SDS patch corners using OpenGL VBOs"),
		"Development",
		k3d::iplugin_factory::EXPERIMENTAL);

		return factory;
	}
	
private:
	virtual void draw(vbo_t& Cache, selection_t& Selection, const k3d::gl::painter_render_state& RenderState)
	{
		glDisable(GL_LIGHTING);
		
		k3d::uint_t point_count = Cache.index_size;
		
		const color_t color = RenderState.node_selection ? selected_mesh_color() : unselected_mesh_color(RenderState.parent_selection);
		const color_t selected_color = RenderState.show_component_selection ? selected_component_color() : color;
		
		const selection_records_t& point_selection_records = Selection.records();
		if (!point_selection_records.empty())
		{
			for (selection_records_t::const_iterator record = point_selection_records.begin(); record != point_selection_records.end() && record->begin < point_count; ++record)
			{ // color by selection
				color4d(record->weight ? selected_color : color);
				k3d::uint_t start = record->begin;
				k3d::uint_t end = record->end;
				end = end > point_count ? point_count : end;
				glDrawArrays(GL_POINTS, start, end-start);
			}
		}
		else
		{ // empty selection, everything has the same color
			color4d(color);
			glDrawArrays(GL_POINTS, 0, point_count);
		}
	}
	
	virtual void select(vbo_t& Cache, const k3d::gl::painter_selection_state& SelectionState)
	{
		if (!SelectionState.select_points)
			return;
		glDisable(GL_LIGHTING);
		
		k3d::uint_t point_count = Cache.index_size;
		
		for (k3d::uint_t point = 0; point != point_count; ++point)
		{
			k3d::gl::push_selection_token(k3d::selection::ABSOLUTE_POINT, point);
			glDrawArrays(GL_POINTS, point, 1);
			k3d::gl::pop_selection_token();
		}
	}
};

/////////////////////////////////////////////////////////////////////////////
// sds_vbo_point_painter_factory

k3d::iplugin_factory& sds_vbo_point_painter_factory()
{
	return sds_vbo_point_painter::get_factory();
}

} // namespace development

} // namespace module

