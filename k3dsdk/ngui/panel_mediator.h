#ifndef K3DSDK_NGUI_PANEL_MEDIATOR_H
#define K3DSDK_NGUI_PANEL_MEDIATOR_H

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

#include <k3dsdk/mesh.h>
#include <k3dsdk/nodes.h>
#include <k3dsdk/selection.h>

#include <iosfwd>
#include <vector>

namespace k3d
{

class idocument;
class inode;

namespace ngui
{

namespace panel
{

/// Mediates interaction between panels on a per-document basis.
class mediator
{
public:
	mediator(idocument& Document);

	/// Called to "focus" the given node.  Typically, this means that the node will be displayed
	/// in one-or-more panels.
	void set_focus(inode& Node);

	/// Connects a slot to a signal that will be emitted whenever a node receives the "focus".
	sigc::connection connect_focus_node_signal(const sigc::slot<bool_t, inode*>& Slot);

private:
	class implementation;
	implementation& internal;
};

} // namespace panel

} // namespace ngui

} // namespace k3d

#endif // !K3DSDK_NGUI_PANEL_MEDIATOR_H

