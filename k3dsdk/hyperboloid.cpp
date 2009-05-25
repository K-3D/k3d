// K-3D
// Copyright (c) 1995-200 7, Timothy M. Shead
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

#include "hyperboloid.h"
#include "metadata_keys.h"
#include "primitive_validation.h"
#include "selection.h"
#include "string_cast.h"

namespace k3d
{

namespace hyperboloid
{

/////////////////////////////////////////////////////////////////////////////////////////////
// const_primitive

const_primitive::const_primitive(
	const mesh::matrices_t& Matrices,
	const mesh::materials_t& Materials,
	const mesh::points_t& StartPoints,
	const mesh::points_t& EndPoints,
	const mesh::doubles_t& SweepAngles,
	const mesh::selection_t& Selections,
	const mesh::attribute_arrays_t& ConstantData,
	const mesh::attribute_arrays_t& UniformData,
	const mesh::attribute_arrays_t& VaryingData
		) :
	matrices(Matrices),
	materials(Materials),
	start_points(StartPoints),
	end_points(EndPoints),
	sweep_angles(SweepAngles),
	selections(Selections),
	constant_data(ConstantData),
	uniform_data(UniformData),
	varying_data(VaryingData)
{
}

/////////////////////////////////////////////////////////////////////////////////////////////
// primitive

primitive::primitive(
	mesh::matrices_t& Matrices,
	mesh::materials_t& Materials,
	mesh::points_t& StartPoints,
	mesh::points_t& EndPoints,
	mesh::doubles_t& SweepAngles,
	mesh::selection_t& Selections,
	mesh::attribute_arrays_t& ConstantData,
	mesh::attribute_arrays_t& UniformData,
	mesh::attribute_arrays_t& VaryingData
		) :
	matrices(Matrices),
	materials(Materials),
	start_points(StartPoints),
	end_points(EndPoints),
	sweep_angles(SweepAngles),
	selections(Selections),
	constant_data(ConstantData),
	uniform_data(UniformData),
	varying_data(VaryingData)
{
}

/////////////////////////////////////////////////////////////////////////////////////////////
// create

primitive* create(mesh& Mesh)
{
	mesh::primitive& generic_primitive = Mesh.primitives.create("hyperboloid");

	primitive* const result = new primitive(
		generic_primitive.structure.create<mesh::matrices_t >("matrices"),
		generic_primitive.structure.create<mesh::materials_t >("materials"),
		generic_primitive.structure.create<mesh::points_t >("start_points"),
		generic_primitive.structure.create<mesh::points_t >("end_points"),
		generic_primitive.structure.create<mesh::doubles_t >("sweep_angles"),
		generic_primitive.structure.create<mesh::selection_t>("selections"),
		generic_primitive.attributes["constant"],
		generic_primitive.attributes["uniform"],
		generic_primitive.attributes["varying"]
		);

	result->selections.set_metadata_value(metadata::key::selection_component(), string_cast(selection::UNIFORM));

	return result;
}

/////////////////////////////////////////////////////////////////////////////////////////////
// validate

const_primitive* validate(const mesh::primitive& Primitive)
{
	if(Primitive.type != "hyperboloid")
		return 0;

	try
	{
		const mesh::matrices_t& matrices = require_const_array<mesh::matrices_t >(Primitive, "matrices");
		const mesh::materials_t& materials = require_const_array<mesh::materials_t >(Primitive, "materials");
		const mesh::points_t& start_points = require_const_array<mesh::points_t >(Primitive, "start_points");
		const mesh::points_t& end_points = require_const_array<mesh::points_t >(Primitive, "end_points");
		const mesh::doubles_t& sweep_angles = require_const_array<mesh::doubles_t >(Primitive, "sweep_angles");
		const mesh::selection_t& selections = require_const_array<mesh::selection_t>(Primitive, "selections");

		const mesh::attribute_arrays_t& constant_data = require_const_attribute_arrays(Primitive, "constant");
		const mesh::attribute_arrays_t& uniform_data = require_const_attribute_arrays(Primitive, "uniform");
		const mesh::attribute_arrays_t& varying_data = require_const_attribute_arrays(Primitive, "varying");

		require_metadata(Primitive, selections, "selections", metadata::key::selection_component(), string_cast(selection::UNIFORM));

		require_array_size(Primitive, materials, "materials", matrices.size());
		require_array_size(Primitive, start_points, "start_points", matrices.size());
		require_array_size(Primitive, end_points, "end_points", matrices.size());
		require_array_size(Primitive, sweep_angles, "sweep_angles", matrices.size());
		require_array_size(Primitive, selections, "selections", matrices.size());

		require_attribute_arrays_size(Primitive, constant_data, "constant", 1);
		require_attribute_arrays_size(Primitive, uniform_data, "uniform", matrices.size());
		require_attribute_arrays_size(Primitive, varying_data, "varying", matrices.size() * 4);

		return new const_primitive(matrices, materials, start_points, end_points, sweep_angles, selections, constant_data, uniform_data, varying_data);
	}
	catch(std::exception& e)
	{
		log() << error << e.what() << std::endl;
	}

	return 0;
}

primitive* validate(mesh::primitive& Primitive)
{
	if(Primitive.type != "hyperboloid")
		return 0;

	try
	{
		mesh::matrices_t& matrices = require_array<mesh::matrices_t >(Primitive, "matrices");
		mesh::materials_t& materials = require_array<mesh::materials_t >(Primitive, "materials");
		mesh::points_t& start_points = require_array<mesh::points_t >(Primitive, "start_points");
		mesh::points_t& end_points = require_array<mesh::points_t >(Primitive, "end_points");
		mesh::doubles_t& sweep_angles = require_array<mesh::doubles_t >(Primitive, "sweep_angles");
		mesh::selection_t& selections = require_array<mesh::selection_t>(Primitive, "selections");

		mesh::attribute_arrays_t& constant_data = require_attribute_arrays(Primitive, "constant");
		mesh::attribute_arrays_t& uniform_data = require_attribute_arrays(Primitive, "uniform");
		mesh::attribute_arrays_t& varying_data = require_attribute_arrays(Primitive, "varying");

		require_metadata(Primitive, selections, "selections", metadata::key::selection_component(), string_cast(selection::UNIFORM));

		require_array_size(Primitive, materials, "materials", matrices.size());
		require_array_size(Primitive, start_points, "start_points", matrices.size());
		require_array_size(Primitive, end_points, "end_points", matrices.size());
		require_array_size(Primitive, sweep_angles, "sweep_angles", matrices.size());
		require_array_size(Primitive, selections, "selections", matrices.size());

		require_attribute_arrays_size(Primitive, constant_data, "constant", 1);
		require_attribute_arrays_size(Primitive, uniform_data, "uniform", matrices.size());
		require_attribute_arrays_size(Primitive, varying_data, "varying", matrices.size() * 4);

		return new primitive(matrices, materials, start_points, end_points, sweep_angles, selections, constant_data, uniform_data, varying_data);
	}
	catch(std::exception& e)
	{
		log() << error << e.what() << std::endl;
	}

	return 0;
}

primitive* validate(pipeline_data<mesh::primitive>& Primitive)
{
  if(!Primitive.get())
    return 0;

	if(Primitive->type != "hyperboloid")
		return 0;

  return validate(Primitive.writable());
}

} // namespace hyperboloid

} // namespace k3d

