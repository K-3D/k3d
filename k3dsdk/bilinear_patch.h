#ifndef K3DSDK_BILINEAR_PATCH_H
#define K3DSDK_BILINEAR_PATCH_H

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

namespace bilinear_patch
{

/// Gathers the member arrays of a bilinear_patch primitive into a convenient package
class const_primitive
{
public:
	const_primitive(
		const mesh::selection_t& PatchSelections,
		const mesh::materials_t& PatchMaterials,
		const mesh::indices_t& PatchPoints,
		const mesh::attribute_arrays_t& ConstantData,
		const mesh::attribute_arrays_t& UniformData,
		const mesh::attribute_arrays_t& VaryingData
		);

	const mesh::selection_t& patch_selections;
	const mesh::materials_t& patch_materials;
	const mesh::indices_t& patch_points;
	const mesh::attribute_arrays_t& constant_data;
	const mesh::attribute_arrays_t& uniform_data;
	const mesh::attribute_arrays_t& varying_data;
};

/// Gathers the member arrays of a bilinear_patch primitive into a convenient package
class primitive
{
public:
	primitive(
		mesh::selection_t& PatchSelections,
		mesh::materials_t& PatchMaterials,
		mesh::indices_t& PatchPoints,
		mesh::attribute_arrays_t& ConstantData,
		mesh::attribute_arrays_t& UniformData,
		mesh::attribute_arrays_t& VaryingData
		);

	mesh::selection_t& patch_selections;
	mesh::materials_t& patch_materials;
	mesh::indices_t& patch_points;
	mesh::attribute_arrays_t& constant_data;
	mesh::attribute_arrays_t& uniform_data;
	mesh::attribute_arrays_t& varying_data;
};

/// Creates a new bilinear_patch mesh primitive, returning references to its member arrays.
/// The caller is responsible for the lifetime of the returned object.
primitive* create(mesh& Mesh);

/// Tests the given mesh primitive to see if it is a valid bilinear_patch primitive, returning references to its member arrays, or NULL.
/// The caller is responsible for the lifetime of the returned object.
const_primitive* validate(const mesh::primitive& GenericPrimitive);
/// Tests the given mesh primitive to see if it is a valid bilinear_patch primitive, returning references to its member arrays, or NULL.
/// The caller is responsible for the lifetime of the returned object.
primitive* validate(mesh::primitive& GenericPrimitive);
/// Tests the given mesh primitive to see if it is a valid bilinear_patch primitive, returning references to its member arrays, or NULL.
/// The caller is responsible for the lifetime of the returned object.
primitive* validate(pipeline_data<mesh::primitive>& GenericPrimitive);

} // namespace bilinear_patch

} // namespace k3d

#endif // !K3DSDK_BILINEAR_PATCH_H

