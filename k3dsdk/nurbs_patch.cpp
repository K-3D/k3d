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
#include "nurbs_patch.h"
#include "primitive_detail.h"
#include "selection.h"
#include "string_cast.h"

#include <numeric>

namespace k3d
{

namespace nurbs_patch
{

/////////////////////////////////////////////////////////////////////////////////////////////
// const_primitive

const_primitive::const_primitive(
	const mesh::indices_t& PatchFirstPoints,
	const mesh::counts_t& PatchUPointCounts,
	const mesh::counts_t& PatchVPointCounts,
	const mesh::orders_t& PatchUOrders,
	const mesh::orders_t& PatchVOrders,
	const mesh::indices_t& PatchUFirstKnots,
	const mesh::indices_t& PatchVFirstKnots,
	const mesh::selection_t& PatchSelections,
	const mesh::materials_t& PatchMaterials,
	const mesh::indices_t& PatchPoints,
	const mesh::weights_t& PatchPointWeights,
	const mesh::knots_t& PatchUKnots,
	const mesh::knots_t& PatchVKnots,
	const mesh::indices_t& PatchFirstTrimLoops,
	const mesh::counts_t& PatchTrimLoopCounts,
	const mesh::indices_t& TrimLoopFirstCurves,
	const mesh::counts_t& TrimLoopCurveCounts,
	const mesh::selection_t& TrimLoopSelections,
	const mesh::indices_t& CurveFirstPoints,
	const mesh::counts_t& CurvePointCounts,
	const mesh::orders_t& CurveOrders,
	const mesh::indices_t& CurveFirstKnots,
	const mesh::selection_t& CurveSelections,
	const mesh::indices_t& CurvePoints,
	const mesh::weights_t& CurvePointWeights,
	const mesh::knots_t& CurveKnots,
	const mesh::points_2d_t& Points,
	const mesh::selection_t& PointSelections,
	const mesh::attribute_arrays_t& ConstantData,
	const mesh::attribute_arrays_t& UniformData,
	const mesh::attribute_arrays_t& VaryingData
		) :
	patch_first_points(PatchFirstPoints),
	patch_u_point_counts(PatchUPointCounts),
	patch_v_point_counts(PatchVPointCounts),
	patch_u_orders(PatchUOrders),
	patch_v_orders(PatchVOrders),
	patch_u_first_knots(PatchUFirstKnots),
	patch_v_first_knots(PatchVFirstKnots),
	patch_selections(PatchSelections),
	patch_materials(PatchMaterials),
	patch_points(PatchPoints),
	patch_point_weights(PatchPointWeights),
	patch_u_knots(PatchUKnots),
	patch_v_knots(PatchVKnots),
	patch_first_trim_loops(PatchFirstTrimLoops),
	patch_trim_loop_counts(PatchTrimLoopCounts),
	trim_loop_first_curves(TrimLoopFirstCurves),
	trim_loop_curve_counts(TrimLoopCurveCounts),
	trim_loop_selections(TrimLoopSelections),
	curve_first_points(CurveFirstPoints),
	curve_point_counts(CurvePointCounts),
	curve_orders(CurveOrders),
	curve_first_knots(CurveFirstKnots),
	curve_selections(CurveSelections),
	curve_points(CurvePoints),
	curve_point_weights(CurvePointWeights),
	curve_knots(CurveKnots),
	points(Points),
	point_selections(PointSelections),
	constant_data(ConstantData),
	uniform_data(UniformData),
	varying_data(VaryingData)
{
}

/////////////////////////////////////////////////////////////////////////////////////////////
// primitive

primitive::primitive(
	mesh::indices_t& PatchFirstPoints,
	mesh::counts_t& PatchUPointCounts,
	mesh::counts_t& PatchVPointCounts,
	mesh::orders_t& PatchUOrders,
	mesh::orders_t& PatchVOrders,
	mesh::indices_t& PatchUFirstKnots,
	mesh::indices_t& PatchVFirstKnots,
	mesh::selection_t& PatchSelections,
	mesh::materials_t& PatchMaterials,
	mesh::indices_t& PatchPoints,
	mesh::weights_t& PatchPointWeights,
	mesh::knots_t& PatchUKnots,
	mesh::knots_t& PatchVKnots,
	mesh::indices_t& PatchFirstTrimLoops,
	mesh::counts_t& PatchTrimLoopCounts,
	mesh::indices_t& TrimLoopFirstCurves,
	mesh::counts_t& TrimLoopCurveCounts,
	mesh::selection_t& TrimLoopSelections,
	mesh::indices_t& CurveFirstPoints,
	mesh::counts_t& CurvePointCounts,
	mesh::orders_t& CurveOrders,
	mesh::indices_t& CurveFirstKnots,
	mesh::selection_t& CurveSelections,
	mesh::indices_t& CurvePoints,
	mesh::weights_t& CurvePointWeights,
	mesh::knots_t& CurveKnots,
	mesh::points_2d_t& Points,
	mesh::selection_t& PointSelections,
	mesh::attribute_arrays_t& ConstantData,
	mesh::attribute_arrays_t& UniformData,
	mesh::attribute_arrays_t& VaryingData
		) :
	patch_first_points(PatchFirstPoints),
	patch_u_point_counts(PatchUPointCounts),
	patch_v_point_counts(PatchVPointCounts),
	patch_u_orders(PatchUOrders),
	patch_v_orders(PatchVOrders),
	patch_u_first_knots(PatchUFirstKnots),
	patch_v_first_knots(PatchVFirstKnots),
	patch_selections(PatchSelections),
	patch_materials(PatchMaterials),
	patch_points(PatchPoints),
	patch_point_weights(PatchPointWeights),
	patch_u_knots(PatchUKnots),
	patch_v_knots(PatchVKnots),
	patch_first_trim_loops(PatchFirstTrimLoops),
	patch_trim_loop_counts(PatchTrimLoopCounts),
	trim_loop_first_curves(TrimLoopFirstCurves),
	trim_loop_curve_counts(TrimLoopCurveCounts),
	trim_loop_selections(TrimLoopSelections),
	curve_first_points(CurveFirstPoints),
	curve_point_counts(CurvePointCounts),
	curve_orders(CurveOrders),
	curve_first_knots(CurveFirstKnots),
	curve_selections(CurveSelections),
	curve_points(CurvePoints),
	curve_point_weights(CurvePointWeights),
	curve_knots(CurveKnots),
	points(Points),
	point_selections(PointSelections),
	constant_data(ConstantData),
	uniform_data(UniformData),
	varying_data(VaryingData)
{
}

/////////////////////////////////////////////////////////////////////////////////////////////
// create

primitive* create(mesh& Mesh)
{
	mesh::primitive& generic_primitive = Mesh.primitives.create("nurbs_patch");

	primitive* const result = new primitive(
		generic_primitive.structure.create<mesh::indices_t>("patch_first_points"),
		generic_primitive.structure.create<mesh::counts_t>("patch_u_point_counts"),
		generic_primitive.structure.create<mesh::counts_t>("patch_v_point_counts"),
		generic_primitive.structure.create<mesh::orders_t>("patch_u_orders"),
		generic_primitive.structure.create<mesh::orders_t>("patch_v_orders"),
		generic_primitive.structure.create<mesh::indices_t>("patch_u_first_knots"),
		generic_primitive.structure.create<mesh::indices_t>("patch_v_first_knots"),
		generic_primitive.structure.create<mesh::selection_t>("patch_selections"),
		generic_primitive.structure.create<mesh::materials_t>("patch_materials"),
		generic_primitive.structure.create<mesh::indices_t>("patch_points"),
		generic_primitive.structure.create<mesh::weights_t>("patch_point_weights"),
		generic_primitive.structure.create<mesh::knots_t>("patch_u_knots"),
		generic_primitive.structure.create<mesh::knots_t>("patch_v_knots"),
		generic_primitive.structure.create<mesh::indices_t>("patch_first_trim_loops"),
		generic_primitive.structure.create<mesh::counts_t>("patch_trim_loop_counts"),
		generic_primitive.structure.create<mesh::indices_t>("trim_loop_first_curves"),
		generic_primitive.structure.create<mesh::counts_t>("trim_loop_curve_counts"),
		generic_primitive.structure.create<mesh::selection_t>("trim_loop_selections"),
		generic_primitive.structure.create<mesh::indices_t>("curve_first_points"),
		generic_primitive.structure.create<mesh::counts_t>("curve_point_counts"),
		generic_primitive.structure.create<mesh::orders_t>("curve_orders"),
		generic_primitive.structure.create<mesh::indices_t>("curve_first_knots"),
		generic_primitive.structure.create<mesh::selection_t>("curve_selections"),
		generic_primitive.structure.create<mesh::indices_t>("curve_points"),
		generic_primitive.structure.create<mesh::weights_t>("curve_point_weights"),
		generic_primitive.structure.create<mesh::knots_t>("curve_knots"),
		generic_primitive.structure.create<mesh::points_2d_t>("points"),
		generic_primitive.structure.create<mesh::selection_t>("point_selections"),
		generic_primitive.attributes["constant"],
		generic_primitive.attributes["uniform"],
		generic_primitive.attributes["varying"]
		);

	result->patch_selections.set_metadata_value(metadata::key::selection_component(), string_cast(selection::UNIFORM));
	result->patch_points.set_metadata_value(metadata::key::domain(), metadata::value::mesh_point_indices_domain());

	return result;
}

/////////////////////////////////////////////////////////////////////////////////////////////
// validate

const_primitive* validate(const mesh::primitive& Primitive)
{
	if(Primitive.type != "nurbs_patch")
		return 0;

	try
	{
		const mesh::indices_t& patch_first_points = require_const_array<mesh::indices_t>(Primitive, "patch_first_points");
		const mesh::counts_t& patch_u_point_counts = require_const_array<mesh::counts_t>(Primitive, "patch_u_point_counts");
		const mesh::counts_t& patch_v_point_counts = require_const_array<mesh::counts_t>(Primitive, "patch_v_point_counts");
		const mesh::orders_t& patch_u_orders = require_const_array<mesh::orders_t>(Primitive, "patch_u_orders");
		const mesh::orders_t& patch_v_orders = require_const_array<mesh::orders_t>(Primitive, "patch_v_orders");
		const mesh::indices_t& patch_u_first_knots = require_const_array<mesh::indices_t>(Primitive, "patch_u_first_knots");
		const mesh::indices_t& patch_v_first_knots = require_const_array<mesh::indices_t>(Primitive, "patch_v_first_knots");
		const mesh::selection_t& patch_selections = require_const_array<mesh::selection_t>(Primitive, "patch_selections");
		const mesh::materials_t& patch_materials = require_const_array<mesh::materials_t>(Primitive, "patch_materials");
		const mesh::indices_t& patch_points = require_const_array<mesh::indices_t>(Primitive, "patch_points");
		const mesh::weights_t& patch_point_weights = require_const_array<mesh::weights_t>(Primitive, "patch_point_weights");
		const mesh::knots_t& patch_u_knots = require_const_array<mesh::knots_t>(Primitive, "patch_u_knots");
		const mesh::knots_t& patch_v_knots = require_const_array<mesh::knots_t>(Primitive, "patch_v_knots");
		const mesh::indices_t& patch_first_trim_loops = require_const_array<mesh::indices_t>(Primitive, "patch_first_trim_loops");
		const mesh::counts_t& patch_trim_loop_counts = require_const_array<mesh::counts_t>(Primitive, "patch_trim_loop_counts");
		const mesh::indices_t& trim_loop_first_curves = require_const_array<mesh::indices_t>(Primitive, "trim_loop_first_curves");
		const mesh::counts_t& trim_loop_curve_counts = require_const_array<mesh::counts_t>(Primitive, "trim_loop_curve_counts");
		const mesh::selection_t& trim_loop_selections = require_const_array<mesh::selection_t>(Primitive, "trim_loop_selections");
		const mesh::indices_t& curve_first_points = require_const_array<mesh::indices_t>(Primitive, "curve_first_points");
		const mesh::counts_t& curve_point_counts = require_const_array<mesh::counts_t>(Primitive, "curve_point_counts");
		const mesh::orders_t& curve_orders = require_const_array<mesh::orders_t>(Primitive, "curve_orders");
		const mesh::indices_t& curve_first_knots = require_const_array<mesh::indices_t>(Primitive, "curve_first_knots");
		const mesh::selection_t& curve_selections = require_const_array<mesh::selection_t>(Primitive, "curve_selections");
		const mesh::indices_t& curve_points = require_const_array<mesh::indices_t>(Primitive, "curve_points");
		const mesh::weights_t& curve_point_weights = require_const_array<mesh::weights_t>(Primitive, "curve_point_weights");
		const mesh::knots_t& curve_knots = require_const_array<mesh::knots_t>(Primitive, "curve_knots");
		const mesh::points_2d_t& points = require_const_array<mesh::points_2d_t>(Primitive, "points");
		const mesh::selection_t& point_selections = require_const_array<mesh::selection_t>(Primitive, "point_selections");

		const mesh::attribute_arrays_t& constant_data = require_const_attribute_arrays(Primitive, "constant");
		const mesh::attribute_arrays_t& uniform_data = require_const_attribute_arrays(Primitive, "uniform");
		const mesh::attribute_arrays_t& varying_data = require_const_attribute_arrays(Primitive, "varying");

		require_metadata(Primitive, patch_selections, "patch_selections", metadata::key::selection_component(), string_cast(selection::UNIFORM));
		require_metadata(Primitive, patch_points, "patch_points", metadata::key::domain(), metadata::value::mesh_point_indices_domain());

		require_array_size(Primitive, patch_u_point_counts, "patch_u_point_counts", patch_first_points.size());
		require_array_size(Primitive, patch_v_point_counts, "patch_v_point_counts", patch_first_points.size());
		require_array_size(Primitive, patch_u_orders, "patch_u_orders", patch_first_points.size());
		require_array_size(Primitive, patch_v_orders, "patch_v_orders", patch_first_points.size());
		require_array_size(Primitive, patch_u_first_knots, "patch_u_first_knots", patch_first_points.size());
		require_array_size(Primitive, patch_v_first_knots, "patch_v_first_knots", patch_first_points.size());
		require_array_size(Primitive, patch_selections, "patch_selections", patch_first_points.size());
		require_array_size(Primitive, patch_materials, "patch_materials", patch_first_points.size());
//		require_array_size(Primitive, patch_points, "patch_points", std::accumulate(patch_point_counts.begin(), curve_point_counts.end(), 0));
//		require_array_size(Primitive, patch_point_weights, "patch_point_weights", std::accumulate(curve_point_counts.begin(), curve_point_counts.end(), 0));
		require_array_size(Primitive, patch_u_knots, "patch_u_knots",
			std::accumulate(patch_u_point_counts.begin(), patch_u_point_counts.end(), 0)
			+ std::accumulate(patch_u_orders.begin(), patch_u_orders.end(), 0));
		require_array_size(Primitive, patch_v_knots, "patch_v_knots",
			std::accumulate(patch_v_point_counts.begin(), patch_v_point_counts.end(), 0)
			+ std::accumulate(patch_v_orders.begin(), patch_v_orders.end(), 0));

		require_array_size(Primitive, patch_first_trim_loops, "patch_first_trim_loops", patch_first_points.size());
		require_array_size(Primitive, patch_trim_loop_counts, "patch_trim_loop_counts", patch_first_points.size());

		require_attribute_arrays_size(Primitive, constant_data, "constant", 1);
		require_attribute_arrays_size(Primitive, uniform_data, "uniform", patch_first_points.size());
//		require_attribute_arrays_size(Primitive, varying_data, "varying", std::accumulate(curve_point_counts.begin(), curve_point_counts.end(), 0));

	return new const_primitive(
		patch_first_points,
		patch_u_point_counts,
		patch_v_point_counts,
		patch_u_orders,
		patch_v_orders,
		patch_u_first_knots,
		patch_v_first_knots,
		patch_selections,
		patch_materials,
		patch_points,
		patch_point_weights,
		patch_u_knots,
		patch_v_knots,
		patch_first_trim_loops,
		patch_trim_loop_counts,
		trim_loop_first_curves,
		trim_loop_curve_counts,
		trim_loop_selections,
		curve_first_points,
		curve_point_counts,
		curve_orders,
		curve_first_knots,
		curve_selections,
		curve_points,
		curve_point_weights,
		curve_knots,
		points,
		point_selections,
		constant_data,
		uniform_data,
		varying_data);
	}
	catch(std::exception& e)
	{
		log() << error << e.what() << std::endl;
	}

	return 0;
}

primitive* validate(mesh::primitive& Primitive)
{
	assert_not_implemented();
	return 0;
}

} // namespace nurbs_patch

} // namespace k3d

