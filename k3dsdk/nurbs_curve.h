#ifndef K3DSDK_NURBS_CURVE_H
#define K3DSDK_NURBS_CURVE_H

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

namespace nurbs_curve
{

/// Gathers the member arrays of a nurbs_curve primitive into a convenient package
class const_primitive
{
public:
	const_primitive(
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
		);

	const mesh::materials_t& material;
	const mesh::indices_t& curve_first_points;
	const mesh::counts_t& curve_point_counts;
	const mesh::orders_t& curve_orders;
	const mesh::indices_t& curve_first_knots;
	const mesh::selection_t& curve_selections;
	const mesh::indices_t& curve_points;
	const mesh::weights_t& curve_point_weights;
	const mesh::knots_t& curve_knots;
	const mesh::attribute_arrays_t& constant_data;
	const mesh::attribute_arrays_t& uniform_data;
	const mesh::attribute_arrays_t& varying_data;
};

/// Gathers the member arrays of a nurbs_curve primitive into a convenient package
class primitive
{
public:
	primitive(
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
		);


	mesh::materials_t& material;
	mesh::indices_t& curve_first_points;
	mesh::counts_t& curve_point_counts;
	mesh::orders_t& curve_orders;
	mesh::indices_t& curve_first_knots;
	mesh::selection_t& curve_selections;
	mesh::indices_t& curve_points;
	mesh::weights_t& curve_point_weights;
	mesh::knots_t& curve_knots;
	mesh::attribute_arrays_t& constant_data;
	mesh::attribute_arrays_t& uniform_data;
	mesh::attribute_arrays_t& varying_data;
};

/// Creates a new nurbs_curve mesh primitive, returning references to its member arrays.
/// The caller is responsible for the lifetime of the returned object.
primitive* create(mesh& Mesh);

/// Tests the given mesh primitive to see if it is a valid nurbs_curve primitive, returning references to its member arrays, or NULL.
/// The caller is responsible for the lifetime of the returned object.
const_primitive* validate(const mesh::primitive& GenericPrimitive);
/// Tests the given mesh primitive to see if it is a valid nurbs_curve primitive, returning references to its member arrays, or NULL.
/// The caller is responsible for the lifetime of the returned object.
primitive* validate(mesh::primitive& GenericPrimitive);

/// Adds a curve to an existing primitive, specified using the order of the curve and a set of control points.
/// Control points will be assigned unity weights and an open uniform knot vector is automatically supplied.
void add_curve(mesh& Mesh, primitive& Primitive, const uint_t Order, const mesh::points_t& ControlPoints, const uint_t RepeatPoints = 0);
/// Adds a curve to an existing primitive, specified using the order of the curve and a set of control points and control point weights.
/// An open uniform knot vector is automatically supplied.
void add_curve(mesh& Mesh, primitive& Primitive, const uint_t Order, const mesh::points_t& ControlPoints, const mesh::weights_t& Weights, const uint_t RepeatPoints = 0);
/// Adds a curve to an existing primitive, specified using the order of the curve and a set of control points, control point weights, and knot vector.
void add_curve(mesh& Mesh, primitive& Primitive, const uint_t Order, const mesh::points_t& ControlPoints, const mesh::weights_t& Weights, const mesh::knots_t& Knots, const uint_t RepeatPoints = 0);
/// Adds multiple curves to an existing primitive, specified by a set of points, curve orders, control point counts, and control point indices.
/// Control points will be assigned unity weights, and curves will be assigned open uniform knot vectors.
void add_curves(mesh& Mesh, primitive& Primitive, const mesh::points_t& Points, const mesh::orders_t& Orders, const mesh::counts_t& ControlPointCounts, const mesh::indices_t& ControlPoints);
/// Adds multiple curves to an existing primitive, specified by a set of points, curve orders, control point counts, control point indices, and control point weights.
/// Curves will be assigned open uniform knot vectors.
void add_curves(mesh& Mesh, primitive& Primitive, const mesh::points_t& Points, const mesh::orders_t& Orders, const mesh::counts_t& ControlPointCounts, const mesh::indices_t& ControlPoints, const mesh::weights_t& ControlPointWeights);
/// Adds multiple curves to an existing primitive, specified by a set of points, curve orders, control point counts, control point indices, control point weights, and knots.
void add_curves(mesh& Mesh, primitive& Primitive, const mesh::points_t& Points, const mesh::orders_t& Orders, const mesh::counts_t& ControlPointCounts, const mesh::indices_t& ControlPoints, const mesh::weights_t& ControlPointWeights, const mesh::knots_t& Knots);

/// Appends an open uniform knot vector to the given collection of knots.
void add_open_uniform_knots(const uint_t Order, const uint_t ControlPointCount, mesh::knots_t& Knots);

/** Computes a set of control points, weights, and knots that define an order-3 circular arc centered at the origin
	\param X Defines the X axis of the plane containing the arc
	\param Y Defines the Y axis of the plane containing the arc
	\param StartAngle Start angle of the arc in radians
	\param EndAngle End angle of the arc in radians
	\param Segments The number of NURBS segments in the resulting arc
	\param Knots Output container for the resulting arc knot vector
	\param Weights Output container for the resulting arc control point weights
	\param ControlPoints Output container for the resulting arc control point positions
*/
void circular_arc(const vector3& X, const vector3& Y, const double_t StartAngle, const double_t EndAngle, const uint_t Segments, mesh::knots_t& Knots, mesh::weights_t& Weights, mesh::points_t& ControlPoints);

} // namespace nurbs_curve

} // namespace k3d

#endif // !K3DSDK_NURBS_CURVE_H

