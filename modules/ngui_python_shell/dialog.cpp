// K-3D
// Copyright (c) 1995-2010, Timothy M. Shead
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
#include <k3dsdk/idocument_sink.h>
#include <k3dsdk/iscript_engine.h>
#include <k3dsdk/log.h>
#include <k3dsdk/module.h>
#include <k3dsdk/ngui/application_window.h>
#include <k3dsdk/ngui/console.h>
#include <k3dsdk/plugins.h>
#include <k3dsdk/result.h>

#include <gtkmm/texttag.h>
#include <gdk/gdkkeysyms.h>

#include <boost/algorithm/string.hpp>
#include <boost/assign/list_of.hpp>
#include <boost/scoped_ptr.hpp>

#include <sstream>

namespace module
{

namespace ngui
{

namespace python_shell
{

namespace detail
{

/// Extracts the command's base, before completion
const k3d::string_t base(const k3d::string_t Command)
{
	const k3d::uint_t last_separator = Command.find_last_of(".= ");
	if(last_separator != k3d::string_t::npos)
	{
		return Command.substr(0, last_separator + 1);
	}
	return "";
}

} // namespace detail

/////////////////////////////////////////////////////////////////////////////
// dialog

/// Provides an interactive Python shell dialog
class dialog :
	public k3d::ngui::application_window,
	public k3d::idocument_sink
{
	typedef k3d::ngui::application_window base;

public:
	dialog() :
		engine(k3d::plugin::create<k3d::iscript_engine>("Python")),
		stdout_slot(sigc::mem_fun(*this, &dialog::print_stdout)),
		stderr_slot(sigc::mem_fun(*this, &dialog::print_stderr)),
		console(Gtk::manage(new k3d::ngui::console::control())),
		document(0)
	{
		prompt_tag = Gtk::TextTag::create("prompt");
		prompt_tag->property_foreground() = "#888888";
		prompt_tag->property_family() = "monospace";

		stdout_tag = Gtk::TextTag::create("stdout");
		stdout_tag->property_foreground() = "#0000ff";

		stderr_tag = Gtk::TextTag::create("stderr");
		stderr_tag->property_foreground() = "#ff0000";

		completion_tag = Gtk::TextTag::create("completion");
		completion_tag->property_foreground() = "#00ff00";

		set_title(_("K-3D Python Shell"));
		set_role("python_shell_window");
		set_border_width(0);
		resize(800, 400);
		set_position(Gtk::WIN_POS_CENTER);

		add(*console);
		console->connect_command_signal(sigc::mem_fun(*this, &dialog::on_command));
		console->connect_complete_key_pressed_signal(sigc::mem_fun(*this, &dialog::on_complete_key_pressed));
		console->set_completion_key(GDK_Tab);

		show_all();

		return_if_fail(engine);

		k3d::iscript_engine::context context;
		engine->execute(get_factory().name(), "import code\n", context);
		engine->execute(get_factory().name(), "__console = code.InteractiveConsole(locals())\n", context);

		engine->execute(get_factory().name(), "def quit():\n  global context\n  context.__close = True\n", context);
		engine->execute(get_factory().name(), "def exit():\n  global context\n  context.__close = True\n", context);
		engine->execute(get_factory().name(), "def help(target=None):\n  if target:\n    __builtins__.help(target)\n  else:\n    sys.stderr.write(\"Python online help isn't available in the K-3D Python Shell.\\n\")\n", context);

		engine->execute(get_factory().name(), "import sys\n", context);
		engine->execute(get_factory().name(), "print \"Python \" + sys.version + \" on \" + sys.platform", context, &stdout_slot, &stderr_slot);
		engine->execute(get_factory().name(), "import k3d\n", context);

		print_prompt(">>> ");
	}

	void set_document(k3d::idocument* Document)
	{
		document = Document;
		if(document)
		{
			k3d::iscript_engine::context context;
			context["Document"] = document;
			engine->execute(get_factory().name(), "", context);
		}
	}

	void on_command(const k3d::string_t& Command)
	{
		return_if_fail(engine);

		k3d::string_t command(Command);
		boost::replace_all(command, "\\", "\\\\");
		boost::replace_all(command, "\"", "\\\"");

		std::ostringstream console_command;
		console_command << "context.__incomplete = __console.push(\"\"\"" << command << "\"\"\")";

		k3d::iscript_engine::context context;
		context["__incomplete"] = false;
		context["__close"] = false;

		engine->execute(get_factory().name(), console_command.str(), context, &stdout_slot, &stderr_slot);

		if(boost::any_cast<k3d::bool_t>(context["__close"]))
		{
			close();
			return;
		}

		print_prompt(boost::any_cast<k3d::bool_t>(context["__incomplete"]) ? "... " : ">>> ");
	}

	void on_complete_key_pressed(const k3d::string_t& Command)
	{
		const k3d::iscript_engine::completions_t completions = engine->complete(Command);
		k3d::string_t completions_string(completions.size() ? "\n" : "");
		for(k3d::uint_t i = 0; i != completions.size(); ++i)
			completions_string += "  " + completions[i];
		print_completion(completions_string + "\n");
		print_prompt(">>> ");
		console->set_current_format(no_tag);
		k3d::string_t completed = Command;
		if(completions.size() == 1)
		{
				completed = detail::base(Command) + completions[0];
		}
		console->print_string(completed, true);
	}

	void print_prompt(const k3d::string_t& Output)
	{
		console->set_current_format(prompt_tag);
		console->prompt_string(Output);
	}

	void print_stdout(const k3d::string_t& Output)
	{
		console->set_current_format(stdout_tag);
		console->print_string(Output);
	}

	void print_stderr(const k3d::string_t& Output)
	{
		console->set_current_format(stderr_tag);
		console->print_string(Output);
	}

	void print_completion(const k3d::string_t& Output)
	{
		console->set_current_format(completion_tag);
		console->print_string(Output);
	}

	boost::scoped_ptr<k3d::iscript_engine> engine;
	k3d::iscript_engine::output_t stdout_slot;
	k3d::iscript_engine::output_t stderr_slot;

	k3d::ngui::console::control* const console;
	Glib::RefPtr<Gtk::TextTag> prompt_tag;
	Glib::RefPtr<Gtk::TextTag> stdout_tag;
	Glib::RefPtr<Gtk::TextTag> stderr_tag;
	Glib::RefPtr<Gtk::TextTag> completion_tag;
	Glib::RefPtr<Gtk::TextTag> no_tag;

	k3d::idocument* document;

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::application_plugin_factory<dialog> factory(
			k3d::uuid(0x0a0be598, 0x944d5b43, 0xfad02490, 0x854433fa),
			"NGUIPythonShellDialog",
			_("Provides an interactive Python shell window"),
			"NGUI Dialog",
			k3d::iplugin_factory::STABLE,
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

