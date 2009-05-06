#ifndef K3DSDK_INODE_COLLECTION_H
#define K3DSDK_INODE_COLLECTION_H

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
		\brief Declares inode_collection, a container of inode instances
		\author Tim Shead (tshead@k-3d.com)
*/

#include "iunknown.h"
#include "signal_system.h"

#include <vector>

namespace k3d
{

class inode;

/// Abstract interface for a collection of document nodes
class inode_collection :
	public virtual iunknown
{
public:
	/// Defines a collection of nodes
	typedef std::vector<inode*> nodes_t;

	/// Adds new nodes to the collection
	virtual void add_nodes(const nodes_t& Objects) = 0;
	/// Returns the set of all nodes in the collection
	virtual const nodes_t& collection() = 0;
	/// Removes nodes from the collection
	virtual void remove_nodes(const nodes_t& Objects) = 0;

	/// Defines a signal that will be emitted whenever nodes are added to the collection
	typedef sigc::signal<void, const nodes_t&> add_nodes_signal_t;
	virtual add_nodes_signal_t& add_nodes_signal() = 0;
	/// Defines a signal that will be emitted whenever nodes are removed from the collection
	typedef sigc::signal<void, const nodes_t&> remove_nodes_signal_t;
	virtual remove_nodes_signal_t& remove_nodes_signal() = 0;
	/// Defines a signal that will be emitted whenever an node is renamed
	typedef sigc::signal<void, inode*> rename_node_signal_t;
	virtual rename_node_signal_t& rename_node_signal() = 0;

protected:
	inode_collection() {}
	inode_collection(const inode_collection& Other) : iunknown(Other) {}
	inode_collection& operator = (const inode_collection&) { return *this; }
	virtual ~inode_collection() {}
};

} // namespace

#endif // !K3DSDK_INODE_COLLECTION_H


