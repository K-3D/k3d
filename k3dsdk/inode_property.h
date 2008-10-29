#ifndef K3DSDK_INODE_PROPERTY_H
#define K3DSDK_INODE_PROPERTY_H

// K-3D
// Copyright (c) 1995-2004, Timothy M. Shead
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
		\brief Declares inode_property, an abstract interface for discovering information about a property that references a document node
		\author Tim Shead (tshead@k-3d.com)
*/

#include "iunknown.h"
#include <string>
#include <vector>

namespace k3d
{

/// Abstract interface for discovering information about a property that references another node
class inode_property :
	public virtual iunknown
{
public:
	virtual bool property_allow_none() = 0;
	virtual bool property_allow(k3d::iplugin_factory& Factory) = 0;
	virtual bool property_allow(inode& Object) = 0;

protected:
	inode_property() {}
	inode_property(const inode_property&) {}
	inode_property& operator = (const inode_property&) { return *this; }
	virtual ~inode_property() {}
};

} // namespace k3d

#endif // !K3DSDK_INODE_PROPERTY_H

