#ifndef K3DSDK_BLOBBY_H
#define K3DSDK_BLOBBY_H

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

namespace blobby
{

/// Predefined primitive types from the RenderMan Specification
enum primitive_type
{
	CONSTANT = 1000,
	ELLIPSOID = 1001,
	SEGMENT = 1002,
};

/// Predefined operator types from the RenderMan Specification
enum operator_type
{
	ADD = 0,
	MULTIPLY = 1,
	MAXIMUM = 2,
	MINIMUM = 3,
	SUBTRACT = 4,
	DIVIDE = 5,
	NEGATE = 6,
	IDENTITY = 7,
};

/// Gathers the member arrays of a blobby primitive into a convenient package
class const_primitive
{
public:
	const_primitive(
		const mesh::indices_t& FirstPrimitives,
		const mesh::counts_t& PrimitiveCounts,
		const mesh::indices_t& FirstOperators,
		const mesh::counts_t& OperatorCounts,
		const mesh::materials_t& Materials,
		const typed_array<int32_t>& Primitives,
		const mesh::indices_t& PrimitiveFirstFloats,
		const mesh::counts_t& PrimitiveFloatCounts,
		const typed_array<int32_t>& Operators,
		const mesh::indices_t& OperatorFirstOperands,
		const mesh::counts_t& OperatorOperandCounts,
		const mesh::doubles_t& Floats,
		const mesh::indices_t& Operands,
		const mesh::attribute_arrays_t& ConstantData,
		const mesh::attribute_arrays_t& UniformData,
		const mesh::attribute_arrays_t& VaryingData,
		const mesh::attribute_arrays_t& VertexData
		);

	const mesh::indices_t& first_primitives;
	const mesh::counts_t& primitive_counts;
	const mesh::indices_t& first_operators;
	const mesh::counts_t& operator_counts;
	const mesh::materials_t& materials;
	const typed_array<int32_t>& primitives;
	const mesh::indices_t& primitive_first_floats;
	const mesh::counts_t& primitive_float_counts;
	const typed_array<int32_t>& operators;
	const mesh::indices_t& operator_first_operands;
	const mesh::counts_t& operator_operand_counts;
	const mesh::doubles_t& floats;
	const mesh::indices_t& operands;
	const mesh::attribute_arrays_t& constant_data;
	const mesh::attribute_arrays_t& uniform_data;
	const mesh::attribute_arrays_t& varying_data;
	const mesh::attribute_arrays_t& vertex_data;
};

/// Gathers the member arrays of a blobby primitive into a convenient package
class primitive
{
public:
	primitive(
		mesh::indices_t& FirstPrimitives,
		mesh::counts_t& PrimitiveCounts,
		mesh::indices_t& FirstOperators,
		mesh::counts_t& OperatorCounts,
		mesh::materials_t& Materials,
		typed_array<int32_t>& Primitives,
		mesh::indices_t& PrimitiveFirstFloats,
		mesh::counts_t& PrimitiveFloatCounts,
		typed_array<int32_t>& Operators,
		mesh::indices_t& OperatorFirstOperands,
		mesh::counts_t& OperatorOperandCounts,
		mesh::doubles_t& Floats,
		mesh::indices_t& Operands,
		mesh::attribute_arrays_t& ConstantData,
		mesh::attribute_arrays_t& UniformData,
		mesh::attribute_arrays_t& VaryingData,
		mesh::attribute_arrays_t& VertexData
		);

	mesh::indices_t& first_primitives;
	mesh::counts_t& primitive_counts;
	mesh::indices_t& first_operators;
	mesh::counts_t& operator_counts;
	mesh::materials_t& materials;
	typed_array<int32_t>& primitives;
	mesh::indices_t& primitive_first_floats;
	mesh::counts_t& primitive_float_counts;
	typed_array<int32_t>& operators;
	mesh::indices_t& operator_first_operands;
	mesh::counts_t& operator_operand_counts;
	mesh::doubles_t& floats;
	mesh::indices_t& operands;
	mesh::attribute_arrays_t& constant_data;
	mesh::attribute_arrays_t& uniform_data;
	mesh::attribute_arrays_t& varying_data;
	mesh::attribute_arrays_t& vertex_data;
};

/// Creates a new blobby mesh primitive, returning references to its member arrays.
/// The caller is responsible for the lifetime of the returned object.
primitive* create(mesh& Mesh);

/// Tests the given mesh primitive to see if it is a valid blobby primitive, returning references to its member arrays, or NULL.
/// The caller is responsible for the lifetime of the returned object.
const_primitive* validate(const mesh::primitive& GenericPrimitive);
/// Tests the given mesh primitive to see if it is a valid blobby primitive, returning references to its member arrays, or NULL.
/// The caller is responsible for the lifetime of the returned object.
primitive* validate(mesh::primitive& GenericPrimitive);

} // namespace blobby

} // namespace k3d

#endif // !K3DSDK_BLOBBY_H

