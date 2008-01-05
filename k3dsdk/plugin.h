#ifndef K3DSDK_PLUGIN_H
#define K3DSDK_PLUGIN_H

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

#include "plugin_detail.h"
#include "types.h"

#include <set>
#include <typeinfo>


namespace k3d
{

class iplugin_factory;
class uuid;

namespace plugin
{

namespace factory
{

/// Defines storage for a collection of plugin factories
typedef std::set<iplugin_factory*> collection_t;

/// Returns the plugin factory that implements a specific factory ID, or NULL.
iplugin_factory* lookup(const uuid& ID);
/// Returns the plugin factory that matches the given name, or NULL.  Note: returns NULL if more than one factory matches Name.
iplugin_factory* lookup(const k3d::string_t& Name);
/// Returns the set of plugin factories that implement the given interface.
const collection_t lookup(const std::type_info& Interface);
/// Returns the set of plugin factories that implement the given interface.
template<typename interface_t>
const collection_t lookup()
{
	return lookup(typeid(interface_t));
}

} // namespace factory

/// Creates an application plugin, returning the requested interface if implemented, otherwise NULL (cleans-up on failure)
template<typename interface_t> interface_t* create(const k3d::string_t& FactoryName)
{
	if(iunknown* const unknown = detail::create_application_plugin(FactoryName))
	{
		if(interface_t* const result = dynamic_cast<interface_t*>(unknown))
		{
			return result;
		}
		else
		{
			log() << error << "Plugin doesn't implement interface: " << FactoryName << std::endl;
		}

		delete dynamic_cast<ideletable*>(unknown);
	}

	return 0;
}

/// Creates an application plugin, returning the requested interface if implemented, otherwise NULL (cleans-up on failure)
template<typename interface_t> interface_t* create(iplugin_factory& Factory)
{
	if(iunknown* const unknown = detail::create_application_plugin(Factory))
	{
		if(interface_t* const result = dynamic_cast<interface_t*>(unknown))
		{
			return result;
		}
		else
		{
			log() << error << "Plugin doesn't implement interface: " << Factory.name() << std::endl;
		}

		delete dynamic_cast<ideletable*>(unknown);
	}

	return 0;
}

/// Creates an application plugin, returning the requested interface if implemented, otherwise NULL (cleans-up on failure)
template<typename interface_t> interface_t* create(const uuid& FactoryID)
{
	if(iunknown* const unknown = detail::create_application_plugin(FactoryID))
	{
		if(interface_t* const result = dynamic_cast<interface_t*>(unknown))
		{
			return result;
		}
		else
		{
			log() << error << "Plugin doesn't implement interface: " << FactoryID << std::endl;
		}

		delete dynamic_cast<ideletable*>(unknown);
	}

	return 0;
}

/// Creates an application plugin, returning the requested interface if implemented, otherwise NULL (cleans-up on failure)
inline iunknown* create(const k3d::string_t& FactoryName)
{
	return detail::create_application_plugin(FactoryName);
}

/// Creates an application plugin, returning the requested interface if implemented, otherwise NULL (cleans-up on failure)
inline iunknown* create(iplugin_factory& Factory)
{
	return detail::create_application_plugin(Factory);
}

/// Creates an application plugin, returning the requested interface if implemented, otherwise NULL (cleans-up on failure)
inline iunknown* create(const uuid& FactoryID)
{
	return detail::create_application_plugin(FactoryID);
}

/// Creates a document plugin, returning the requested interface if implemented, otherwise NULL (cleans-up on failure)
template<typename interface_t> interface_t* create(const k3d::string_t& FactoryName, idocument& Document, const k3d::string_t& Name = k3d::string_t())
{
	if(inode* const object = detail::create_document_plugin(FactoryName, Document, Name))
	{
		if(interface_t* const result = dynamic_cast<interface_t*>(object))
		{
			object->set_name(Name);
			undoable_new(dynamic_cast<ideletable*>(object), Document);
			Document.nodes().add_nodes(make_collection<inode_collection::nodes_t>(object));

			return result;
		}
		else
		{
			log() << error << "Plugin doesn't implement interface: " << FactoryName << std::endl;
		}

		delete dynamic_cast<ideletable*>(object);
	}

	return 0;
}

/// Creates a document plugin, returning the requested interface if implemented, otherwise NULL (cleans-up on failure)
template<typename interface_t> interface_t* create(iplugin_factory& Factory, idocument& Document, const k3d::string_t& Name = k3d::string_t())
{
	if(inode* const object = detail::create_document_plugin(Factory, Document, Name))
	{
		if(interface_t* const result = dynamic_cast<interface_t*>(object))
		{
			object->set_name(Name);
			undoable_new(dynamic_cast<ideletable*>(object), Document);
			Document.nodes().add_nodes(make_collection<inode_collection::nodes_t>(object));

			return result;
		}
		else
		{
			log() << error << "Plugin doesn't implement interface: " << Factory.name() << std::endl;
		}

		delete dynamic_cast<ideletable*>(object);
	}

	return 0;
}

/// Creates a document plugin, returning the requested interface if implemented, otherwise NULL (cleans-up on failure)
template<typename interface_t> interface_t* create(const uuid& FactoryID, idocument& Document, const k3d::string_t& Name = k3d::string_t())
{
	if(inode* const object = detail::create_document_plugin(FactoryID, Document, Name))
	{
		if(interface_t* const result = dynamic_cast<interface_t*>(object))
		{
			object->set_name(Name);
			undoable_new(dynamic_cast<ideletable*>(object), Document);
			Document.nodes().add_nodes(make_collection<inode_collection::nodes_t>(object));

			return result;
		}
		else
		{
			log() << error << "Plugin doesn't implement interface: " << FactoryID << std::endl;
		}

		delete dynamic_cast<ideletable*>(object);
	}

	return 0;
}

/// Creates a document plugin, returning the requested interface if implemented, otherwise NULL (cleans-up on failure)
inline iunknown* create(const k3d::string_t& FactoryName, idocument& Document, const k3d::string_t& Name = k3d::string_t())
{
	if(inode* const object = detail::create_document_plugin(FactoryName, Document, Name))
	{
		object->set_name(Name);
		undoable_new(dynamic_cast<ideletable*>(object), Document);
		Document.nodes().add_nodes(make_collection<inode_collection::nodes_t>(object));

		return object;
	}

	return 0;
}

/// Creates a document plugin, returning the requested interface if implemented, otherwise NULL (cleans-up on failure)
inline iunknown* create(iplugin_factory& Factory, idocument& Document, const k3d::string_t& Name = k3d::string_t())
{
	if(inode* const object = detail::create_document_plugin(Factory, Document, Name))
	{
		object->set_name(Name);
		undoable_new(dynamic_cast<ideletable*>(object), Document);
		Document.nodes().add_nodes(make_collection<inode_collection::nodes_t>(object));

		return object;
	}

	return 0;
}

/// Creates a document plugin, returning the requested interface if implemented, otherwise NULL (cleans-up on failure)
inline iunknown* create(const uuid& FactoryID, idocument& Document, const k3d::string_t& Name = k3d::string_t())
{
	if(inode* const object = detail::create_document_plugin(FactoryID, Document, Name))
	{
		object->set_name(Name);
		undoable_new(dynamic_cast<ideletable*>(object), Document);
		Document.nodes().add_nodes(make_collection<inode_collection::nodes_t>(object));

		return object;
	}

	return 0;
}

} // namespace plugin

} // namespace k3d

#endif // !K3DSDK_PLUGIN_H

