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

#include "array_metadata.h"
#include "blobby.h"
#include "primitive_detail.h"

#include <numeric>

namespace k3d
{

namespace blobby
{

/////////////////////////////////////////////////////////////////////////////////////////////
// const_primitive

const_primitive::const_primitive(
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
		) :
	first_primitives(FirstPrimitives),
	primitive_counts(PrimitiveCounts),
	first_operators(FirstOperators),
	operator_counts(OperatorCounts),
	materials(Materials),
	primitives(Primitives),
	primitive_first_floats(PrimitiveFirstFloats),
	primitive_float_counts(PrimitiveFloatCounts),
	operators(Operators),
	operator_first_operands(OperatorFirstOperands),
	operator_operand_counts(OperatorOperandCounts),
	floats(Floats),
	operands(Operands),
	constant_data(ConstantData),
	uniform_data(UniformData),
	varying_data(VaryingData),
	vertex_data(VertexData)
{
}

/////////////////////////////////////////////////////////////////////////////////////////////
// primitive

primitive::primitive(
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
		) :
	first_primitives(FirstPrimitives),
	primitive_counts(PrimitiveCounts),
	first_operators(FirstOperators),
	operator_counts(OperatorCounts),
	materials(Materials),
	primitives(Primitives),
	primitive_first_floats(PrimitiveFirstFloats),
	primitive_float_counts(PrimitiveFloatCounts),
	operators(Operators),
	operator_first_operands(OperatorFirstOperands),
	operator_operand_counts(OperatorOperandCounts),
	floats(Floats),
	operands(Operands),
	constant_data(ConstantData),
	uniform_data(UniformData),
	varying_data(VaryingData),
	vertex_data(VertexData)
{
}

/////////////////////////////////////////////////////////////////////////////////////////////
// create

primitive* create(mesh& Mesh)
{
	mesh::primitive& generic_primitive = Mesh.primitives.create("blobby");

	primitive* const result = new primitive(
		generic_primitive.topology.create<uint_t_array >("first_primitives"),
		generic_primitive.topology.create<uint_t_array >("primitive_counts"),
		generic_primitive.topology.create<uint_t_array >("first_operators"),
		generic_primitive.topology.create<uint_t_array >("operator_counts"),
		generic_primitive.topology.create<typed_array<imaterial*> >("materials"),
		generic_primitive.topology.create<typed_array<int32_t> >("primitives"),
		generic_primitive.topology.create<uint_t_array >("primitive_first_floats"),
		generic_primitive.topology.create<uint_t_array >("primitive_float_counts"),
		generic_primitive.topology.create<typed_array<int32_t> >("operators"),
		generic_primitive.topology.create<uint_t_array >("operator_first_operands"),
		generic_primitive.topology.create<uint_t_array >("operator_operand_counts"),
		generic_primitive.topology.create<typed_array<double_t> >("floats"),
		generic_primitive.topology.create<uint_t_array >("operands"),
		generic_primitive.attributes["constant"],
		generic_primitive.attributes["uniform"],
		generic_primitive.attributes["varying"],
		generic_primitive.attributes["vertex"]
		);

	return result;
}

/////////////////////////////////////////////////////////////////////////////////////////////
// validate

const_primitive* validate(const mesh::primitive& Primitive)
{
	if(Primitive.type != "blobby")
		return 0;

	try
	{
		const uint_t_array& first_primitives = require_const_array<uint_t_array >(Primitive, "first_primitives");
		const uint_t_array& primitive_counts = require_const_array<uint_t_array >(Primitive, "primitive_counts");
		const uint_t_array& first_operators = require_const_array<uint_t_array >(Primitive, "first_operators");
		const uint_t_array& operator_counts = require_const_array<uint_t_array >(Primitive, "operator_counts");
		const typed_array<imaterial*>& materials = require_const_array<typed_array<imaterial*> >(Primitive, "materials");
		const typed_array<int32_t>& primitives = require_const_array<typed_array<int32_t> >(Primitive, "primitives");
		const uint_t_array& primitive_first_floats = require_const_array<uint_t_array >(Primitive, "primitive_first_floats");
		const uint_t_array& primitive_float_counts = require_const_array<uint_t_array >(Primitive, "primitive_float_counts");
		const typed_array<int32_t>& operators = require_const_array<typed_array<int32_t> >(Primitive, "operators");
		const uint_t_array& operator_first_operands = require_const_array<uint_t_array >(Primitive, "operator_first_operands");
		const uint_t_array& operator_operand_counts = require_const_array<uint_t_array >(Primitive, "operator_operand_counts");
		const typed_array<double_t>& floats = require_const_array<typed_array<double_t> >(Primitive, "floats");
		const uint_t_array& operands = require_const_array<uint_t_array >(Primitive, "operands");

		const attribute_arrays& constant_data = require_const_attribute_arrays(Primitive, "constant");
		const attribute_arrays& uniform_data = require_const_attribute_arrays(Primitive, "uniform");
		const attribute_arrays& varying_data = require_const_attribute_arrays(Primitive, "varying");
		const attribute_arrays& vertex_data = require_const_attribute_arrays(Primitive, "vertex");

/*
		require_array_size(Primitive, curve_counts, "curve_counts", first_curves.size());
		require_array_size(Primitive, periodic_curves, "periodic_curves", first_curves.size());
		require_array_size(Primitive, materials, "materials", first_curves.size());

		require_array_size(Primitive, curve_first_points, "curve_first_points", std::accumulate(curve_counts.begin(), curve_counts.end(), 0));
		require_array_size(Primitive, curve_point_counts, "curve_point_counts", std::accumulate(curve_counts.begin(), curve_counts.end(), 0));
		require_array_size(Primitive, curve_selections, "curve_selections", std::accumulate(curve_counts.begin(), curve_counts.end(), 0));

		require_array_size(Primitive, curve_points, "curve_points", std::accumulate(curve_point_counts.begin(), curve_point_counts.end(), 0));

		require_attribute_arrays_size(Primitive, constant_data, "constant", first_curves.size());
		
		require_attribute_arrays_size(Primitive, uniform_data, "uniform", std::accumulate(curve_counts.begin(), curve_counts.end(), 0));
		
		require_attribute_arrays_size(Primitive, varying_data, "varying", std::accumulate(curve_point_counts.begin(), curve_point_counts.end(), 0));
*/

		return new const_primitive(first_primitives, primitive_counts, first_operators, operator_counts, materials, primitives, primitive_first_floats, primitive_float_counts, operators, operator_first_operands, operator_operand_counts, floats, operands, constant_data, uniform_data, varying_data, vertex_data);
	}
	catch(std::exception& e)
	{
		log() << error << e.what() << std::endl;
	}

	return 0;
}

primitive* validate(mesh::primitive& Primitive)
{
	if(Primitive.type != "blobby")
		return 0;

	try
	{
		uint_t_array& first_primitives = require_array<uint_t_array >(Primitive, "first_primitives");
		uint_t_array& primitive_counts = require_array<uint_t_array >(Primitive, "primitive_counts");
		uint_t_array& first_operators = require_array<uint_t_array >(Primitive, "first_operators");
		uint_t_array& operator_counts = require_array<uint_t_array >(Primitive, "operator_counts");
		typed_array<imaterial*>& materials = require_array<typed_array<imaterial*> >(Primitive, "materials");
		typed_array<int32_t>& primitives = require_array<typed_array<int32_t> >(Primitive, "primitives");
		uint_t_array& primitive_first_floats = require_array<uint_t_array >(Primitive, "primitive_first_floats");
		uint_t_array& primitive_float_counts = require_array<uint_t_array >(Primitive, "primitive_float_counts");
		typed_array<int32_t>& operators = require_array<typed_array<int32_t> >(Primitive, "operators");
		uint_t_array& operator_first_operands = require_array<uint_t_array >(Primitive, "operator_first_operands");
		uint_t_array& operator_operand_counts = require_array<uint_t_array >(Primitive, "operator_operand_counts");
		typed_array<double_t>& floats = require_array<typed_array<double_t> >(Primitive, "floats");
		uint_t_array& operands = require_array<uint_t_array >(Primitive, "operands");

		attribute_arrays& constant_data = require_attribute_arrays(Primitive, "constant");
		attribute_arrays& uniform_data = require_attribute_arrays(Primitive, "uniform");
		attribute_arrays& varying_data = require_attribute_arrays(Primitive, "varying");
		attribute_arrays& vertex_data = require_attribute_arrays(Primitive, "vertex");

/*
		require_array_size(Primitive, curve_counts, "curve_counts", first_curves.size());
		require_array_size(Primitive, periodic_curves, "periodic_curves", first_curves.size());
		require_array_size(Primitive, materials, "materials", first_curves.size());

		require_array_size(Primitive, curve_first_points, "curve_first_points", std::accumulate(curve_counts.begin(), curve_counts.end(), 0));
		require_array_size(Primitive, curve_point_counts, "curve_point_counts", std::accumulate(curve_counts.begin(), curve_counts.end(), 0));
		require_array_size(Primitive, curve_selections, "curve_selections", std::accumulate(curve_counts.begin(), curve_counts.end(), 0));

		require_array_size(Primitive, curve_points, "curve_points", std::accumulate(curve_point_counts.begin(), curve_point_counts.end(), 0));

		require_attribute_arrays_size(Primitive, constant_data, "constant", first_curves.size());
		
		require_attribute_arrays_size(Primitive, uniform_data, "uniform", std::accumulate(curve_counts.begin(), curve_counts.end(), 0));
		
		require_attribute_arrays_size(Primitive, varying_data, "varying", std::accumulate(curve_point_counts.begin(), curve_point_counts.end(), 0));
*/

		return new primitive(first_primitives, primitive_counts, first_operators, operator_counts, materials, primitives, primitive_first_floats, primitive_float_counts, operators, operator_first_operands, operator_operand_counts, floats, operands, constant_data, uniform_data, varying_data, vertex_data);
	}
	catch(std::exception& e)
	{
		log() << error << e.what() << std::endl;
	}

	return 0;
}

} // namespace blobby

} // namespace k3d

