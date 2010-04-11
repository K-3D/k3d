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

#include <k3dsdk/uuid.h>
#include <k3dsdk/imetadata.h>
#include <k3dsdk/inode_collection.h>
#include <k3dsdk/inode.h>

#include <k3dsdk/utility.h>

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

/// Returns the node that owns the given property (could return NULL)
/** \deprecated Use methods in k3d::node instead */
inode* find_node(inode_collection& Nodes, iproperty& Property);

/// Returns a unique node name based on the one supplied
const std::string unique_name(inode_collection& Nodes, const std::string& Name);

/// Deletes a collection of nodes, cleaning-up all references and resources (this operation is undo-able, if state change recording is in effect)
void delete_nodes(idocument& Document, const nodes_t& Nodes);

/// Marks the given nodes as visible in the given node collection sink
void make_visible(const nodes_t& Nodes, inode_collection_sink& NodeCollectionSink);

} // namespace k3d

#endif // !K3DSDK_NODES_H

