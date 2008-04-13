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
#include <k3dsdk/icommand_node.h>
#include <k3dsdk/property_collection.h>
#include <k3dsdk/resource/resource.h>
#include <k3dsdk/scripted_plugin.h>

namespace module
{

namespace scripting
{

/////////////////////////////////////////////////////////////////////////////
// command_node_script

class command_node_script :
	public k3d::scripted_plugin<k3d::property_collection>,
	public k3d::icommand_node
{
public:
	command_node_script()
	{
		set_script(k3d::resource::get_string("/module/scripting/command_node_script.py"));
	}

	const result execute_command(const k3d::string_t& Command, const k3d::string_t& Arguments)
	{
		k3d::iscript_engine::context_t context;
		context["Command"] = Command;
		context["Arguments"] = Arguments;
		
		execute_script(context);

		return RESULT_CONTINUE;
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::application_plugin_factory<command_node_script > factory(
			k3d::uuid(0xb754daf5, 0x3a401b87, 0xdd144ba2, 0x00395dde),
			"CommandNodeScript",
			_("Script node that executes a script in response to command-node commands"),
			"Scripting",
			k3d::iplugin_factory::STABLE);

		return factory;
	}
};

/////////////////////////////////////////////////////////////////////////////
// command_node_script_factory

k3d::iplugin_factory& command_node_script_factory()
{
	return command_node_script::get_factory();
}

} // namespace scripting

} // namespace module


