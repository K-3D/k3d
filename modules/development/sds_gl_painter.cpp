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

#include <k3dsdk/document_plugin_factory.h>
#include <k3dsdk/extension_gl.h>
#include <k3dsdk/gl.h>
#include <k3dsdk/hints.h>
#include <k3d-i18n-config.h>
#include <k3dsdk/imesh_painter_gl.h>
#include <k3dsdk/measurement.h>
#include <k3dsdk/mesh_operations.h>
#include <k3dsdk/node.h>
#include <k3dsdk/painter_render_state_gl.h>
#include <k3dsdk/painter_selection_state_gl.h>
#include <k3dsdk/persistent.h>
#include <k3dsdk/selection.h>
#include <k3dsdk/utility_gl.h>
#include <subdivision_surface/k3d_sds_binding.h>

#include "vbo.h"
#include "colored_selection_painter_gl.h"

namespace module
{

namespace development
{

	/////////////////////////////////////////////////////////////////////////////
// sds_gl_face_painter

class sds_gl_face_painter :
	public colored_selection_painter,
	public k3d::hint::hint_processor
{
	typedef colored_selection_painter base;
	/// Defines the set of sds caches associated with this painter
	typedef std::set<boost::shared_ptr<const k3d::mesh::points_t> > sds_cache_set_t;

public:
	sds_gl_face_painter(k3d::iplugin_factory& Factory, k3d::idocument& Document, const k3d::color Unselected = k3d::color(0.2,0.2,0.2), const k3d::color Selected = k3d::color(0.6,0.6,0.6)) :
		base(Factory, Document, Unselected, Selected),
		m_sds_cache(k3d::painter_cache<boost::shared_ptr<const k3d::mesh::points_t>, sds_gl_cache>::instance(Document)),
		m_levels(init_owner(*this) + init_name("levels") + init_label(_("Levels")) + init_description(_("Number of SDS levels")) + init_value(2) + init_constraint(constraint::minimum(2L)) + init_step_increment(1) + init_units(typeid(k3d::measurement::scalar)))
	{
		m_levels.changed_signal().connect(sigc::mem_fun(*this, &sds_gl_face_painter::on_levels_changed));
	}
	
	~sds_gl_face_painter()
	{
		m_sds_cache.remove_painter(this);
	}
	
	void on_levels_changed(k3d::iunknown* Hint)
	{
		for (sds_cache_set_t::iterator points = m_sds_cache_set.begin(); points != m_sds_cache_set.end(); ++points) {
			sds_gl_cache* cache = m_sds_cache.get_data(*points);
			if (cache)
				cache->level_changed();
		}
		
		k3d::gl::redraw_all(document(), k3d::gl::irender_engine::ASYNCHRONOUS);
	}
	
	void on_paint_mesh(const k3d::mesh& Mesh, const k3d::gl::painter_render_state& RenderState)
	{
		if(!k3d::validate_polyhedra(Mesh))
			return;

		if (!k3d::is_sds(Mesh))
			return;
		
		sds_gl_cache* cache = m_sds_cache.get_data(Mesh.points);
		if (!cache)
		{
			cache = m_sds_cache.create_data(Mesh.points);
			cache->cache.set_input(&Mesh);
		}
		
		m_sds_cache_set.insert(Mesh.points);
		cache->register_property(&m_levels);
		
		cache->execute(Mesh);
		
		k3d::gl::store_attributes attributes;
		
		draw(cache->cache, RenderState);
	}
	
	void on_select_mesh(const k3d::mesh& Mesh, const k3d::gl::painter_render_state& RenderState, const k3d::gl::painter_selection_state& SelectionState)
	{
		if(!k3d::validate_polyhedra(Mesh))
			return;
			
		if (!k3d::is_sds(Mesh))
			return;
		
		sds_gl_cache* cache = m_sds_cache.get_data(Mesh.points);
		if (!cache)
		{
			cache = m_sds_cache.create_data(Mesh.points);
			cache->cache.set_input(&Mesh);
		}
		
		m_sds_cache_set.insert(Mesh.points);
		cache->register_property(&m_levels);
		
		cache->execute(Mesh);
		
		select(cache->cache, SelectionState);
	}
	
	void on_mesh_changed(const k3d::mesh& Mesh, k3d::iunknown* Hint)
	{
		if(!k3d::validate_polyhedra(Mesh))
			return;
			
		if (!k3d::is_sds(Mesh))
			return;

		process(Mesh, Hint);
		
		sds_gl_cache* cache = m_sds_cache.get_data(Mesh.points);
		if (cache)
			cache->schedule(Mesh, Hint);
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<sds_gl_face_painter, k3d::interface_list<k3d::gl::imesh_painter > > factory(
				k3d::uuid(0xf8578aba, 0x674bbc2d, 0x40622ea4, 0x9167eaf9),
		"SDSGLFacePainter",
		_("Renders mesh as SDS faces using OpenGL 1.1"),
		"Development",
		k3d::iplugin_factory::EXPERIMENTAL);

		return factory;
	}

protected:
	k3d::painter_cache<boost::shared_ptr<const k3d::mesh::points_t>, sds_gl_cache>& m_sds_cache;
	k3d_data(long, immutable_name, change_signal, with_undo, local_storage, with_constraint, measurement_property, with_serialization) m_levels;
	bool m_do_update;
	
	/// Hint processor implementation
	void on_geometry_changed(const k3d::mesh& Mesh, k3d::iunknown* Hint)
	{
		m_sds_cache.create_data(Mesh.points)->update = true;
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
	}
	
	virtual void on_mesh_deleted(const k3d::mesh& Mesh, k3d::iunknown* Hint)
	{
		m_sds_cache_set.erase(Mesh.points);
		m_sds_cache.remove_data(Mesh.points);
	}
	
	// override to choose drawing mode
	virtual void draw(k3d::sds::k3d_basic_opengl_sds_cache& Cache, const k3d::gl::painter_render_state& RenderState)
	{
		glFrontFace(RenderState.inside_out ? GL_CCW : GL_CW);
		glEnable(GL_CULL_FACE);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glEnable(GL_POLYGON_OFFSET_FILL);
		glPolygonOffset(1.0, 1.0);
		glEnable(GL_LIGHTING);
		glEnable(GL_COLOR_MATERIAL);
		glColorMaterial(GL_FRONT_AND_BACK, GL_DIFFUSE);
		if (!RenderState.node_selection)
			k3d::gl::color3d(unselected_mesh_color());
		
		Cache.draw_faces(m_levels.pipeline_value(), RenderState.node_selection);
	}
	
	// override to choose selection mode
	virtual void select(k3d::sds::k3d_basic_opengl_sds_cache& Cache, const k3d::gl::painter_selection_state& SelectionState)
	{
		if (!SelectionState.select_faces)
			return;
		Cache.select_faces(m_levels.pipeline_value());
	}
	
private:
	sds_cache_set_t m_sds_cache_set;
};

/////////////////////////////////////////////////////////////////////////////
// sds_gl_face_painter_factory

k3d::iplugin_factory& sds_gl_face_painter_factory()
{
	return sds_gl_face_painter::get_factory();
}
	
////////////
// sds_gl_edge_painter
////////////
	
class sds_gl_edge_painter : public sds_gl_face_painter
{
public:
	sds_gl_edge_painter(k3d::iplugin_factory& Factory, k3d::idocument& Document) : sds_gl_face_painter(Factory, Document, k3d::color(0,0,0), k3d::color(1,1,1))
	{
	}
	
	k3d::iplugin_factory& factory()
	{
		return get_factory();
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<sds_gl_edge_painter, k3d::interface_list<k3d::gl::imesh_painter > > factory(
				k3d::uuid(0x55238c22, 0xed466597, 0xb86c86a3, 0x0de46700),
		"SDSGLEdgePainter",
		_("Renders mesh as SDS patch borders using OpenGL 1.1"),
		"Development",
		k3d::iplugin_factory::EXPERIMENTAL);

		return factory;
	}
	
private:
	virtual void draw(k3d::sds::k3d_basic_opengl_sds_cache& Cache, const k3d::gl::painter_render_state& RenderState)
	{
		glDisable(GL_LIGHTING);
		k3d::gl::color3d(unselected_mesh_color());
		Cache.draw_borders(m_levels.pipeline_value(), RenderState.node_selection);
	}
	
	virtual void select(k3d::sds::k3d_basic_opengl_sds_cache& Cache, const k3d::gl::painter_selection_state& SelectionState)
	{
		if (!SelectionState.select_edges)
			return;
		Cache.select_borders(m_levels.pipeline_value());
	}
};

/////////////////////////////////////////////////////////////////////////////
// sds_gl_edge_painter_factory

k3d::iplugin_factory& sds_gl_edge_painter_factory()
{
	return sds_gl_edge_painter::get_factory();
}

////////////
// sds_gl_point_painter
////////////
	
class sds_gl_point_painter : public sds_gl_face_painter
{
public:
	sds_gl_point_painter(k3d::iplugin_factory& Factory, k3d::idocument& Document) : sds_gl_face_painter(Factory, Document, k3d::color(0,0,0), k3d::color(1,1,1))
	{
	}
	
	k3d::iplugin_factory& factory()
	{
		return get_factory();
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<sds_gl_point_painter, k3d::interface_list<k3d::gl::imesh_painter > > factory(
				k3d::uuid(0xc336f1ed, 0x0a4fab65, 0x7ceeb380, 0x4a67eca1),
		"SDSGLPointPainter",
		_("Renders mesh as SDS patch corners using OpenGL 1.1"),
		"Development",
		k3d::iplugin_factory::EXPERIMENTAL);

		return factory;
	}
	
private:
	virtual void draw(k3d::sds::k3d_basic_opengl_sds_cache& Cache, const k3d::gl::painter_render_state& RenderState)
	{
		glDisable(GL_LIGHTING);
		k3d::gl::color3d(unselected_mesh_color());
		Cache.draw_corners(m_levels.pipeline_value(), RenderState.node_selection);
	}
	
	virtual void select(k3d::sds::k3d_basic_opengl_sds_cache& Cache, const k3d::gl::painter_selection_state& SelectionState)
	{
		if (!SelectionState.select_points)
			return;
		Cache.select_corners(m_levels.pipeline_value());
	}
};

/////////////////////////////////////////////////////////////////////////////
// sds_gl_point_painter_factory

k3d::iplugin_factory& sds_gl_point_painter_factory()
{
	return sds_gl_point_painter::get_factory();
}

} // namespace development

} // namespace module

