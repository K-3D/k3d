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

/** \file
	\author Tim Shead (tshead@k-3d.com)
*/

#include "common.h"

#include <k3d-i18n-config.h>
#include <k3dsdk/basic_math.h>
#include <k3dsdk/color.h>
#include <k3dsdk/document_plugin_factory.h>
#include <k3dsdk/irenderable_gl.h>
#include <k3dsdk/measurement.h>
#include <k3dsdk/options.h>
#include <k3dsdk/node.h>
#include <k3dsdk/render_state_gl.h>
#include <k3dsdk/selection.h>
#include <k3dsdk/share.h>
#include <k3dsdk/utility_gl.h>
#include <k3dsdk/vectors.h>

#include <FTGL/ftgl.h>

namespace module
{

namespace annotation
{

/////////////////////////////////////////////////////////////////////////////
// annotation_2d

class annotation_2d :
	public k3d::node,
	public k3d::gl::irenderable
{
	typedef k3d::node base;

public:
	annotation_2d(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_font_path(init_owner(*this) + init_name("font") + init_label(_("Font")) + init_description(_("Font path")) + init_value(k3d::share_path() / k3d::filesystem::generic_path("fonts/VeraBd.ttf")) + init_path_mode(k3d::ipath_property::READ) + init_path_type(k3d::options::path::fonts())),
		m_font_size(init_owner(*this) + init_name("font_size") + init_label(_("Font Size")) + init_description(_("Font size.")) + init_value(14.0)),
		m_line_width(init_owner(*this) + init_name("line_width") + init_label(_("Line Width")) + init_description(_("Maximum width of a single line of text..")) + init_value(200.0)),
		m_line_spacing(init_owner(*this) + init_name("line_spacing") + init_label(_("Line Spacing")) + init_description(_("Controls the spacing between lines of text.")) + init_value(1.0)),
		m_alignment(init_owner(*this) + init_name("alignment") + init_label(_("Alignment")) + init_description(_("Controls the alignment of adjacent lines of text.")) + init_value(LEFT) + init_values(alignment_values())),
		m_text(init_owner(*this) + init_name("text") + init_label(_("Text")) + init_description(_("Annotation text")) + init_value(k3d::string_t(_("Annotation")))),
		m_color(init_owner(*this) + init_name("color") + init_label(_("Color")) + init_description(_("Annotation color")) + init_value(k3d::color(0, 0, 0))),
		m_x(init_owner(*this) + init_name("x") + init_label(_("X")) + init_description(_("X Position")) + init_value(0.0)),
		m_y(init_owner(*this) + init_name("y") + init_label(_("Y")) + init_description(_("Y Position")) + init_value(0.0))
	{
		m_text.set_metadata("k3d:property-type", "k3d:multi-line-text");

		m_font_path.changed_signal().connect(make_async_redraw_slot());
		m_font_size.changed_signal().connect(make_async_redraw_slot());
		m_line_width.changed_signal().connect(make_async_redraw_slot());
		m_line_spacing.changed_signal().connect(make_async_redraw_slot());
		m_alignment.changed_signal().connect(make_async_redraw_slot());
		m_text.changed_signal().connect(make_async_redraw_slot());
		m_color.changed_signal().connect(make_async_redraw_slot());
		m_x.changed_signal().connect(make_async_redraw_slot());
		m_y.changed_signal().connect(make_async_redraw_slot());
	}

	sigc::slot<void, iunknown*> make_async_redraw_slot()
	{
		return sigc::mem_fun(*this, &annotation_2d::async_redraw);
	}

	void async_redraw(iunknown*)
	{
		k3d::gl::redraw_all(document(), k3d::gl::irender_viewport::ASYNCHRONOUS);
	}

	void gl_draw(const k3d::gl::render_state& State)
	{
		k3d::gl::store_attributes attributes;

		k3d::gl::color3d(State.node_selection ? k3d::color(1, 1, 1) : m_color.pipeline_value());
		draw(State);
	}

	void gl_select(const k3d::gl::render_state& State, const k3d::gl::selection_state& SelectState)
	{
		/** \note Selection is disabled for this plugin for a couple of reasons:
		First, the combination of the custom matrix used to position the text and the
		pick matrix causes weird behavior.
		Second, we assume that it is useful to have on-screen displays that don't
		interfere with "normal" selection of geometry. */
	}

	void draw(const k3d::gl::render_state& State)
	{
		FTGLPixmapFont font(m_font_path.pipeline_value().native_filesystem_string().c_str());
		if(font.Error())
		{
			k3d::log() << error << "error initializing font" << std::endl;
			return;
		}
		font.FaceSize(static_cast<unsigned int>(m_font_size.pipeline_value()));

		FTSimpleLayout layout;
		layout.SetFont(&font);
		layout.SetLineLength(m_line_width.pipeline_value());
		layout.SetLineSpacing(m_line_spacing.pipeline_value());

		switch(m_alignment.pipeline_value())
		{
			case LEFT:
				layout.SetAlignment(FTGL::ALIGN_LEFT);
				break;
			case CENTER:
				layout.SetAlignment(FTGL::ALIGN_CENTER);
				break;
			case RIGHT:
				layout.SetAlignment(FTGL::ALIGN_RIGHT);
				break;
			case JUSTIFY:
				layout.SetAlignment(FTGL::ALIGN_JUSTIFY);
				break;
		}

		glDisable(GL_LIGHTING);
		glDisable(GL_TEXTURE_1D);
		glDisable(GL_TEXTURE_2D);
		glDisable(GL_BLEND);

		glMatrixMode(GL_PROJECTION);
		glPushMatrix();
		glLoadIdentity();
		glOrtho(-1, 1, -1, 1, -1, 1);

		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		glLoadIdentity();

		glDisable(GL_LIGHTING);
		glDisable(GL_TEXTURE_1D);
		glDisable(GL_TEXTURE_2D);
		glDisable(GL_BLEND);

		const std::string text = m_text.pipeline_value();

		glRasterPos3d(m_x.pipeline_value(), m_y.pipeline_value(), 0);
		layout.Render(m_text.pipeline_value().c_str());

		glMatrixMode(GL_MODELVIEW);
		glPopMatrix();

		glMatrixMode(GL_PROJECTION);
		glPopMatrix();
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<annotation_2d>factory(
			k3d::uuid(0x2ad16705, 0x62f1410d, 0xbcc9800c, 0xfa93762e),
			"Annotation2D",
			_("Displays 2D text annotations, primarily for documentation / tutorials"),
			"Annotation",
			k3d::iplugin_factory::STABLE);

		return factory;
	}

private:
	k3d_data(k3d::filesystem::path, immutable_name, change_signal, with_undo, local_storage, no_constraint, path_property, path_serialization) m_font_path;
	k3d_data(k3d::double_t, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_font_size;
	k3d_data(k3d::double_t, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_line_width;
	k3d_data(k3d::double_t, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_line_spacing;
	k3d_data(alignment_t, immutable_name, change_signal, with_undo, local_storage, no_constraint, enumeration_property, with_serialization) m_alignment;
	k3d::metadata_property<k3d_data(k3d::string_t, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization)> m_text;
	k3d_data(k3d::color, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_color;
	k3d_data(double, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_x;
	k3d_data(double, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_y;
};

/////////////////////////////////////////////////////////////////////////////
// annotation_2d_factory

k3d::iplugin_factory& annotation_2d_factory()
{
	return annotation_2d::get_factory();
}

} // namespace annotation

} // namespace module

