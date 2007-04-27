#ifndef K3DSDK_IMATERIAL_CLIENT_H
#define K3DSDK_IMATERIAL_CLIENT_H

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
		\brief Declares k3d::imaterial_client, an abstract interface for nodes that reference a surface material
		\author Tim Shead (tshead@k-3d.com)
*/

#include "iunknown.h"

namespace k3d
{

// Forward class references ...
class imaterial;

/// Abstract interface for clients of k3d::imaterial objects
class imaterial_client :
	public virtual iunknown
{
public:
	/// Returns the assigned material (could return NULL if none assigned)
	virtual imaterial* material() = 0;
	/// Sets the assigned material (NULL is allowed, for none assigned)
	virtual void set_material(imaterial* const Material) = 0;

protected:
	imaterial_client() {}
	imaterial_client(const imaterial_client& RHS) {}
	imaterial_client& operator=(const imaterial_client& RHS) { return *this; }
	virtual ~imaterial_client() {}
};

} // namespace k3d

#endif // K3DSDK_IMATERIAL_CLIENT_H

