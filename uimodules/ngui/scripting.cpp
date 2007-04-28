// K-3D
// Copyright (c) 1995-2006, Timothy M. Shead
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

#include "messages.h"
#include "scripting.h"

#include <k3dsdk/create_plugins.h>
#include <k3dsdk/fstream.h>
#include <k3dsdk/i18n.h>
#include <k3dsdk/result.h>
#include <k3dsdk/string_cast.h>

#include <boost/format.hpp>

#include <gtkmm/main.h>
#include <gdk/gdkkeysyms.h>

#include <stack>

namespace libk3dngui
{

namespace detail
{

/// Keeps track of running script engines, so they can be halted if the user hits "escape"
std::stack<k3d::iscript_engine*> script_engine_stack;

/// Halts any running scripts if the user hits "escape"
int script_escape_handler(Gtk::Widget* Widget, GdkEventKey* Event)
{
	if(Event->type != GDK_KEY_RELEASE)
		return false;

	if(Event->keyval != GDK_Escape)
		return false;

	return_val_if_fail(script_engine_stack.size(), false);

	std::vector<std::string> buttons;
	buttons.push_back(_("Yes"));
	buttons.push_back(_("No"));
	if(1 == query_message(_("Halt running script?"), 2, buttons))
		script_engine_stack.top()->halt();

	return true;
}

/// Executes a script using the given plugin factory to create the script engine
bool execute_script(const k3d::script::code& Script, const std::string& ScriptName, k3d::iscript_engine::context_t& Context, const k3d::script::language& Language)
{
	// Sanity checks ...
	return_val_if_fail(ScriptName.size(), false);

	if(!Language.factory())
	{
		error_message(
			_("Could not determine scripting language.  K-3D supports multiple scripting languages, but the language for this script was "
			"not recognized. Most K-3D script engines use some type of \"magic token\" at the beginning of a script to recognize it, e.g. \"#k3dscript\" "
			"in the first 12 characters of a script for K-3D's built-in K3DScript engine.  If you are writing a K-3D script, check the documentation "
			"for the scripting language you're writing in to see how to make it recognizable."));
		return false;
	}

	// Get the requested scripting engine ...
	k3d::iscript_engine* const engine = k3d::create_plugin<k3d::iscript_engine>(*Language.factory());
	return_val_if_fail(engine, false);

	// Intercept global key events ...
	script_engine_stack.push(engine);
	sigc::connection script_escape_handler_connection = Gtk::Main::signal_key_snooper().connect(sigc::ptr_fun(script_escape_handler));

	// Run that bad-boy ...
	const bool result = engine->execute(ScriptName, Script.source(), Context);

	script_escape_handler_connection.disconnect();
	script_engine_stack.pop();

	// Done with the engine ...
	delete dynamic_cast<k3d::ideletable*>(engine);

	if(!result)
	{
		error_message(
			_("Error executing script"));
	}

	return result;
}

} // namespace detail

bool execute_script(const k3d::script::code& Script, const std::string& ScriptName, k3d::iscript_engine::context_t& Context, const k3d::script::language& Language)
{
	return detail::execute_script(Script, ScriptName, Context, Language);
}

bool execute_script(const k3d::script::code& Script, const std::string& ScriptName, k3d::iscript_engine::context_t& Context)
{
	return detail::execute_script(Script, ScriptName, Context, k3d::script::language(Script));
}

bool execute_script(const k3d::filesystem::path& Script, k3d::iscript_engine::context_t& Context)
{
	if(!k3d::filesystem::exists(Script))
	{
		error_message(
			k3d::string_cast(boost::format(_("Requested script file %1% doesn't exist.")) % Script.native_utf8_string().raw()));
		return false;
	}

	k3d::filesystem::ifstream file(Script);
	const k3d::script::code script(file);
	const k3d::script::language language(script);

	return detail::execute_script(script, Script.native_utf8_string().raw(), Context, language);
}

} // namespace libk3dngui

