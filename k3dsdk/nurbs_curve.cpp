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

#include "basic_math.h"
#include "geometric_operations.h"
#include "metadata_keys.h"
#include "nurbs_curve.h"
#include "primitive_detail.h"
#include "selection.h"
#include "string_cast.h"

#include <numeric>

namespace k3d
{

namespace nurbs_curve
{

/////////////////////////////////////////////////////////////////////////////////////////////
// const_primitive

const_primitive::const_primitive(
	const mesh::materials_t& Material,
	const mesh::indices_t& CurveFirstPoints,
	const mesh::counts_t& CurvePointCounts,
	const mesh::orders_t& CurveOrders,
	const mesh::indices_t& CurveFirstKnots,
	const mesh::selection_t& CurveSelections,
	const mesh::indices_t& CurvePoints,
	const mesh::weights_t& CurvePointWeights,
	const mesh::knots_t& CurveKnots,
	const mesh::attribute_arrays_t& ConstantData,
	const mesh::attribute_arrays_t& UniformData,
	const mesh::attribute_arrays_t& VaryingData
		) :
	material(Material),
	curve_first_points(CurveFirstPoints),
	curve_point_counts(CurvePointCounts),
	curve_orders(CurveOrders),
	curve_first_knots(CurveFirstKnots),
	curve_selections(CurveSelections),
	curve_points(CurvePoints),
	curve_point_weights(CurvePointWeights),
	curve_knots(CurveKnots),
	constant_data(ConstantData),
	uniform_data(UniformData),
	varying_data(VaryingData)
{
}

/////////////////////////////////////////////////////////////////////////////////////////////
// primitive

primitive::primitive(
	mesh::materials_t& Material,
	mesh::indices_t& CurveFirstPoints,
	mesh::counts_t& CurvePointCounts,
	mesh::orders_t& CurveOrders,
	mesh::indices_t& CurveFirstKnots,
	mesh::selection_t& CurveSelections,
	mesh::indices_t& CurvePoints,
	mesh::weights_t& CurvePointWeights,
	mesh::knots_t& CurveKnots,
	mesh::attribute_arrays_t& ConstantData,
	mesh::attribute_arrays_t& UniformData,
	mesh::attribute_arrays_t& VaryingData
		) :
	material(Material),
	curve_first_points(CurveFirstPoints),
	curve_point_counts(CurvePointCounts),
	curve_orders(CurveOrders),
	curve_first_knots(CurveFirstKnots),
	curve_selections(CurveSelections),
	curve_points(CurvePoints),
	curve_point_weights(CurvePointWeights),
	curve_knots(CurveKnots),
	constant_data(ConstantData),
	uniform_data(UniformData),
	varying_data(VaryingData)
{
}

/////////////////////////////////////////////////////////////////////////////////////////////
// create

primitive* create(mesh& Mesh)
{
	mesh::primitive& generic_primitive = Mesh.primitives.create("nurbs_curve");

	primitive* const result = new primitive(
		generic_primitive.structure.create<mesh::materials_t>("material"),
		generic_primitive.structure.create<mesh::indices_t>("curve_first_points"),
		generic_primitive.structure.create<mesh::counts_t>("curve_point_counts"),
		generic_primitive.structure.create<mesh::orders_t>("curve_orders"),
		generic_primitive.structure.create<mesh::indices_t>("curve_first_knots"),
		generic_primitive.structure.create<mesh::selection_t>("curve_selections"),
		generic_primitive.structure.create<mesh::indices_t>("curve_points"),
		generic_primitive.structure.create<mesh::weights_t>("curve_point_weights"),
		generic_primitive.structure.create<mesh::knots_t>("curve_knots"),
		generic_primitive.attributes["constant"],
		generic_primitive.attributes["uniform"],
		generic_primitive.attributes["varying"]
		);

	result->curve_selections.set_metadata_value(metadata::key::selection_component(), string_cast(selection::UNIFORM));
	result->curve_points.set_metadata_value(metadata::key::domain(), metadata::value::mesh_point_indices_domain());

	return result;
}

/////////////////////////////////////////////////////////////////////////////////////////////
// validate

const_primitive* validate(const mesh::primitive& Primitive)
{
	if(Primitive.type != "nurbs_curve")
		return 0;

	try
	{
		const mesh::materials_t& material = require_const_array<mesh::materials_t>(Primitive, "material");
		const mesh::indices_t& curve_first_points = require_const_array<mesh::indices_t>(Primitive, "curve_first_points");
		const mesh::counts_t& curve_point_counts = require_const_array<mesh::counts_t>(Primitive, "curve_point_counts");
		const mesh::orders_t& curve_orders = require_const_array<mesh::orders_t>(Primitive, "curve_orders");
		const mesh::indices_t& curve_first_knots = require_const_array<mesh::indices_t>(Primitive, "curve_first_knots");
		const mesh::selection_t& curve_selections = require_const_array<mesh::selection_t>(Primitive, "curve_selections");
		const mesh::indices_t& curve_points = require_const_array<mesh::indices_t>(Primitive, "curve_points");
		const mesh::weights_t& curve_point_weights = require_const_array<mesh::weights_t>(Primitive, "curve_point_weights");
		const mesh::knots_t& curve_knots = require_const_array<mesh::knots_t>(Primitive, "curve_knots");

		const mesh::attribute_arrays_t& constant_data = require_const_attribute_arrays(Primitive, "constant");
		const mesh::attribute_arrays_t& uniform_data = require_const_attribute_arrays(Primitive, "uniform");
		const mesh::attribute_arrays_t& varying_data = require_const_attribute_arrays(Primitive, "varying");

		require_metadata(Primitive, curve_selections, "curve_selections", metadata::key::selection_component(), string_cast(selection::UNIFORM));
		require_metadata(Primitive, curve_points, "curve_points", metadata::key::domain(), metadata::value::mesh_point_indices_domain());

		require_array_size(Primitive, material, "material", 1);
		require_array_size(Primitive, curve_point_counts, "curve_point_counts", curve_first_points.size());
		require_array_size(Primitive, curve_orders, "curve_orders", curve_first_points.size());
		require_array_size(Primitive, curve_first_knots, "curve_first_knots", curve_first_points.size());
		require_array_size(Primitive, curve_selections, "curve_selections", curve_first_points.size());
		require_array_size(Primitive, curve_points, "curve_points", std::accumulate(curve_point_counts.begin(), curve_point_counts.end(), 0));
		require_array_size(Primitive, curve_point_weights, "curve_point_weights", std::accumulate(curve_point_counts.begin(), curve_point_counts.end(), 0));
		require_array_size(Primitive, curve_knots, "curve_knots",
			std::accumulate(curve_point_counts.begin(), curve_point_counts.end(), 0)
			+ std::accumulate(curve_orders.begin(), curve_orders.end(), 0));

		require_attribute_arrays_size(Primitive, constant_data, "constant", 1);
		require_attribute_arrays_size(Primitive, uniform_data, "uniform", curve_first_points.size());
		require_attribute_arrays_size(Primitive, varying_data, "varying", std::accumulate(curve_point_counts.begin(), curve_point_counts.end(), 0));

		return new const_primitive(material, curve_first_points, curve_point_counts, curve_orders, curve_first_knots, curve_selections, curve_points,curve_point_weights, curve_knots, constant_data, uniform_data, varying_data);
	}
	catch(std::exception& e)
	{
		log() << error << e.what() << std::endl;
	}

	return 0;
}

primitive* validate(mesh::primitive& Primitive)
{
	if(Primitive.type != "nurbs_curve")
		return 0;

	try
	{
		mesh::materials_t& material = require_array<mesh::materials_t>(Primitive, "material");
		mesh::indices_t& curve_first_points = require_array<mesh::indices_t>(Primitive, "curve_first_points");
		mesh::counts_t& curve_point_counts = require_array<mesh::counts_t>(Primitive, "curve_point_counts");
		mesh::orders_t& curve_orders = require_array<mesh::orders_t>(Primitive, "curve_orders");
		mesh::indices_t& curve_first_knots = require_array<mesh::indices_t>(Primitive, "curve_first_knots");
		mesh::selection_t& curve_selections = require_array<mesh::selection_t>(Primitive, "curve_selections");
		mesh::indices_t& curve_points = require_array<mesh::indices_t>(Primitive, "curve_points");
		mesh::weights_t& curve_point_weights = require_array<mesh::weights_t>(Primitive, "curve_point_weights");
		mesh::knots_t& curve_knots = require_array<mesh::knots_t>(Primitive, "curve_knots");

		mesh::attribute_arrays_t& constant_data = require_attribute_arrays(Primitive, "constant");
		mesh::attribute_arrays_t& uniform_data = require_attribute_arrays(Primitive, "uniform");
		mesh::attribute_arrays_t& varying_data = require_attribute_arrays(Primitive, "varying");

		require_metadata(Primitive, curve_selections, "curve_selections", metadata::key::selection_component(), string_cast(selection::UNIFORM));
		require_metadata(Primitive, curve_points, "curve_points", metadata::key::domain(), metadata::value::mesh_point_indices_domain());

		require_array_size(Primitive, material, "material", 1);
		require_array_size(Primitive, curve_point_counts, "curve_point_counts", curve_first_points.size());
		require_array_size(Primitive, curve_orders, "curve_orders", curve_first_points.size());
		require_array_size(Primitive, curve_first_knots, "curve_first_knots", curve_first_points.size());
		require_array_size(Primitive, curve_selections, "curve_selections", curve_first_points.size());
		require_array_size(Primitive, curve_points, "curve_points", std::accumulate(curve_point_counts.begin(), curve_point_counts.end(), 0));
		require_array_size(Primitive, curve_point_weights, "curve_point_weights", std::accumulate(curve_point_counts.begin(), curve_point_counts.end(), 0));
		require_array_size(Primitive, curve_knots, "curve_knots",
			std::accumulate(curve_point_counts.begin(), curve_point_counts.end(), 0)
			+ std::accumulate(curve_orders.begin(), curve_orders.end(), 0));

		require_attribute_arrays_size(Primitive, constant_data, "constant", 1);
		require_attribute_arrays_size(Primitive, uniform_data, "uniform", curve_first_points.size());
		require_attribute_arrays_size(Primitive, varying_data, "varying", std::accumulate(curve_point_counts.begin(), curve_point_counts.end(), 0));

		return new primitive(material, curve_first_points, curve_point_counts, curve_orders, curve_first_knots, curve_selections, curve_points,curve_point_weights, curve_knots, constant_data, uniform_data, varying_data);
	}
	catch(std::exception& e)
	{
		log() << error << e.what() << std::endl;
	}

	return 0;
}

void add_curve(mesh& Mesh, primitive& Primitive, const uint_t Order, const mesh::points_t& ControlPoints, imaterial* const Material, const uint_t RepeatPoints)
{
	add_curve(Mesh, Primitive, Order, ControlPoints, mesh::weights_t(ControlPoints.size(), 1), Material, RepeatPoints);
}

void add_curve(mesh& Mesh, primitive& Primitive, const uint_t Order, const mesh::points_t& ControlPoints, const mesh::weights_t& Weights, imaterial* const Material, const uint_t RepeatPoints)
{
	return_if_fail(ControlPoints.size() + RepeatPoints >= Order);

	mesh::knots_t knots;
        knots.insert(knots.end(), Order, 0);
	for(uint_t i = 1; i <= ControlPoints.size() + RepeatPoints - Order; ++i)
		knots.insert(knots.end(), 1, i);
	knots.insert(knots.end(), Order, ControlPoints.size() + RepeatPoints - Order + 1);

	add_curve(Mesh, Primitive, Order, ControlPoints, Weights, knots, Material, RepeatPoints);
}

void add_curve(mesh& Mesh, primitive& Primitive, const uint_t Order, const mesh::points_t& ControlPoints, const mesh::weights_t& Weights, const mesh::knots_t& Knots, imaterial* const Material, const uint_t RepeatPoints)
{
	return_if_fail(Mesh.points);
	return_if_fail(Mesh.point_selection);

	return_if_fail(Order >= 2);
	return_if_fail(ControlPoints.size() + RepeatPoints >= Order);
	return_if_fail(ControlPoints.size() == Weights.size());
	return_if_fail(Knots.size() == ControlPoints.size() + RepeatPoints + Order);

	mesh::points_t& points = Mesh.points.writable();
	mesh::selection_t& point_selection = Mesh.point_selection.writable();

	Primitive.material.push_back(Material);
        Primitive.curve_first_points.push_back(Primitive.curve_points.size());
        Primitive.curve_point_counts.push_back(ControlPoints.size() + RepeatPoints);
        Primitive.curve_orders.push_back(Order);
        Primitive.curve_first_knots.push_back(Primitive.curve_knots.size());
        Primitive.curve_selections.push_back(0);

	const uint_t start_index = Primitive.curve_points.size();
        for(uint_t i = 0; i != ControlPoints.size(); ++i)
        {
                Primitive.curve_points.push_back(points.size());
                Primitive.curve_point_weights.push_back(Weights[i]);
                points.push_back(ControlPoints[i]);
                point_selection.push_back(0);
        }

	for(uint_t i = 0; i != RepeatPoints; ++i)
	{
		const uint_t repeat_index = start_index + (i % ControlPoints.size());
		Primitive.curve_points.push_back(Primitive.curve_points[repeat_index]);
		Primitive.curve_point_weights.push_back(Primitive.curve_point_weights[repeat_index]);
	}

        Primitive.curve_knots.insert(Primitive.curve_knots.end(), Knots.begin(), Knots.end());
}

void circular_arc(const vector3& X, const vector3& Y, const double_t StartAngle, const double_t EndAngle, const uint_t Segments, mesh::knots_t& Knots, mesh::weights_t& Weights, mesh::points_t& ControlPoints)
{
	return_if_fail(Segments);

	const double_t theta = (EndAngle - StartAngle) / static_cast<double_t>(Segments);
	const double_t weight = std::cos(std::fabs(theta) * 0.5);

	Knots.clear();
	Knots.insert(Knots.end(), 3, 0);
	for(uint_t i = 1; i != Segments; ++i)
		Knots.insert(Knots.end(), 2, i);
	Knots.insert(Knots.end(), 3, Segments);

	Weights.clear();
	Weights.push_back(1.0);
	for(uint_t i = 0; i != Segments; ++i)
	{
		Weights.push_back(weight);
		Weights.push_back(1);
	}

	ControlPoints.clear();
	ControlPoints.push_back(k3d::to_point(std::cos(StartAngle) * X + std::sin(StartAngle) * Y));
	for(uint_t i = 0; i != Segments; ++i)
	{
		const double_t a0 = mix(StartAngle, EndAngle, static_cast<double_t>(i) / static_cast<double_t>(Segments));
		const double_t a2 = mix(StartAngle, EndAngle, static_cast<double_t>(i+1) / static_cast<double_t>(Segments));

		const point3 p0(k3d::to_point(std::cos(a0) * X + std::sin(a0) * Y));
		const point3 p2(k3d::to_point(std::cos(a2) * X + std::sin(a2) * Y));

		const point3 t0(k3d::to_point(-std::sin(a0) * X + std::cos(a0) * Y));
		const point3 t2(k3d::to_point(-std::sin(a2) * X + std::cos(a2) * Y));

		point3 p1;
		intersect_lines(p0, to_vector(t0), p2, to_vector(t2), p1);

		ControlPoints.push_back(p1);
		ControlPoints.push_back(p2);
	}
}

} // namespace nurbs_curve

} // namespace k3d

