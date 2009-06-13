#ifndef K3DSDK_NODES_H
#define K3DSDK_NODES_H

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

#include "uuid.h"
#include "imetadata.h"
#include "inode_collection.h"
#include "inode.h"

#include "utility.h"

#include <vector>

#ifdef interface
#undef interface
#endif // interface

namespace k3d
{

// Forward declarations
class idocument;
class iproperty;
class inode_collection_sink;

/// Defines a collection of nodes
typedef inode_collection::nodes_t nodes_t;

/// Returns the node that matches the given name (returns NULL if no node matches or if more-than-one node matches)
/** \deprecated Use methods in k3d::node instead */
inode* find_node(inode_collection& Nodes, const std::string& NodeName);

/// Returns the node that owns the given property (could return NULL)
/** \deprecated Use methods in k3d::node instead */
inode* find_node(inode_collection& Nodes, iproperty& Property);

/// Returns the set of nodes that match a specific uuid (could return empty set!)
/** \deprecated Use methods in k3d::node instead */
const nodes_t find_nodes(inode_collection& Nodes, const uuid ClassID);

/// Returns the set of nodes that match the given name (be prepared to handle zero, one, or many results)
/** \deprecated Use methods in k3d::node instead */
const nodes_t find_nodes(inode_collection& Nodes, const std::string& NodeName);

/// Returns the set of nodes that implement a specific interface type (could return empty set!)
/** \deprecated Use methods in k3d::node instead */
template<typename interface_t>
const nodes_t find_nodes(inode_collection& Nodes)
{
	nodes_t nodes;

	const nodes_t::const_iterator end(Nodes.collection().end());
	for(nodes_t::const_iterator node = Nodes.collection().begin(); node != end; ++node)
	{
		if(dynamic_cast<interface_t*>(*node))
			nodes.insert(nodes.end(), *node);
	}

	return nodes;
}

/** \deprecated Use methods in k3d::node instead */
template<typename interface_t>
const nodes_t find_nodes(inode_collection& Nodes, const string_t& MetaName, const string_t& MetaValue)
{
	nodes_t meta_nodes = find_nodes<imetadata>(Nodes);
	nodes_t nodes;
	for(nodes_t::iterator node = meta_nodes.begin(); node != meta_nodes.end(); ++node)
	{
		imetadata* meta_node = dynamic_cast<imetadata*>(*node);
		imetadata::metadata_t metadata = meta_node->get_metadata();
		imetadata::metadata_t::iterator pair = metadata.find(MetaName);
		if(pair != metadata.end() && pair->second == MetaValue)
		{
			if(dynamic_cast<interface_t*>(*node))
				nodes.push_back(*node);
		}
	}
	
	return nodes;
}

/// Returns a unique node name based on the one supplied
const std::string unique_name(inode_collection& Nodes, const std::string& Name);

/// Deletes a collection of nodes, cleaning-up all references and resources (this operation is undo-able, if state change recording is in effect)
void delete_nodes(idocument& Document, const nodes_t& Nodes);

/// Marks the given nodes as visible in the given node collection sink
void make_visible(const nodes_t& Nodes, inode_collection_sink& NodeCollectionSink);

} // namespace k3d

#endif // !K3DSDK_NODES_H

