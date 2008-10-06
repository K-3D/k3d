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
#include "primitive_detail.h"

namespace k3d
{

namespace hyperboloid
{

/////////////////////////////////////////////////////////////////////////////////////////////
// const_primitive

const_primitive::const_primitive(
	const typed_array<matrix4>& Matrices,
	const typed_array<imaterial*>& Materials,
	const typed_array<point3>& StartPoints,
	const typed_array<point3>& EndPoints,
	const typed_array<double_t>& SweepAngles,
	const attribute_arrays& ConstantData,
	const attribute_arrays& UniformData,
	const attribute_arrays& VaryingData) :
	matrices(Matrices),
	materials(Materials),
	start_points(StartPoints),
	end_points(EndPoints),
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
	typed_array<point3>& StartPoints,
	typed_array<point3>& EndPoints,
	typed_array<double_t>& SweepAngles,
	attribute_arrays& ConstantData,
	attribute_arrays& UniformData,
	attribute_arrays& VaryingData) :
	matrices(Matrices),
	materials(Materials),
	start_points(StartPoints),
	end_points(EndPoints),
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
	mesh::primitive& generic_primitive = Mesh.primitives.create("hyperboloid");

	return new primitive(
		generic_primitive.topology.create<typed_array<matrix4> >("matrices"),
		generic_primitive.topology.create<typed_array<imaterial*> >("materials"),
		generic_primitive.topology.create<typed_array<point3> >("start_points"),
		generic_primitive.topology.create<typed_array<point3> >("end_points"),
		generic_primitive.topology.create<typed_array<double_t> >("sweep_angles"),
		generic_primitive.attributes["constant"],
		generic_primitive.attributes["uniform"],
		generic_primitive.attributes["varying"]);
}

/////////////////////////////////////////////////////////////////////////////////////////////
// validate

const_primitive* validate(const mesh::primitive& GenericPrimitive)
{
	if(GenericPrimitive.type != "hyperboloid")
		return 0;

	require_const_array(GenericPrimitive, matrices, typed_array<matrix4>);
	require_const_array(GenericPrimitive, materials, typed_array<imaterial*>);
	require_const_array(GenericPrimitive, start_points, typed_array<point3>);
	require_const_array(GenericPrimitive, end_points, typed_array<point3>);
	require_const_array(GenericPrimitive, sweep_angles, typed_array<double_t>);

	require_const_attribute_arrays(GenericPrimitive, constant);
	require_const_attribute_arrays(GenericPrimitive, uniform);
	require_const_attribute_arrays(GenericPrimitive, varying);

	if(!(
		matrices->size() == materials->size()
		&& matrices->size() == start_points->size()
		&& matrices->size() == end_points->size()
		&& matrices->size() == sweep_angles->size()
		))
	{
		log() << error << "[" << GenericPrimitive.type << "] primitive array-length mismatch" << std::endl;
		return 0;
	}

	if(!constant_data->match_size(matrices->size()))
	{
		log() << error << "[" << GenericPrimitive.type << "] constant attributes must contain one value per hyperboloid" << std::endl;
		return 0;
	}

	if(!uniform_data->match_size(matrices->size()))
	{
		log() << error << "[" << GenericPrimitive.type << "] uniform attributes must contain one value per hyperboloid" << std::endl;
		return 0;
	}

	if(!varying_data->match_size(matrices->size() * 4))
	{
		log() << error << "[" << GenericPrimitive.type << "] varying attributes must contain four values per hyperboloid" << std::endl;
		return 0;
	}

	return new const_primitive(*matrices, *materials, *start_points, *end_points, *sweep_angles, *constant_data, *uniform_data, *varying_data);
}

primitive* validate(mesh::primitive& GenericPrimitive)
{
	if(GenericPrimitive.type != "hyperboloid")
		return 0;

	require_array(GenericPrimitive, matrices, typed_array<matrix4>);
	require_array(GenericPrimitive, materials, typed_array<imaterial*>);
	require_array(GenericPrimitive, start_points, typed_array<point3>);
	require_array(GenericPrimitive, end_points, typed_array<point3>);
	require_array(GenericPrimitive, sweep_angles, typed_array<double_t>);

	require_attribute_arrays(GenericPrimitive, constant);
	require_attribute_arrays(GenericPrimitive, uniform);
	require_attribute_arrays(GenericPrimitive, varying);

	if(!(
		matrices->size() == materials->size()
		&& matrices->size() == start_points->size()
		&& matrices->size() == end_points->size()
		&& matrices->size() == sweep_angles->size()
		))
	{
		log() << error << "[" << GenericPrimitive.type << "] primitive array-length mismatch" << std::endl;
		return 0;
	}

	if(!constant_data->match_size(matrices->size()))
	{
		log() << error << "[" << GenericPrimitive.type << "] constant attributes must contain one value per hyperboloid" << std::endl;
		return 0;
	}

	if(!uniform_data->match_size(matrices->size()))
	{
		log() << error << "[" << GenericPrimitive.type << "] uniform attributes must contain one value per hyperboloid" << std::endl;
		return 0;
	}

	if(!varying_data->match_size(matrices->size() * 4))
	{
		log() << error << "[" << GenericPrimitive.type << "] varying attributes must contain four values per hyperboloid" << std::endl;
		return 0;
	}

	return new primitive(*matrices, *materials, *start_points, *end_points, *sweep_angles, *constant_data, *uniform_data, *varying_data);
}

} // namespace hyperboloid

} // namespace k3d

