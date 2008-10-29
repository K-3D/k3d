#ifndef K3DSDK_CONE_H
#define K3DSDK_CONE_H

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

namespace cone
{

/// Gathers the member arrays of a cone primitive into a convenient package
class const_primitive
{
public:
	const_primitive(
		const typed_array<matrix4>& Matrices,
		const typed_array<imaterial*>& Materials,
		const typed_array<double_t>& Heights,
		const typed_array<double_t>& Radii,
		const typed_array<double_t>& SweepAngles,
		const attribute_arrays& ConstantData,
		const attribute_arrays& UniformData,
		const attribute_arrays& VaryingData);

	const typed_array<matrix4>& matrices;
	const typed_array<imaterial*>& materials;
	const typed_array<double_t>& heights;
	const typed_array<double_t>& radii;
	const typed_array<double_t>& sweep_angles;
	const attribute_arrays& constant_data;
	const attribute_arrays& uniform_data;
	const attribute_arrays& varying_data;
};

/// Gathers the member arrays of a cone primitive into a convenient package
class primitive
{
public:
	primitive(
		typed_array<matrix4>& Matrices,
		typed_array<imaterial*>& Materials,
		typed_array<double_t>& Heights,
		typed_array<double_t>& Radii,
		typed_array<double_t>& SweepAngles,
		attribute_arrays& ConstantData,
		attribute_arrays& UniformData,
		attribute_arrays& VaryingData);

	typed_array<matrix4>& matrices;
	typed_array<imaterial*>& materials;
	typed_array<double_t>& heights;
	typed_array<double_t>& radii;
	typed_array<double_t>& sweep_angles;
	attribute_arrays& constant_data;
	attribute_arrays& uniform_data;
	attribute_arrays& varying_data;
};

/// Creates a new cone mesh primitive, returning references to its member arrays.
/// The caller is responsible for the lifetime of the returned object.
primitive* create(mesh& Mesh);

/// Tests the given mesh primitive to see if it is a valid cone primitive, returning references to its member arrays, or NULL.
/// The caller is responsible for the lifetime of the returned object.
const_primitive* validate(const mesh::primitive& GenericPrimitive);
/// Tests the given mesh primitive to see if it is a valid cone primitive, returning references to its member arrays, or NULL.
/// The caller is responsible for the lifetime of the returned object.
primitive* validate(mesh::primitive& GenericPrimitive);

} // namespace cone

} // namespace k3d

#endif // !K3DSDK_CONE_H

