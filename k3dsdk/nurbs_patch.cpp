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
#include "nurbs_patch.h"
#include "primitive_detail.h"

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
	const mesh::counts_t& PatchTrimLoopCounts,
	const mesh::indices_t& PatchFirstTrimLoops,
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
	patch_trim_loop_counts(PatchTrimLoopCounts),
	patch_first_trim_loops(PatchFirstTrimLoops),
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
	mesh::counts_t& PatchTrimLoopCounts,
	mesh::indices_t& PatchFirstTrimLoops,
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
	patch_trim_loop_counts(PatchTrimLoopCounts),
	patch_first_trim_loops(PatchFirstTrimLoops),
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
	mesh::nurbs_patches_t& patches = Mesh.nurbs_patches.create();
	
	primitive* const result = new primitive(
		patches.patch_first_points.create(),
		patches.patch_u_point_counts.create(),
		patches.patch_v_point_counts.create(),
		patches.patch_u_orders.create(),
		patches.patch_v_orders.create(),
		patches.patch_u_first_knots.create(),
		patches.patch_v_first_knots.create(),
		patches.patch_selection.create(),
		patches.patch_materials.create(),
		patches.patch_points.create(),
		patches.patch_point_weights.create(),
		patches.patch_u_knots.create(),
		patches.patch_v_knots.create(),
		patches.patch_trim_curve_loop_counts.create(),
		patches.patch_first_trim_curve_loops.create(),
		patches.first_trim_curves.create(),
		patches.trim_curve_counts.create(),
		patches.trim_curve_loop_selection.create(),
		patches.trim_curve_first_points.create(),
		patches.trim_curve_point_counts.create(),
		patches.trim_curve_orders.create(),
		patches.trim_curve_first_knots.create(),
		patches.trim_curve_selection.create(),
		patches.trim_curve_points.create(),
		patches.trim_curve_point_weights.create(),
		patches.trim_curve_knots.create(),
		patches.trim_points.create(),
		patches.trim_point_selection.create(),
		patches.constant_data,
		patches.uniform_data,
		patches.varying_data
		);

	return result;
}

/*
/////////////////////////////////////////////////////////////////////////////////////////////
// validate

const_primitive* validate(const mesh::primitive& Primitive)
{
	if(Primitive.type != "nurbs_patch")
		return 0;

	try
	{
		const mesh::indices_t& first_patchs = require_const_array<mesh::indices_t>(Primitive, "first_patchs");
		const mesh::counts_t& patch_counts = require_const_array<mesh::counts_t>(Primitive, "patch_counts");
		const mesh::bools_t& periodic_patchs = require_const_array<mesh::bools_t>(Primitive, "periodic_patchs");
		const mesh::materials_t& materials = require_const_array<mesh::materials_t>(Primitive, "materials");
		const mesh::indices_t& patch_first_points = require_const_array<mesh::indices_t>(Primitive, "patch_first_points");
		const mesh::counts_t& patch_point_counts = require_const_array<mesh::counts_t>(Primitive, "patch_point_counts");
		const mesh::selection_t& patch_selections = require_const_array<mesh::selection_t>(Primitive, "patch_selections");
		const mesh::indices_t& patch_points = require_const_array<mesh::indices_t>(Primitive, "patch_points");

		const mesh::attribute_arrays_t& constant_data = require_const_attribute_arrays(Primitive, "constant");
		const mesh::attribute_arrays_t& uniform_data = require_const_attribute_arrays(Primitive, "uniform");
		const mesh::attribute_arrays_t& varying_data = require_const_attribute_arrays(Primitive, "varying");

		require_metadata(Primitive, patch_points, "patch_points", mesh_point_indices(), "true");

		require_array_size(Primitive, patch_counts, "patch_counts", first_patchs.size());
		require_array_size(Primitive, periodic_patchs, "periodic_patchs", first_patchs.size());
		require_array_size(Primitive, materials, "materials", first_patchs.size());

		require_array_size(Primitive, patch_first_points, "patch_first_points", std::accumulate(patch_counts.begin(), patch_counts.end(), 0));
		require_array_size(Primitive, patch_point_counts, "patch_point_counts", std::accumulate(patch_counts.begin(), patch_counts.end(), 0));
		require_array_size(Primitive, patch_selections, "patch_selections", std::accumulate(patch_counts.begin(), patch_counts.end(), 0));

		require_array_size(Primitive, patch_points, "patch_points", std::accumulate(patch_point_counts.begin(), patch_point_counts.end(), 0));

		require_attribute_arrays_size(Primitive, constant_data, "constant", first_patchs.size());
		
		require_attribute_arrays_size(Primitive, uniform_data, "uniform", std::accumulate(patch_counts.begin(), patch_counts.end(), 0));
		
		require_attribute_arrays_size(Primitive, varying_data, "varying", std::accumulate(patch_point_counts.begin(), patch_point_counts.end(), 0));

		return new const_primitive(first_patchs, patch_counts, periodic_patchs, materials, patch_first_points, patch_point_counts, patch_selections, patch_points, constant_data, uniform_data, varying_data);
	}
	catch(std::exception& e)
	{
		log() << error << e.what() << std::endl;
	}

	return 0;
}

primitive* validate(mesh::primitive& Primitive)
{
	if(Primitive.type != "nurbs_patch")
		return 0;

	try
	{
		mesh::indices_t& first_patchs = require_array<mesh::indices_t>(Primitive, "first_patchs");
		mesh::counts_t& patch_counts = require_array<mesh::counts_t>(Primitive, "patch_counts");
		mesh::bools_t& periodic_patchs = require_array<mesh::bools_t>(Primitive, "periodic_patchs");
		mesh::materials_t& materials = require_array<mesh::materials_t>(Primitive, "materials");
		mesh::indices_t& patch_first_points = require_array<mesh::indices_t>(Primitive, "patch_first_points");
		mesh::counts_t& patch_point_counts = require_array<mesh::counts_t>(Primitive, "patch_point_counts");
		mesh::selection_t& patch_selections = require_array<mesh::selection_t>(Primitive, "patch_selections");
		mesh::indices_t& patch_points = require_array<mesh::indices_t>(Primitive, "patch_points");

		mesh::attribute_arrays_t& constant_data = require_attribute_arrays(Primitive, "constant");
		mesh::attribute_arrays_t& uniform_data = require_attribute_arrays(Primitive, "uniform");
		mesh::attribute_arrays_t& varying_data = require_attribute_arrays(Primitive, "varying");

		require_metadata(Primitive, patch_points, "patch_points", mesh_point_indices(), "true");

		require_array_size(Primitive, patch_counts, "patch_counts", first_patchs.size());
		require_array_size(Primitive, periodic_patchs, "periodic_patchs", first_patchs.size());
		require_array_size(Primitive, materials, "materials", first_patchs.size());

		require_array_size(Primitive, patch_first_points, "patch_first_points", std::accumulate(patch_counts.begin(), patch_counts.end(), 0));
		require_array_size(Primitive, patch_point_counts, "patch_point_counts", std::accumulate(patch_counts.begin(), patch_counts.end(), 0));
		require_array_size(Primitive, patch_selections, "patch_selections", std::accumulate(patch_counts.begin(), patch_counts.end(), 0));

		require_array_size(Primitive, patch_points, "patch_points", std::accumulate(patch_point_counts.begin(), patch_point_counts.end(), 0));

		require_attribute_arrays_size(Primitive, constant_data, "constant", first_patchs.size());

		require_attribute_arrays_size(Primitive, uniform_data, "uniform", std::accumulate(patch_counts.begin(), patch_counts.end(), 0));

		require_attribute_arrays_size(Primitive, varying_data, "varying", std::accumulate(patch_point_counts.begin(), patch_point_counts.end(), 0));

		return new primitive(first_patchs, patch_counts, periodic_patchs, materials, patch_first_points, patch_point_counts, patch_selections, patch_points, constant_data, uniform_data, varying_data);
	}
	catch(std::exception& e)
	{
		log() << error << e.what() << std::endl;
	}

	return 0;
}
*/

} // namespace nurbs_patch

} // namespace k3d

