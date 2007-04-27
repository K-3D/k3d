#ifndef K3DSDK_CREATE_PLUGINS_H
#define K3DSDK_CREATE_PLUGINS_H

// K-3D
// Copyright (c) 1995-2005, Timothy M. Shead
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

#include "data.h"
#include "ideletable.h"
#include "idocument.h"
#include "inode_collection.h"
#include "iplugin_factory.h"

namespace k3d
{

namespace detail
{

iunknown* create_application_plugin(const std::string& Plugin);
iunknown* create_application_plugin(iplugin_factory& Factory);
iunknown* create_application_plugin(const uuid& ClassID);
inode* create_document_plugin(const std::string& Plugin, idocument& Document, const std::string& Name);
inode* create_document_plugin(iplugin_factory& Factory, idocument& Document, const std::string& Name);
inode* create_document_plugin(const uuid& ClassID, idocument& Document, const std::string& Name);

} // namespace detail

/// Creates an application plugin, returning the requested interface if implemented, otherwise NULL (cleans-up on failure)
template<typename interface_t> interface_t* create_plugin(const std::string& Plugin)
{
	if(iunknown* const unknown = detail::create_application_plugin(Plugin))
	{
		if(interface_t* const result = dynamic_cast<interface_t*>(unknown))
		{
			return result;
		}
		else
		{
			log() << error << "Plugin doesn't implement interface: " << Plugin << std::endl;
		}

		delete dynamic_cast<ideletable*>(unknown);
	}

	return 0;
}

/// Creates an application plugin, returning the requested interface if implemented, otherwise NULL (cleans-up on failure)
template<typename interface_t> interface_t* create_plugin(iplugin_factory& Factory)
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
template<typename interface_t> interface_t* create_plugin(const uuid& ClassID)
{
	if(iunknown* const unknown = detail::create_application_plugin(ClassID))
	{
		if(interface_t* const result = dynamic_cast<interface_t*>(unknown))
		{
			return result;
		}
		else
		{
			log() << error << "Plugin doesn't implement interface: " << ClassID << std::endl;
		}

		delete dynamic_cast<ideletable*>(unknown);
	}

	return 0;
}

/// Creates an application plugin, returning the requested interface if implemented, otherwise NULL (cleans-up on failure)
inline iunknown* create_plugin(const std::string& Plugin)
{
	return detail::create_application_plugin(Plugin);
}

/// Creates an application plugin, returning the requested interface if implemented, otherwise NULL (cleans-up on failure)
inline iunknown* create_plugin(iplugin_factory& Factory)
{
	return detail::create_application_plugin(Factory);
}

/// Creates an application plugin, returning the requested interface if implemented, otherwise NULL (cleans-up on failure)
inline iunknown* create_plugin(const uuid& ClassID)
{
	return detail::create_application_plugin(ClassID);
}

/// Creates a document plugin, returning the requested interface if implemented, otherwise NULL (cleans-up on failure)
template<typename interface_t> interface_t* create_plugin(const std::string& Plugin, idocument& Document, const std::string& Name = std::string())
{
	if(inode* const object = detail::create_document_plugin(Plugin, Document, Name))
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
			log() << error << "Plugin doesn't implement interface: " << Plugin << std::endl;
		}

		delete dynamic_cast<ideletable*>(object);
	}

	return 0;
}

/// Creates a document plugin, returning the requested interface if implemented, otherwise NULL (cleans-up on failure)
template<typename interface_t> interface_t* create_plugin(iplugin_factory& Factory, idocument& Document, const std::string& Name = std::string())
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
template<typename interface_t> interface_t* create_plugin(const uuid& ClassID, idocument& Document, const std::string& Name = std::string())
{
	if(inode* const object = detail::create_document_plugin(ClassID, Document, Name))
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
			log() << error << "Plugin doesn't implement interface: " << ClassID << std::endl;
		}

		delete dynamic_cast<ideletable*>(object);
	}

	return 0;
}

/// Creates a document plugin, returning the requested interface if implemented, otherwise NULL (cleans-up on failure)
inline iunknown* create_plugin(const std::string& Plugin, idocument& Document, const std::string& Name = std::string())
{
	if(inode* const object = detail::create_document_plugin(Plugin, Document, Name))
	{
		object->set_name(Name);
		undoable_new(dynamic_cast<ideletable*>(object), Document);
		Document.nodes().add_nodes(make_collection<inode_collection::nodes_t>(object));

		return object;
	}

	return 0;
}

/// Creates a document plugin, returning the requested interface if implemented, otherwise NULL (cleans-up on failure)
inline iunknown* create_plugin(iplugin_factory& Factory, idocument& Document, const std::string& Name = std::string())
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
inline iunknown* create_plugin(const uuid& ClassID, idocument& Document, const std::string& Name = std::string())
{
	if(inode* const object = detail::create_document_plugin(ClassID, Document, Name))
	{
		object->set_name(Name);
		undoable_new(dynamic_cast<ideletable*>(object), Document);
		Document.nodes().add_nodes(make_collection<inode_collection::nodes_t>(object));

		return object;
	}

	return 0;
}

} // namespace k3d

#endif // !K3DSDK_CREATE_PLUGINS_H


