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

#include <k3d-version-config.h>

#include <k3dsdk/iapplication_plugin_factory.h>
#include <k3dsdk/idocument_plugin_factory.h>
#include <k3dsdk/iplugin_factory.h>
#include <k3dsdk/plugin_factory_collection.h>
#include <k3dsdk/types.h>
#include <k3dsdk/xml.h>

#include <iostream>

/////////////////////////////////////////////////////////////////////////////////////////////
// plugin_message_handler

void plugin_message_handler(const std::string& Message)
{
	std::cerr << Message << std::endl;
}

//////////////////////////////////////////////////////////////////////////////////////////////
// usage

/// Prints usage info
void usage(std::ostream& Stream)
{
	Stream << "usage: k3d-make-module-proxy [Module]" << std::endl;
	Stream << std::endl;
}

///////////////////////////////////////////////////////////////////////////////////////////////
// main

int main(int argc, char* argv[])
{
	if(argc != 2)
	{
		usage(std::cerr);
		return 1;
	}

	k3d::filesystem::path module_path = k3d::filesystem::native_path(k3d::ustring::from_utf8(argv[1]));

	k3d::plugin_factory_collection plugins;
	plugins.connect_message_signal(sigc::ptr_fun(plugin_message_handler));
	plugins.load_module(module_path, k3d::plugin_factory_collection::IGNORE_PROXIES);

	const k3d::iplugin_factory_collection::factories_t factories = plugins.factories();
	if(factories.empty())
		return 0;

	k3d::xml::element xml_proxy("k3dml",
		k3d::xml::attribute("package", K3D_PACKAGE),
		k3d::xml::attribute("version", K3D_VERSION),
		k3d::xml::attribute("host", K3D_HOST));

	k3d::xml::element& xml_module = xml_proxy.append(
		k3d::xml::element("module",
			k3d::xml::attribute("name", module_path.leaf().raw())));

	k3d::xml::element& xml_plugins = xml_module.append(k3d::xml::element("plugins"));

	for(k3d::iplugin_factory_collection::factories_t::const_iterator factory = factories.begin(); factory != factories.end(); ++factory)
	{
		k3d::xml::element& xml_plugin = xml_plugins.append(
			k3d::xml::element("plugin",
				k3d::xml::attribute("name", (*factory)->name()),
				k3d::xml::attribute("class", (*factory)->class_id()),
				k3d::xml::attribute("quality", (*factory)->quality())));

		if(dynamic_cast<k3d::iapplication_plugin_factory*>(*factory))
		{
			xml_plugin.append(k3d::xml::attribute("type", "application"));
		}
		else if(dynamic_cast<k3d::idocument_plugin_factory*>(*factory))
		{
			xml_plugin.append(k3d::xml::attribute("type", "document"));
		}
		else
		{
			std::cerr << "Unknown factory type for plugin [" << (*factory)->name() << "] - terminating proxy generation" << std::endl;
			return 1;
		}

		xml_plugin.append(k3d::xml::element("short_description", (*factory)->short_description()));

		k3d::xml::element& xml_categories = xml_plugin.append(k3d::xml::element("categories"));
		for(k3d::iplugin_factory::categories_t::const_iterator category = (*factory)->categories().begin(); category != (*factory)->categories().end(); ++category)
			xml_categories.append(k3d::xml::element("category", *category));

		k3d::xml::element& xml_interfaces = xml_plugin.append(k3d::xml::element("interfaces"));
		const k3d::iplugin_factory::interfaces_t interfaces = (*factory)->interfaces();
		for(k3d::iplugin_factory::interfaces_t::const_iterator interface = interfaces.begin(); interface != interfaces.end(); ++interface)
		{
			const std::string xml_interface = k3d::type_string(**interface);
			if(xml_interface.empty())
			{
				std::cerr << "Unknown interface type, terminating proxy generation" << std::endl;
				return 2;
			}

			xml_interfaces.append(k3d::xml::element("interface", k3d::type_string(**interface)));
		}

		k3d::xml::element& xml_metadata = xml_plugin.append(k3d::xml::element("metadata"));
		const k3d::iplugin_factory::metadata_t& metadata = (*factory)->metadata();
		for(k3d::iplugin_factory::metadata_t::const_iterator pair = metadata.begin(); pair != metadata.end(); ++pair)
			xml_metadata.append(k3d::xml::element("pair", k3d::xml::attribute("name", pair->first), k3d::xml::attribute("value", pair->second)));
	}

	std::cout << k3d::xml::declaration() << xml_proxy;

	return 0;
}

