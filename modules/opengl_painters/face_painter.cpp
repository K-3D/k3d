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

/** \file Painter that triangulates its input first
 * 	\author Bart Janssens (bart.janssens@lid.kviv.be)
 */

#include <k3dsdk/array.h>
#include <k3dsdk/document_plugin_factory.h>
#include <k3dsdk/extension_gl.h>
#include <k3dsdk/gl.h>
#include <k3dsdk/hints.h>
#include <k3d-i18n-config.h>
#include <k3dsdk/imesh_painter_gl.h>
#include <k3dsdk/mesh_operations.h>
#include <k3dsdk/named_array_operations.h>
#include <k3dsdk/node.h>
#include <k3dsdk/painter_render_state_gl.h>
#include <k3dsdk/painter_selection_state_gl.h>
#include <k3dsdk/persistent.h>
#include <k3dsdk/selection.h>
#include <k3dsdk/utility_gl.h>

#include "cached_triangulation.h"
#include "colored_selection_painter_gl.h"

namespace module
{

namespace opengl
{

namespace painters
{

/////////////////////////////////////////////////////////////////////////////
// face_painter

class face_painter :
	public colored_selection_painter,
	public k3d::hint::hint_processor
{
	typedef colored_selection_painter base;

public:
	face_painter(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document, k3d::color(0.2,0.2,0.2), k3d::color(0.6,0.6,0.6)),
		m_triangle_cache(k3d::painter_cache<cached_triangulation>::instance(Document))
	{
	}
	
	~face_painter()
	{
		m_triangle_cache.remove_painter(this);
	}

	void on_paint_mesh(const k3d::mesh& Mesh, const k3d::gl::painter_render_state& RenderState)
	{
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
		glEnable(GL_COLOR_MATERIAL);
		glColorMaterial(GL_FRONT_AND_BACK, GL_DIFFUSE);
		
		enable_blending();
		
		const color_t color = RenderState.node_selection ? selected_mesh_color() : unselected_mesh_color(RenderState.parent_selection);
		const color_t selected_color = RenderState.show_component_selection ? selected_component_color() : color;
		
		cached_triangulation* triangles = m_triangle_cache.create_data(&Mesh); 
		triangles->execute(Mesh); 
		
		size_t face_count = Mesh.polyhedra->face_first_loops->size();
		
		glEnable(GL_LIGHTING);
		
		glEnableClientState(GL_VERTEX_ARRAY);
		glVertexPointer(3, GL_DOUBLE, 0, &(triangles->points().at(0)));
		
		k3d::typed_array<k3d::normal3> normals(triangles->points().size(), k3d::normal3(0, 0, 1));
		cached_triangulation::index_vectors_t& face_points = triangles->face_points();
		const k3d::mesh::normals_t* calculated_normals = k3d::get_array<k3d::mesh::normals_t>(Mesh.polyhedra->uniform_data, "N");
		if (calculated_normals)
		{
			for (k3d::uint_t face = 0; face != face_points.size(); ++face)
			{
				k3d::mesh::indices_t& corners = face_points[face];
				for (k3d::uint_t i = 0; i != corners.size(); ++i)
				{ 
					normals[corners[i]] = calculated_normals->at(face);
				}
			}
		}
		else
		{
			const k3d::mesh::indices_t& edge_points = *Mesh.polyhedra->edge_points;
			const k3d::mesh::indices_t& clockwise_edges = *Mesh.polyhedra->clockwise_edges;
			const k3d::mesh::indices_t& loop_first_edges = *Mesh.polyhedra->loop_first_edges;
			const k3d::mesh::indices_t& face_first_loops = *Mesh.polyhedra->face_first_loops;
			const k3d::mesh::indices_t& face_loop_counts = *Mesh.polyhedra->face_loop_counts;
			const k3d::mesh::points_t& mesh_points = *Mesh.points;
			for (k3d::uint_t face = 0; face != face_points.size(); ++face)
			{
				k3d::normal3 n = k3d::normal(edge_points, clockwise_edges, mesh_points, loop_first_edges[face_first_loops[face]]);
				k3d::mesh::indices_t& corners = face_points[face];
				for (k3d::uint_t i = 0; i != corners.size(); ++i)
				{ 
					normals[corners[i]] = n; 
				}
			}
		}
		
		glEnableClientState(GL_NORMAL_ARRAY);
		glNormalPointer(GL_DOUBLE, 0, &normals[0]);
		
		std::vector<color_t> colors(triangles->points().size(), color);
		const k3d::mesh::selection_t& face_selection = *Mesh.polyhedra->face_selection; 
		for (k3d::uint_t face = 0; face != face_points.size(); ++face)
		{
			if (!face_selection[face])
				continue;
			k3d::mesh::indices_t& corners = face_points[face];
			for (k3d::uint_t i = 0; i != corners.size(); ++i)
			{ 
				colors[corners[i]] = selected_color;
			}
		}
		
		glEnableClientState(GL_COLOR_ARRAY);
		glColorPointer(4, GL_DOUBLE, 0, &colors[0]);
		
		glDrawElements(GL_TRIANGLES, triangles->indices().size(), GL_UNSIGNED_INT, &(triangles->indices().at(0)));
		
		glDisableClientState(GL_VERTEX_ARRAY);
		glDisableClientState(GL_COLOR_ARRAY);
		glDisableClientState(GL_SECONDARY_COLOR_ARRAY);
		glDisableClientState(GL_INDEX_ARRAY);
		glDisableClientState(GL_NORMAL_ARRAY);
		glDisableClientState(GL_FOG_COORDINATE_ARRAY);
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
		glDisableClientState(GL_EDGE_FLAG_ARRAY);
		
		disable_blending();
	}
	
	void on_select_mesh(const k3d::mesh& Mesh, const k3d::gl::painter_render_state& RenderState, const k3d::gl::painter_selection_state& SelectionState)
	{
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
		
		cached_triangulation* triangles = m_triangle_cache.create_data(&Mesh); 
		triangles->execute(Mesh);
		
		glEnableClientState(GL_VERTEX_ARRAY);
		glVertexPointer(3, GL_DOUBLE, 0, &(triangles->points().at(0)));
		
		k3d::mesh::indices_t& face_starts = triangles->face_starts();
		if (face_starts.empty())
			return;
		cached_triangulation::indices_t& indices = triangles->indices();
		
		size_t face_count = Mesh.polyhedra->face_first_loops->size();
		for(size_t face = 0; face != face_count; ++face)
		{
			k3d::gl::push_selection_token(k3d::selection::ABSOLUTE_FACE, face);

			k3d::uint_t startindex = face_starts[face];
			k3d::uint_t endindex = face+1 == (face_starts.size()) ? indices.size() : face_starts[face+1];
			glDrawElements(GL_TRIANGLES, endindex - startindex, GL_UNSIGNED_INT, &indices[startindex]);

			k3d::gl::pop_selection_token(); // ABSOLUTE_FACE
		}
	}
	
	void on_mesh_changed(const k3d::mesh& Mesh, k3d::iunknown* Hint)
	{
		if(!k3d::validate_polyhedra(Mesh))
			return;
			
		if (k3d::is_sds(Mesh))
			return;
		
		m_triangle_cache.register_painter(&Mesh, this);
		
		process(Mesh, Hint);
	}
	
	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<face_painter, k3d::interface_list<k3d::gl::imesh_painter > > factory(
			k3d::uuid(0xd4abf63c, 0x2242c17e, 0x2afcb18d, 0x0a8ebdd5),
			"OpenGLFacePainter",
			_("Renders mesh faces, after trianglulating them (OpenGL 1.1)"),
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
		cached_triangulation* triangles = m_triangle_cache.create_data(&Mesh); 
		triangles->schedule(Mesh, Hint);
	}
	
	virtual void on_topology_changed(const k3d::mesh& Mesh, k3d::iunknown* Hint)
	{
		on_mesh_deleted(Mesh, Hint);
	}
	
	virtual void on_mesh_deleted(const k3d::mesh& Mesh, k3d::iunknown* Hint)
	{
		m_triangle_cache.remove_data(&Mesh);
	}
	
private:
	k3d::painter_cache<cached_triangulation>& m_triangle_cache;
};

/////////////////////////////////////////////////////////////////////////////
// face_painter_factory

k3d::iplugin_factory& face_painter_factory()
{
	return face_painter::get_factory();
}

} // namespace opengl

} // namespace painters

} // namespace module
