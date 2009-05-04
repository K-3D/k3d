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

#include "cached_triangulation.h"
#include "colored_selection_painter_gl.h"
#include "normal_cache.h"

#include <k3d-i18n-config.h>
#include <k3dsdk/array.h>
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
#include <k3dsdk/texture3.h>
#include <k3dsdk/utility_gl.h>

#include <boost/scoped_ptr.hpp>

#include "utility.h"

namespace module
{

namespace opengl
{

namespace painters
{

/////////////////////////////////////////////////////////////////////////////
// textured_face_painter

class textured_face_painter :
	public colored_selection_painter
{
	typedef colored_selection_painter base;

public:
	textured_face_painter(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document, k3d::color(0.2,0.2,0.2), k3d::color(0.6,0.6,0.6)),
		m_bitmap(init_owner(*this) + init_name("bitmap") + init_label(_("Bitmap")) + init_description(_("Bitmap")) + init_value<k3d::bitmap*>(0)),
		m_texture_name(0)
	{
		m_bitmap.changed_signal().connect(sigc::mem_fun(*this, &textured_face_painter::bitmap_changed));
	}

	~textured_face_painter()
	{
		if(m_texture_name)
			glDeleteTextures(1, &m_texture_name);
	}

	void bitmap_changed(k3d::ihint* Hint)
	{
		if(m_texture_name)
		{
			glDeleteTextures(1, &m_texture_name);
			m_texture_name = 0;
		}

		async_redraw(Hint);
	}

	void async_redraw(k3d::ihint*)
	{
		k3d::gl::redraw_all(document(), k3d::gl::irender_viewport::ASYNCHRONOUS);
	}

	void on_paint_mesh(const k3d::mesh& Mesh, const k3d::gl::painter_render_state& RenderState)
	{
		if(!has_non_sds_polyhedra(Mesh))
			return;
		
		k3d::bitmap* const bitmap = m_bitmap.pipeline_value();
		if(!(bitmap && bitmap->width() && bitmap->height()))
			return;

		if(0 == m_texture_name)
		{
			glGenTextures(1, &m_texture_name);
			if(0 == m_texture_name)
				return;

			glBindTexture(GL_TEXTURE_2D, m_texture_name);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP_SGIS, GL_TRUE);

			k3d::gl::tex_image_2d(*bitmap);
		}

		k3d::gl::store_attributes attributes;

		glFrontFace(RenderState.inside_out ? GL_CCW : GL_CW);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		k3d::gl::set(GL_CULL_FACE, RenderState.draw_two_sided);

		glEnable(GL_POLYGON_OFFSET_FILL);
		glPolygonOffset(1.0, 1.0);
		
		const color_t color = RenderState.node_selection ? selected_mesh_color() : unselected_mesh_color(RenderState.parent_selection);
		const color_t selected_color = RenderState.show_component_selection ? selected_component_color() : color;
		
		cached_triangulation& triangles = get_data<cached_triangulation>(&Mesh, this);
		const k3d::mesh::indices_t& face_starts = triangles.face_starts();
		if (face_starts.empty())
			return;
		const k3d::mesh::points_t& points = triangles.points();
		const cached_triangulation::indices_t& indices = triangles.indices();
		
		glEnable(GL_LIGHTING);
		enable_blending();
		
		k3d::uint_t face_offset = 0;
		for(k3d::mesh::primitives_t::const_iterator primitive = Mesh.primitives.begin(); primitive != Mesh.primitives.end(); ++primitive)
		{
			boost::scoped_ptr<k3d::polyhedron::const_primitive> polyhedron(k3d::polyhedron::validate(**primitive));
			if(!polyhedron.get())
				continue;
			
			if (k3d::polyhedron::is_sds(*polyhedron))
				continue;
		
			normal_cache& n_cache = get_data<normal_cache>(&Mesh, this);
	
			const k3d::typed_array<k3d::texture3>* texcoords = 0;
			for(k3d::named_arrays::const_iterator array_it = polyhedron->face_varying_data.begin(); array_it != polyhedron->face_varying_data.end(); ++array_it)
			{
				texcoords = dynamic_cast<const k3d::typed_array<k3d::texture3>* >(array_it->second.get());
				if(texcoords)
					break;
			}
	
			glEnable(GL_TEXTURE_2D);
			glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
			glBindTexture(GL_TEXTURE_2D, m_texture_name);
	
			glBegin(GL_TRIANGLES);
			const k3d::uint_t face_count = polyhedron->face_first_loops.size();
			for (k3d::uint_t poly_face = 0; poly_face != face_starts.size(); ++poly_face)
			{
				const k3d::uint_t face = poly_face + face_offset;
				k3d::uint_t startindex = face_starts[face];
				k3d::uint_t endindex = face+1 == (face_starts.size()) ? indices.size() : face_starts[face+1];
				const color_t& face_color = polyhedron->face_selections[poly_face] ? selected_color : color;
				k3d::gl::normal3d(n_cache.face_normals(this).at(face));
				k3d::gl::material(GL_FRONT_AND_BACK, GL_DIFFUSE, k3d::color(face_color.red, face_color.green, face_color.blue), face_color.alpha);
				k3d::uint_t point_offset = indices[startindex];
				k3d::uint_t edge_offset = polyhedron->loop_first_edges[polyhedron->face_first_loops[poly_face]];
				for (k3d::uint_t corner = startindex; corner != endindex; ++corner)
				{
					if(texcoords)
					{
						// TODO: This mapping isn't very nice, but the idea is to get the edge index of the triangulated point
						//		 Maybe it's possible some better way already?
						size_t coord_index = (indices[corner]-point_offset) + edge_offset;
						glTexCoord2d((*texcoords)[coord_index].n[0], (*texcoords)[coord_index].n[1]);
					}
					k3d::gl::vertex3d(points[indices[corner]]);
				}
			}
			glEnd();
			face_offset += face_count;
		}
		disable_blending();
	}
	
	void on_select_mesh(const k3d::mesh& Mesh, const k3d::gl::painter_render_state& RenderState, const k3d::gl::painter_selection_state& SelectionState)
	{
		if(!has_non_sds_polyhedra(Mesh))
			return;
			
		if (!SelectionState.select_faces)
			return;

		k3d::gl::store_attributes attributes;
		
		glDisable(GL_LIGHTING);

		glFrontFace(RenderState.inside_out ? GL_CCW : GL_CW);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		k3d::gl::set(GL_CULL_FACE, !SelectionState.select_backfacing);

		glEnable(GL_POLYGON_OFFSET_FILL);
		glPolygonOffset(1.0, 1.0);
		
		cached_triangulation& triangles = get_data<cached_triangulation>(&Mesh, this); 
		
		const k3d::mesh::indices_t& face_starts = triangles.face_starts();
		if (face_starts.empty())
			return;
		const k3d::mesh::points_t& points = triangles.points();
		const cached_triangulation::indices_t& indices = triangles.indices();
		
		k3d::uint_t face_offset = 0;
		for(k3d::mesh::primitives_t::const_iterator primitive = Mesh.primitives.begin(); primitive != Mesh.primitives.end(); ++primitive)
		{
			boost::scoped_ptr<k3d::polyhedron::const_primitive> polyhedron(k3d::polyhedron::validate(**primitive));
			if(!polyhedron.get())
				continue;
			
			if (k3d::polyhedron::is_sds(*polyhedron))
				continue;
			const k3d::uint_t face_count = polyhedron->face_first_loops.size();
			for(k3d::uint_t poly_face = 0; poly_face != face_count; ++poly_face)
			{
				const k3d::uint_t face = poly_face + face_offset;
				k3d::gl::push_selection_token(k3d::selection::ABSOLUTE_FACE, poly_face);
	
				k3d::uint_t startindex = face_starts[face];
				k3d::uint_t endindex = face+1 == (face_starts.size()) ? indices.size() : face_starts[face+1];
				glBegin(GL_TRIANGLES);
				for (k3d::uint_t corner = startindex; corner != endindex; ++corner)
					k3d::gl::vertex3d(points[indices[corner]]);
				glEnd();
	
				k3d::gl::pop_selection_token(); // ABSOLUTE_FACE
			}
			face_offset += face_count;
		}
	}
	
	void on_mesh_changed(const k3d::mesh& Mesh, k3d::ihint* Hint)
	{
		if(!has_non_sds_polyhedra(Mesh))
			return;
		
		schedule_data<cached_triangulation>(&Mesh, Hint, this);
		schedule_data<normal_cache>(&Mesh, Hint, this);
	}
	
	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<textured_face_painter, k3d::interface_list<k3d::gl::imesh_painter > > factory(
			k3d::uuid(0x44f5c208, 0x4e8206cd, 0x29843cb3, 0x0cf46093),
			"OpenGLTexturedFacePainter",
			_("Renders textured mesh faces, after triangulating them (OpenGL 1.1)"),
			"OpenGL Painter",
			k3d::iplugin_factory::EXPERIMENTAL);

		return factory;
	}

protected:

	k3d_data(k3d::bitmap*, immutable_name, change_signal, no_undo, local_storage, no_constraint, writable_property, no_serialization) m_bitmap;
	GLuint m_texture_name;
};

/////////////////////////////////////////////////////////////////////////////
// textured_face_painter_factory

k3d::iplugin_factory& textured_face_painter_factory()
{
	return textured_face_painter::get_factory();
}

} // namespace opengl

} // namespace painters

} // namespace module

