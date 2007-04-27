#ifndef K3DSDK_INODE_CHANGE_SIGNAL_H
#define K3DSDK_INODE_CHANGE_SIGNAL_H

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
		\brief Declares inode_change_signal, an abstract interface for a name-value pair with fixed type
		\author Tim Shead (tshead@k-3d.com)
*/

#include "iunknown.h"
#include "signal_system.h"

namespace k3d
{

class inode;

/// Abstract interface for nodes that broadcast a change signal to observers when their state is modified
class inode_change_signal :
	public virtual iunknown
{
public:
	/// Defines a signal that will be emitted whenever the object state is modified, including an optional "hint" that describes the change.
	typedef sigc::signal<void, iunknown*> node_changed_signal_t;
	virtual node_changed_signal_t& node_changed_signal() = 0;

protected:
	inode_change_signal() {}
	inode_change_signal(const inode_change_signal&) {}
	inode_change_signal& operator = (const inode_change_signal&) { return *this; }
	virtual ~inode_change_signal() {}
};

} // namespace k3d

#endif // K3DSDK_INODE_CHANGE_SIGNAL_H

