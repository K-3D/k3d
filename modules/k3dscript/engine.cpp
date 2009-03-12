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
#include <k3dsdk/classes.h>
#include <k3dsdk/command_node.h>
#include <k3dsdk/file_helpers.h>
#include <k3dsdk/iscript_engine.h>
#include <k3dsdk/module.h>
#include <k3dsdk/result.h>
#include <k3dsdk/string_modifiers.h>
#include <k3dsdk/xml.h>

#include <boost/assign/list_of.hpp>

namespace module
{

/// Namespace reserved for the K3DScript plugin module, to protect public symbols from name clashes with other modules
namespace k3dscript
{

/// Defines a "magic token" for automatic identification of scripts
const k3d::string_t magic_token("#k3dscript");

/// Provides an implementation of k3d::iscript_engine primarily designed for recording and playback of tutorials
class engine :
	public k3d::iscript_engine
{
public:
	engine() :
		m_halt_request(false)
	{
	}

	virtual ~engine()
	{
	}

	k3d::iplugin_factory& factory()
	{
		return get_factory();
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::application_plugin_factory<engine, k3d::interface_list<k3d::iscript_engine> > factory(
			k3d::classes::K3DScriptEngine(),
			"K3DScript",
			_("K3DScript scripting engine"),
			"ScriptEngine",
			k3d::iplugin_factory::STABLE,
			boost::assign::map_list_of("k3d:mime-types", "text/x-k3dscript"));

		return factory;
	}

	const k3d::string_t language()
	{
		return "K3DScript";
	}

	k3d::bool_t execute(const k3d::string_t& ScriptName, const k3d::string_t& Script, context_t& Context, output_t* Stdout, output_t* Stderr)
	{
		return_val_if_fail(ScriptName.size(), false);

		m_halt_request = false;

		std::stringstream script(Script);
		k3d::string_t line;
		for(k3d::getline(script, line); !script.eof(); k3d::getline(script, line))
		{
			if(m_halt_request)
				return true;

			if(line.empty())
				continue;

			if(line[0] == '#')
				continue;

			std::stringstream buffer(line);
			k3d::xml::element command;

			try
			{
				buffer >> command;
			}
			catch(std::exception& e)
			{
				k3d::log() << error << e.what() << std::endl;
				return false;
			}

			return_val_if_fail(command.name == "command", false);

			const k3d::string_t node_text = k3d::xml::attribute_text(command, "node");
			return_val_if_fail(!node_text.empty(), false);
			
			const k3d::string_t command_text = k3d::xml::attribute_text(command, "command");
			return_val_if_fail(!command_text.empty(), false);

			const k3d::string_t arguments_text = k3d::xml::attribute_text(command, "arguments");

			k3d::icommand_node* const node = k3d::command_node::lookup(node_text);
			if(!node)
			{
				k3d::log() << error << "Could not find command node [" << node_text << "]" << std::endl;
				return false;
			}

			switch(node->execute_command(command_text, arguments_text))
			{
				case k3d::icommand_node::RESULT_STOP:
					return true;
				case k3d::icommand_node::RESULT_CONTINUE:
					break;
				case k3d::icommand_node::RESULT_ERROR:
				case k3d::icommand_node::RESULT_UNKNOWN_COMMAND:
					return false;
			}
		}

		return true;
	}

	k3d::bool_t halt()
	{
		m_halt_request = true;
		return true;
	}

	void bless_script(std::ostream& Script)
	{
		Script << magic_token << "\n\n";
	}

	void append_comment(std::ostream& Script, const k3d::string_t& Comment)
	{
		std::stringstream buffer(Comment);
		while(true)
		{
			k3d::string_t line;
			k3d::getline(buffer, line);
			Script << "# " << line << "\n";

			if(!buffer)
				break;
		}
	}

	void append_command(std::ostream& Script, k3d::icommand_node& Node, const k3d::string_t& Command, const k3d::string_t& Arguments)
	{
		k3d::xml::element command(
			"command",
			k3d::xml::attribute("node", k3d::command_node::path(Node)),
			k3d::xml::attribute("command", Command),
			k3d::xml::attribute("arguments", Arguments));

		Script << k3d::xml::single_line() << command << "\n";
	}

private:
	/// Stores a request to halt execution of a running script
	k3d::bool_t m_halt_request;
};

} // namespace k3dscript

} // namespace module

K3D_MODULE_START(Registry)
	Registry.register_factory(module::k3dscript::engine::get_factory());
K3D_MODULE_END


