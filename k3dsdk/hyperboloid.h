#ifndef K3DSDK_HYPERBOLOID_H
#define K3DSDK_HYPERBOLOID_H

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

namespace hyperboloid
{

/// Gathers the member arrays of a hyperboloid primitive into a convenient package
class const_primitive
{
public:
	const_primitive(
		const typed_array<matrix4>& Matrices,
		const typed_array<imaterial*>& Materials,
		const typed_array<point3>& StartPoints,
		const typed_array<point3>& EndPoints,
		const typed_array<double_t>& SweepAngles,
		const attribute_arrays& ConstantData,
		const attribute_arrays& UniformData,
		const attribute_arrays& VaryingData);

	const typed_array<matrix4>& matrices;
	const typed_array<imaterial*>& materials;
	const typed_array<point3>& start_points;
	const typed_array<point3>& end_points;
	const typed_array<double_t>& sweep_angles;
	const attribute_arrays& constant_data;
	const attribute_arrays& uniform_data;
	const attribute_arrays& varying_data;
};

/// Gathers the member arrays of a hyperboloid primitive into a convenient package
class primitive
{
public:
	primitive(
		typed_array<matrix4>& Matrices,
		typed_array<imaterial*>& Materials,
		typed_array<point3>& StartPoints,
		typed_array<point3>& EndPoints,
		typed_array<double_t>& SweepAngles,
		attribute_arrays& ConstantData,
		attribute_arrays& UniformData,
		attribute_arrays& VaryingData);

	typed_array<matrix4>& matrices;
	typed_array<imaterial*>& materials;
	typed_array<point3>& start_points;
	typed_array<point3>& end_points;
	typed_array<double_t>& sweep_angles;
	attribute_arrays& constant_data;
	attribute_arrays& uniform_data;
	attribute_arrays& varying_data;
};

/// Creates a new hyperboloid mesh primitive, returning references to its member arrays.
/// The caller is responsible for the lifetime of the returned object.
primitive* create(mesh& Mesh);

/// Tests the given mesh primitive to see if it is a valid hyperboloid primitive, returning references to its member arrays, or NULL.
/// The caller is responsible for the lifetime of the returned object.
const_primitive* validate(const mesh::primitive& GenericPrimitive);
/// Tests the given mesh primitive to see if it is a valid hyperboloid primitive, returning references to its member arrays, or NULL.
/// The caller is responsible for the lifetime of the returned object.
primitive* validate(mesh::primitive& GenericPrimitive);

} // namespace hyperboloid

} // namespace k3d

#endif // !K3DSDK_HYPERBOLOID_H

