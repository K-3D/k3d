#ifndef K3DSDK_BICUBIC_PATCH_H
#define K3DSDK_BICUBIC_PATCH_H

// K-3D
// Copyright (c) 1995-2009, Timothy M. Shead
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

#include <k3dsdk/mesh.h>

namespace k3d
{

namespace bicubic_patch
{

/// Gathers the member arrays of a bicubic_patch primitive into a convenient package
class const_primitive
{
public:
	const_primitive(
		const mesh::selection_t& PatchSelections,
		const mesh::materials_t& PatchMaterials,
		const mesh::indices_t& PatchPoints,
		const mesh::table_t& ConstantAttributes,
		const mesh::table_t& PatchAttributes,
		const mesh::table_t& ParameterAttributes,
		const mesh::table_t& VertexAttributes
		);

	const mesh::selection_t& patch_selections;
	const mesh::materials_t& patch_materials;
	const mesh::indices_t& patch_points;
	const mesh::table_t& constant_attributes;
	const mesh::table_t& patch_attributes;
	const mesh::table_t& parameter_attributes;
	const mesh::table_t& vertex_attributes;
};

/// Gathers the member arrays of a bicubic_patch primitive into a convenient package
class primitive
{
public:
	primitive(
		mesh::selection_t& PatchSelections,
		mesh::materials_t& PatchMaterials,
		mesh::indices_t& PatchPoints,
		mesh::table_t& ConstantAttributes,
		mesh::table_t& PatchAttributes,
		mesh::table_t& ParameterAttributes,
		mesh::table_t& VertexAttributes
		);

	mesh::selection_t& patch_selections;
	mesh::materials_t& patch_materials;
	mesh::indices_t& patch_points;
	mesh::table_t& constant_attributes;
	mesh::table_t& patch_attributes;
	mesh::table_t& parameter_attributes;
	mesh::table_t& vertex_attributes;
};

/// Creates a new bicubic_patch mesh primitive, returning references to its member arrays.
/// The caller is responsible for the lifetime of the returned object.
primitive* create(mesh& Mesh);

/// Tests the given mesh primitive to see if it is a valid bicubic_patch primitive, returning references to its member arrays, or NULL.
/// The caller is responsible for the lifetime of the returned object.
const_primitive* validate(const mesh& Mesh, const mesh::primitive& GenericPrimitive);
/// Tests the given mesh primitive to see if it is a valid bicubic_patch primitive, returning references to its member arrays, or NULL.
/// The caller is responsible for the lifetime of the returned object.
primitive* validate(const mesh& Mesh, mesh::primitive& GenericPrimitive);
/// Tests the given mesh primitive to see if it is a valid bicubic_patch primitive, returning references to its member arrays, or NULL.
/// The caller is responsible for the lifetime of the returned object.
primitive* validate(const mesh& Mesh, pipeline_data<mesh::primitive>& GenericPrimitive);

} // namespace bicubic_patch

} // namespace k3d

#endif // !K3DSDK_BICUBIC_PATCH_H

