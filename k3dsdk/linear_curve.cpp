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

#include "linear_curve.h"
#include "metadata_keys.h"
#include "primitive_validation.h"
#include "selection.h"
#include "string_cast.h"

#include <numeric>

namespace k3d
{

namespace linear_curve
{

/////////////////////////////////////////////////////////////////////////////////////////////
// const_primitive

const_primitive::const_primitive(
	const mesh::bools_t& Periodic,
	const mesh::materials_t& Material,
	const mesh::indices_t& CurveFirstPoints,
	const mesh::counts_t& CurvePointCounts,
	const mesh::selection_t& CurveSelections,
	const mesh::indices_t& CurvePoints,
	const mesh::table_t& ConstantAttributes,
	const mesh::table_t& UniformAttributes,
	const mesh::table_t& VaryingAttributes,
	const mesh::table_t& VertexAttributes
		) :
	periodic(Periodic),
	material(Material),
	curve_first_points(CurveFirstPoints),
	curve_point_counts(CurvePointCounts),
	curve_selections(CurveSelections),
	curve_points(CurvePoints),
	constant_attributes(ConstantAttributes),
	uniform_attributes(UniformAttributes),
	varying_attributes(VaryingAttributes),
	vertex_attributes(VertexAttributes)
{
}

/////////////////////////////////////////////////////////////////////////////////////////////
// primitive

primitive::primitive(
	mesh::bools_t& Periodic,
	mesh::materials_t& Material,
	mesh::indices_t& CurveFirstPoints,
	mesh::counts_t& CurvePointCounts,
	mesh::selection_t& CurveSelections,
	mesh::indices_t& CurvePoints,
	mesh::table_t& ConstantAttributes,
	mesh::table_t& UniformAttributes,
	mesh::table_t& VaryingAttributes,
	mesh::table_t& VertexAttributes
		) :
	periodic(Periodic),
	material(Material),
	curve_first_points(CurveFirstPoints),
	curve_point_counts(CurvePointCounts),
	curve_selections(CurveSelections),
	curve_points(CurvePoints),
	constant_attributes(ConstantAttributes),
	uniform_attributes(UniformAttributes),
	varying_attributes(VaryingAttributes),
	vertex_attributes(VertexAttributes)
{
}

/////////////////////////////////////////////////////////////////////////////////////////////
// create

primitive* create(mesh& Mesh)
{
	mesh::primitive& generic_primitive = Mesh.primitives.create("linear_curve");

	primitive* const result = new primitive(
		generic_primitive.structure["constant"].create<mesh::bools_t>("periodic"),
		generic_primitive.structure["constant"].create<mesh::materials_t>("material"),
		generic_primitive.structure["uniform"].create<mesh::indices_t >("curve_first_points"),
		generic_primitive.structure["uniform"].create<mesh::counts_t >("curve_point_counts"),
		generic_primitive.structure["uniform"].create<mesh::selection_t>("curve_selections"),
		generic_primitive.structure["vertex"].create<mesh::indices_t >("curve_points"),
		generic_primitive.attributes["constant"],
		generic_primitive.attributes["uniform"],
		generic_primitive.attributes["varying"],
		generic_primitive.attributes["vertex"]
		);

	result->curve_selections.set_metadata_value(metadata::key::role(), metadata::value::selection_role());
	result->curve_points.set_metadata_value(metadata::key::domain(), metadata::value::mesh_point_indices_domain());

	return result;
}

/////////////////////////////////////////////////////////////////////////////////////////////
// validate

const_primitive* validate(const mesh& Mesh, const mesh::primitive& Primitive)
{
	if(Primitive.type != "linear_curve")
		return 0;

	try
	{
		require_valid_primitive(Mesh, Primitive);

		const mesh::table_t& constant_structure = require_structure(Primitive, "constant");
		const mesh::table_t& uniform_structure = require_structure(Primitive, "uniform");
		const mesh::table_t& vertex_structure = require_structure(Primitive, "vertex");

		const mesh::table_t& constant_attributes = require_attributes(Primitive, "constant");
		const mesh::table_t& uniform_attributes = require_attributes(Primitive, "uniform");
		const mesh::table_t& varying_attributes = require_attributes(Primitive, "varying");
		const mesh::table_t& vertex_attributes = require_attributes(Primitive, "vertex");

		const mesh::bools_t& periodic = require_array<mesh::bools_t>(Primitive, constant_structure, "periodic");
		const mesh::materials_t& material = require_array<mesh::materials_t>(Primitive, constant_structure, "material");
		const mesh::indices_t& curve_first_points = require_array<mesh::indices_t>(Primitive, uniform_structure, "curve_first_points");
		const mesh::counts_t& curve_point_counts = require_array<mesh::counts_t>(Primitive, uniform_structure, "curve_point_counts");
		const mesh::selection_t& curve_selections = require_array<mesh::selection_t>(Primitive, uniform_structure, "curve_selections");
		const mesh::indices_t& curve_points = require_array<mesh::indices_t>(Primitive, vertex_structure, "curve_points");

		require_metadata(Primitive, curve_selections, "curve_selections", metadata::key::role(), metadata::value::selection_role());
		require_metadata(Primitive, curve_points, "curve_points", metadata::key::domain(), metadata::value::mesh_point_indices_domain());

		require_table_row_count(Primitive, vertex_structure, "vertex", std::accumulate(curve_point_counts.begin(), curve_point_counts.end(), 0));

		/** \todo Compute varying attributes size here */
		//require_table_row_count(Primitive, varying_attributes, "varying", );

		return new const_primitive(periodic, material, curve_first_points, curve_point_counts, curve_selections, curve_points, constant_attributes, uniform_attributes, varying_attributes, vertex_attributes);
	}
	catch(std::exception& e)
	{
		log() << error << e.what() << std::endl;
	}

	return 0;
}

primitive* validate(const mesh& Mesh, mesh::primitive& Primitive)
{
	if(Primitive.type != "linear_curve")
		return 0;

	try
	{
		require_valid_primitive(Mesh, Primitive);

		mesh::table_t& constant_structure = require_structure(Primitive, "constant");
		mesh::table_t& uniform_structure = require_structure(Primitive, "uniform");
		mesh::table_t& vertex_structure = require_structure(Primitive, "vertex");

		mesh::table_t& constant_attributes = require_attributes(Primitive, "constant");
		mesh::table_t& uniform_attributes = require_attributes(Primitive, "uniform");
		mesh::table_t& varying_attributes = require_attributes(Primitive, "varying");
		mesh::table_t& vertex_attributes = require_attributes(Primitive, "vertex");

		mesh::bools_t& periodic = require_array<mesh::bools_t>(Primitive, constant_structure, "periodic");
		mesh::materials_t& material = require_array<mesh::materials_t>(Primitive, constant_structure, "material");
		mesh::indices_t& curve_first_points = require_array<mesh::indices_t>(Primitive, uniform_structure, "curve_first_points");
		mesh::counts_t& curve_point_counts = require_array<mesh::counts_t>(Primitive, uniform_structure, "curve_point_counts");
		mesh::selection_t& curve_selections = require_array<mesh::selection_t>(Primitive, uniform_structure, "curve_selections");
		mesh::indices_t& curve_points = require_array<mesh::indices_t>(Primitive, vertex_structure, "curve_points");

		require_metadata(Primitive, curve_selections, "curve_selections", metadata::key::role(), metadata::value::selection_role());
		require_metadata(Primitive, curve_points, "curve_points", metadata::key::domain(), metadata::value::mesh_point_indices_domain());

		require_table_row_count(Primitive, vertex_structure, "vertex", std::accumulate(curve_point_counts.begin(), curve_point_counts.end(), 0));

		/** \todo Compute varying attributes size here */
		//require_table_row_count(Primitive, varying_attributes, "varying", );

		return new primitive(periodic, material, curve_first_points, curve_point_counts, curve_selections, curve_points, constant_attributes, uniform_attributes, varying_attributes, vertex_attributes);
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

	if(Primitive->type != "linear_curve")
		return 0;

  return validate(Mesh, Primitive.writable());
}

} // namespace linear_curve

} // namespace k3d

