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

#include "cubic_curve.h"
#include "metadata_keys.h"
#include "primitive_detail.h"

#include <numeric>

namespace k3d
{

namespace cubic_curve
{

/////////////////////////////////////////////////////////////////////////////////////////////
// const_primitive

const_primitive::const_primitive(
	const mesh::indices_t& FirstCurves,
	const mesh::counts_t& CurveCounts,
	const mesh::bools_t& PeriodicCurves,
	const mesh::materials_t& Materials,
	const mesh::indices_t& CurveFirstPoints,
	const mesh::counts_t& CurvePointCounts,
	const mesh::selection_t& CurveSelections,
	const mesh::indices_t& CurvePoints,
	const mesh::attribute_arrays_t& ConstantData,
	const mesh::attribute_arrays_t& UniformData,
	const mesh::attribute_arrays_t& VaryingData
		) :
	first_curves(FirstCurves),
	curve_counts(CurveCounts),
	periodic_curves(PeriodicCurves),
	materials(Materials),
	curve_first_points(CurveFirstPoints),
	curve_point_counts(CurvePointCounts),
	curve_selections(CurveSelections),
	curve_points(CurvePoints),
	constant_data(ConstantData),
	uniform_data(UniformData),
	varying_data(VaryingData)
{
}

/////////////////////////////////////////////////////////////////////////////////////////////
// primitive

primitive::primitive(
	mesh::indices_t& FirstCurves,
	mesh::counts_t& CurveCounts,
	mesh::bools_t& PeriodicCurves,
	mesh::materials_t& Materials,
	mesh::indices_t& CurveFirstPoints,
	mesh::counts_t& CurvePointCounts,
	mesh::selection_t& CurveSelections,
	mesh::indices_t& CurvePoints,
	mesh::attribute_arrays_t& ConstantData,
	mesh::attribute_arrays_t& UniformData,
	mesh::attribute_arrays_t& VaryingData
		) :
	first_curves(FirstCurves),
	curve_counts(CurveCounts),
	periodic_curves(PeriodicCurves),
	materials(Materials),
	curve_first_points(CurveFirstPoints),
	curve_point_counts(CurvePointCounts),
	curve_selections(CurveSelections),
	curve_points(CurvePoints),
	constant_data(ConstantData),
	uniform_data(UniformData),
	varying_data(VaryingData)
{
}

/////////////////////////////////////////////////////////////////////////////////////////////
// create

primitive* create(mesh& Mesh)
{
	mesh::primitive& generic_primitive = Mesh.primitives.create("cubic_curve");

	primitive* const result = new primitive(
		generic_primitive.topology.create<mesh::indices_t>("first_curves"),
		generic_primitive.topology.create<mesh::counts_t>("curve_counts"),
		generic_primitive.topology.create<mesh::bools_t>("periodic_curves"),
		generic_primitive.topology.create<mesh::materials_t>("materials"),
		generic_primitive.topology.create<mesh::indices_t>("curve_first_points"),
		generic_primitive.topology.create<mesh::counts_t>("curve_point_counts"),
		generic_primitive.topology.create<mesh::selection_t>("curve_selections"),
		generic_primitive.topology.create<mesh::indices_t>("curve_points"),
		generic_primitive.attributes["constant"],
		generic_primitive.attributes["uniform"],
		generic_primitive.attributes["varying"]
		);

	result->curve_points.set_metadata_value(metadata::key::domain(), metadata::value::mesh_point_indices_domain());

	return result;
}

/////////////////////////////////////////////////////////////////////////////////////////////
// validate

const_primitive* validate(const mesh::primitive& Primitive)
{
	if(Primitive.type != "cubic_curve")
		return 0;

	try
	{
		const mesh::indices_t& first_curves = require_const_array<mesh::indices_t>(Primitive, "first_curves");
		const mesh::counts_t& curve_counts = require_const_array<mesh::counts_t>(Primitive, "curve_counts");
		const mesh::bools_t& periodic_curves = require_const_array<mesh::bools_t>(Primitive, "periodic_curves");
		const mesh::materials_t& materials = require_const_array<mesh::materials_t>(Primitive, "materials");
		const mesh::indices_t& curve_first_points = require_const_array<mesh::indices_t>(Primitive, "curve_first_points");
		const mesh::counts_t& curve_point_counts = require_const_array<mesh::counts_t>(Primitive, "curve_point_counts");
		const mesh::selection_t& curve_selections = require_const_array<mesh::selection_t>(Primitive, "curve_selections");
		const mesh::indices_t& curve_points = require_const_array<mesh::indices_t>(Primitive, "curve_points");

		const mesh::attribute_arrays_t& constant_data = require_const_attribute_arrays(Primitive, "constant");
		const mesh::attribute_arrays_t& uniform_data = require_const_attribute_arrays(Primitive, "uniform");
		const mesh::attribute_arrays_t& varying_data = require_const_attribute_arrays(Primitive, "varying");

		require_metadata(Primitive, curve_points, "curve_points", metadata::key::domain(), metadata::value::mesh_point_indices_domain());

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

		return new const_primitive(first_curves, curve_counts, periodic_curves, materials, curve_first_points, curve_point_counts, curve_selections, curve_points, constant_data, uniform_data, varying_data);
	}
	catch(std::exception& e)
	{
		log() << error << e.what() << std::endl;
	}

	return 0;
}

primitive* validate(mesh::primitive& Primitive)
{
	if(Primitive.type != "cubic_curve")
		return 0;

	try
	{
		mesh::indices_t& first_curves = require_array<mesh::indices_t>(Primitive, "first_curves");
		mesh::counts_t& curve_counts = require_array<mesh::counts_t>(Primitive, "curve_counts");
		mesh::bools_t& periodic_curves = require_array<mesh::bools_t>(Primitive, "periodic_curves");
		mesh::materials_t& materials = require_array<mesh::materials_t>(Primitive, "materials");
		mesh::indices_t& curve_first_points = require_array<mesh::indices_t>(Primitive, "curve_first_points");
		mesh::counts_t& curve_point_counts = require_array<mesh::counts_t>(Primitive, "curve_point_counts");
		mesh::selection_t& curve_selections = require_array<mesh::selection_t>(Primitive, "curve_selections");
		mesh::indices_t& curve_points = require_array<mesh::indices_t>(Primitive, "curve_points");

		mesh::attribute_arrays_t& constant_data = require_attribute_arrays(Primitive, "constant");
		mesh::attribute_arrays_t& uniform_data = require_attribute_arrays(Primitive, "uniform");
		mesh::attribute_arrays_t& varying_data = require_attribute_arrays(Primitive, "varying");

		require_metadata(Primitive, curve_points, "curve_points", metadata::key::domain(), metadata::value::mesh_point_indices_domain());

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

		return new primitive(first_curves, curve_counts, periodic_curves, materials, curve_first_points, curve_point_counts, curve_selections, curve_points, constant_data, uniform_data, varying_data);
	}
	catch(std::exception& e)
	{
		log() << error << e.what() << std::endl;
	}

	return 0;
}

} // namespace cubic_curve

} // namespace k3d

