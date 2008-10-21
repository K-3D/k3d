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

#include "point_group.h"
#include "primitive_detail.h"

#include <numeric>

namespace k3d
{

namespace point_group
{

/////////////////////////////////////////////////////////////////////////////////////////////
// const_primitive

const_primitive::const_primitive(
	const typed_array<uint_t>& FirstPoints,
	const typed_array<uint_t>& PointCounts,
	const typed_array<imaterial*>& Materials,
	const typed_array<uint_t>& Points,
	const attribute_arrays& ConstantData,
	const attribute_arrays& VaryingData
	) :
	first_points(FirstPoints),
	point_counts(PointCounts),
	materials(Materials),
	points(Points),
	constant_data(ConstantData),
	varying_data(VaryingData)
{
}

/////////////////////////////////////////////////////////////////////////////////////////////
// primitive

primitive::primitive(
	typed_array<uint_t>& FirstPoints,
	typed_array<uint_t>& PointCounts,
	typed_array<imaterial*>& Materials,
	typed_array<uint_t>& Points,
	attribute_arrays& ConstantData,
	attribute_arrays& VaryingData
	) :
	first_points(FirstPoints),
	point_counts(PointCounts),
	materials(Materials),
	points(Points),
	constant_data(ConstantData),
	varying_data(VaryingData)
{
}

/////////////////////////////////////////////////////////////////////////////////////////////
// create

primitive* create(mesh& Mesh)
{
	mesh::primitive& generic_primitive = Mesh.primitives.create("point_group");

	return new primitive(
		generic_primitive.topology.create<typed_array<uint_t> >("first_points"),
		generic_primitive.topology.create<typed_array<uint_t> >("point_counts"),
		generic_primitive.topology.create<typed_array<imaterial*> >("materials"),
		generic_primitive.topology.create<typed_array<uint_t> >("points"),
		generic_primitive.attributes["constant"],
		generic_primitive.attributes["varying"]);
}

/////////////////////////////////////////////////////////////////////////////////////////////
// validate

const_primitive* validate(const mesh::primitive& GenericPrimitive)
{
	if(GenericPrimitive.type != "point_group")
		return 0;

	require_const_array(GenericPrimitive, first_points, typed_array<uint_t>);
	require_const_array(GenericPrimitive, point_counts, typed_array<uint_t>);
	require_const_array(GenericPrimitive, materials, typed_array<imaterial*>);
	require_const_array(GenericPrimitive, points, typed_array<uint_t>);

	require_const_attribute_arrays(GenericPrimitive, constant);
	require_const_attribute_arrays(GenericPrimitive, varying);

	if(!(
		first_points->size() == point_counts->size()
		&& first_points->size() == materials->size()
		))
	{
		log() << error << "[" << GenericPrimitive.type << "] primitive array-length mismatch" << std::endl;
		return 0;
	}

	if(!constant_data->match_size(first_points->size()))
	{
		log() << error << "[" << GenericPrimitive.type << "] constant attributes must contain one value per point_group" << std::endl;
		return 0;
	}

	if(!varying_data->match_size(std::accumulate(point_counts->begin(), point_counts->end(), 0)))
	{
		log() << error << "[" << GenericPrimitive.type << "] varying attributes must contain one value per point" << std::endl;
		return 0;
	}

	return new const_primitive(*first_points, *point_counts, *materials, *points, *constant_data, *varying_data);
}

primitive* validate(mesh::primitive& GenericPrimitive)
{
	if(GenericPrimitive.type != "point_group")
		return 0;

	require_array(GenericPrimitive, first_points, typed_array<uint_t>);
	require_array(GenericPrimitive, point_counts, typed_array<uint_t>);
	require_array(GenericPrimitive, materials, typed_array<imaterial*>);
	require_array(GenericPrimitive, points, typed_array<uint_t>);

	require_attribute_arrays(GenericPrimitive, constant);
	require_attribute_arrays(GenericPrimitive, varying);

	if(!(
		first_points->size() == point_counts->size()
		&& first_points->size() == materials->size()
		))
	{
		log() << error << "[" << GenericPrimitive.type << "] primitive array-length mismatch" << std::endl;
		return 0;
	}

	if(!constant_data->match_size(first_points->size()))
	{
		log() << error << "[" << GenericPrimitive.type << "] constant attributes must contain one value per point_group" << std::endl;
		return 0;
	}

	if(!varying_data->match_size(std::accumulate(point_counts->begin(), point_counts->end(), 0)))
	{
		log() << error << "[" << GenericPrimitive.type << "] varying attributes must contain one value per point" << std::endl;
		return 0;
	}

	return new primitive(*first_points, *point_counts, *materials, *points, *constant_data, *varying_data);
}

} // namespace point_group

} // namespace k3d

