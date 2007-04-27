#ifndef K3DSDK_ICOMMAND_TREE_H
#define K3DSDK_ICOMMAND_TREE_H

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
		\brief Declares icommand_tree, an abstract interface for a directed graph of icommand_tree objects
		\author Tim Shead (tshead@k-3d.com)
*/

#include "icommand_node.h"
#include "iunknown.h"
#include "signal_system.h"

#include <vector>

namespace k3d
{

/// Abstract interface for a directed graph of icommand_tree objects
class icommand_tree :
	public virtual iunknown
{
public:
	/// Adds a node to the tree, setting its name and parent node (top-level nodes have a NULL parent)
	virtual void add(icommand_node& Node, const std::string& Name, icommand_node* const Parent) = 0;
	/// Removes a node from the tree
	virtual void remove(icommand_node& Node) = 0;
	/// Returns a node's name
	virtual const std::string name(icommand_node& Node) = 0;
	/// Returns a node's parent (could be NULL if it's a root node)
	virtual icommand_node* parent(icommand_node& Node) = 0;

	/// Defines a collection of command nodes
	typedef std::vector<icommand_node*> nodes_t;
	virtual const nodes_t children(icommand_node* const Node) = 0;

	/// Connects a slot to a signal that will be emitted whenever the tree is modified
	virtual sigc::connection connect_changed_signal(const sigc::slot<void>& Slot) = 0;

	/// Defines a signal for distributing K-3D commands to recorders
	typedef sigc::signal<void, icommand_node&, const icommand_node::type, const std::string&, const std::string&> command_signal_t;
	/// Returns a signal for distributing K-3D commands to recorders
	virtual command_signal_t& command_signal() = 0;

protected:
	icommand_tree() {}
	icommand_tree(const icommand_tree&) {}
	icommand_tree& operator=(const icommand_tree&) { return *this; }
	virtual ~icommand_tree() {}
};

} // namespace k3d

#endif // K3DSDK_ICOMMAND_TREE_H


