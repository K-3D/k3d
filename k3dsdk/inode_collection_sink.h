#ifndef K3DSDK_INODE_COLLECTION_SINK_H
#define K3DSDK_INODE_COLLECTION_SINK_H

// K-3D
// Copyright (c) 1995-2007, Timothy M. Shead
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

#include "iunknown.h"

namespace k3d
{

class iproperty;

/// Abstract interface for objects that have one-or-more inode_collection_property properties.
///
/// Typically, inode_collection_sink is implemented by "render engine" objects that maintain
/// explicit lists of "visible" nodes, "enabled" lights, etc.
class inode_collection_sink :
	public virtual iunknown
{
public:
	/// Defines a collection of properties
	typedef std::vector<iproperty*> properties_t;

	/// Returns a set of inode_collection_property properties
	virtual const properties_t node_collection_properties() = 0;

protected:
	inode_collection_sink() {}
	inode_collection_sink(const inode_collection_sink&) {}
	inode_collection_sink& operator=(const inode_collection_sink&) { return *this; }
	virtual ~inode_collection_sink() {}
};

} // namespace k3d

#endif // K3DSDK_INODE_COLLECTION_SINK_H

