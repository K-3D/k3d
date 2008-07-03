#ifndef K3DSDK_NODE_CHANGE_SIGNAL_H
#define K3DSDK_NODE_CHANGE_SIGNAL_H

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

#include "inode_change_signal.h"
#include "signal_slots.h"

namespace k3d
{

/// Provides a mixin-class implementation of k3d::inode_change_signal
template<typename derived_t>
class node_change_signal :
	public inode_change_signal
{
public:
	sigc::connection connect_node_changed_signal(const sigc::slot<void, ihint*>& Slot)
	{
		return m_node_changed_signal.connect(Slot);
	}

protected:
	signal::loop_safe_slot<sigc::signal<void, ihint*> > make_node_change_slot()
	{
		return signal::make_loop_safe_slot(m_node_changed_signal);
	}

private:
	sigc::signal<void, ihint*> m_node_changed_signal;
};

} // namespace k3d

#endif // K3DSDK_NODE_CHANGE_SIGNAL_H

