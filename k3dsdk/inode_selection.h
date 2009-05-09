#ifndef K3DSDK_INODE_SELECTION_H
#define K3DSDK_INODE_SELECTION_H

// K-3D
// Copyright (c) 1995-2008, Timothy M. Shead
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
		\brief Declares inode_selection, an abstract interface for objects storing a list of selected nodes with their selection weight
		\author Bart Janssens (bart.janssens@lid.kviv.be)
*/

#include <list>

#include "iunknown.h"
#include "signal_system.h"
#include "types.h"

namespace k3d
{

class inode;
class ihint;

/// Abstract interface for objects that store document node selections and their weights
class inode_selection :
	public virtual iunknown
{
public:
	/// Storage for a list of selected nodes
	/**
	 * Note: we use a list for fast removal at any position. Fast random access is not needed
	 */
	typedef std::list<inode*> selected_nodes_t;

	/// Selects a node
	/**
	 * \param Node the node to select
	 * \param Weight The selection weight. Setting this to 0 removes the node from the selection
	 */
	virtual void select(inode& Node, const double_t Weight) = 0;

	/// Return the selection weight of the given node
	virtual double_t selection_weight(inode& Node) = 0;

	/// List of selected nodes, in the order they were selected
	virtual const selected_nodes_t selected_nodes() = 0;
	
	/// Deselect all nodes
	virtual void deselect_all() = 0;
	
	/// Changed signal emitted when the selection changed
	typedef sigc::signal<void, ihint*> changed_signal_t;
	virtual changed_signal_t& selection_changed_signal() = 0;

protected:
	inode_selection() {}
	inode_selection(const inode_selection& Other) : iunknown(Other) {}
	inode_selection& operator=(const inode_selection&) { return *this; }
	virtual ~inode_selection() {}
};

} // namespace k3d

#endif // !K3DSDK_INODE_SELECTION_H
