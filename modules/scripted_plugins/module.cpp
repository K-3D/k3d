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

#include <k3d-i18n-config.h>
#include <k3dsdk/create_plugins.h>
#include <k3dsdk/fstream.h>
#include <k3dsdk/iapplication_plugin_factory.h>
#include <k3dsdk/idocument_plugin_factory.h>
#include <k3dsdk/interface_list.h>
#include <k3dsdk/log.h>
#include <k3dsdk/module.h>
#include <k3dsdk/path.h>
#include <k3dsdk/plugin_factory.h>
#include <k3dsdk/plugins.h>
#include <k3dsdk/property.h>
#include <k3dsdk/share.h>

#include <boost/regex.hpp>

#include <sstream>

namespace module
{

/// Namespace reserved for the scripted_plugins plugin module, to protect public symbols from name clashes with other modules
namespace scripted_plugins
{

/// k3d::iapplication_plugin_factory implementation that creates a scripted plugin instance
class application_factory :
	public k3d::plugin_factory,
	public k3d::iapplication_plugin_factory
{
public:
	application_factory(const k3d::filesystem::path& ScriptPath, const k3d::string_t& DelegateFactoryName, const k3d::uuid& FactoryID, const k3d::string_t& Name, const k3d::string_t& ShortDescription, const k3d::string_t& Categories, const quality_t Quality = STABLE, const metadata_t& Metadata = metadata_t()) :
		plugin_factory(FactoryID, Name, ShortDescription, Categories, Quality, Metadata),
		script_path(ScriptPath),
		delegate_factory_name(DelegateFactoryName),
		delegate_factory(0)
	{
	}

	k3d::iunknown* create_plugin()
	{
		if(!delegate_factory)
		{
			const k3d::factories_t factories = k3d::plugins(delegate_factory_name);
			return_val_if_fail(1 == factories.size(), 0);

			delegate_factory = dynamic_cast<k3d::iapplication_plugin_factory*>(*factories.begin());
		}

		return_val_if_fail(delegate_factory, 0);

		k3d::iunknown* const plugin = delegate_factory->create_plugin();
		return_val_if_fail(plugin, 0);

		k3d::filesystem::ifstream script_file(script_path);
		std::stringstream script_stream;
		script_stream << script_file.rdbuf();

		k3d::property::set_internal_value(*plugin, "script", script_stream.str());

		return plugin;
	}

	bool implements(const std::type_info& InterfaceType)
	{
		return false;
	}

	const interfaces_t interfaces()
	{
		interfaces_t results;
		return results;
	}

private:
	const k3d::filesystem::path script_path;
	const k3d::string_t delegate_factory_name;
	k3d::iapplication_plugin_factory* delegate_factory;
};

/// k3d::idocument_plugin_factory implementation that creates a scripted document node
class document_factory :
	public k3d::plugin_factory,
	public k3d::idocument_plugin_factory
{
public:
	document_factory(const k3d::filesystem::path& ScriptPath, const k3d::string_t& DelegateFactoryName, const k3d::uuid& FactoryID, const k3d::string_t& Name, const k3d::string_t& ShortDescription, const k3d::string_t& Categories, const quality_t Quality = STABLE, const metadata_t& Metadata = metadata_t()) :
		plugin_factory(FactoryID, Name, ShortDescription, Categories, Quality, Metadata),
		script_path(ScriptPath),
		delegate_factory_name(DelegateFactoryName),
		delegate_factory(0)
	{
	}

	k3d::inode* create_plugin(k3d::iplugin_factory& Factory, k3d::idocument& Document)
	{
		if(&Factory != this)
		{
			k3d::log() << error << "Cannot use factory [" << name() << "] from within a virtual plugin factory" << std::endl;
			return 0;
		}

		if(!delegate_factory)
		{
			const k3d::factories_t factories = k3d::plugins(delegate_factory_name);
			return_val_if_fail(1 == factories.size(), 0);

			delegate_factory = dynamic_cast<k3d::idocument_plugin_factory*>(*factories.begin());
		}

		return_val_if_fail(delegate_factory, 0);

		k3d::inode* const node = delegate_factory->create_plugin(*dynamic_cast<k3d::iplugin_factory*>(delegate_factory), Document);
		return_val_if_fail(node, 0);

		k3d::filesystem::ifstream script_file(script_path);
		std::stringstream script_stream;
		script_stream << script_file.rdbuf();

		k3d::property::set_internal_value(*node, "script", script_stream.str());

		return node;
	}

	bool implements(const std::type_info& InterfaceType)
	{
		return false;
	}

	const interfaces_t interfaces()
	{
		interfaces_t results;
		return results;
	}

private:
	const k3d::filesystem::path script_path;
	const k3d::string_t delegate_factory_name;
	k3d::idocument_plugin_factory* delegate_factory;
};

/// Creates plugin factories at runtime based on the contents of the share/scripts/scripted_nodes directory
void register_plugins(k3d::iplugin_registry& Registry)
{
	boost::regex class_expression("k3d:plugin-class=\"([^\"]*)\"");
	boost::regex type_expression("k3d:plugin-type=\"([^\"]*)\"");
	boost::regex name_expression("k3d:plugin-name=\"([^\"]*)\"");
	boost::regex description_expression("k3d:plugin-description=\"([^\"]*)\"");

	// There are very few SDK functions that can be safely called at this point in execution, but k3d::share_path() happens to be one of them ...
	for(k3d::filesystem::directory_iterator script_path(k3d::share_path() / k3d::filesystem::generic_path("scripts/scripted_plugins")); script_path != k3d::filesystem::directory_iterator(); ++script_path)
	{
		if(k3d::filesystem::is_directory(*script_path))
			continue;

		k3d::filesystem::ifstream script_file(*script_path);
		std::stringstream script_stream;
		script_stream << script_file.rdbuf();
		k3d::string_t script = script_stream.str();
		
		boost::match_results<k3d::string_t::iterator> class_match;
		if(!boost::regex_search(script.begin(), script.end(), class_match, class_expression))
		{
			continue;
		}
		const k3d::string_t plugin_class = class_match[1].str();
		if(plugin_class != "application" && plugin_class != "document")
		{
			k3d::log() << error << "Script [" << script_path->native_console_string() << "] using unknown plugin class [" << plugin_class << "] will not be loaded" << std::endl;
			continue;
		}

		boost::match_results<k3d::string_t::iterator> type_match;
		if(!boost::regex_search(script.begin(), script.end(), type_match, type_expression))
		{
			k3d::log() << error << "Script [" << script_path->native_console_string() << "] without k3d:plugin-type property will not be loaded" << std::endl;
			continue;
		}
		const k3d::string_t plugin_type = type_match[1].str();

		boost::match_results<k3d::string_t::iterator> name_match;
		if(!boost::regex_search(script.begin(), script.end(), name_match, name_expression))
		{
			k3d::log() << error << "Script [" << script_path->native_console_string() << "] without k3d:plugin-name property will not be loaded" << std::endl;
			continue;
		}
		const k3d::string_t plugin_name = name_match[1].str();

		k3d::string_t plugin_description = _("Scripted plugin");
		boost::match_results<k3d::string_t::iterator> description_match;
		if(boost::regex_search(script.begin(), script.end(), description_match, description_expression))
			plugin_description = description_match[1].str();

		if(plugin_class == "application")
		{
			k3d::iplugin_factory* const factory = new application_factory(
				*script_path,
				plugin_type,
				k3d::uuid::random(),
				plugin_name,
				plugin_description,
				"Scripts");

			Registry.register_factory(*factory);
		}
		else if(plugin_class == "document")
		{
			k3d::iplugin_factory* const factory = new document_factory(
				*script_path,
				plugin_type,
				k3d::uuid::random(),
				plugin_name,
				plugin_description,
				"Scripts");

			Registry.register_factory(*factory);
		}
	}
}

} // namespace scripted_plugins

} // namespace module

K3D_MODULE_START(Registry)
	module::scripted_plugins::register_plugins(Registry);
K3D_MODULE_END

