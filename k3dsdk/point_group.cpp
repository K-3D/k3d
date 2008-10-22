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
	const uint_t_array& FirstPoints,
	const uint_t_array& PointCounts,
	const typed_array<imaterial*>& Materials,
	const uint_t_array& Points,
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
	uint_t_array& FirstPoints,
	uint_t_array& PointCounts,
	typed_array<imaterial*>& Materials,
	uint_t_array& Points,
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

	primitive* const result = new primitive(
		generic_primitive.topology.create<uint_t_array >("first_points"),
		generic_primitive.topology.create<uint_t_array >("point_counts"),
		generic_primitive.topology.create<typed_array<imaterial*> >("materials"),
		generic_primitive.topology.create<uint_t_array >("points"),
		generic_primitive.attributes["constant"],
		generic_primitive.attributes["varying"]
		);

	result->points.set_metadata_value("k3d:mesh-point-indices", "true");

	return result;
}

/////////////////////////////////////////////////////////////////////////////////////////////
// validate

const_primitive* validate(const mesh::primitive& Primitive)
{
	if(Primitive.type != "point_group")
		return 0;

	try
	{
		const uint_t_array& first_points = require_const_array<uint_t_array >(Primitive, "first_points");
		const uint_t_array& point_counts = require_const_array<uint_t_array >(Primitive, "point_counts");
		const typed_array<imaterial*>& materials = require_const_array<typed_array<imaterial*> >(Primitive, "materials");
		const uint_t_array& points = require_const_array<uint_t_array >(Primitive, "points");

		const attribute_arrays& constant_data = require_const_attribute_arrays(Primitive, "constant");
		const attribute_arrays& varying_data = require_const_attribute_arrays(Primitive, "varying");

		require_array_size(Primitive, point_counts, "point_counts", first_points.size());
		require_array_size(Primitive, materials, "materials", first_points.size());

		require_attribute_arrays_size(Primitive, constant_data, "constant", first_points.size());
		require_attribute_arrays_size(Primitive, varying_data, "varying", std::accumulate(point_counts.begin(), point_counts.end(), 0));

		return new const_primitive(first_points, point_counts, materials, points, constant_data, varying_data);
	}
	catch(std::exception& e)
	{
		log() << error << e.what() << std::endl;
	}

	return 0;
}

primitive* validate(mesh::primitive& Primitive)
{
	if(Primitive.type != "point_group")
		return 0;

	try
	{
		uint_t_array& first_points = require_array<uint_t_array >(Primitive, "first_points");
		uint_t_array& point_counts = require_array<uint_t_array >(Primitive, "point_counts");
		typed_array<imaterial*>& materials = require_array<typed_array<imaterial*> >(Primitive, "materials");
		uint_t_array& points = require_array<uint_t_array >(Primitive, "points");

		attribute_arrays& constant_data = require_attribute_arrays(Primitive, "constant");
		attribute_arrays& varying_data = require_attribute_arrays(Primitive, "varying");

		require_array_size(Primitive, point_counts, "point_counts", first_points.size());
		require_array_size(Primitive, materials, "materials", first_points.size());

		require_attribute_arrays_size(Primitive, constant_data, "constant", first_points.size());
		require_attribute_arrays_size(Primitive, varying_data, "varying", std::accumulate(point_counts.begin(), point_counts.end(), 0));

		return new primitive(first_points, point_counts, materials, points, constant_data, varying_data);
	}
	catch(std::exception& e)
	{
		log() << error << e.what() << std::endl;
	}

	return 0;
}

} // namespace point_group

} // namespace k3d

