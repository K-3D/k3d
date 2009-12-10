#ifndef MODULES_NURBS_NURBS_CURVES_H
#define MODULES_NURBS_NURBS_CURVES_H
// K-3D
// Copyright (c) 1995-2009, Timothy M. Shead
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

/** \file
	\author Carsten Haubold (CarstenHaubold@web.de)
	\author Bart Janssens (bart.janssens@lid.kviv.be)
*/

#include <k3dsdk/types.h>
#include <k3dsdk/linear_curve.h>
#include <k3dsdk/mesh.h>
#include <k3dsdk/metadata.h>
#include <k3dsdk/metadata_keys.h>
#include <k3dsdk/nurbs_curve.h>

#include <boost/scoped_ptr.hpp>
#include <stdexcept>

namespace module
{

namespace nurbs
{

/// Storage for an array of 4D points
typedef k3d::typed_array<k3d::point4> points4_t;

/// Adds the given curve to the other primitive and mesh
void add_curve(k3d::mesh& OutputMesh, k3d::nurbs_curve::primitive& OutputCurves, const k3d::mesh& InputMesh, const k3d::nurbs_curve::const_primitive& InputCurves, const k3d::uint_t Curve);

/// Deletes a curve
void delete_curve(k3d::nurbs_curve::primitive& Curves, const k3d::uint_t Curve);

/// Normalizes the knot vector of the given curve number in the given NURBS curve primitive
void normalize_knot_vector(k3d::nurbs_curve::primitive& NurbsCurve, const k3d::uint_t Curve);

///Replace a point on the curve with a new one (by changing the index)
/**\param newIndex the index of the point which shall be used from now on
 * \param curve The curve we're operating on
 * \param point The index of the point which shall be replaced
 * \param continuous If this is set to true, the curve's knot vector is changed so that the curve intersects the point
 **/
void replace_point(k3d::nurbs_curve::primitive& NurbsCurve, k3d::uint_t newIndex, k3d::uint_t curve, k3d::uint_t point, bool continuous);

/// Replace duplicate point indices in the mesh
void replace_duplicate_points(k3d::mesh& Mesh);

/// Only affects the given range of primitives
void replace_duplicate_points(k3d::mesh& Mesh, const k3d::mesh::primitives_t::iterator Begin, const k3d::mesh::primitives_t::iterator End);

///Close the selected curve either by adding a new point or by replacing the former end points by their average
/** \param curve The curve to close
 *  \param keep_ends If this is true then we're going to add a new point, otherwise the old end points get discarded
 **/
void close_curve(k3d::mesh& OutputMesh, k3d::nurbs_curve::primitive& OutputCurves, const k3d::mesh& InputMesh, const k3d::nurbs_curve::const_primitive& InputCurves, const k3d::uint_t Curve, const k3d::bool_t keep_ends);

/// Elevate the degree of the curves listed in Curves, storing the resulting mesh in OutputMesh and OutputCurves
void elevate_curve_degree(k3d::mesh& OutputMesh, k3d::nurbs_curve::primitive& OutputCurves, const k3d::mesh& InputMesh, const k3d::nurbs_curve::const_primitive& InputCurves, const k3d::uint_t Curve, const k3d::uint_t Elevations = 1);

/// Flips a curve in the given mesh
void flip_curve(k3d::nurbs_curve::primitive& NurbsCurves, const k3d::uint_t curve);

/// Connect the two input curves at their selected end points, storing the result in Output
void connect_curves(k3d::mesh& OutputMesh, k3d::nurbs_curve::primitive& OutputCurves, const k3d::mesh& InputMesh, const k3d::uint_t Primitive1Index, const k3d::uint_t Curve1Index, const k3d::bool_t Curve1FirstPointSelection, const k3d::uint_t Primitive2Index, const k3d::uint_t Curve2Index, const k3d::bool_t Curve2FirstPointSelection);

/// Merge all input curves into one curve, if their endpoints are identical
void merge_connected_curves(k3d::mesh& OutputMesh, k3d::nurbs_curve::primitive& OutputCurves, const k3d::mesh& InputMesh, const k3d::uint_t InputCurvesPrimIdx);

///Insert a knot into a curve, makes use of the algorithm in "The NURBS book" by A. Piegl and W. Tiller
/**
 * \param curve The curve
 * \param u The u-value where to insert the knot
 * \param r The multiplicity of the new knot
 */
void insert_knot(k3d::mesh& OutputMesh, k3d::nurbs_curve::primitive& OutputCurves, const k3d::mesh& InputMesh, const k3d::nurbs_curve::const_primitive& InputCurves, k3d::uint_t Curve, const k3d::double_t u, const k3d::uint_t r);
void insert_knot(k3d::mesh::points_t& Points, k3d::mesh::knots_t& Knots, k3d::mesh::weights_t& Weights, const k3d::double_t u, const k3d::uint_t r, const k3d::uint_t Order);

/// Splits a curve at the given u parameter value
void split_curve(k3d::mesh& OutputMesh, k3d::nurbs_curve::primitive& OutputCurves, const k3d::mesh& InputMesh, const k3d::nurbs_curve::const_primitive& InputCurves, k3d::uint_t Curve, const k3d::double_t u);

/// Returns the multiplicity of the given curve
/**
 * \param Knots a collection of knot vectors
 * \param Begin The first knot of the knot vector to consider
 * \param Count The number of knots to consider
 */
const k3d::uint_t multiplicity(const k3d::mesh::knots_t& Knots, const k3d::double_t u, const k3d::uint_t Begin, const k3d::uint_t Count);

/// Extracts the points, knots and weights arrays from the given curve in the given mesh and curve primitive
void extract_curve_arrays(k3d::mesh::points_t& Points, k3d::mesh::knots_t& Knots, k3d::mesh::weights_t& Weights, const k3d::mesh& Mesh, const k3d::nurbs_curve::const_primitive& Curves, const k3d::uint_t Curve, const k3d::bool_t NormalizeKnots = false);

/// Appends new knots found in the given curve to the given output knot vector.
void append_common_knot_vector(k3d::mesh::knots_t& CommonKnotVector, const k3d::nurbs_curve::const_primitive& NurbsCurves, const k3d::uint_t Curve);

/// Find the centroid of all points in a curve
const k3d::point3 centroid(const k3d::mesh::points_t& Points, const k3d::nurbs_curve::const_primitive& Curves, const k3d::uint_t Curve);

/// Deletes all selected curves from the mesh
void delete_selected_curves(k3d::mesh& Mesh);

/// Adds a straight line to the given NURBS curve set. New points are added to the OutputMesh
void straight_line(const k3d::point3& Start, const k3d::point3 End, const k3d::uint_t Segments, k3d::nurbs_curve::primitive& NurbsCurves, k3d::mesh& OutputMesh, const k3d::uint_t Order = 2);

/// True if the given curve is closed
k3d::bool_t is_closed(const k3d::nurbs_curve::const_primitive& NurbsCurves, const k3d::uint_t Curve);

/// Evaluate the postion (x*weight, y*weight, z*weight, weight) using the given curve arrays
const k3d::point4 evaluate_position(const k3d::mesh::points_t& Points, const k3d::mesh::weights_t& Weights, const k3d::mesh::knots_t& Knots, const k3d::double_t U);

/// Get the normalized tangent vector at the given location on the curve
const k3d::vector3 tangent(const k3d::mesh::points_t& Points, const k3d::mesh::weights_t& Weights, const k3d::mesh::knots_t& Knots, const k3d::double_t U, const k3d::double_t DeltaU = 0.00001);

/// Calculate the non-zero values of the B-spline basis functions at the given parameter value
void basis_functions(k3d::mesh::knots_t& BasisFunctions, const k3d::mesh::knots_t& Knots, const k3d::uint_t Order, const k3d::double_t U);

/// Least squares approximation of the given sample points.
void approximate(k3d::mesh::points_t& Points, k3d::mesh::weights_t& Weights, const k3d::mesh::knots_t& SampleParameters, const points4_t& SamplePoints, const k3d::uint_t Order, const k3d::mesh::knots_t& Knots);

/// Polygonize a curve
void polygonize(k3d::mesh& OutputMesh, k3d::linear_curve::primitive& OutputCurve, const k3d::mesh& InputMesh, const k3d::nurbs_curve::const_primitive& InputCurves, const k3d::uint_t Curve, const k3d::uint_t Samples);

} //namespace nurbs

} //namespace module

#endif // !MODULES_NURBS_NURBS_CURVES_H
