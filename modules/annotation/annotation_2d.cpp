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

#include <k3dsdk/basic_math.h>
#include <k3dsdk/document_plugin_factory.h>
#include <k3dsdk/drawable_gl.h>
#include <k3d-i18n-config.h>
#include <k3dsdk/measurement.h>
#include <k3dsdk/node.h>
#include <k3dsdk/persistent.h>
#include <k3dsdk/selection.h>
#include <k3dsdk/vectors.h>

namespace libk3dannotation
{

/////////////////////////////////////////////////////////////////////////////
// annotation_2d

class annotation_2d :
	public k3d::persistent<k3d::node>,
	public k3d::gl::idrawable
{
	typedef k3d::persistent<k3d::node> base;

public:
	annotation_2d(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_text(init_owner(*this) + init_name("text") + init_label(_("Text")) + init_description(_("Annotation text")) + init_value(std::string(_("Annotation")))),
		m_color(init_owner(*this) + init_name("color") + init_label(_("Color")) + init_description(_("Annotation color")) + init_value(k3d::color(0, 0, 0))),
		m_x(init_owner(*this) + init_name("x") + init_label(_("X")) + init_description(_("X Position")) + init_value(0.0)),
		m_y(init_owner(*this) + init_name("y") + init_label(_("Y")) + init_description(_("Y Position")) + init_value(0.0))
	{
		m_selection_weight.changed_signal().connect(make_async_redraw_slot());
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
		k3d::gl::redraw_all(document(), k3d::gl::irender_engine::ASYNCHRONOUS);
	}

	void gl_draw(const k3d::gl::render_state& State)
	{
		k3d::gl::store_attributes attributes;

		k3d::gl::color3d(get_selection_weight() ? k3d::color(1, 1, 1) : m_color.value());
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

		const std::string text = m_text.value();

		glRasterPos3d(m_x.value(), m_y.value(), 0);
		glListBase(State.gl_ascii_font_list_base);
		glCallLists(text.size(), GL_UNSIGNED_BYTE, text.c_str());

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
	k3d_data(std::string, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_text;
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

} // namespace libk3dannotation

