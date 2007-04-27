#ifndef K3DSDK_INODE_H
#define K3DSDK_INODE_H

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
		\brief Declares inode, an abstract interface which MUST be implemented by ALL document nodes
		\author Tim Shead (tshead@k-3d.com)
*/

#include "iunknown.h"
#include "signal_system.h"

#include <string>

namespace k3d
{

class idocument;
class iplugin_factory;

/////////////////////////////////////////////////////////////////////////////
// inode

/// Abstract interface which MUST be implemented by ALL document nodes
class inode :
	public virtual iunknown
{
public:
	/// Sets the node name (could fail or be overridden)
	virtual void set_name(const std::string Name) = 0;
	/// Returns the node name
	virtual const std::string name() = 0;

	/// Returns a reference to the factory that created this node
	virtual iplugin_factory& factory() = 0;
	/// Returns a reference to the document that owns this node
	virtual idocument& document() = 0;

	/// Defines a signal that will be emitted iff the node is deleted
	typedef sigc::signal<void> deleted_signal_t;
	virtual deleted_signal_t& deleted_signal() = 0;

	/// Defines a signal that will be emitted if the node name changes (including via undo / redo)
	typedef sigc::signal<void> name_changed_signal_t;
	virtual name_changed_signal_t& name_changed_signal() = 0;

protected:
	inode() {}
	inode(const inode&) {}
	inode& operator=(const inode&) { return *this; }
	virtual ~inode() {}
};

} // namespace k3d

#endif // !K3DSDK_INODE_H


