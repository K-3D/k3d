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
// face_array_painter

template<class face_t>
class face_array_painter :
	public colored_selection_painter
{
	typedef colored_selection_painter base;
public:
	face_array_painter(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		// overrride default colors to differ between selected/unselected meshes and to contrast edges and faces
		base(Factory, Document, k3d::color(0.2,0.2,0.2), k3d::color(0.6,0.6,0.6)),
		m_points_cache(painter_cache<boost::shared_ptr<const k3d::dev::mesh::points_t>, vbo>::instance(Document)),
		m_faces_cache(painter_cache<boost::shared_ptr<const k3d::dev::mesh::indices_t>, face_t>::instance(Document)),
		m_selection_cache(painter_cache<boost::shared_ptr<const k3d::dev::mesh::indices_t>, selection_records_t>::instance(Document)),
		m_hint_cache(painter_cache<boost::shared_ptr<const k3d::dev::mesh::points_t>, k3d::hint::mesh_geometry_changed_t>::instance(Document))
	{
	}
	
	~face_array_painter()
	{
		m_points_cache.remove_painter(this);
		m_faces_cache.remove_painter(this);
		m_selection_cache.remove_painter(this);
	}
	
	/// Updates the vertex buffer object.
	void update_buffer(const k3d::dev::mesh& Mesh)
	{
		if(!Mesh.polyhedra)
			return;
		if(!Mesh.polyhedra->face_first_loops)
			return;
		if(!Mesh.polyhedra->face_selection)
			return;
		if(!Mesh.polyhedra->loop_first_edges)
			return;
		if(!Mesh.polyhedra->edge_points)
			return;
		if(!Mesh.polyhedra->clockwise_edges)
			return;
		if(!Mesh.points)
			return;
		
		// Fill vertex buffer and update normals if needed
		create_point_vbo(Mesh.points, m_points_cache);
		k3d::hint::mesh_geometry_changed_t* changed_hint = m_hint_cache.get_data(Mesh.points);
		if (changed_hint)
		{
			update_point_vbo(Mesh.points, m_points_cache, changed_hint->changed_points);
			// Update normals
			face_t* face_indices = m_faces_cache.get_data(Mesh.polyhedra->face_first_loops);
			if (face_indices)
			{
				face_indices->update(*Mesh.points, *Mesh.polyhedra, changed_hint->changed_points);
			}
			m_hint_cache.remove_data(Mesh.points);
		}
		
		// Create face index buffers
		face* face_indices = m_faces_cache.get_data(Mesh.polyhedra->face_first_loops);
		if (!face_indices)
		{
			face_indices = m_faces_cache.create_data(Mesh.polyhedra->face_first_loops);
			face_indices->convert(*Mesh.points, *Mesh.polyhedra);
		}
		
		// Create selection
		selection_records_t* selection = m_selection_cache.get_data(Mesh.polyhedra->face_first_loops);
		if (!selection)
		{
			selection = m_selection_cache.create_data(Mesh.polyhedra->face_first_loops);
			array_to_selection(*Mesh.polyhedra->face_selection, *selection);
		}
	}
	
	void on_paint_mesh(const k3d::dev::mesh& Mesh, const k3d::gl::painter_render_state& RenderState)
	{
		return_if_fail(k3d::gl::extension::query_vbo());

		if(!validate_polyhedra(Mesh))
			return;
			
		if (k3d::dev::is_sds(Mesh))
			return;

		update_buffer(Mesh);

		k3d::gl::store_attributes attributes;

		glFrontFace(GL_CW);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		k3d::gl::set(GL_CULL_FACE, RenderState.draw_two_sided);

		glEnable(GL_POLYGON_OFFSET_FILL);
		glPolygonOffset(1.0, 1.0);
		
		const k3d::color color = RenderState.node_selection ? selected_mesh_color() : unselected_mesh_color();
		const k3d::color selected_color = RenderState.show_component_selection ? selected_component_color() : color;
		
		face* faces = m_faces_cache.get_data(Mesh.polyhedra->face_first_loops);
		return_if_fail(faces);
		
		size_t face_count = Mesh.polyhedra->face_first_loops->size();
		selection_records_t* const face_selection = m_selection_cache.get_data(Mesh.polyhedra->face_first_loops); // obtain selection data
		return_if_fail(face_selection);

		glEnable(GL_LIGHTING);

		clean_vbo_state();

		// Draw smooth points
		vbo* const buffer = m_points_cache.get_data(Mesh.points);
		return_if_fail(glIsBuffer(*buffer));
		glBindBuffer(GL_ARRAY_BUFFER, *buffer);
		glVertexPointer(3, GL_DOUBLE, 0, 0);
		glEnableClientState(GL_VERTEX_ARRAY);
		if (!face_selection->empty())
		{
			size_t drawn_faces = 0;
			for (selection_records_t::const_iterator record = face_selection->begin(); record != face_selection->end() && record->begin < face_count; ++record)
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
			if (!face_selection->empty())
			{
				for (selection_records_t::const_iterator record = face_selection->begin(); record != face_selection->end() && record->begin < face_count; ++record)
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
	
	void on_select_mesh(const k3d::dev::mesh& Mesh, const k3d::gl::painter_render_state& RenderState, const k3d::gl::painter_selection_state& SelectionState)
	{
		return_if_fail(k3d::gl::extension::query_vbo());

		if(!validate_polyhedra(Mesh))
			return;
			
		if (k3d::dev::is_sds(Mesh))
			return;

		update_buffer(Mesh);

		k3d::gl::store_attributes attributes;
		
		glDisable(GL_LIGHTING);

		glFrontFace(GL_CW);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		k3d::gl::set(GL_CULL_FACE, RenderState.draw_two_sided);

		glEnable(GL_POLYGON_OFFSET_FILL);
		glPolygonOffset(1.0, 1.0);
		
		clean_vbo_state();
		
		vbo* const buffer = m_points_cache.get_data(Mesh.points);
		return_if_fail(glIsBuffer(*buffer));
		glBindBuffer(GL_ARRAY_BUFFER, *buffer);
		glVertexPointer(3, GL_DOUBLE, 0, 0);
		glEnableClientState(GL_VERTEX_ARRAY);
		
		face* faces = m_faces_cache.get_data(Mesh.polyhedra->face_first_loops);
		return_if_fail(faces);
		faces->draw_selection();
		
		clean_vbo_state();
	}
	
	void on_mesh_changed(const k3d::dev::mesh& Mesh, k3d::iunknown* Hint)
	{
		return_if_fail(k3d::gl::extension::query_vbo());

		if(!k3d::dev::validate_polyhedra(Mesh))
			return;
			
		if (k3d::dev::is_sds(Mesh))
			return;
		
		m_points_cache.register_painter(Mesh.points, this);
		m_faces_cache.register_painter(Mesh.polyhedra->face_first_loops, this);
		m_selection_cache.register_painter(Mesh.polyhedra->face_first_loops, this);
		
		k3d::hint::mesh_geometry_changed_t* changed_hint = dynamic_cast<k3d::hint::mesh_geometry_changed_t*>(Hint);
		k3d::hint::selection_changed_t* selection_hint = dynamic_cast<k3d::hint::selection_changed_t*>(Hint);
		if (selection_hint)
		{
			m_selection_cache.remove_data(Mesh.polyhedra->face_first_loops);
		}
		else if (changed_hint && !changed_hint->changed_points.empty())
		{
			k3d::hint::mesh_geometry_changed_t& stored_changed_hint = *(m_hint_cache.create_data(Mesh.points));
			stored_changed_hint = *changed_hint;
		}
		else
		{
			m_points_cache.remove_data(Mesh.points);
			m_faces_cache.remove_data(Mesh.polyhedra->face_first_loops);
			m_selection_cache.remove_data(Mesh.polyhedra->face_first_loops);
			m_hint_cache.remove_data(Mesh.points);
		}
	}
	
private:
	painter_cache<boost::shared_ptr<const k3d::dev::mesh::points_t>, vbo>& m_points_cache;
	painter_cache<boost::shared_ptr<const k3d::dev::mesh::indices_t>, face_t>& m_faces_cache;
	painter_cache<boost::shared_ptr<const k3d::dev::mesh::indices_t>, selection_records_t>& m_selection_cache;
	painter_cache<boost::shared_ptr<const k3d::dev::mesh::points_t>, k3d::hint::mesh_geometry_changed_t>& m_hint_cache;
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

} // namespace libk3ddevelopment

