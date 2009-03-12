// K-3D
// Copyright (c) 1995-2008, Timothy M. Shead
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

#include <k3d-i18n-config.h>
#include <k3dsdk/application_plugin_factory.h>
#include <k3dsdk/command_tree.h>
#include <k3dsdk/iscript_engine.h>
#include <k3dsdk/log.h>
#include <k3dsdk/module.h>
#include <k3dsdk/plugins.h>
#include <k3dsdk/ngui/application_window.h>
#include <k3dsdk/ngui/console.h>
#include <k3dsdk/result.h>

#include <gtkmm/texttag.h>

#include <boost/assign/list_of.hpp>
#include <boost/python.hpp>
#include <boost/scoped_ptr.hpp>

#include <sstream>

namespace module
{

namespace ngui
{

namespace python_shell
{

/////////////////////////////////////////////////////////////////////////////
// dialog

/// Provides an interactive Python shell dialog
class dialog :
	public libk3dngui::application_window
{
	typedef libk3dngui::application_window base;

public:
	dialog() :
		stdout_slot(sigc::mem_fun(*this, &dialog::on_stdout)),
		stderr_slot(sigc::mem_fun(*this, &dialog::on_stderr)),
		console(Gtk::manage(new k3d::ngui::console::control(*this, "console")))
	{
		k3d::command_tree().add(*this, "python_shell_window");

		prompt_tag = Gtk::TextTag::create("prompt");
		prompt_tag->property_foreground() = "#888888";

		stdout_tag = Gtk::TextTag::create("stdout");
		stdout_tag->property_foreground() = "#0000ff";

		stderr_tag = Gtk::TextTag::create("stderr");
		stderr_tag->property_foreground() = "#ff0000";

		set_title(_("K-3D Python Shell"));
		set_role("python_shell_window");
		set_border_width(0);
		resize(800, 400);
		set_position(Gtk::WIN_POS_CENTER);

		add(*console);
		console->connect_command_signal(sigc::mem_fun(*this, &dialog::on_command));

		on_stdout(k3d::string_cast(boost::format(_("Python %1% on %2%\n")) % Py_GetVersion() % Py_GetPlatform()));
		show_prompt();

		show_all();
	}

	void show_prompt()
	{
		console->set_current_format(prompt_tag);
		console->prompt_string(">>> ");
	}

	void on_command(const k3d::string_t& Command)
	{
		if(!engine)
		{
			engine.reset(k3d::plugin::create<k3d::iscript_engine>("Python"));
			return_if_fail(engine);
		}

		k3d::iscript_engine::context_t context;
		if(!engine->execute("interactive-script", Command, context, &stdout_slot, &stderr_slot))
			k3d::log() << error << "error executing command: " << Command << std::endl;

		show_prompt();
	}

	void on_stdout(const k3d::string_t& Output)
	{
		console->set_current_format(stdout_tag);
		console->print_string(Output);
	}

	void on_stderr(const k3d::string_t& Output)
	{
		console->set_current_format(stderr_tag);
		console->print_string(Output);
	}

	boost::scoped_ptr<k3d::iscript_engine> engine;
	k3d::iscript_engine::output_t stdout_slot;
	k3d::iscript_engine::output_t stderr_slot;

	k3d::ngui::console::control* const console;
	Glib::RefPtr<Gtk::TextTag> prompt_tag;
	Glib::RefPtr<Gtk::TextTag> stdout_tag;
	Glib::RefPtr<Gtk::TextTag> stderr_tag;

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::application_plugin_factory<dialog> factory(
			k3d::uuid(0x0a0be598, 0x944d5b43, 0xfad02490, 0x854433fa),
			"NGUIPythonShellDialog",
			_("Provides an interactive Python shell window"),
			"NGUI Dialog",
			k3d::iplugin_factory::EXPERIMENTAL,
			boost::assign::map_list_of("ngui:component-type", "dialog"));

		return factory;
	}
};

} // namespace python_shell

} // namespace ngui

} // namespace module

K3D_MODULE_START(Registry)
	Registry.register_factory(module::ngui::python_shell::dialog::get_factory());
K3D_MODULE_END

