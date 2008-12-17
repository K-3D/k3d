#ifndef K3DSDK_BEZIER_TRIANGLE_PATCH_H
#define K3DSDK_BEZIER_TRIANGLE_PATCH_H

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

/** \file
		\author Ashish Myles (marcianx@gmail.com)
*/

#include "mesh.h"

namespace k3d
{

namespace bezier_triangle_patch
{

/// Gathers the member arrays of a bezier_triangle_patch primitive into a convenient package
class const_primitive
{
public:
	const_primitive(
		const mesh::indices_t& PatchFirstPoints,
		const mesh::orders_t& PatchOrders,
		const mesh::selection_t& PatchSelections,
		const mesh::materials_t& PatchMaterials,
		const mesh::indices_t& PatchPoints,
		const mesh::weights_t& PatchPointWeights,
		const mesh::attribute_arrays_t& ConstantData,
		const mesh::attribute_arrays_t& UniformData,
		const mesh::attribute_arrays_t& VaryingData
		);

	const mesh::indices_t& patch_first_points;
	const mesh::orders_t& patch_orders;
	const mesh::selection_t& patch_selections;
	const mesh::materials_t& patch_materials;
	const mesh::indices_t& patch_points;
	const mesh::weights_t& patch_point_weights;
	const mesh::attribute_arrays_t& constant_data;
	const mesh::attribute_arrays_t& uniform_data;
	const mesh::attribute_arrays_t& varying_data;
};

/// Gathers the member arrays of a bezier_triangle_patch primitive into a convenient package
class primitive
{
public:
	primitive(
		mesh::indices_t& PatchFirstPoints,
		mesh::orders_t& PatchOrders,
		mesh::selection_t& PatchSelections,
		mesh::materials_t& PatchMaterials,
		mesh::indices_t& PatchPoints,
		mesh::weights_t& PatchPointWeights,
		mesh::attribute_arrays_t& ConstantData,
		mesh::attribute_arrays_t& UniformData,
		mesh::attribute_arrays_t& VaryingData
		);

	mesh::indices_t& patch_first_points;
	mesh::orders_t& patch_orders;
	mesh::selection_t& patch_selections;
	mesh::materials_t& patch_materials;
	mesh::indices_t& patch_points;
	mesh::weights_t& patch_point_weights;
	mesh::attribute_arrays_t& constant_data;
	mesh::attribute_arrays_t& uniform_data;
	mesh::attribute_arrays_t& varying_data;
};

/// Creates a new bezier_triangle_patch mesh primitive, returning references to its member arrays.
/// The caller is responsible for the lifetime of the returned object.
primitive* create(mesh& Mesh);

/// Tests the given mesh primitive to see if it is a valid bezier_triangle_patch primitive, returning references to its member arrays, or NULL.
/// The caller is responsible for the lifetime of the returned object.
const_primitive* validate(const mesh::primitive& GenericPrimitive);
/// Tests the given mesh primitive to see if it is a valid bezier_triangle_patch primitive, returning references to its member arrays, or NULL.
/// The caller is responsible for the lifetime of the returned object.
primitive* validate(mesh::primitive& GenericPrimitive);

} // namespace bezier_triangle_patch

} // namespace k3d

#endif // !K3DSDK_BEZIER_TRIANGLE_PATCH_H

