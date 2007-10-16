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

/** \file
	\author Timothy M. Shead (tshead@k-3d.com)
*/

#include <k3dsdk/create_plugins.h>
#include <k3dsdk/fstream.h>
#include <k3dsdk/iapplication_plugin_factory.h>
#include <k3dsdk/idocument_plugin_factory.h>
#include <k3dsdk/interface_list.h>
#include <k3dsdk/log.h>
#include <k3dsdk/module.h>
#include <k3dsdk/path.h>
#include <k3dsdk/plugins.h>
#include <k3dsdk/plugin_factory.h>
#include <k3dsdk/property.h>
#include <k3dsdk/share.h>

#include <sstream>

namespace module
{

/// Namespace reserved for the scripted_plugins plugin module, to protect public symbols from name clashes with other modules
namespace scripted_plugins
{

template<typename interface_list = k3d::null_interface>
class document_factory :
	public k3d::plugin_factory,
	public k3d::idocument_plugin_factory
{
public:
	document_factory(const k3d::filesystem::path& ScriptPath, const k3d::uuid& FactoryID, const k3d::uuid& PersistentFactoryID, const std::string& Name, const std::string& ShortDescription, const std::string& Categories, const quality_t Quality = STABLE, const metadata_t& Metadata = metadata_t()) :
		plugin_factory(FactoryID, PersistentFactoryID, Name, ShortDescription, Categories, Quality, Metadata),
		script_path(ScriptPath),
		delegate_factory(0)
	{
	}

	k3d::inode* create_plugin(k3d::iplugin_factory& Factory, k3d::idocument& Document)
	{
		if(!delegate_factory)
			delegate_factory = dynamic_cast<k3d::idocument_plugin_factory*>(k3d::plugin(persistent_factory_id()));

		return_val_if_fail(delegate_factory, 0);

		k3d::inode* const node = delegate_factory->create_plugin(*this, Document);
		return_val_if_fail(node, 0);

		k3d::filesystem::ifstream script_file(script_path);

		std::stringstream script_stream;
		script_stream << script_file.rdbuf();

		k3d::property::set_internal_value(*node, "script", script_stream.str());

		return node;
	}

	bool implements(const std::type_info& InterfaceType)
	{
		return k3d::implements_interface<interface_list>()(InterfaceType);
	}

	const interfaces_t interfaces()
	{
		interfaces_t results;
		k3d::get_interfaces<interface_list>()(results);
		return results;
	}

private:
	const k3d::filesystem::path script_path;
	k3d::idocument_plugin_factory* delegate_factory;
};

void register_plugins(k3d::iplugin_registry& Registry)
{
	// There are very few SDK functions that can be safely called at this point in execution, but k3d::share_path() happens to be one of them ...
	for(k3d::filesystem::directory_iterator script_path(k3d::share_path() / k3d::filesystem::generic_path("scripts/scripted_plugins")); script_path != k3d::filesystem::directory_iterator(); ++script_path)
	{
		if(k3d::filesystem::is_directory(*script_path))
			continue;

		k3d::log() << debug << "Loading script [" << script_path->native_console_string() << "]" << std::endl;

		k3d::iplugin_factory* const factory = new document_factory<>(
			*script_path,
			k3d::uuid::random(),
			k3d::uuid(0xc68d0187, 0xbb334026, 0xb4ca6a9f, 0x0d980cc9), // MeshSourceScript
			script_path->leaf().raw(),
			"",
			"Scripts");

		Registry.register_factory(*factory);
	}
}

} // namespace scripted_plugins

} // namespace module

K3D_MODULE_START(Registry)
	module::scripted_plugins::register_plugins(Registry);
K3D_MODULE_END

