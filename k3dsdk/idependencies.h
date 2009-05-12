#ifndef K3DSDK_IDEPENDENCIES_H
#define K3DSDK_IDEPENDENCIES_H

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
		\brief Declares k3d::idependencies, an abstract interface that allows objects to register inter-object dependencies for serialization
		\author Tim Shead (tshead@k-3d.com)
*/

#include "iunknown.h"

namespace k3d
{

// Forward class references
class inode;

/// Abstract interface that allows objects to register dependencies for serialization
/** \note Currently unused */
class idependencies :
	public virtual iunknown
{
public:
	virtual void register_object_dependency(inode& Object, const bool CopyByReference) = 0;

protected:
	idependencies() {}
	idependencies(const idependencies& Other) : iunknown(Other) {}
	idependencies& operator=(const idependencies&) { return *this; }
	virtual ~idependencies() {}
};

} // namespace k3d

#endif // !K3DSDK_IDEPENDENCIES_H
