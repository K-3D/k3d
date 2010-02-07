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

#include <k3d-i18n-config.h>
#include <k3dsdk/application_plugin_factory.h>
#include <k3dsdk/gl.h>
#include <k3dsdk/module.h>
#include <k3dsdk/ngui/application_window.h>
#include <k3dsdk/ngui/button.h>
#include <k3dsdk/ngui/widget_manip.h>
#include <k3dsdk/types.h>

#include <gtkmm/box.h>
#include <gtkmm/buttonbox.h>
#include <gtkmm/scrolledwindow.h>
#include <gtkmm/stock.h>
#include <gtkmm/textview.h>

#include <boost/assign/list_of.hpp>
#include <boost/tokenizer.hpp>

#include <set>

using namespace k3d::ngui;

namespace module
{

namespace ngui
{

namespace opengl
{

static const k3d::string_t get_string(GLenum name)
{
	const GLubyte* result = glGetString(name);
	return result ? k3d::string_t(reinterpret_cast<const char*>(result)) + "\n" : k3d::string_t("<unknown>");
}

/////////////////////////////////////////////////////////////////////////////
// dialog

/// Creates an "about" box that displays program version and copyright
class dialog :
	public k3d::ngui::application_window,
	public k3d::iunknown
{
	typedef k3d::ngui::application_window base;

public:
	dialog()
	{
		set_title(_("OpenGL Information"));
		set_role("opengl_information");
		set_position(Gtk::WIN_POS_CENTER);
		set_default_size(300, 400);

		Gtk::TextView* const text_view = new Gtk::TextView();
		text_view->set_editable(false);
		text_view->set_wrap_mode(Gtk::WRAP_WORD);
		text_view->set_justification(Gtk::JUSTIFY_LEFT);

		Glib::RefPtr<Gtk::TextBuffer> text_buffer = text_view->get_buffer();

		Glib::RefPtr<Gtk::TextTag> heading_tag = Gtk::TextTag::create("name");
		heading_tag->property_foreground() = "#000000";
		heading_tag->property_weight() = Pango::WEIGHT_BOLD;
		heading_tag->property_justification() = Gtk::JUSTIFY_LEFT;
		text_buffer->get_tag_table()->add(heading_tag);

		text_buffer->insert_with_tag(text_buffer->end(), "OpenGL Vendor:\n\n", heading_tag);
		text_buffer->insert(text_buffer->end(), reinterpret_cast<const char*>(glGetString(GL_VENDOR)));
		text_buffer->insert(text_buffer->end(), "\n\n");

		text_buffer->insert_with_tag(text_buffer->end(), "OpenGL Renderer:\n\n", heading_tag);
		text_buffer->insert(text_buffer->end(), reinterpret_cast<const char*>(glGetString(GL_RENDERER)));
		text_buffer->insert(text_buffer->end(), "\n\n");

		text_buffer->insert_with_tag(text_buffer->end(), "OpenGL Version:\n\n", heading_tag);
		text_buffer->insert(text_buffer->end(), reinterpret_cast<const char*>(glGetString(GL_VERSION)));
		text_buffer->insert(text_buffer->end(), "\n\n");

		const k3d::string_t gl_extensions_string(glGetString(GL_EXTENSIONS) ? reinterpret_cast<const char*>(glGetString(GL_EXTENSIONS)) : "");
		std::set<k3d::string_t> gl_extensions;
		boost::char_separator<char> gl_separator(" ");
		boost::tokenizer<boost::char_separator<char> > gl_tokenizer(gl_extensions_string, gl_separator);
		std::copy(gl_tokenizer.begin(), gl_tokenizer.end(), std::inserter(gl_extensions, gl_extensions.end()));

		text_buffer->insert_with_tag(text_buffer->end(), "OpenGL Extensions:\n\n", heading_tag);
		for(std::set<k3d::string_t>::const_iterator extension = gl_extensions.begin(); extension != gl_extensions.end(); ++extension)
		{
			text_buffer->insert(text_buffer->end(), *extension);
			text_buffer->insert(text_buffer->end(), "\n");
		}
		text_buffer->insert(text_buffer->end(), "\n");

		text_buffer->insert_with_tag(text_buffer->end(), "OpenGLU Version:\n\n", heading_tag);
		text_buffer->insert(text_buffer->end(), reinterpret_cast<const char*>(gluGetString(GLU_VERSION)));
		text_buffer->insert(text_buffer->end(), "\n\n");

		const k3d::string_t glu_extensions_string(gluGetString(GLU_EXTENSIONS) ? reinterpret_cast<const char*>(gluGetString(GLU_EXTENSIONS)) : "");
		std::set<k3d::string_t> glu_extensions;
		boost::char_separator<char> glu_separator(" ");
		boost::tokenizer<boost::char_separator<char> > glu_tokenizer(glu_extensions_string, glu_separator);
		std::copy(glu_tokenizer.begin(), glu_tokenizer.end(), std::inserter(glu_extensions, glu_extensions.end()));

		text_buffer->insert_with_tag(text_buffer->end(), "OpenGLU Extensions:\n\n", heading_tag);
		for(std::set<k3d::string_t>::const_iterator extension = glu_extensions.begin(); extension != glu_extensions.end(); ++extension)
		{
			text_buffer->insert(text_buffer->end(), *extension);
			text_buffer->insert(text_buffer->end(), "\n");
		}
		text_buffer->insert(text_buffer->end(), "\n");

		Gtk::ScrolledWindow* const scrolled_window = new Gtk::ScrolledWindow();
		scrolled_window->set_policy(Gtk::POLICY_NEVER, Gtk::POLICY_AUTOMATIC);
		scrolled_window->add(*Gtk::manage(text_view));

		Gtk::HButtonBox* const button_box = new Gtk::HButtonBox(Gtk::BUTTONBOX_END);
		button_box->pack_start(*Gtk::manage(
			new Gtk::Button(Gtk::Stock::CLOSE) << connect_button(sigc::mem_fun(*this, &dialog::close))
			), Gtk::PACK_SHRINK);

		Gtk::VBox* const box = new Gtk::VBox(false, 5);
		box->set_border_width(5);
		box->pack_start(*Gtk::manage(scrolled_window), Gtk::PACK_EXPAND_WIDGET);
		box->pack_start(*Gtk::manage(button_box), Gtk::PACK_SHRINK);

		add(*Gtk::manage(box));

		show_all();
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::application_plugin_factory<dialog> factory(
			k3d::uuid(0xa631c644, 0x634cc7e5, 0x55c41489, 0xc069b6e7),
			"NGUIOpenGLDialog",
			_("Displays information about the OpenGL implementation."),
			"NGUI Dialog",
			k3d::iplugin_factory::STABLE,
			boost::assign::map_list_of("ngui:component-type", "dialog"));

		return factory;
	}
};

} // namespace opengl

} // namespace ngui

} // namespace module

K3D_MODULE_START(Registry)
	Registry.register_factory(module::ngui::opengl::dialog::get_factory());
K3D_MODULE_END

