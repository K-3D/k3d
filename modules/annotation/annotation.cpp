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
#include <k3dsdk/document_plugin_factory.h>
#include <k3dsdk/measurement.h>
#include <k3dsdk/metadata.h>
#include <k3dsdk/node.h>
#include <k3dsdk/options.h>
#include <k3dsdk/renderable_gl.h>
#include <k3dsdk/selection.h>
#include <k3dsdk/share.h>
#include <k3dsdk/transformable.h>
#include <k3dsdk/vectors.h>

#include <FTGL/ftgl.h>

#include <boost/scoped_ptr.hpp>

namespace module
{

namespace annotation
{

namespace detail
{

const k3d::filesystem::path default_font()
{
	return k3d::share_path() / k3d::filesystem::generic_path("fonts/VeraBd.ttf");
}

} // namespace detail

/////////////////////////////////////////////////////////////////////////////
// annotation

class annotation :
	public k3d::gl::renderable<k3d::transformable<k3d::node > >
{
	typedef k3d::gl::renderable<k3d::transformable<k3d::node > > base;

public:
	annotation(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_font_path(init_owner(*this) + init_name("font") + init_label(_("Font")) + init_description(_("Font path")) + init_value(detail::default_font()) + init_path_mode(k3d::ipath_property::READ) + init_path_type(k3d::options::path::fonts())),
		m_font_size(init_owner(*this) + init_name("font_size") + init_label(_("Font Size")) + init_description(_("Font size.")) + init_value(14.0)),
		m_line_width(init_owner(*this) + init_name("line_width") + init_label(_("Line Width")) + init_description(_("Maximum width of a single line of text..")) + init_value(200.0)),
		m_line_spacing(init_owner(*this) + init_name("line_spacing") + init_label(_("Line Spacing")) + init_description(_("Controls the spacing between lines of text.")) + init_value(1.0)),
		m_alignment(init_owner(*this) + init_name("alignment") + init_label(_("Alignment")) + init_description(_("Controls the alignment of adjacent lines of text.")) + init_value(LEFT) + init_values(alignment_values())),
		m_text(init_owner(*this) + init_name("text") + init_label(_("Text")) + init_description(_("Annotation text")) + init_value(k3d::string_t(_("Annotation")))),
		m_color(init_owner(*this) + init_name("color") + init_label(_("Color")) + init_description(_("Annotation color")) + init_value(k3d::color(0, 0, 0))),
		m_leader(init_owner(*this) + init_name("leader") + init_label(_("Leader")) + init_description(_("Leader line")) + init_value(false)),
		m_leader_target(init_owner(*this) + init_name("leader_target") + init_label(_("Leader Target")) + init_description(_("Leader line target")) + init_value(k3d::identity3D()))
	{
		m_text.set_metadata("k3d:property-type", "k3d:multi-line-text");

		m_selection_weight.changed_signal().connect(make_async_redraw_slot());
		m_font_path.changed_signal().connect(make_async_redraw_slot());
		m_font_size.changed_signal().connect(make_async_redraw_slot());
		m_line_width.changed_signal().connect(make_async_redraw_slot());
		m_line_spacing.changed_signal().connect(make_async_redraw_slot());
		m_alignment.changed_signal().connect(make_async_redraw_slot());
		m_text.changed_signal().connect(make_async_redraw_slot());
		m_color.changed_signal().connect(make_async_redraw_slot());
		m_leader.changed_signal().connect(make_async_redraw_slot());
		m_leader_target.changed_signal().connect(make_async_redraw_slot());
		m_input_matrix.changed_signal().connect(make_async_redraw_slot());
	}

	void on_gl_draw(const k3d::gl::render_state& State)
	{
		k3d::gl::color3d(get_selection_weight() ? k3d::color(1, 1, 1) : m_color.pipeline_value());
		draw(State);
	}

	void on_gl_select(const k3d::gl::render_state& State, const k3d::gl::selection_state& SelectState)
	{
		k3d::gl::push_selection_token(this);
		draw(State);
		k3d::gl::pop_selection_token();
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

		glRasterPos3d(0, 0, 0);
		layout.Render(m_text.pipeline_value().c_str());

		if(m_leader.pipeline_value())
		{
			glMatrixMode(GL_MODELVIEW);
			glPushMatrix();
			k3d::gl::push_matrix(k3d::inverse(matrix()));

			const k3d::point3 a = m_input_matrix.pipeline_value() * k3d::point3(0, 0, 0);
			const k3d::point3 c = m_leader_target.pipeline_value() * k3d::point3(0, 0, 0);
			const k3d::point3 b = a + (0.05 * (b - a));

			glBegin(GL_LINES);
			k3d::gl::vertex3d(b);
			k3d::gl::vertex3d(c);
			glEnd();

			glPopMatrix();
		}
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<annotation,
				k3d::interface_list<k3d::itransform_source,
				k3d::interface_list<k3d::itransform_sink > > >factory(
			k3d::uuid(0x951d3c20, 0xe2f74d6d, 0x8bc90ef8, 0x9a8967b6),
			"Annotation",
			_("Displays text annotations in the 3D document, primarily for documentation / tutorials"),
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
	k3d_data(k3d::bool_t, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_leader;
	k3d_data(k3d::matrix4, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_leader_target;
};

/////////////////////////////////////////////////////////////////////////////
// annotation_factory

k3d::iplugin_factory& annotation_factory()
{
	return annotation::get_factory();
}

} // namespace annotation

} // namespace module

