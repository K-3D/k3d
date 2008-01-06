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
	\author Tim Shead (tshead@k-3d.com)
*/

#include "fstream.h"
#include "iapplication_plugin_factory.h"
#include "idocument_plugin_factory.h"
#include "iplugin_factory.h"
#include "iplugin_registry.h"
#include "iscript_engine.h"
#include "k3d-i18n-config.h"
#include "log.h"
#include "os_load_module.h"
#include "plugin_factory_collection.h"
#include "result.h"
#include "string_cast.h"
#include "string_modifiers.h"
#include "system.h"
#include "type_registry.h"
#include "xml.h"

#include <iostream>

namespace k3d
{

namespace detail
{

/////////////////////////////////////////////////////////////////////////////
// same_factory_id

class same_factory_id
{
public:
	same_factory_id(const uuid& FactoryID) :
		m_factory_id(FactoryID)
	{
	}

	bool operator()(iplugin_factory* Factory) const
	{
		return Factory->factory_id() == m_factory_id;
	}

private:
	const uuid m_factory_id;
};

/////////////////////////////////////////////////////////////////////////////
// same_name

class same_name
{
public:
	same_name(const std::string& Name) :
		m_name(Name)
	{
	}

	bool operator()(iplugin_factory* Factory) const
	{
		return Factory->name() == m_name;
	}

private:
	const std::string m_name;
};

typedef sigc::signal<void, const std::string&> message_signal_t;

/////////////////////////////////////////////////////////////////////////////
// plugin_registry

/// Implements iplugin_registry, allowing modules to register new plugin factories, which are pushed into two collections - a set of all plugin factories, and a set of factories for the given module
class plugin_registry :
	public iplugin_registry
{
public:
	plugin_registry(message_signal_t& MessageSignal, iplugin_factory_collection::factories_t& Factories) :
		m_message_signal(MessageSignal),
		m_factories(Factories)
	{
	}

	void register_factory(iplugin_factory& Factory)
	{
		m_message_signal.emit(string_cast(boost::format(_("Loading plugin %1%")) % Factory.name()));

		// Ensure we don't have any duplicate class IDs ...
		if(std::count_if(m_factories.begin(), m_factories.end(), same_factory_id(Factory.factory_id())))
		{
			log() << error << "Plugin factory [" << Factory.name() << "] with duplicate factory ID [" << Factory.factory_id() << "] will not be registered." << std::endl;
			return;
		}

		// Warn if we have duplicate names ...
		if(std::count_if(m_factories.begin(), m_factories.end(), same_name(Factory.name())))
		{
			log() << error << "Plugin factory [" << Factory.factory_id() << "] with duplicate name [" << Factory.name() << "] will not be registered." << std::endl;
			return;
		}

		// Stash that baby!
		m_factories.insert(&Factory);
	}

private:
	message_signal_t& m_message_signal;
	iplugin_factory_collection::factories_t& m_factories;
};

/// Stores a mapping of plugin class id to plugin factory, so we can lookup recently-loaded factories quickly
std::map<uuid, iplugin_factory*> proxied_factories;
/// Stores a mapping of plugin class id to plugin module path, so we can load modules that were proxied
std::map<uuid, filesystem::path> proxied_modules;

iplugin_factory* load_proxied_factory(const uuid& FactoryID)
{
	// See if this factory has been loaded ...
	if(proxied_factories.count(FactoryID) && proxied_factories[FactoryID])
		return proxied_factories[FactoryID];

	// OK, just load the module already!
	register_plugins_entry_point register_plugins = 0;
	os_load_module(proxied_modules[FactoryID], register_plugins);

	if(!register_plugins)
		return 0;

	// It's a K-3D module, all-right - give it a chance to register its plugins
	log() << info << "Loading plugin module " << proxied_modules[FactoryID].native_console_string() << std::endl;

	message_signal_t message_signal;
	iplugin_factory_collection::factories_t factories;
	plugin_registry registry(message_signal, factories);
	register_plugins(registry);
	for(iplugin_factory_collection::factories_t::iterator factory = factories.begin(); factory != factories.end(); ++factory)
		proxied_factories[(*factory)->factory_id()] = (*factory);

	return proxied_factories[FactoryID];
}

/////////////////////////////////////////////////////////////////////////////
// application_plugin_factory_proxy

/// Proxies plugin factories so we can delay loading shared libraries at runtime until they're needed
class application_plugin_factory_proxy :
	public iplugin_factory,
	public iapplication_plugin_factory
{
public:
	application_plugin_factory_proxy(const uuid& FactoryID, const std::string& Name, const std::string& ShortDescription, const iplugin_factory::categories_t& Categories, const iplugin_factory::quality_t Quality, const iplugin_factory::interfaces_t& Interfaces, const iplugin_factory::metadata_t& Metadata) :
		m_factory(0),
		m_application_factory(0),
		m_factory_id(FactoryID),
		m_name(Name),
		m_short_description(ShortDescription),
		m_categories(Categories),
		m_quality(Quality),
		m_interfaces(Interfaces),
		m_metadata(Metadata)
	{
	}

	const uuid& factory_id()
	{
		return m_factory_id;
	}

	const std::string name()
	{
		return m_name;
	}

	const std::string short_description()
	{
		return m_short_description;
	}

	const categories_t& categories()
	{
		return m_categories;
	}

	quality_t quality()
	{
		return m_quality;
	}

	bool implements(const std::type_info& InterfaceType)
	{
		for(iplugin_factory::interfaces_t::const_iterator iface = m_interfaces.begin(); iface != m_interfaces.end(); ++iface)
		{
			if((**iface) == InterfaceType)
				return true;
		}

		return false;
	}

	const interfaces_t interfaces()
	{
		return m_interfaces;
	}

	const metadata_t& metadata()
	{
		return m_metadata;
	}

	iunknown* create_plugin()
	{
		if(!m_factory)
		{
			m_factory = load_proxied_factory(m_factory_id);
			if(!m_factory)
				k3d::log() << error << "Couldn't load proxied factory for plugin: " << name() << std::endl;

			m_application_factory = dynamic_cast<iapplication_plugin_factory*>(m_factory);
			if(!m_application_factory)
				k3d::log() << error << "Not an application plugin factory: " << name() << std::endl;
		}

		return_val_if_fail(m_application_factory, 0);
		return m_application_factory->create_plugin();
	}

private:
	iplugin_factory* m_factory;
	iapplication_plugin_factory* m_application_factory;

	const uuid m_factory_id;
	const std::string m_name;
	const std::string m_short_description;
	const iplugin_factory::categories_t m_categories;
	const iplugin_factory::quality_t m_quality;
	const iplugin_factory::interfaces_t m_interfaces;
	const iplugin_factory::metadata_t m_metadata;
};

/////////////////////////////////////////////////////////////////////////////
// document_plugin_factory_proxy

/// Proxies plugin factories so we can delay loading shared libraries at runtime until they're needed
class document_plugin_factory_proxy :
	public iplugin_factory,
	public idocument_plugin_factory
{
public:
	document_plugin_factory_proxy(const uuid& FactoryID, const std::string& Name, const std::string& ShortDescription, const iplugin_factory::categories_t& Categories, const iplugin_factory::quality_t Quality, const iplugin_factory::interfaces_t& Interfaces, const iplugin_factory::metadata_t& Metadata) :
		m_factory(0),
		m_document_factory(0),
		m_factory_id(FactoryID),
		m_name(Name),
		m_short_description(ShortDescription),
		m_categories(Categories),
		m_quality(Quality),
		m_interfaces(Interfaces),
		m_metadata(Metadata)
	{
	}

	const uuid& factory_id()
	{
		return m_factory_id;
	}

	const std::string name()
	{
		return m_name;
	}

	const std::string short_description()
	{
		return m_short_description;
	}

	const categories_t& categories()
	{
		return m_categories;
	}

	quality_t quality()
	{
		return m_quality;
	}

	bool implements(const std::type_info& InterfaceType)
	{
		for(iplugin_factory::interfaces_t::const_iterator iface = m_interfaces.begin(); iface != m_interfaces.end(); ++iface)
		{
			if((**iface) == InterfaceType)
				return true;
		}

		return false;
	}

	const interfaces_t interfaces()
	{
		return m_interfaces;
	}

	const metadata_t& metadata()
	{
		return m_metadata;
	}

	inode* create_plugin(iplugin_factory& Factory, idocument& Document)
	{
		if(!m_factory)
		{
			m_factory = load_proxied_factory(m_factory_id);
			if(!m_factory)
				k3d::log() << error << "Couldn't load proxied factory for plugin: " << name() << std::endl;

			m_document_factory = dynamic_cast<idocument_plugin_factory*>(m_factory);
			if(!m_document_factory)
				k3d::log() << error << "Not a document plugin factory: " << name() << std::endl;
		}

		return_val_if_fail(m_document_factory, 0);
		return m_document_factory->create_plugin(Factory, Document);
	}

private:
	iplugin_factory* m_factory;
	idocument_plugin_factory* m_document_factory;

	const uuid m_factory_id;
	const std::string m_name;
	const std::string m_short_description;
	const iplugin_factory::categories_t m_categories;
	const iplugin_factory::quality_t m_quality;
	const iplugin_factory::interfaces_t m_interfaces;
	const iplugin_factory::metadata_t m_metadata;
};

} // namespace detail

/////////////////////////////////////////////////////////////////////////////
// plugin_factory_collection::implementation

struct plugin_factory_collection::implementation
{
	bool proxy_module(const filesystem::path& Path, const filesystem::path& ProxyPath)
	{
		m_message_signal.emit(string_cast(boost::format(_("Proxying plugin module %1%")) % Path.native_utf8_string().raw()));

		try
		{
			filesystem::ifstream proxy_stream(ProxyPath);
			xml::element xml_document;
			proxy_stream >> xml_document;

			if(xml_document.name != "k3dml")
				throw std::runtime_error("Not a k3dml document");

			xml::element* const xml_module = xml::find_element(xml_document, "module");
			if(!xml_module)
				throw std::runtime_error("Missing <module> tag");

			xml::element* const xml_plugins = xml::find_element(*xml_module, "plugins");
			if(!xml_plugins)
				throw std::runtime_error("Missing <plugins> tag");

			for(xml::element::elements_t::iterator xml_plugin = xml_plugins->children.begin(); xml_plugin != xml_plugins->children.end(); ++xml_plugin)
			{
				if(xml_plugin->name != "plugin")
					continue;

				const std::string factory_name = xml::attribute_text(*xml_plugin, "name");
				m_message_signal.emit(string_cast(boost::format(_("Proxying plugin %1%")) % factory_name));

				const uuid plugin_factory_id = xml::attribute_value<uuid>(*xml_plugin, "factory_id", uuid::null());
				if(plugin_factory_id == uuid::null())
				{
					log() << error << "Plugin " << factory_name << " with missing factory ID will not be loaded" << std::endl;
					continue;
				}

				// Ensure we don't have any duplicate factory IDs ...
				if(std::count_if(m_factories.begin(), m_factories.end(), detail::same_factory_id(plugin_factory_id)))
				{
					log() << error << "Plugin " << factory_name << " with duplicate factory ID " << plugin_factory_id << " will not be loaded" << std::endl;
					continue;
				}

				// Warn if we have duplicate names ...
				if(std::count_if(m_factories.begin(), m_factories.end(), detail::same_name(factory_name)))
					log() << warning << "Loading plugin with duplicate name " << factory_name << std::endl;

				const std::string plugin_short_description = xml::element_text(*xml_plugin, "short_description");
				const iplugin_factory::quality_t plugin_quality = xml::attribute_value<iplugin_factory::quality_t>(*xml_plugin, "quality", iplugin_factory::EXPERIMENTAL);
				const std::string plugin_type = xml::attribute_text(*xml_plugin, "type");

				iplugin_factory::categories_t plugin_categories;
				if(xml::element* const xml_categories = xml::find_element(*xml_plugin, "categories"))
				{
					for(xml::element::elements_t::iterator xml_category = xml_categories->children.begin(); xml_category != xml_categories->children.end(); ++xml_category)
					{
						if(xml_category->name != "category")
							continue;

						plugin_categories.push_back(xml_category->text);
					}
				}

				iplugin_factory::interfaces_t plugin_interfaces;
				if(xml::element* const xml_interfaces = xml::find_element(*xml_plugin, "interfaces"))
				{
					for(xml::element::elements_t::iterator xml_interface = xml_interfaces->children.begin(); xml_interface != xml_interfaces->children.end(); ++xml_interface)
					{
						if(xml_interface->name != "interface")
							continue;

						plugin_interfaces.push_back(type_id(xml_interface->text));
					}
				}
				plugin_interfaces.erase(std::find(plugin_interfaces.begin(), plugin_interfaces.end(), static_cast<std::type_info*>(0)), plugin_interfaces.end());

				iplugin_factory::metadata_t metadata;
				if(xml::element* const xml_metadata = xml::find_element(*xml_plugin, "metadata"))
				{
					for(xml::element::elements_t::iterator xml_pair = xml_metadata->children.begin(); xml_pair != xml_metadata->children.end(); ++xml_pair)
					{
						if(xml_pair->name != "pair")
							continue;

						metadata.insert(std::make_pair(xml::attribute_text(*xml_pair, "name"), xml::attribute_text(*xml_pair, "value")));
					}
				}

				if(plugin_type == "application")
				{
					m_factories.insert(new detail::application_plugin_factory_proxy(plugin_factory_id, factory_name, plugin_short_description, plugin_categories, plugin_quality, plugin_interfaces, metadata));
				}
				else if(plugin_type == "document")
				{
					m_factories.insert(new detail::document_plugin_factory_proxy(plugin_factory_id, factory_name, plugin_short_description, plugin_categories, plugin_quality, plugin_interfaces, metadata));
				}
				else
				{
					log() << error << "Unknown plugin factory type " << plugin_type << " will be ignored" << std::endl;
					continue;
				}

				detail::proxied_modules[plugin_factory_id] = Path;
				detail::proxied_factories[plugin_factory_id] = 0;
			}

			return true;
		}
		catch(std::exception& e)
		{
			log() << error << "Error proxying plugin module " << ProxyPath.native_console_string() << std::endl;
			return false;
		}

		return false;
	}

	/// Stores a signal that will be emitted to display loading progress
	detail::message_signal_t m_message_signal;
	/// Stores the set of available plugin factories
	factories_t m_factories;
};

/////////////////////////////////////////////////////////////////////////////
// plugin_factory_collection

plugin_factory_collection::plugin_factory_collection() :
	m_implementation(new implementation())
{
}

plugin_factory_collection::~plugin_factory_collection()
{
	delete m_implementation;
}

sigc::connection plugin_factory_collection::connect_message_signal(const sigc::slot<void, const std::string&>& Slot)
{
	return m_implementation->m_message_signal.connect(Slot);
}

void plugin_factory_collection::bind_module(const std::string& ModuleName, register_plugins_entry_point RegisterPlugins)
{
	return_if_fail(RegisterPlugins);

	m_implementation->m_message_signal.emit(string_cast(boost::format(_("Binding plugin module %1%")) % ModuleName));

	detail::plugin_registry registry(m_implementation->m_message_signal, m_implementation->m_factories);
	RegisterPlugins(registry);
}

void plugin_factory_collection::load_module(const filesystem::path& Path, const load_proxy_t LoadProxies)
{
	// K-3D modules now have the same extension on all platforms ...
	if(filesystem::extension(Path).lowercase().raw() != ".module")
		return;

	// If the module can be proxied for fast startup, do that and return ...
	if(LoadProxies == LOAD_PROXIES)
	{
		filesystem::path proxy_path = Path + ".proxy";
		if(filesystem::exists(proxy_path) && m_implementation->proxy_module(Path, proxy_path))
			return;
	}

	// OK, just load the module ...
	m_implementation->m_message_signal.emit(string_cast(boost::format(_("Loading plugin module %1%")) % Path.native_utf8_string().raw()));

	register_plugins_entry_point register_plugins = 0;
	os_load_module(Path, register_plugins);
	if(!register_plugins)
		return;

	// It's a K-3D module, all-right - give it a chance to register its plugins
	detail::plugin_registry registry(m_implementation->m_message_signal, m_implementation->m_factories);
	register_plugins(registry);
}

void plugin_factory_collection::load_modules(const filesystem::path& Path, const bool Recursive, const load_proxy_t LoadProxies)
{
	m_implementation->m_message_signal.emit(string_cast(boost::format(_("Searching for plugins in %1%")) % Path.native_utf8_string().raw()));

	for(k3d::filesystem::directory_iterator path(Path); path != k3d::filesystem::directory_iterator(); ++path)
	{
		if(k3d::filesystem::is_directory(*path))
		{
			if(Recursive)
				load_modules(*path, Recursive, LoadProxies);
		}
		else
		{
			load_module(*path, LoadProxies);
		}
	}
}

void plugin_factory_collection::load_modules(const std::string& Paths, const bool Recursive, const load_proxy_t LoadProxies)
{
	const system::paths_t paths = system::decompose_path_list(Paths);
	for(system::paths_t::const_iterator path = paths.begin(); path != paths.end(); ++path)
		load_modules(*path, Recursive, LoadProxies);
}

const iplugin_factory_collection::factories_t& plugin_factory_collection::factories()
{
	return m_implementation->m_factories;
}

} // namespace k3d

