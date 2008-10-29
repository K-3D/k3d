#ifndef K3DSDK_IMESH_STORAGE_H
#define K3DSDK_IMESH_STORAGE_H

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
		\brief Declares imesh_storage, an interface for objects that can provide persistent storage of meshes
		\author Tim Shead (tshead@k-3d.com)
*/

#include "iunknown.h"

namespace k3d
{

class mesh;

/// Abstract interface for objects that can provide persistent storage of meshes (the FrozenMesh object being the canonical example)
class imesh_storage :
	public virtual iunknown
{
public:
	/// Replaces any existing stored mesh with the given object, assuming responsibility for its lifetime
	virtual void reset_mesh(mesh* const Mesh) = 0;

protected:
	imesh_storage() {}
	imesh_storage(const imesh_storage&) {}
	imesh_storage& operator=(const imesh_storage&) { return *this; }
	virtual ~imesh_storage() {}
};

} // namespace k3d

#endif // !K3DSDK_IMESH_STORAGE_H

