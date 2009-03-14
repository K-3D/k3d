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
#include "nurbs_curve.h"
#include "primitive_detail.h"

#include <numeric>

namespace k3d
{

namespace nurbs_curve
{

/////////////////////////////////////////////////////////////////////////////////////////////
// const_primitive

const_primitive::const_primitive(
	const mesh::indices_t& FirstCurves,
	const mesh::counts_t& CurveCounts,
	const mesh::materials_t& Materials,
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
	first_curves(FirstCurves),
	curve_counts(CurveCounts),
	materials(Materials),
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
	mesh::indices_t& FirstCurves,
	mesh::counts_t& CurveCounts,
	mesh::materials_t& Materials,
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
	first_curves(FirstCurves),
	curve_counts(CurveCounts),
	materials(Materials),
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
	mesh::nurbs_curve_groups_t& curves = Mesh.nurbs_curve_groups.create();
	mesh::indices_t& first_curves = curves.first_curves.create();
	mesh::counts_t& curve_counts = curves.curve_counts.create();
	mesh::materials_t& materials = curves.materials.create();
	mesh::indices_t& curve_first_points = curves.curve_first_points.create();
	mesh::counts_t& curve_point_counts = curves.curve_point_counts.create();
	mesh::orders_t& curve_orders = curves.curve_orders.create();
	mesh::indices_t& curve_first_knots = curves.curve_first_knots.create();
	mesh::selection_t& curve_selections = curves.curve_selection.create();
	mesh::indices_t& curve_points = curves.curve_points.create();
	mesh::weights_t& curve_point_weights = curves.curve_point_weights.create();
	mesh::knots_t& curve_knots = curves.curve_knots.create();

	primitive* const result = new primitive(
		first_curves,
		curve_counts,
		materials,
		curve_first_points,
		curve_point_counts,
		curve_orders,
		curve_first_knots,
		curve_selections,
		curve_points,
		curve_point_weights,
		curve_knots,
		curves.constant_data,
		curves.uniform_data,
		curves.varying_data
		);

	return result;
}

/////////////////////////////////////////////////////////////////////////////////////////////
// validate

const bool_t legacy_validate_nurbs_curve_groups(const mesh& Mesh)
{
	if(!Mesh.nurbs_curve_groups)
		return false;

	return_val_if_fail(Mesh.nurbs_curve_groups->first_curves, false);
	return_val_if_fail(Mesh.nurbs_curve_groups->curve_counts, false);
	return_val_if_fail(Mesh.nurbs_curve_groups->curve_first_points, false);
	return_val_if_fail(Mesh.nurbs_curve_groups->curve_point_counts, false);
	return_val_if_fail(Mesh.nurbs_curve_groups->curve_orders, false);
	return_val_if_fail(Mesh.nurbs_curve_groups->curve_first_knots, false);
	return_val_if_fail(Mesh.nurbs_curve_groups->curve_points, false);
	return_val_if_fail(Mesh.nurbs_curve_groups->curve_point_weights, false);
	return_val_if_fail(Mesh.nurbs_curve_groups->curve_knots, false);
	return_val_if_fail(Mesh.nurbs_curve_groups->curve_selection, false);
	return_val_if_fail(Mesh.nurbs_curve_groups->materials, false);

	return true;
}

const_primitive* validate(const mesh& Mesh)
{
	if(!legacy_validate_nurbs_curve_groups(Mesh))
		return 0;

	return new const_primitive(
		*Mesh.nurbs_curve_groups->first_curves,
		*Mesh.nurbs_curve_groups->curve_counts,
		*Mesh.nurbs_curve_groups->materials,
		*Mesh.nurbs_curve_groups->curve_first_points,
		*Mesh.nurbs_curve_groups->curve_point_counts,
		*Mesh.nurbs_curve_groups->curve_orders,
		*Mesh.nurbs_curve_groups->curve_first_knots,
		*Mesh.nurbs_curve_groups->curve_selection,
		*Mesh.nurbs_curve_groups->curve_points,
		*Mesh.nurbs_curve_groups->curve_point_weights,
		*Mesh.nurbs_curve_groups->curve_knots,
		Mesh.nurbs_curve_groups->constant_data,
		Mesh.nurbs_curve_groups->uniform_data,
		Mesh.nurbs_curve_groups->varying_data);
}

primitive* validate(mesh& Mesh)
{
	if(!legacy_validate_nurbs_curve_groups(Mesh))
		return 0;

	mesh::nurbs_curve_groups_t& nurbs = Mesh.nurbs_curve_groups.writable();

	return new primitive(
		nurbs.first_curves.writable(),
		nurbs.curve_counts.writable(),
		nurbs.materials.writable(),
		nurbs.curve_first_points.writable(),
		nurbs.curve_point_counts.writable(),
		nurbs.curve_orders.writable(),
		nurbs.curve_first_knots.writable(),
		nurbs.curve_selection.writable(),
		nurbs.curve_points.writable(),
		nurbs.curve_point_weights.writable(),
		nurbs.curve_knots.writable(),
		nurbs.constant_data,
		nurbs.uniform_data,
		nurbs.varying_data);
}

} // namespace nurbs_curve

} // namespace k3d

