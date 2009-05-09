#ifndef K3DSDK_INODE_CHANGE_SIGNAL_H
#define K3DSDK_INODE_CHANGE_SIGNAL_H

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
	\author Tim Shead (tshead@k-3d.com)
*/

#include "ihint.h"
#include "signal_system.h"

namespace k3d
{

class inode;

/// Abstract interface for nodes that broadcast a change signal to observers when their state is modified
class inode_change_signal :
	public virtual iunknown
{
public:
	/// Connects a slot to a signal that will be emitted whenever the node state is modified, including an optional "hint" that describes the change.
	virtual sigc::connection connect_node_changed_signal(const sigc::slot<void, ihint*>& Slot) = 0;

protected:
	inode_change_signal() {}
	inode_change_signal(const inode_change_signal& Other) : iunknown(Other) {}
	inode_change_signal& operator = (const inode_change_signal&) { return *this; }
	virtual ~inode_change_signal() {}
};

} // namespace k3d

#endif // !K3DSDK_INODE_CHANGE_SIGNAL_H

