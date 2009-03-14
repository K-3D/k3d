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
// face_numbering_painter

class face_numbering_painter :
	public k3d::gl::mesh_painter
{
	typedef k3d::gl::mesh_painter base;

public:
	face_numbering_painter(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_font_path(init_owner(*this) + init_name("font") + init_label(_("Font")) + init_description(_("Font path")) + init_value(k3d::share_path() / k3d::filesystem::generic_path("fonts/VeraBd.ttf")) + init_path_mode(k3d::ipath_property::READ) + init_path_type(k3d::options::path::fonts())),
		m_font_size(init_owner(*this) + init_name("font_size") + init_label(_("Font Size")) + init_description(_("Font size.")) + init_value(14.0)),
		m_antialias(init_owner(*this) + init_name("antialias") + init_label(_("Font Antialiasing")) + init_description(_("Render antialiased text.")) + init_value(true)),
		m_draw_selected(init_owner(*this) + init_name("draw_selected") + init_label(_("Draw Selected")) + init_description(_("Draw normals for selected polygons")) + init_value(true)),
		m_draw_unselected(init_owner(*this) + init_name("draw_unselected") + init_label(_("Draw Unselected")) + init_description(_("Draw normals for unselected polygons")) + init_value(false)),
		m_selected_color(init_owner(*this) + init_name("selected_color") + init_label(_("Selected Color")) + init_description(_("Normal color for selected polygons")) + init_value(k3d::color(0, 0, 1))),
		m_unselected_color(init_owner(*this) + init_name("unselected_color") + init_label(_("Unselected Color")) + init_description(_("Normal color for unselected polygons")) + init_value(k3d::color(0, 0, 0.5))),
		m_offset(init_owner(*this) + init_name("offset") + init_label(_("Offset")) + init_description(_("Offsets numbers away from their polygons")) + init_value(0.2))
	{
		m_font_path.changed_signal().connect(sigc::mem_fun(*this, &face_numbering_painter::on_font_changed));
		m_font_size.changed_signal().connect(sigc::mem_fun(*this, &face_numbering_painter::on_font_changed));
		m_antialias.changed_signal().connect(sigc::mem_fun(*this, &face_numbering_painter::on_font_changed));
		m_draw_selected.changed_signal().connect(make_async_redraw_slot());
		m_draw_unselected.changed_signal().connect(make_async_redraw_slot());
		m_selected_color.changed_signal().connect(make_async_redraw_slot());
		m_unselected_color.changed_signal().connect(make_async_redraw_slot());
		m_offset.changed_signal().connect(make_async_redraw_slot());
	}

	void on_font_changed(k3d::ihint*)
	{
		m_font.reset();
		async_redraw(0);
	}

	template<typename FunctorT>
	void draw(const k3d::typed_array<k3d::point3>& Centers, const k3d::typed_array<k3d::normal3>& Normals, const k3d::color& Color, const double Offset, const FunctorT& FaceTest, FTFont& Font)
	{
		k3d::gl::color3d(Color);

		const k3d::uint_t face_begin = 0;
		const k3d::uint_t face_end = face_begin + Centers.size();
		for(k3d::uint_t face = face_begin; face != face_end; ++face)
		{
			if(FaceTest(face))
			{
				const k3d::point3 position = Centers[face] + (Offset * k3d::to_vector(k3d::normalize(Normals[face])));
				glRasterPos3d(position[0], position[1], position[2]);
				Font.Render(k3d::string_cast(face).c_str());
			}
		}
	}

	void on_paint_mesh(const k3d::mesh& Mesh, const k3d::gl::painter_render_state& RenderState)
	{
		const k3d::bool_t draw_selected = m_draw_selected.pipeline_value();
		const k3d::bool_t draw_unselected = m_draw_unselected.pipeline_value();
		if(!draw_selected && !draw_unselected)
			return;

		boost::scoped_ptr<k3d::polyhedron::const_primitive> polyhedron(k3d::polyhedron::validate(Mesh));
		if(!polyhedron)
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

		const k3d::double_t offset = m_offset.pipeline_value();

		const k3d::mesh::points_t& points = *Mesh.points;

		const k3d::uint_t face_count = polyhedron->face_first_loops.size();

		// Calculate face centers and normals ...
		k3d::typed_array<k3d::point3> centers(face_count);
		k3d::typed_array<k3d::normal3> normals(face_count);
		for(k3d::uint_t face = 0; face != face_count; ++face)
		{
			centers[face] = k3d::polyhedron::center(polyhedron->edge_points, polyhedron->clockwise_edges, points, polyhedron->loop_first_edges[polyhedron->face_first_loops[face]]);
			normals[face] = k3d::polyhedron::normal(polyhedron->edge_points, polyhedron->clockwise_edges, points, polyhedron->loop_first_edges[polyhedron->face_first_loops[face]]);
		}

		k3d::gl::store_attributes attributes;
		glDisable(GL_LIGHTING);

		if(draw_selected)
			draw(centers, normals, m_selected_color.pipeline_value(), offset, selected_faces(Mesh), *m_font);

		if(draw_unselected)
			draw(centers, normals, m_unselected_color.pipeline_value(), offset, unselected_faces(Mesh), *m_font);
	}
	
	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<face_numbering_painter, k3d::interface_list<k3d::gl::imesh_painter > > factory(
			k3d::uuid(0xb04f5b7b, 0x4849cb29, 0xdc339ea9, 0xfb22e728),
			"OpenGLFaceNumberingPainter",
			_("Numbers polyhedron faces"),
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
	k3d_data(double, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_offset;

	boost::scoped_ptr<FTFont> m_font;
};

/////////////////////////////////////////////////////////////////////////////
// face_numbering_painter_factory

k3d::iplugin_factory& face_numbering_painter_factory()
{
	return face_numbering_painter::get_factory();
}

} // namespace painters

} // namespace opengl

} // namespace module


