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
#include <k3dsdk/nurbs_curve.h>
#include <k3dsdk/options.h>
#include <k3dsdk/painter_render_state_gl.h>
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
// nurbs_curve_numbering_painter

class nurbs_curve_numbering_painter :
	public k3d::gl::mesh_painter
{
	typedef k3d::gl::mesh_painter base;

public:
	nurbs_curve_numbering_painter(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_font_path(init_owner(*this) + init_name("font") + init_label(_("Font")) + init_description(_("Font path")) + init_value(k3d::share_path() / k3d::filesystem::generic_path("fonts/VeraBd.ttf")) + init_path_mode(k3d::ipath_property::READ) + init_path_type(k3d::options::path::fonts())),
		m_font_size(init_owner(*this) + init_name("font_size") + init_label(_("Font Size")) + init_description(_("Font size.")) + init_value(14.0)),
		m_antialias(init_owner(*this) + init_name("antialias") + init_label(_("Font Antialiasing")) + init_description(_("Render antialiased text.")) + init_value(true)),
		m_draw_selected(init_owner(*this) + init_name("draw_selected") + init_label(_("Draw Selected")) + init_description(_("Draw normals for selected curves.")) + init_value(true)),
		m_draw_unselected(init_owner(*this) + init_name("draw_unselected") + init_label(_("Draw Unselected")) + init_description(_("Draw normals for unselected curves.")) + init_value(false)),
		m_selected_color(init_owner(*this) + init_name("selected_color") + init_label(_("Selected Color")) + init_description(_("Normal color for selected curves.")) + init_value(k3d::color(0, 1, 0))),
		m_unselected_color(init_owner(*this) + init_name("unselected_color") + init_label(_("Unselected Color")) + init_description(_("Normal color for unselected curves.")) + init_value(k3d::color(0, 0.5, 0))),
		m_x_offset(init_owner(*this) + init_name("x_offset") + init_label(_("X-Offset")) + init_description(_("Offsets numbers away from their curve.")) + init_value(0.0)),
		m_y_offset(init_owner(*this) + init_name("y_offset") + init_label(_("Y-Offset")) + init_description(_("Offsets numbers away from their curve.")) + init_value(0.0)),
		m_z_offset(init_owner(*this) + init_name("z_offset") + init_label(_("Z-Offset")) + init_description(_("Offsets numbers away from their curve.")) + init_value(0.7))
	{
		m_font_path.changed_signal().connect(sigc::mem_fun(*this, &nurbs_curve_numbering_painter::on_font_changed));
		m_font_size.changed_signal().connect(sigc::mem_fun(*this, &nurbs_curve_numbering_painter::on_font_changed));
		m_antialias.changed_signal().connect(sigc::mem_fun(*this, &nurbs_curve_numbering_painter::on_font_changed));
		m_draw_selected.changed_signal().connect(make_async_redraw_slot());
		m_draw_unselected.changed_signal().connect(make_async_redraw_slot());
		m_selected_color.changed_signal().connect(make_async_redraw_slot());
		m_unselected_color.changed_signal().connect(make_async_redraw_slot());
		m_x_offset.changed_signal().connect(make_async_redraw_slot());
		m_y_offset.changed_signal().connect(make_async_redraw_slot());
		m_z_offset.changed_signal().connect(make_async_redraw_slot());
	}

	void on_font_changed(k3d::ihint*)
	{
		m_font.reset();
		async_redraw(0);
	}

	void on_paint_mesh(const k3d::mesh& Mesh, const k3d::gl::painter_render_state& RenderState)
	{
		const k3d::bool_t draw_selected = m_draw_selected.pipeline_value();
		const k3d::bool_t draw_unselected = m_draw_unselected.pipeline_value();
		if(!draw_selected && !draw_unselected)
			return;

		boost::scoped_ptr<k3d::nurbs_curve::const_primitive> nurbs(k3d::nurbs_curve::validate(Mesh));
		if(!nurbs)
			return;

		const k3d::mesh::points_t& points = *Mesh.points;

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

		const k3d::double_t x_offset = m_x_offset.pipeline_value();
		const k3d::double_t y_offset = m_y_offset.pipeline_value();
		const k3d::double_t z_offset = m_z_offset.pipeline_value();

		const k3d::color selected_color = m_selected_color.pipeline_value();
		const k3d::color unselected_color = m_unselected_color.pipeline_value();

		k3d::gl::store_attributes attributes;
		glDisable(GL_LIGHTING);

		const k3d::uint_t group_begin = 0;
		const k3d::uint_t group_end = group_begin + nurbs->first_curves.size();

		for(k3d::uint_t group = group_begin; group < group_end; ++group)
		{
			const k3d::uint_t curves_begin = nurbs->first_curves[group];
			const k3d::uint_t curves_end = curves_begin + nurbs->curve_counts[group];
			for(k3d::uint_t curve = curves_begin; curve < curves_end; curve++)
			{
				k3d::gl::color3d(nurbs->curve_selections[curve] ? selected_color : unselected_color);

				//calculate center
				k3d::point3 center;
				const k3d::uint_t points_begin = nurbs->curve_first_points[curve];
				const k3d::uint_t points_end = points_begin + nurbs->curve_point_counts[curve];

				for(k3d::uint_t point = points_begin; point < points_end; point++)
					center = center + points[nurbs->curve_points[point]];

				center = center / nurbs->curve_point_counts[curve];

				//draw the number
				const k3d::point3 position = center + k3d::point3(x_offset, y_offset, z_offset);

				glRasterPos3d(position[0], position[1], position[2]);
				m_font->Render(k3d::string_cast(curve).c_str());
			}
		}
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<nurbs_curve_numbering_painter, k3d::interface_list<k3d::gl::imesh_painter > > factory(
			k3d::uuid(0x0bb513e8, 0x704f0d15, 0x22631aa7, 0x996b3548),
			"OpenGLNURBSCurveNumberingPainter",
			_("Numbers NURBS curves"),
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
	k3d_data(k3d::double_t, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_x_offset;
	k3d_data(k3d::double_t, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_y_offset;
	k3d_data(k3d::double_t, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_z_offset;

	boost::scoped_ptr<FTFont> m_font;
};

/////////////////////////////////////////////////////////////////////////////
// nurbs_curve_numbering_painter_factory

k3d::iplugin_factory& nurbs_curve_numbering_painter_factory()
{
	return nurbs_curve_numbering_painter::get_factory();
}

} // namespace painters

} // namespace opengl

} // namespace module

