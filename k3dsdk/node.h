#ifndef K3DSDK_NODE_H
#define K3DSDK_NODE_H

// K-3D
// Copyright (c) 1995-2009, Timothy M. Shead
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

#include <k3dsdk/data.h>
#include <k3dsdk/idocument.h>
#include <k3dsdk/inode.h>
#include <k3dsdk/inode_collection.h>
#include <k3dsdk/ipersistent.h>
#include <k3dsdk/metadata.h>
#include <k3dsdk/persistent_property_collection.h>
#include <k3dsdk/property_collection.h>
#include <k3dsdk/result.h>

namespace k3d
{

class iplugin_factory;

/// Implements the minimum functionality required of any K-3D node.  You should derive document plugins from node and override
/// the default method implementations and/or implement additional interfaces as-needed.
class node :
	public inode,
	public ipersistent,
	public property_collection,
	public persistent_property_collection,
	public metadata::storage,
	public sigc::trackable
{
public:
	node(iplugin_factory& Factory, idocument& Document);
	virtual ~node();

	void set_name(const std::string Name);
	const std::string name();
	idocument& document();
	iplugin_factory& factory();
	deleted_signal_t& deleted_signal();
	name_changed_signal_t& name_changed_signal();

	void save(xml::element& Element, const ipersistent::save_context& Context);
	void load(xml::element& Element, const ipersistent::load_context& Context);

	/// Returns the set of all nodes for the given document
	static const std::vector<inode*> lookup(idocument& Document);

	/// Returns the set of nodes that match a specific factory ID (could return empty set!)
	static const std::vector<inode*> lookup(idocument& Document, const uuid FactoryID);

	/// Returns the set of nodes that match the given name (be prepared to handle zero, one, or many results)
	static const std::vector<inode*> lookup(idocument& Document, const string_t& NodeName);

	/// Returns the set of nodes that match the given metadata name and value
	static const std::vector<inode*> lookup(idocument& Document, const string_t& MetaName, const string_t& MetaValue);

	/// Returns the set of nodes that implement the requested interface type (could return an empty set).
	template<typename interface_t>
	static const std::vector<interface_t*> lookup(idocument& Document)
	{
		std::vector<interface_t*> result;
		const std::vector<inode*> nodes = lookup(Document);
		const std::vector<inode*>::const_iterator end(nodes.end());
		for(std::vector<inode*>::const_iterator node = nodes.begin(); node != end; ++node)
		{
			if(interface_t* const requested = dynamic_cast<interface_t*>(*node))
				result.push_back(requested);
		}
		return result;
	}

	/// Returns the set of nodes that implement the requested interface type and match the given factory ID.
	template<typename interface_t>
	static const std::vector<interface_t*> lookup(idocument& Document, const uuid FactoryID)
	{
		std::vector<interface_t*> result;
		const std::vector<inode*> nodes = lookup(Document, FactoryID);
		const std::vector<inode*>::const_iterator end(nodes.end());
		for(std::vector<inode*>::const_iterator node = nodes.begin(); node != end; ++node)
		{
			if(interface_t* const requested = dynamic_cast<interface_t*>(*node))
				result.push_back(requested);
		}
		return result;
	}

	/// Returns the set of nodes that implement the requested interface type and match the given metadata name and value.	
	template<typename interface_t>
	static const std::vector<interface_t*> lookup(idocument& Document, const string_t& MetaName, const string_t& MetaValue)
	{
		std::vector<interface_t*> result;
		const std::vector<inode*> nodes = lookup(Document, MetaName, MetaValue);
		const std::vector<inode*>::const_iterator end(nodes.end());
		for(std::vector<inode*>::const_iterator node = nodes.begin(); node != end; ++node)
		{
			if(interface_t* const requested = dynamic_cast<interface_t*>(*node))
				result.push_back(requested);
		}
		return result;
	}

private:
	void on_deleted();

	/// Stores the factory that created this node
	iplugin_factory& m_factory;
	/// Stores the Document that owns this node
	idocument& m_document;
	/// Stores the name for this node
	k3d_data(std::string, data::immutable_name, data::change_signal, data::with_undo, data::local_storage, data::no_constraint, data::writable_property, data::no_serialization) m_name;
	/// Used to signal observers when this node is deleted
	deleted_signal_t m_deleted_signal;
	/// Used to signal observers when this node's name changes
	name_changed_signal_t m_name_changed_signal;
};

} // namespace k3d

#endif // !K3DSDK_NODE_H

