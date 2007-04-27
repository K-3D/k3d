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

#include "create_plugins.h"
#include "iapplication_plugin_factory.h"
#include "idocument_plugin_factory.h"
#include "plugins.h"
#include "utility.h"

namespace k3d
{

namespace detail
{

iunknown* create_application_plugin(const std::string& Plugin)
{
	const factories_t factories = plugins(Plugin);
	if(factories.size() == 0)
	{
		log() << error << "No plugin factory named: " << Plugin << std::endl;
	}
	else if(factories.size() > 1)
	{
		log() << error << "More than one factory named: " << Plugin << std::endl;
	}
	else
	{
		return detail::create_application_plugin(**factories.begin());
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
	if(iplugin_factory* const factory = plugin(ClassID))
	{
		return detail::create_application_plugin(*factory);
	}
	else
	{
		log() << error << "No plugin factory: " << ClassID << std::endl;
	}

	return 0;
}

inode* create_document_plugin(const std::string& Plugin, idocument& Document, const std::string& Name)
{
	const factories_t factories = plugins(Plugin);
	if(factories.size() == 0)
	{
		log() << error << "No plugin factory named: " << Plugin << std::endl;
	}
	else if(factories.size() > 1)
	{
		log() << error << "More than one factory named: " << Plugin << std::endl;
	}
	else
	{
		return detail::create_document_plugin(**factories.begin(), Document, Name);
	}

	return 0;
}

inode* create_document_plugin(iplugin_factory& Factory, idocument& Document, const std::string& Name)
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

inode* create_document_plugin(const uuid& ClassID, idocument& Document, const std::string& Name)
{
	if(iplugin_factory* const factory = plugin(ClassID))
	{
		return create_document_plugin(*factory, Document, Name);
	}
	else
	{
		log() << error << "No plugin factory: " << ClassID << std::endl;
	}

	return 0;
}
	
} // namespace detail

} // namespace k3d

