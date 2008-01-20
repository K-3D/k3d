// K-3D
// Copyright (c) 1995-2004, Timothy M. Shead
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

#include "application.h"
#include "iapplication.h"
#include "iapplication_plugin_factory.h"
#include "idocument_plugin_factory.h"
#include "iplugin_factory.h"
#include "iplugin_factory_collection.h"
#include "log.h"
#include "plugin.h"
#include "utility.h"

#include <boost/tokenizer.hpp>

namespace k3d
{

namespace plugin
{

namespace detail
{

iunknown* create_application_plugin(const k3d::string_t& Plugin)
{
	iplugin_factory* const plugin_factory = plugin::factory::lookup(Plugin);
	if(!plugin_factory)
	{
		log() << error << "No plugin factory named: " << Plugin << std::endl;
	}
	else
	{
		return detail::create_application_plugin(*plugin_factory);
	}

	return 0;
}

iunknown* create_application_plugin(iplugin_factory& Factory)
{
	if(iapplication_plugin_factory* const application_plugin_factory = dynamic_cast<iapplication_plugin_factory*>(&Factory))
	{
		if(iunknown* const unknown = application_plugin_factory->create_plugin())
		{
			return unknown;
		}
		else
		{
			log() << error << "Error creating application plugin: " << Factory.name() << std::endl;
		}
	}
	else
	{
		log() << error << "Not an application plugin factory: " << Factory.name() << std::endl;
	}
	
	return 0;
}

iunknown* create_application_plugin(const uuid& ClassID)
{
	if(iplugin_factory* const plugin_factory = plugin::factory::lookup(ClassID))
	{
		return create_application_plugin(*plugin_factory);
	}
	else
	{
		log() << error << "No plugin factory: " << ClassID << std::endl;
	}

	return 0;
}

inode* create_document_plugin(const k3d::string_t& Plugin, idocument& Document, const k3d::string_t& Name)
{
	k3d::iplugin_factory* const plugin_factory = plugin::factory::lookup(Plugin);
	if(!plugin_factory)
	{
		log() << error << "No plugin factory named: " << Plugin << std::endl;
	}
	else
	{
		return create_document_plugin(*plugin_factory, Document, Name);
	}

	return 0;
}

inode* create_document_plugin(iplugin_factory& Factory, idocument& Document, const k3d::string_t& Name)
{
	if(idocument_plugin_factory* const document_plugin_factory = dynamic_cast<idocument_plugin_factory*>(&Factory))
	{
		if(inode* const object = document_plugin_factory->create_plugin(Factory, Document))
		{
			return object;
		}
		else
		{
			log() << error << "Error creating document plugin: " << Factory.name() << std::endl;
		}
	}
	else
	{
		log() << error << "Not a document plugin factory: " << Factory.name() << std::endl;
	}
	
	return 0;
}

inode* create_document_plugin(const uuid& ClassID, idocument& Document, const k3d::string_t& Name)
{
	if(iplugin_factory* const plugin_factory = plugin::factory::lookup(ClassID))
	{
		return create_document_plugin(*plugin_factory, Document, Name);
	}
	else
	{
		log() << error << "No plugin factory: " << ClassID << std::endl;
	}

	return 0;
}
	
} // namespace detail

namespace factory
{

iplugin_factory* lookup(const uuid& ID)
{
	for(iplugin_factory_collection::factories_t::const_iterator factory = application().plugins().begin(); factory != application().plugins().end(); ++factory)
	{
		if((*factory)->factory_id() == ID)
			return *factory;
	}

	return 0;
}

iplugin_factory* lookup(const k3d::string_t& Name)
{
	collection_t results;
	for(iplugin_factory_collection::factories_t::const_iterator factory = application().plugins().begin(); factory != application().plugins().end(); ++factory)
	{
		if((*factory)->name() == Name)
			results.insert(*factory);
	}

	switch(results.size())
	{
		case 0:
			return 0;
		case 1:
			return *results.begin();
		default:
			k3d::log() << error << "multiple plugin factories with name [" << Name << "]" << std::endl;
			return 0;
	}
}

const collection_t lookup(const std::type_info& Interface)
{
	collection_t results;
	for(iplugin_factory_collection::factories_t::const_iterator factory = application().plugins().begin(); factory != application().plugins().end(); ++factory)
	{
		if((*factory)->implements(Interface) == false)
			continue;

		results.insert(*factory);
	}

	return results;
}

const collection_t lookup(const std::type_info& Interface, const string_t& MIMEType)
{
	collection_t results;
	for(iplugin_factory_collection::factories_t::const_iterator factory = application().plugins().begin(); factory != application().plugins().end(); ++factory)
	{
		if((*factory)->implements(Interface) == false)
			continue;

		k3d::iplugin_factory::metadata_t metadata = (**factory).metadata();
		const string_t mime_types = metadata["k3d:mime-types"];

		typedef boost::char_separator<char> separator_t;
		separator_t separator(" ");

		typedef boost::tokenizer<separator_t> tokenizer_t; 
		tokenizer_t tokenizer(mime_types, separator);

		for(tokenizer_t::const_iterator mime_type = tokenizer.begin(); mime_type != tokenizer.end(); ++mime_type)
		{
			if(*mime_type == MIMEType)
			{
				results.insert(*factory);
				break;
			}
		}
	}

	return results;
}

} // namespace factory

} // namespace plugin

} // namespace k3d

