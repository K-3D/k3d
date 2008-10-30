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

#include "array_metadata.h"
#include "linear_curve.h"
#include "primitive_detail.h"

#include <numeric>

namespace k3d
{

namespace linear_curve
{

/////////////////////////////////////////////////////////////////////////////////////////////
// const_primitive

const_primitive::const_primitive(
	const uint_t_array& FirstCurves,
	const uint_t_array& CurveCounts,
	const typed_array<bool_t>& PeriodicCurves,
	const typed_array<imaterial*>& Materials,
	const uint_t_array& CurveFirstPoints,
	const uint_t_array& CurvePointCounts,
	const typed_array<double_t>& CurveSelections,
	const uint_t_array& CurvePoints,
	const attribute_arrays& ConstantData,
	const attribute_arrays& UniformData,
	const attribute_arrays& VaryingData
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
	uint_t_array& FirstCurves,
	uint_t_array& CurveCounts,
	typed_array<bool_t>& PeriodicCurves,
	typed_array<imaterial*>& Materials,
	uint_t_array& CurveFirstPoints,
	uint_t_array& CurvePointCounts,
	typed_array<double_t>& CurveSelections,
	uint_t_array& CurvePoints,
	attribute_arrays& ConstantData,
	attribute_arrays& UniformData,
	attribute_arrays& VaryingData
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
	mesh::primitive& generic_primitive = Mesh.primitives.create("linear_curve");

	primitive* const result = new primitive(
		generic_primitive.topology.create<uint_t_array >("first_curves"),
		generic_primitive.topology.create<uint_t_array >("curve_counts"),
		generic_primitive.topology.create<typed_array<bool_t> >("periodic_curves"),
		generic_primitive.topology.create<typed_array<imaterial*> >("materials"),
		generic_primitive.topology.create<uint_t_array >("curve_first_points"),
		generic_primitive.topology.create<uint_t_array >("curve_point_counts"),
		generic_primitive.topology.create<typed_array<double_t> >("curve_selections"),
		generic_primitive.topology.create<uint_t_array >("curve_points"),
		generic_primitive.attributes["constant"],
		generic_primitive.attributes["uniform"],
		generic_primitive.attributes["varying"]
		);

	result->curve_points.set_metadata_value(mesh_point_indices(), "true");

	return result;
}

/////////////////////////////////////////////////////////////////////////////////////////////
// validate

const_primitive* validate(const mesh::primitive& Primitive)
{
	if(Primitive.type != "linear_curve")
		return 0;

	try
	{
		const uint_t_array& first_curves = require_const_array<uint_t_array >(Primitive, "first_curves");
		const uint_t_array& curve_counts = require_const_array<uint_t_array >(Primitive, "curve_counts");
		const typed_array<bool_t>& periodic_curves = require_const_array<typed_array<bool_t> >(Primitive, "periodic_curves");
		const typed_array<imaterial*>& materials = require_const_array<typed_array<imaterial*> >(Primitive, "materials");
		const uint_t_array& curve_first_points = require_const_array<uint_t_array >(Primitive, "curve_first_points");
		const uint_t_array& curve_point_counts = require_const_array<uint_t_array >(Primitive, "curve_point_counts");
		const typed_array<double_t>& curve_selections = require_const_array<typed_array<double_t> >(Primitive, "curve_selections");
		const uint_t_array& curve_points = require_const_array<uint_t_array >(Primitive, "curve_points");

		const attribute_arrays& constant_data = require_const_attribute_arrays(Primitive, "constant");
		const attribute_arrays& uniform_data = require_const_attribute_arrays(Primitive, "uniform");
		const attribute_arrays& varying_data = require_const_attribute_arrays(Primitive, "varying");

		require_metadata(Primitive, curve_points, "curve_points", mesh_point_indices(), "true");

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
	if(Primitive.type != "linear_curve")
		return 0;

	try
	{
		uint_t_array& first_curves = require_array<uint_t_array >(Primitive, "first_curves");
		uint_t_array& curve_counts = require_array<uint_t_array >(Primitive, "curve_counts");
		typed_array<bool_t>& periodic_curves = require_array<typed_array<bool_t> >(Primitive, "periodic_curves");
		typed_array<imaterial*>& materials = require_array<typed_array<imaterial*> >(Primitive, "materials");
		uint_t_array& curve_first_points = require_array<uint_t_array >(Primitive, "curve_first_points");
		uint_t_array& curve_point_counts = require_array<uint_t_array >(Primitive, "curve_point_counts");
		typed_array<double_t>& curve_selections = require_array<typed_array<double_t> >(Primitive, "curve_selections");
		uint_t_array& curve_points = require_array<uint_t_array >(Primitive, "curve_points");

		attribute_arrays& constant_data = require_attribute_arrays(Primitive, "constant");
		attribute_arrays& uniform_data = require_attribute_arrays(Primitive, "uniform");
		attribute_arrays& varying_data = require_attribute_arrays(Primitive, "varying");

		require_metadata(Primitive, curve_points, "curve_points", mesh_point_indices(), "true");

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

} // namespace linear_curve

} // namespace k3d

