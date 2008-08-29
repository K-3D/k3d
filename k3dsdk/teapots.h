#ifndef K3DSDK_TEAPOTS_H
#define K3DSDK_TEAPOTS_H

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

#include "mesh.h"

namespace k3d
{

namespace teapots
{

/// Gathers the member arrays of a teapots primitive into a convenient package
class primitive
{
public:
	primitive();

	const k3d::typed_array<k3d::matrix4>* matrices;
	const k3d::typed_array<k3d::imaterial*>* materials;
	const k3d::attribute_arrays* uniform_data;
};

/// Returns true iff the given mesh primitive contains a valid teapots primitive
const bool_t validate(const mesh::primitive& GenericPrimitive, primitive& Primitive);

} // namespace teapots

} // namespace k3d

#endif // K3DSDK_TEAPOTS_H

