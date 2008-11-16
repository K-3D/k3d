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

#include "sphere.h"
#include "primitive_detail.h"

namespace k3d
{

namespace sphere
{

/////////////////////////////////////////////////////////////////////////////////////////////
// const_primitive

const_primitive::const_primitive(
	const mesh::matrices_t& Matrices,
	const mesh::materials_t& Materials,
	const mesh::doubles_t& Radii,
	const mesh::doubles_t& ZMin,
	const mesh::doubles_t& ZMax,
	const mesh::doubles_t& SweepAngles,
	const mesh::attribute_arrays_t& ConstantData,
	const mesh::attribute_arrays_t& UniformData,
	const mesh::attribute_arrays_t& VaryingData
		) :
	matrices(Matrices),
	materials(Materials),
	radii(Radii),
	z_min(ZMin),
	z_max(ZMax),
	sweep_angles(SweepAngles),
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
	mesh::doubles_t& Radii,
	mesh::doubles_t& ZMin,
	mesh::doubles_t& ZMax,
	mesh::doubles_t& SweepAngles,
	mesh::attribute_arrays_t& ConstantData,
	mesh::attribute_arrays_t& UniformData,
	mesh::attribute_arrays_t& VaryingData
		) :
	matrices(Matrices),
	materials(Materials),
	radii(Radii),
	z_min(ZMin),
	z_max(ZMax),
	sweep_angles(SweepAngles),
	constant_data(ConstantData),
	uniform_data(UniformData),
	varying_data(VaryingData)
{
}

/////////////////////////////////////////////////////////////////////////////////////////////
// create

primitive* create(mesh& Mesh)
{
	mesh::primitive& generic_primitive = Mesh.primitives.create("sphere");

	primitive* const result = new primitive(
		generic_primitive.topology.create<mesh::matrices_t >("matrices"),
		generic_primitive.topology.create<mesh::materials_t >("materials"),
		generic_primitive.topology.create<mesh::doubles_t >("radii"),
		generic_primitive.topology.create<mesh::doubles_t >("z_min"),
		generic_primitive.topology.create<mesh::doubles_t >("z_max"),
		generic_primitive.topology.create<mesh::doubles_t >("sweep_angles"),
		generic_primitive.attributes["constant"],
		generic_primitive.attributes["uniform"],
		generic_primitive.attributes["varying"]
		);

	return result;
}

/////////////////////////////////////////////////////////////////////////////////////////////
// validate

const_primitive* validate(const mesh::primitive& Primitive)
{
	if(Primitive.type != "sphere")
		return 0;

	try
	{
		const mesh::matrices_t& matrices = require_const_array<mesh::matrices_t >(Primitive, "matrices");
		const mesh::materials_t& materials = require_const_array<mesh::materials_t >(Primitive, "materials");
		const mesh::doubles_t& radii = require_const_array<mesh::doubles_t >(Primitive, "radii");
		const mesh::doubles_t& z_min = require_const_array<mesh::doubles_t >(Primitive, "z_min");
		const mesh::doubles_t& z_max = require_const_array<mesh::doubles_t >(Primitive, "z_max");
		const mesh::doubles_t& sweep_angles = require_const_array<mesh::doubles_t >(Primitive, "sweep_angles");

		const mesh::attribute_arrays_t& constant_data = require_const_attribute_arrays(Primitive, "constant");
		const mesh::attribute_arrays_t& uniform_data = require_const_attribute_arrays(Primitive, "uniform");
		const mesh::attribute_arrays_t& varying_data = require_const_attribute_arrays(Primitive, "varying");

		require_array_size(Primitive, materials, "materials", matrices.size());
		require_array_size(Primitive, radii, "radii", matrices.size());
		require_array_size(Primitive, z_min, "z_min", matrices.size());
		require_array_size(Primitive, z_max, "z_max", matrices.size());
		require_array_size(Primitive, sweep_angles, "sweep_angles", matrices.size());

		require_attribute_arrays_size(Primitive, constant_data, "constant", matrices.size());
		require_attribute_arrays_size(Primitive, uniform_data, "uniform", matrices.size());
		require_attribute_arrays_size(Primitive, varying_data, "varying", matrices.size() * 4);

		return new const_primitive(matrices, materials, radii, z_min, z_max, sweep_angles, constant_data, uniform_data, varying_data);
	}
	catch(std::exception& e)
	{
		log() << error << e.what() << std::endl;
	}

	return 0;
}

primitive* validate(mesh::primitive& Primitive)
{
	if(Primitive.type != "sphere")
		return 0;

	try
	{
		mesh::matrices_t& matrices = require_array<mesh::matrices_t >(Primitive, "matrices");
		mesh::materials_t& materials = require_array<mesh::materials_t >(Primitive, "materials");
		mesh::doubles_t& radii = require_array<mesh::doubles_t >(Primitive, "radii");
		mesh::doubles_t& z_min = require_array<mesh::doubles_t >(Primitive, "z_min");
		mesh::doubles_t& z_max = require_array<mesh::doubles_t >(Primitive, "z_max");
		mesh::doubles_t& sweep_angles = require_array<mesh::doubles_t >(Primitive, "sweep_angles");

		mesh::attribute_arrays_t& constant_data = require_attribute_arrays(Primitive, "constant");
		mesh::attribute_arrays_t& uniform_data = require_attribute_arrays(Primitive, "uniform");
		mesh::attribute_arrays_t& varying_data = require_attribute_arrays(Primitive, "varying");

		require_array_size(Primitive, materials, "materials", matrices.size());
		require_array_size(Primitive, radii, "radii", matrices.size());
		require_array_size(Primitive, z_min, "z_min", matrices.size());
		require_array_size(Primitive, z_max, "z_max", matrices.size());
		require_array_size(Primitive, sweep_angles, "sweep_angles", matrices.size());

		require_attribute_arrays_size(Primitive, constant_data, "constant", matrices.size());
		require_attribute_arrays_size(Primitive, uniform_data, "uniform", matrices.size());
		require_attribute_arrays_size(Primitive, varying_data, "varying", matrices.size() * 4);

		return new primitive(matrices, materials, radii, z_min, z_max, sweep_angles, constant_data, uniform_data, varying_data);
	}
	catch(std::exception& e)
	{
		log() << error << e.what() << std::endl;
	}

	return 0;
}

} // namespace sphere

} // namespace k3d

