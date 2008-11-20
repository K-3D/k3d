#ifndef K3DSDK_NURBS_PATCH_H
#define K3DSDK_NURBS_PATCH_H

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

#include "mesh.h"

namespace k3d
{

namespace nurbs_patch
{

/// Gathers the member arrays of a nurbs_patch primitive into a convenient package
class const_primitive
{
public:
	const_primitive(
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
		);

	const mesh::indices_t& patch_first_points;
	const mesh::counts_t& patch_u_point_counts;
	const mesh::counts_t& patch_v_point_counts;
	const mesh::orders_t& patch_u_orders;
	const mesh::orders_t& patch_v_orders;
	const mesh::indices_t& patch_u_first_knots;
	const mesh::indices_t& patch_v_first_knots;
	const mesh::selection_t& patch_selections;
	const mesh::materials_t& patch_materials;
	const mesh::indices_t& patch_points;
	const mesh::weights_t& patch_point_weights;
	const mesh::knots_t& patch_u_knots;
	const mesh::knots_t& patch_v_knots;
	const mesh::counts_t& patch_trim_loop_counts;
	const mesh::indices_t& patch_first_trim_loops;
	const mesh::indices_t& trim_loop_first_curves;
	const mesh::counts_t& trim_loop_curve_counts;
	const mesh::selection_t& trim_loop_selections;
	const mesh::indices_t& curve_first_points;
	const mesh::counts_t& curve_point_counts;
	const mesh::orders_t& curve_orders;
	const mesh::indices_t& curve_first_knots;
	const mesh::selection_t& curve_selections;
	const mesh::indices_t& curve_points;
	const mesh::weights_t& curve_point_weights;
	const mesh::knots_t& curve_knots;
	const mesh::points_2d_t& points;
	const mesh::selection_t& point_selections;
	const mesh::attribute_arrays_t& constant_data;
	const mesh::attribute_arrays_t& uniform_data;
	const mesh::attribute_arrays_t& varying_data;
};

/// Gathers the member arrays of a nurbs_patch primitive into a convenient package
class primitive
{
public:
	primitive(
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
		);

	mesh::indices_t& patch_first_points;
	mesh::counts_t& patch_u_point_counts;
	mesh::counts_t& patch_v_point_counts;
	mesh::orders_t& patch_u_orders;
	mesh::orders_t& patch_v_orders;
	mesh::indices_t& patch_u_first_knots;
	mesh::indices_t& patch_v_first_knots;
	mesh::selection_t& patch_selections;
	mesh::materials_t& patch_materials;
	mesh::indices_t& patch_points;
	mesh::weights_t& patch_point_weights;
	mesh::knots_t& patch_u_knots;
	mesh::knots_t& patch_v_knots;
	mesh::counts_t& patch_trim_loop_counts;
	mesh::indices_t& patch_first_trim_loops;
	mesh::indices_t& trim_loop_first_curves;
	mesh::counts_t& trim_loop_curve_counts;
	mesh::selection_t& trim_loop_selections;
	mesh::indices_t& curve_first_points;
	mesh::counts_t& curve_point_counts;
	mesh::orders_t& curve_orders;
	mesh::indices_t& curve_first_knots;
	mesh::selection_t& curve_selections;
	mesh::indices_t& curve_points;
	mesh::weights_t& curve_point_weights;
	mesh::knots_t& curve_knots;
	mesh::points_2d_t& points;
	mesh::selection_t& point_selections;
	mesh::attribute_arrays_t& constant_data;
	mesh::attribute_arrays_t& uniform_data;
	mesh::attribute_arrays_t& varying_data;
};

/// Creates a new nurbs_patch mesh primitive, returning references to its member arrays.
/// The caller is responsible for the lifetime of the returned object.
primitive* create(mesh& Mesh);

/*
/// Tests the given mesh primitive to see if it is a valid nurbs_patch primitive, returning references to its member arrays, or NULL.
/// The caller is responsible for the lifetime of the returned object.
const_primitive* validate(const mesh::primitive& GenericPrimitive);
/// Tests the given mesh primitive to see if it is a valid nurbs_patch primitive, returning references to its member arrays, or NULL.
/// The caller is responsible for the lifetime of the returned object.
primitive* validate(mesh::primitive& GenericPrimitive);
*/

} // namespace nurbs_patch

} // namespace k3d

#endif // !K3DSDK_NURBS_PATCH_H

