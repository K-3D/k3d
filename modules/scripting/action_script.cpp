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
#include <k3dsdk/iscripted_action.h>
#include <k3dsdk/property_collection.h>
#include <k3dsdk/resource/resource.h>
#include <k3dsdk/scripted_plugin.h>

namespace module
{

namespace scripting
{

/////////////////////////////////////////////////////////////////////////////
// action_script

class action_script :
	public k3d::scripted_plugin<k3d::property_collection>,
	public k3d::iscripted_action
{
public:
	action_script()
	{
		set_script(k3d::resource::get_string("/module/scripting/action_script.py"));
	}

	k3d::bool_t execute(k3d::iscript_engine::context_t& Context)
	{
		return execute_script(Context);
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::application_plugin_factory<action_script > factory(
			k3d::uuid(0x479bc86c, 0xe4428a92, 0x9fabc692, 0xad655ebb),
			"ActionScript",
			_("Executes a script on-demand"),
			"Script",
			k3d::iplugin_factory::STABLE);

		return factory;
	}
};

/////////////////////////////////////////////////////////////////////////////
// action_script_factory

k3d::iplugin_factory& action_script_factory()
{
	return action_script::get_factory();
}

} // namespace scripting

} // namespace module

