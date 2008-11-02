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
		const uint_t_array& FirstPrimitives,
		const uint_t_array& PrimitiveCounts,
		const uint_t_array& FirstOperators,
		const uint_t_array& OperatorCounts,
		const typed_array<imaterial*>& Materials,
		const typed_array<int32_t>& Primitives,
		const uint_t_array& PrimitiveFirstFloats,
		const uint_t_array& PrimitiveFloatCounts,
		const typed_array<int32_t>& Operators,
		const uint_t_array& OperatorFirstOperands,
		const uint_t_array& OperatorOperandCounts,
		const typed_array<double_t>& Floats,
		const uint_t_array& Operands,
		const attribute_arrays& ConstantData,
		const attribute_arrays& UniformData,
		const attribute_arrays& VaryingData,
		const attribute_arrays& VertexData
		);

	const uint_t_array& first_primitives;
	const uint_t_array& primitive_counts;
	const uint_t_array& first_operators;
	const uint_t_array& operator_counts;
	const typed_array<imaterial*>& materials;
	const typed_array<int32_t>& primitives;
	const uint_t_array& primitive_first_floats;
	const uint_t_array& primitive_float_counts;
	const typed_array<int32_t>& operators;
	const uint_t_array& operator_first_operands;
	const uint_t_array& operator_operand_counts;
	const typed_array<double_t>& floats;
	const uint_t_array& operands;
	const attribute_arrays& constant_data;
	const attribute_arrays& uniform_data;
	const attribute_arrays& varying_data;
	const attribute_arrays& vertex_data;
};

/// Gathers the member arrays of a blobby primitive into a convenient package
class primitive
{
public:
	primitive(
		uint_t_array& FirstPrimitives,
		uint_t_array& PrimitiveCounts,
		uint_t_array& FirstOperators,
		uint_t_array& OperatorCounts,
		typed_array<imaterial*>& Materials,
		typed_array<int32_t>& Primitives,
		uint_t_array& PrimitiveFirstFloats,
		uint_t_array& PrimitiveFloatCounts,
		typed_array<int32_t>& Operators,
		uint_t_array& OperatorFirstOperands,
		uint_t_array& OperatorOperandCounts,
		typed_array<double_t>& Floats,
		uint_t_array& Operands,
		attribute_arrays& ConstantData,
		attribute_arrays& UniformData,
		attribute_arrays& VaryingData,
		attribute_arrays& VertexData
		);

	uint_t_array& first_primitives;
	uint_t_array& primitive_counts;
	uint_t_array& first_operators;
	uint_t_array& operator_counts;
	typed_array<imaterial*>& materials;
	typed_array<int32_t>& primitives;
	uint_t_array& primitive_first_floats;
	uint_t_array& primitive_float_counts;
	typed_array<int32_t>& operators;
	uint_t_array& operator_first_operands;
	uint_t_array& operator_operand_counts;
	typed_array<double_t>& floats;
	uint_t_array& operands;
	attribute_arrays& constant_data;
	attribute_arrays& uniform_data;
	attribute_arrays& varying_data;
	attribute_arrays& vertex_data;
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

