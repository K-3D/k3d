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
	\author Romain Behar (romainbehar@yahoo.com)
	\author Timothy M. Shead (tshead@k-3d.com)
*/

#include "utility.h"

#include <k3d-i18n-config.h>
#include <k3dsdk/document_plugin_factory.h>
#include <k3dsdk/mesh_painter_gl.h>
#include <k3dsdk/options.h>
#include <k3dsdk/painter_render_state_gl.h>
#include <k3dsdk/polyhedron.h>
#include <k3dsdk/selection.h>
#include <k3dsdk/share.h>

#include <FTGL/ftgl.h>

#include <boost/scoped_ptr.hpp>

namespace module
{

namespace opengl
{

namespace painters
{

/////////////////////////////////////////////////////////////////////////////
// edge_numbering_painter

class edge_numbering_painter :
	public k3d::gl::mesh_painter
{
	typedef k3d::gl::mesh_painter base;

public:
	edge_numbering_painter(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_font_path(init_owner(*this) + init_name("font") + init_label(_("Font")) + init_description(_("Font path")) + init_value(k3d::share_path() / k3d::filesystem::generic_path("fonts/VeraBd.ttf")) + init_path_mode(k3d::ipath_property::READ) + init_path_type(k3d::options::path::fonts())),
		m_font_size(init_owner(*this) + init_name("font_size") + init_label(_("Font Size")) + init_description(_("Font size.")) + init_value(14.0)),
		m_antialias(init_owner(*this) + init_name("antialias") + init_label(_("Font Antialiasing")) + init_description(_("Render antialiased text.")) + init_value(true)),
		m_draw_selected(init_owner(*this) + init_name("draw_selected") + init_label(_("Draw Selected")) + init_description(_("Draw normals for selected polygons.")) + init_value(true)),
		m_draw_unselected(init_owner(*this) + init_name("draw_unselected") + init_label(_("Draw Unselected")) + init_description(_("Draw normals for unselected polygons.")) + init_value(false)),
		m_selected_color(init_owner(*this) + init_name("selected_color") + init_label(_("Selected Color")) + init_description(_("Normal color for selected polygons.")) + init_value(k3d::color(0, 1, 0))),
		m_unselected_color(init_owner(*this) + init_name("unselected_color") + init_label(_("Unselected Color")) + init_description(_("Normal color for unselected polygons.")) + init_value(k3d::color(0, 0.5, 0))),
		m_edge_offset(init_owner(*this) + init_name("edge_offset") + init_label(_("Edge Offset")) + init_description(_("Offsets numbers away from their edges.")) + init_value(0.5)),
		m_face_offset(init_owner(*this) + init_name("face_offset") + init_label(_("Face Offset")) + init_description(_("Offsets numbers away from their polygons.")) + init_value(0.2))
	{
		m_font_path.changed_signal().connect(sigc::mem_fun(*this, &edge_numbering_painter::on_font_changed));
		m_font_size.changed_signal().connect(sigc::mem_fun(*this, &edge_numbering_painter::on_font_changed));
		m_antialias.changed_signal().connect(sigc::mem_fun(*this, &edge_numbering_painter::on_font_changed));
		m_draw_selected.changed_signal().connect(make_async_redraw_slot());
		m_draw_unselected.changed_signal().connect(make_async_redraw_slot());
		m_selected_color.changed_signal().connect(make_async_redraw_slot());
		m_unselected_color.changed_signal().connect(make_async_redraw_slot());
		m_edge_offset.changed_signal().connect(make_async_redraw_slot());
		m_face_offset.changed_signal().connect(make_async_redraw_slot());
	}

	void on_font_changed(k3d::ihint*)
	{
		m_font.reset();
		async_redraw(0);
	}

	template<typename FunctorT>
	void draw(const k3d::polyhedron::const_primitive& Polyhedron, const k3d::mesh::points_t& Points, const k3d::mesh::normals_t& FaceNormals, const k3d::color& Color, const k3d::double_t EdgeOffset, const k3d::double_t FaceOffset, const FunctorT& EdgeTest, FTFont& Font)
	{
		k3d::gl::color3d(Color);

		const k3d::uint_t face_begin = 0;
		const k3d::uint_t face_end = face_begin + Polyhedron.face_first_loops.size();
		for(k3d::uint_t face = face_begin; face != face_end; ++face)
		{
			const k3d::uint_t loop_begin = Polyhedron.face_first_loops[face];
			const k3d::uint_t loop_end = loop_begin + Polyhedron.face_loop_counts[face];
			for(k3d::uint_t loop = loop_begin; loop != loop_end; ++loop)
			{
				const k3d::uint_t first_edge = Polyhedron.loop_first_edges[loop];
				for(k3d::uint_t edge = first_edge; ; )
				{
					if(EdgeTest(edge))
					{
						const k3d::point3 vertex1 = Points[Polyhedron.edge_points[edge]];
						const k3d::point3 vertex2 = Points[Polyhedron.edge_points[Polyhedron.clockwise_edges[edge]]];
						const k3d::vector3 edge_vector = vertex2 - vertex1;

						const k3d::point3 position = vertex1 + (0.5 * edge_vector) + (EdgeOffset * k3d::normalize(FaceNormals[face] ^ edge_vector)) + (FaceOffset * k3d::to_vector(k3d::normalize(FaceNormals[face])));

						glRasterPos3d(position[0], position[1], position[2]);
						Font.Render(k3d::string_cast(edge).c_str());
					}

					edge = Polyhedron.clockwise_edges[edge];
					if(edge == first_edge)
						break;
				}
			}
		}
	}

	void on_paint_mesh(const k3d::mesh& Mesh, const k3d::gl::painter_render_state& RenderState)
	{
		const k3d::bool_t draw_selected = m_draw_selected.pipeline_value();
		const k3d::bool_t draw_unselected = m_draw_unselected.pipeline_value();
		if(!draw_selected && !draw_unselected)
			return;

		if(!m_font)
		{
			if(m_antialias.pipeline_value())
				m_font.reset(new FTPixmapFont(m_font_path.pipeline_value().native_filesystem_string().c_str()));
			else
				m_font.reset(new FTBitmapFont(m_font_path.pipeline_value().native_filesystem_string().c_str()));

			m_font->FaceSize(static_cast<unsigned int>(m_font_size.pipeline_value()));
			m_font->UseDisplayList(true);
			if(m_font->Error())
			{
				k3d::log() << error << "error initializing font" << std::endl;
				return;
			}
		}

		const k3d::double_t edge_offset = m_edge_offset.pipeline_value();
		const k3d::double_t face_offset = m_face_offset.pipeline_value();

		const k3d::mesh::points_t& points = *Mesh.points;

		for(k3d::mesh::primitives_t::const_iterator primitive = Mesh.primitives.begin(); primitive != Mesh.primitives.end(); ++primitive)
		{
			boost::scoped_ptr<k3d::polyhedron::const_primitive> polyhedron(k3d::polyhedron::validate(**primitive));
			if(!polyhedron.get())
				continue;
		
			const k3d::uint_t face_count = polyhedron->face_first_loops.size();
	
			// Calculate face normals ...
			k3d::typed_array<k3d::normal3> normals(face_count);
			for(k3d::uint_t face = 0; face != face_count; ++face)
				normals[face] = k3d::polyhedron::normal(polyhedron->edge_points, polyhedron->clockwise_edges, points, polyhedron->loop_first_edges[polyhedron->face_first_loops[face]]);
	
			k3d::gl::store_attributes attributes;
			glDisable(GL_LIGHTING);
	
			if(draw_selected)
				draw(*polyhedron, points, normals, m_selected_color.pipeline_value(), edge_offset, face_offset, selected_edges(*polyhedron), *m_font);
	
			if(draw_unselected)
				draw(*polyhedron, points, normals, m_unselected_color.pipeline_value(), edge_offset, face_offset, unselected_edges(*polyhedron), *m_font);
		}
	}
	
	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<edge_numbering_painter, k3d::interface_list<k3d::gl::imesh_painter > > factory(
			k3d::uuid(0xd15d2acb, 0xa647d85b, 0x54dfbbb4, 0x69578f6f),
			"OpenGLEdgeNumberingPainter",
			_("Numbers polyhedron edges"),
			"OpenGL Painter",
			k3d::iplugin_factory::EXPERIMENTAL);

		return factory;
	}

private:
	k3d_data(k3d::filesystem::path, immutable_name, change_signal, with_undo, local_storage, no_constraint, path_property, path_serialization) m_font_path;
	k3d_data(k3d::double_t, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_font_size;
	k3d_data(k3d::bool_t, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_antialias;
	k3d_data(k3d::bool_t, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_draw_selected;
	k3d_data(k3d::bool_t, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_draw_unselected;
	k3d_data(k3d::color, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_selected_color;
	k3d_data(k3d::color, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_unselected_color;
	k3d_data(k3d::double_t, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_edge_offset;
	k3d_data(k3d::double_t, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_face_offset;

	boost::scoped_ptr<FTFont> m_font;
};

/////////////////////////////////////////////////////////////////////////////
// edge_numbering_painter_factory

k3d::iplugin_factory& edge_numbering_painter_factory()
{
	return edge_numbering_painter::get_factory();
}

} // namespace painters

} // namespace opengl

} // namespace module

