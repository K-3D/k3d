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

#include "metadata.h"
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

/////////////////////////////////////////////////////////////////////////////////////////////
// validate

const bool_t legacy_validate_nurbs_patches(const mesh& Mesh)
{
	if(!Mesh.nurbs_patches)
		return false;

	return_val_if_fail(Mesh.nurbs_patches->patch_first_points, false);
	return_val_if_fail(Mesh.nurbs_patches->patch_u_point_counts, false);
	return_val_if_fail(Mesh.nurbs_patches->patch_v_point_counts, false);
	return_val_if_fail(Mesh.nurbs_patches->patch_u_orders, false);
	return_val_if_fail(Mesh.nurbs_patches->patch_v_orders, false);
	return_val_if_fail(Mesh.nurbs_patches->patch_u_first_knots, false);
	return_val_if_fail(Mesh.nurbs_patches->patch_v_first_knots, false);
	return_val_if_fail(Mesh.nurbs_patches->patch_selection, false);
	return_val_if_fail(Mesh.nurbs_patches->patch_materials, false);
	return_val_if_fail(Mesh.nurbs_patches->patch_points, false);
	return_val_if_fail(Mesh.nurbs_patches->patch_point_weights, false);
	return_val_if_fail(Mesh.nurbs_patches->patch_u_knots, false);
	return_val_if_fail(Mesh.nurbs_patches->patch_v_knots, false);

	return_val_if_fail(Mesh.nurbs_patches->patch_trim_curve_loop_counts, false);
	return_val_if_fail(Mesh.nurbs_patches->patch_first_trim_curve_loops, false);
	return_val_if_fail(Mesh.nurbs_patches->trim_points, false);
	return_val_if_fail(Mesh.nurbs_patches->trim_point_selection, false);
	return_val_if_fail(Mesh.nurbs_patches->first_trim_curves, false);
	return_val_if_fail(Mesh.nurbs_patches->trim_curve_counts, false);
	return_val_if_fail(Mesh.nurbs_patches->trim_curve_loop_selection, false);
	return_val_if_fail(Mesh.nurbs_patches->trim_curve_first_points, false);
	return_val_if_fail(Mesh.nurbs_patches->trim_curve_point_counts, false);
	return_val_if_fail(Mesh.nurbs_patches->trim_curve_orders, false);
	return_val_if_fail(Mesh.nurbs_patches->trim_curve_first_knots, false);
	return_val_if_fail(Mesh.nurbs_patches->trim_curve_selection, false);
	return_val_if_fail(Mesh.nurbs_patches->trim_curve_points, false);
	return_val_if_fail(Mesh.nurbs_patches->trim_curve_point_weights, false);
	return_val_if_fail(Mesh.nurbs_patches->trim_curve_knots, false);

	return true;
}

const_primitive* validate(const mesh& Mesh)
{
	if(!legacy_validate_nurbs_patches(Mesh))
		return 0;

	return new const_primitive(
		*Mesh.nurbs_patches->patch_first_points,
		*Mesh.nurbs_patches->patch_u_point_counts,
		*Mesh.nurbs_patches->patch_v_point_counts,
		*Mesh.nurbs_patches->patch_u_orders,
		*Mesh.nurbs_patches->patch_v_orders,
		*Mesh.nurbs_patches->patch_u_first_knots,
		*Mesh.nurbs_patches->patch_v_first_knots,
		*Mesh.nurbs_patches->patch_selection,
		*Mesh.nurbs_patches->patch_materials,
		*Mesh.nurbs_patches->patch_points,
		*Mesh.nurbs_patches->patch_point_weights,
		*Mesh.nurbs_patches->patch_u_knots,
		*Mesh.nurbs_patches->patch_v_knots,
		*Mesh.nurbs_patches->patch_trim_curve_loop_counts,
		*Mesh.nurbs_patches->patch_first_trim_curve_loops,
		*Mesh.nurbs_patches->first_trim_curves,
		*Mesh.nurbs_patches->trim_curve_counts,
		*Mesh.nurbs_patches->trim_curve_loop_selection,
		*Mesh.nurbs_patches->trim_curve_first_points,
		*Mesh.nurbs_patches->trim_curve_point_counts,
		*Mesh.nurbs_patches->trim_curve_orders,
		*Mesh.nurbs_patches->trim_curve_first_knots,
		*Mesh.nurbs_patches->trim_curve_selection,
		*Mesh.nurbs_patches->trim_curve_points,
		*Mesh.nurbs_patches->trim_curve_point_weights,
		*Mesh.nurbs_patches->trim_curve_knots,
		*Mesh.nurbs_patches->trim_points,
		*Mesh.nurbs_patches->trim_point_selection,
		Mesh.nurbs_patches->constant_data,
		Mesh.nurbs_patches->uniform_data,
		Mesh.nurbs_patches->varying_data);
}

primitive* validate(mesh& Mesh)
{
	if(!legacy_validate_nurbs_patches(Mesh))
		return 0;

	mesh::nurbs_patches_t& nurbs = Mesh.nurbs_patches.writable();

	return new primitive(
		nurbs.patch_first_points.writable(),
		nurbs.patch_u_point_counts.writable(),
		nurbs.patch_v_point_counts.writable(),
		nurbs.patch_u_orders.writable(),
		nurbs.patch_v_orders.writable(),
		nurbs.patch_u_first_knots.writable(),
		nurbs.patch_v_first_knots.writable(),
		nurbs.patch_selection.writable(),
		nurbs.patch_materials.writable(),
		nurbs.patch_points.writable(),
		nurbs.patch_point_weights.writable(),
		nurbs.patch_u_knots.writable(),
		nurbs.patch_v_knots.writable(),
		nurbs.patch_trim_curve_loop_counts.writable(),
		nurbs.patch_first_trim_curve_loops.writable(),
		nurbs.first_trim_curves.writable(),
		nurbs.trim_curve_counts.writable(),
		nurbs.trim_curve_loop_selection.writable(),
		nurbs.trim_curve_first_points.writable(),
		nurbs.trim_curve_point_counts.writable(),
		nurbs.trim_curve_orders.writable(),
		nurbs.trim_curve_first_knots.writable(),
		nurbs.trim_curve_selection.writable(),
		nurbs.trim_curve_points.writable(),
		nurbs.trim_curve_point_weights.writable(),
		nurbs.trim_curve_knots.writable(),
		nurbs.trim_points.writable(),
		nurbs.trim_point_selection.writable(),
		nurbs.constant_data,
		nurbs.uniform_data,
		nurbs.varying_data);
}

} // namespace nurbs_patch

} // namespace k3d

