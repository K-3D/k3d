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

#include "metadata_keys.h"
#include "point_group.h"
#include "primitive_validation.h"

#include <numeric>

namespace k3d
{

namespace point_group
{

/////////////////////////////////////////////////////////////////////////////////////////////
// const_primitive

const_primitive::const_primitive(
	const typed_array<imaterial*>& Material,
	const uint_t_array& Points,
	const attribute_arrays& ConstantData,
	const attribute_arrays& VaryingData
		) :
	material(Material),
	points(Points),
	constant_data(ConstantData),
	varying_data(VaryingData)
{
}

/////////////////////////////////////////////////////////////////////////////////////////////
// primitive

primitive::primitive(
	typed_array<imaterial*>& Material,
	uint_t_array& Points,
	attribute_arrays& ConstantData,
	attribute_arrays& VaryingData
		) :
	material(Material),
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
		generic_primitive.structure.create<typed_array<imaterial*> >("material"),
		generic_primitive.structure.create<uint_t_array >("points"),
		generic_primitive.attributes["constant"],
		generic_primitive.attributes["varying"]
		);

	result->points.set_metadata_value(metadata::key::domain(), metadata::value::mesh_point_indices_domain());

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
		const typed_array<imaterial*>& material = require_const_array<typed_array<imaterial*> >(Primitive, "material");
		const uint_t_array& points = require_const_array<uint_t_array >(Primitive, "points");

		const attribute_arrays& constant_data = require_const_attribute_arrays(Primitive, "constant");
		const attribute_arrays& varying_data = require_const_attribute_arrays(Primitive, "varying");

		require_metadata(Primitive, points, "points", metadata::key::domain(), metadata::value::mesh_point_indices_domain());

		require_array_size(Primitive, material, "material", 1);

		require_attribute_arrays_size(Primitive, constant_data, "constant", 1);
		require_attribute_arrays_size(Primitive, varying_data, "varying", points.size());

		return new const_primitive(material, points, constant_data, varying_data);
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
		typed_array<imaterial*>& material = require_array<typed_array<imaterial*> >(Primitive, "material");
		uint_t_array& points = require_array<uint_t_array >(Primitive, "points");

		attribute_arrays& constant_data = require_attribute_arrays(Primitive, "constant");
		attribute_arrays& varying_data = require_attribute_arrays(Primitive, "varying");

		require_metadata(Primitive, points, "points", metadata::key::domain(), metadata::value::mesh_point_indices_domain());

		require_array_size(Primitive, material, "material", 1);

		require_attribute_arrays_size(Primitive, constant_data, "constant", 1);
		require_attribute_arrays_size(Primitive, varying_data, "varying", points.size());

		return new primitive(material, points, constant_data, varying_data);
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

	if(Primitive->type != "point_group")
		return 0;

  return validate(Primitive.writable());
}

} // namespace point_group

} // namespace k3d

