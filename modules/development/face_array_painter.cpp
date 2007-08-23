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

/** \file Paint faces using VBOs
 */

#include <k3dsdk/array.h>
#include <k3dsdk/document_plugin_factory.h>
#include <k3dsdk/extension_gl.h>
#include <k3dsdk/gl.h>
#include <k3dsdk/hints.h>
#include <k3d-i18n-config.h>
#include <k3dsdk/imesh_painter_gl.h>
#include <k3dsdk/mesh_operations.h>
#include <k3dsdk/node.h>
#include <k3dsdk/painter_render_state_gl.h>
#include <k3dsdk/painter_selection_state_gl.h>
#include <k3dsdk/persistent.h>
#include <k3dsdk/selection.h>
#include <k3dsdk/utility_gl.h>

#include "colored_selection_painter_gl.h"
#include "vbo.h"

namespace module
{

namespace development
{

	/////////////////////////////////////////////////////////////////////////////
// face_array_painter

template<class face_t>
class face_array_painter :
	public colored_selection_painter,
	public k3d::hint::hint_processor
{
	typedef colored_selection_painter base;
public:
	face_array_painter(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		// overrride default colors to differ between selected/unselected meshes and to contrast edges and faces
		base(Factory, Document, k3d::color(0.2,0.2,0.2), k3d::color(0.6,0.6,0.6)),
		m_points_cache(k3d::painter_cache<boost::shared_ptr<const k3d::mesh::points_t>, point_vbo>::instance(Document)),
		m_faces_cache(k3d::painter_cache<boost::shared_ptr<const k3d::mesh::indices_t>, face_t>::instance(Document)),
		m_selection_cache(k3d::painter_cache<boost::shared_ptr<const k3d::mesh::indices_t>, face_selection>::instance(Document))
	{
	}
	
	~face_array_painter()
	{
		m_points_cache.remove_painter(this);
		m_faces_cache.remove_painter(this);
		m_selection_cache.remove_painter(this);
	}
	
	void on_paint_mesh(const k3d::mesh& Mesh, const k3d::gl::painter_render_state& RenderState)
	{
		return_if_fail(k3d::gl::extension::query_vbo());

		if(!validate_polyhedra(Mesh))
			return;
			
		if (k3d::is_sds(Mesh))
			return;

		k3d::gl::store_attributes attributes;

		glFrontFace(RenderState.inside_out ? GL_CCW : GL_CW);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		k3d::gl::set(GL_CULL_FACE, RenderState.draw_two_sided);

		glEnable(GL_POLYGON_OFFSET_FILL);
		glPolygonOffset(1.0, 1.0);
		
		const k3d::color color = RenderState.node_selection ? selected_mesh_color() : unselected_mesh_color();
		const k3d::color selected_color = RenderState.show_component_selection ? selected_component_color() : color;
		
		point_vbo* const point_buffer = m_points_cache.create_data(Mesh.points);
		face* faces = m_faces_cache.create_data(Mesh.polyhedra->face_first_loops);
		point_buffer->execute(Mesh);
		point_buffer->bind();
		
		face_selection* selected_faces = m_selection_cache.create_data(Mesh.polyhedra->face_first_loops);
		selected_faces->execute(Mesh);
		
		faces->execute(Mesh);
		
		size_t face_count = Mesh.polyhedra->face_first_loops->size();
		const selection_records_t& face_selection_records = selected_faces->records();

		glEnable(GL_LIGHTING);

		clean_vbo_state();

		if (!face_selection_records.empty())
		{
			size_t drawn_faces = 0;
			for (selection_records_t::const_iterator record = face_selection_records.begin(); record != face_selection_records.end() && record->begin < face_count; ++record)
			{ // color by selection
				k3d::gl::material(GL_FRONT_AND_BACK, GL_DIFFUSE, record->weight ? selected_color : color);
				size_t start = record->begin;
				size_t end = record->end;
				size_t debug_end = end > Mesh.polyhedra->face_first_loops->size() ? Mesh.polyhedra->face_first_loops->size() : end;
				drawn_faces += debug_end - start;
				end = end > face_count ? face_count : end;
				faces->draw_smooth_range(start, end-1);
			}
			if (drawn_faces != Mesh.polyhedra->face_first_loops->size())
				k3d::log() << warning << "Failed to draw " << Mesh.polyhedra->face_first_loops->size()-drawn_faces << " faces" << std::endl;
		}
		else
		{ // empty selection, everything has the same color
			k3d::gl::color3d(color);
			faces->draw_smooth_range(0, face_count-1);
		}

		// Draw flat points
		if (faces->bind_flat_points())
		{
			if (!face_selection_records.empty())
			{
				for (selection_records_t::const_iterator record = face_selection_records.begin(); record != face_selection_records.end() && record->begin < face_count; ++record)
				{ // color by selection
					k3d::gl::material(GL_FRONT_AND_BACK, GL_DIFFUSE, record->weight ? selected_color : color);
					size_t start = record->begin;
					size_t end = record->end;
					end = end > face_count ? face_count : end;
					faces->draw_flat_range(start, end-1);
				}
			}
			else
			{ // empty selection, everything has the same color
				k3d::gl::color3d(color);
				faces->draw_flat_range(0, face_count-1);
			}
		}
		
		clean_vbo_state();
	}
	
	void on_select_mesh(const k3d::mesh& Mesh, const k3d::gl::painter_render_state& RenderState, const k3d::gl::painter_selection_state& SelectionState)
	{
		return_if_fail(k3d::gl::extension::query_vbo());

		if(!validate_polyhedra(Mesh))
			return;
			
		if (k3d::is_sds(Mesh))
			return;
			
		if (!SelectionState.select_faces)
			return;

		k3d::gl::store_attributes attributes;
		
		glDisable(GL_LIGHTING);

		glFrontFace(RenderState.inside_out ? GL_CCW : GL_CW);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		k3d::gl::set(GL_CULL_FACE, RenderState.draw_two_sided);

		glEnable(GL_POLYGON_OFFSET_FILL);
		glPolygonOffset(1.0, 1.0);
		
		clean_vbo_state();
		
		point_vbo* const point_buffer = m_points_cache.create_data(Mesh.points);
		face* faces = m_faces_cache.create_data(Mesh.polyhedra->face_first_loops);
		point_buffer->execute(Mesh);
		point_buffer->bind();
		
		faces->execute(Mesh);
		faces->draw_selection();
		
		clean_vbo_state();
	}
	
	void on_mesh_changed(const k3d::mesh& Mesh, k3d::iunknown* Hint)
	{
		return_if_fail(k3d::gl::extension::query_vbo());

		if(!k3d::validate_polyhedra(Mesh))
			return;
			
		if (k3d::is_sds(Mesh))
			return;
		
		m_points_cache.register_painter(Mesh.points, this);
		m_faces_cache.register_painter(Mesh.polyhedra->face_first_loops, this);
		m_selection_cache.register_painter(Mesh.polyhedra->face_first_loops, this);
		
		process(Mesh, Hint);
	}
	
protected:

	///////
	// hint processor implementation
	///////
	
	virtual void on_geometry_changed(const k3d::mesh& Mesh, k3d::iunknown* Hint)
	{
		m_points_cache.create_data(Mesh.points)->schedule(Mesh, Hint);
		m_faces_cache.create_data(Mesh.polyhedra->face_first_loops)->schedule(Mesh, Hint);
	}
	
	virtual void on_selection_changed(const k3d::mesh& Mesh, k3d::iunknown* Hint)
	{
		m_selection_cache.create_data(Mesh.polyhedra->face_first_loops)->schedule(Mesh, Hint);
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
		m_faces_cache.switch_key(address_hint->old_face_first_loops_address, Mesh.polyhedra->face_first_loops);
		m_selection_cache.switch_key(address_hint->old_face_first_loops_address, Mesh.polyhedra->face_first_loops);
	}
	
	virtual void on_mesh_deleted(const k3d::mesh& Mesh, k3d::iunknown* Hint)
	{
		m_points_cache.remove_data(Mesh.points);
		m_faces_cache.remove_data(Mesh.polyhedra->face_first_loops);
		m_selection_cache.remove_data(Mesh.polyhedra->face_first_loops);
	}

private:
	k3d::painter_cache<boost::shared_ptr<const k3d::mesh::points_t>, point_vbo>& m_points_cache;
	k3d::painter_cache<boost::shared_ptr<const k3d::mesh::indices_t>, face_t>& m_faces_cache;
	k3d::painter_cache<boost::shared_ptr<const k3d::mesh::indices_t>, face_selection>& m_selection_cache;
};

class face_painter_edge_normals : public face_array_painter<edge_face>
{
public:
	face_painter_edge_normals(k3d::iplugin_factory& Factory, k3d::idocument& Document) : face_array_painter<edge_face>(Factory, Document) {}
	
	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<face_painter_edge_normals, k3d::interface_list<k3d::gl::imesh_painter > > factory(
				k3d::uuid(0x65d1b68f, 0x2ca14ecd, 0x93e67154, 0xdeb01ebb),
		"VBOFacePainterEdgeNormals",
		_("Renders mesh faces, determining flat or smooth shading based on edge sharpness"),
		"Development",
		k3d::iplugin_factory::EXPERIMENTAL);

		return factory;
	}
};

class face_painter_flat_normals : public face_array_painter<flat_face>
{
	public:
		face_painter_flat_normals(k3d::iplugin_factory& Factory, k3d::idocument& Document) : face_array_painter<flat_face>(Factory, Document) {}
	
		k3d::iplugin_factory& factory()
		{
			return get_factory();
		}

		static k3d::iplugin_factory& get_factory()
		{
			static k3d::document_plugin_factory<face_painter_flat_normals, k3d::interface_list<k3d::gl::imesh_painter > > factory(
					k3d::uuid(0x05ced6d5, 0x758c4be9, 0x9bfc4d3c, 0x4ecd19a0),
			"VBOFacePainterFlatNormals",
			_("Renders mesh faces, using flat shading"),
			"Development",
			k3d::iplugin_factory::EXPERIMENTAL);

			return factory;
		}
};

class face_painter_smooth_normals : public face_array_painter<smooth_face>
{
	public:
		face_painter_smooth_normals(k3d::iplugin_factory& Factory, k3d::idocument& Document) : face_array_painter<smooth_face>(Factory, Document) {}
	
		k3d::iplugin_factory& factory()
		{
			return get_factory();
		}

		static k3d::iplugin_factory& get_factory()
		{
			static k3d::document_plugin_factory<face_painter_smooth_normals, k3d::interface_list<k3d::gl::imesh_painter > > factory(
					k3d::uuid(0x7709114c, 0x663540ff, 0xa7481259, 0xa175e7c8),
			"VBOFacePainterSmoothNormals",
			_("Renders mesh faces, using smooth normals"),
			"Development",
			k3d::iplugin_factory::EXPERIMENTAL);

			return factory;
		}
};

	/////////////////////////////////////////////////////////////////////////////
// face_array_painter factories

	k3d::iplugin_factory& face_painter_edge_normals_factory()
	{
		return face_painter_edge_normals::get_factory();
	}
	
	k3d::iplugin_factory& face_painter_flat_normals_factory()
	{
		return face_painter_flat_normals::get_factory();
	}
	
	k3d::iplugin_factory& face_painter_smooth_normals_factory()
	{
		return face_painter_smooth_normals::get_factory();
	}

} // namespace development

} // namespace module


