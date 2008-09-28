#ifndef K3DSDK_TEAPOT_H
#define K3DSDK_TEAPOT_H

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

namespace teapot
{

/// Gathers the member arrays of a teapot primitive into a convenient package
class primitive
{
public:
	primitive(const typed_array<matrix4>&, const typed_array<imaterial*>&, const attribute_arrays&, const attribute_arrays&);

	const typed_array<matrix4>& matrices;
	const typed_array<imaterial*>& materials;
	const attribute_arrays& constant_data;
	const attribute_arrays& uniform_data;
};

/// Gathers the member arrays of a teapot primitive into a convenient package
class writable_primitive
{
public:
	writable_primitive(typed_array<matrix4>&, typed_array<imaterial*>&, attribute_arrays&, attribute_arrays&);

	typed_array<matrix4>& matrices;
	typed_array<imaterial*>& materials;
	attribute_arrays& constant_data;
	attribute_arrays& uniform_data;
};

/// Creates a new teapot mesh primitive, returning references to its member arrays.
/// The caller is responsible for the lifetime of the returned object.
writable_primitive* create(mesh& Mesh);

/// Tests the given mesh primitive to see if it is a valid teapot primitive, returning references to its member arrays, or NULL.
/// The caller is responsible for the lifetime of the returned object.
primitive* validate(const mesh::primitive& GenericPrimitive);
/// Tests the given mesh primitive to see if it is a valid teapot primitive, returning references to its member arrays, or NULL.
/// The caller is responsible for the lifetime of the returned object.
writable_primitive* validate(mesh::primitive& GenericPrimitive);

typedef double_t points_array_t[306][3];
typedef uint_t patches_array_t[32][16];

/// Returns an array of points which can be used to recreate a Newell Teapot from scratch.
const points_array_t& points();
/// Returns an array of bicubic patch point-indices which can be used to recreate a Newell Teapot from scratch.
const patches_array_t& patches();

} // namespace teapot

} // namespace k3d

#endif // K3DSDK_TEAPOT_H

