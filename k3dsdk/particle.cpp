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
#include "particle.h"
#include "primitive_validation.h"

#include <numeric>

namespace k3d
{

namespace particle
{

/////////////////////////////////////////////////////////////////////////////////////////////
// const_primitive

const_primitive::const_primitive(
	const typed_array<imaterial*>& Material,
	const uint_t_array& Points,
	const table& ConstantAttributes,
	const table& VertexAttributes
		) :
	material(Material),
	points(Points),
	constant_attributes(ConstantAttributes),
	vertex_attributes(VertexAttributes)
{
}

/////////////////////////////////////////////////////////////////////////////////////////////
// primitive

primitive::primitive(
	typed_array<imaterial*>& Material,
	uint_t_array& Points,
	table& ConstantAttributes,
	table& VertexAttributes
		) :
	material(Material),
	points(Points),
	constant_attributes(ConstantAttributes),
	vertex_attributes(VertexAttributes)
{
}

/////////////////////////////////////////////////////////////////////////////////////////////
// create

primitive* create(mesh& Mesh)
{
	mesh::primitive& generic_primitive = Mesh.primitives.create("particle");

	primitive* const result = new primitive(
		generic_primitive.structure["constant"].create<typed_array<imaterial*> >("material"),
		generic_primitive.structure["vertex"].create<uint_t_array >("points"),
		generic_primitive.attributes["constant"],
		generic_primitive.attributes["vertex"]
		);

	result->points.set_metadata_value(metadata::key::domain(), metadata::value::mesh_point_indices_domain());

	return result;
}

/////////////////////////////////////////////////////////////////////////////////////////////
// validate

const_primitive* validate(const mesh& Mesh, const mesh::primitive& Primitive)
{
	if(Primitive.type != "particle")
		return 0;

	try
	{
		require_valid_primitive(Mesh, Primitive);

		const table& constant_structure = require_structure(Primitive, "constant");
		const table& vertex_structure = require_structure(Primitive, "vertex");

		const typed_array<imaterial*>& material = require_array<typed_array<imaterial*> >(Primitive, constant_structure, "material");
		const uint_t_array& points = require_array<uint_t_array >(Primitive, vertex_structure, "points");

		const table& constant_attributes = require_attributes(Primitive, "constant");
		const table& vertex_attributes = require_attributes(Primitive, "vertex");

		require_metadata(Primitive, points, "points", metadata::key::domain(), metadata::value::mesh_point_indices_domain());

		require_table_row_count(Primitive, vertex_attributes, "vertex", vertex_structure.row_count());

		return new const_primitive(material, points, constant_attributes, vertex_attributes);
	}
	catch(std::exception& e)
	{
		log() << error << e.what() << std::endl;
	}

	return 0;
}

primitive* validate(const mesh& Mesh, mesh::primitive& Primitive)
{
	if(Primitive.type != "particle")
		return 0;

	try
	{
		require_valid_primitive(Mesh, Primitive);

		table& constant_structure = require_structure(Primitive, "constant");
		table& vertex_structure = require_structure(Primitive, "vertex");

		typed_array<imaterial*>& material = require_array<typed_array<imaterial*> >(Primitive, constant_structure, "material");
		uint_t_array& points = require_array<uint_t_array >(Primitive, vertex_structure, "points");

		table& constant_attributes = require_attributes(Primitive, "constant");
		table& vertex_attributes = require_attributes(Primitive, "vertex");

		require_metadata(Primitive, points, "points", metadata::key::domain(), metadata::value::mesh_point_indices_domain());

		require_table_row_count(Primitive, vertex_attributes, "vertex", vertex_structure.row_count());

		return new primitive(material, points, constant_attributes, vertex_attributes);
	}
	catch(std::exception& e)
	{
		log() << error << e.what() << std::endl;
	}

	return 0;
}

primitive* validate(const mesh& Mesh, pipeline_data<mesh::primitive>& Primitive)
{
  if(!Primitive.get())
    return 0;

	if(Primitive->type != "particle")
		return 0;

  return validate(Mesh, Primitive.writable());
}

} // namespace particle

} // namespace k3d

