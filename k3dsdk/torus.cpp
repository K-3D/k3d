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

#include "primitive_detail.h"
#include "torus.h"

namespace k3d
{

namespace torus
{

/////////////////////////////////////////////////////////////////////////////////////////////
// const_primitive

const_primitive::const_primitive(
	const typed_array<matrix4>& Matrices,
	const typed_array<imaterial*>& Materials,
	const typed_array<double_t>& MajorRadii,
	const typed_array<double_t>& MinorRadii,
	const typed_array<double_t>& PhiMin,
	const typed_array<double_t>& PhiMax,
	const typed_array<double_t>& SweepAngles,
	const attribute_arrays& ConstantData,
	const attribute_arrays& UniformData,
	const attribute_arrays& VaryingData
		) :
	matrices(Matrices),
	materials(Materials),
	major_radii(MajorRadii),
	minor_radii(MinorRadii),
	phi_min(PhiMin),
	phi_max(PhiMax),
	sweep_angles(SweepAngles),
	constant_data(ConstantData),
	uniform_data(UniformData),
	varying_data(VaryingData)
{
}

/////////////////////////////////////////////////////////////////////////////////////////////
// primitive

primitive::primitive(
	typed_array<matrix4>& Matrices,
	typed_array<imaterial*>& Materials,
	typed_array<double_t>& MajorRadii,
	typed_array<double_t>& MinorRadii,
	typed_array<double_t>& PhiMin,
	typed_array<double_t>& PhiMax,
	typed_array<double_t>& SweepAngles,
	attribute_arrays& ConstantData,
	attribute_arrays& UniformData,
	attribute_arrays& VaryingData
		) :
	matrices(Matrices),
	materials(Materials),
	major_radii(MajorRadii),
	minor_radii(MinorRadii),
	phi_min(PhiMin),
	phi_max(PhiMax),
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
	mesh::primitive& generic_primitive = Mesh.primitives.create("torus");

	primitive* const result = new primitive(
		generic_primitive.topology.create<typed_array<matrix4> >("matrices"),
		generic_primitive.topology.create<typed_array<imaterial*> >("materials"),
		generic_primitive.topology.create<typed_array<double_t> >("major_radii"),
		generic_primitive.topology.create<typed_array<double_t> >("minor_radii"),
		generic_primitive.topology.create<typed_array<double_t> >("phi_min"),
		generic_primitive.topology.create<typed_array<double_t> >("phi_max"),
		generic_primitive.topology.create<typed_array<double_t> >("sweep_angles"),
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
	if(Primitive.type != "torus")
		return 0;

	try
	{
		const typed_array<matrix4>& matrices = require_const_array<typed_array<matrix4> >(Primitive, "matrices");
		const typed_array<imaterial*>& materials = require_const_array<typed_array<imaterial*> >(Primitive, "materials");
		const typed_array<double_t>& major_radii = require_const_array<typed_array<double_t> >(Primitive, "major_radii");
		const typed_array<double_t>& minor_radii = require_const_array<typed_array<double_t> >(Primitive, "minor_radii");
		const typed_array<double_t>& phi_min = require_const_array<typed_array<double_t> >(Primitive, "phi_min");
		const typed_array<double_t>& phi_max = require_const_array<typed_array<double_t> >(Primitive, "phi_max");
		const typed_array<double_t>& sweep_angles = require_const_array<typed_array<double_t> >(Primitive, "sweep_angles");

		const attribute_arrays& constant_data = require_const_attribute_arrays(Primitive, "constant");
		const attribute_arrays& uniform_data = require_const_attribute_arrays(Primitive, "uniform");
		const attribute_arrays& varying_data = require_const_attribute_arrays(Primitive, "varying");

		require_array_size(Primitive, materials, "materials", matrices.size());
		require_array_size(Primitive, major_radii, "major_radii", matrices.size());
		require_array_size(Primitive, minor_radii, "minor_radii", matrices.size());
		require_array_size(Primitive, phi_min, "phi_min", matrices.size());
		require_array_size(Primitive, phi_max, "phi_max", matrices.size());
		require_array_size(Primitive, sweep_angles, "sweep_angles", matrices.size());

		require_attribute_arrays_size(Primitive, constant_data, "constant", matrices.size());
		require_attribute_arrays_size(Primitive, uniform_data, "uniform", matrices.size());
		require_attribute_arrays_size(Primitive, varying_data, "varying", matrices.size() * 4);

		return new const_primitive(matrices, materials, major_radii, minor_radii, phi_min, phi_max, sweep_angles, constant_data, uniform_data, varying_data);
	}
	catch(std::exception& e)
	{
		log() << error << e.what() << std::endl;
	}

	return 0;
}

primitive* validate(mesh::primitive& Primitive)
{
	if(Primitive.type != "torus")
		return 0;

	try
	{
		typed_array<matrix4>& matrices = require_array<typed_array<matrix4> >(Primitive, "matrices");
		typed_array<imaterial*>& materials = require_array<typed_array<imaterial*> >(Primitive, "materials");
		typed_array<double_t>& major_radii = require_array<typed_array<double_t> >(Primitive, "major_radii");
		typed_array<double_t>& minor_radii = require_array<typed_array<double_t> >(Primitive, "minor_radii");
		typed_array<double_t>& phi_min = require_array<typed_array<double_t> >(Primitive, "phi_min");
		typed_array<double_t>& phi_max = require_array<typed_array<double_t> >(Primitive, "phi_max");
		typed_array<double_t>& sweep_angles = require_array<typed_array<double_t> >(Primitive, "sweep_angles");

		attribute_arrays& constant_data = require_attribute_arrays(Primitive, "constant");
		attribute_arrays& uniform_data = require_attribute_arrays(Primitive, "uniform");
		attribute_arrays& varying_data = require_attribute_arrays(Primitive, "varying");

		require_array_size(Primitive, materials, "materials", matrices.size());
		require_array_size(Primitive, major_radii, "major_radii", matrices.size());
		require_array_size(Primitive, minor_radii, "minor_radii", matrices.size());
		require_array_size(Primitive, phi_min, "phi_min", matrices.size());
		require_array_size(Primitive, phi_max, "phi_max", matrices.size());
		require_array_size(Primitive, sweep_angles, "sweep_angles", matrices.size());

		require_attribute_arrays_size(Primitive, constant_data, "constant", matrices.size());
		require_attribute_arrays_size(Primitive, uniform_data, "uniform", matrices.size());
		require_attribute_arrays_size(Primitive, varying_data, "varying", matrices.size() * 4);

		return new primitive(matrices, materials, major_radii, minor_radii, phi_min, phi_max, sweep_angles, constant_data, uniform_data, varying_data);
	}
	catch(std::exception& e)
	{
		log() << error << e.what() << std::endl;
	}

	return 0;
}

} // namespace torus

} // namespace k3d

