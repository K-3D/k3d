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
#include <k3dsdk/table_copier.h>

#include <boost/scoped_ptr.hpp>
#include <stdexcept>

namespace module
{

namespace nurbs
{

/// Storage for an array of 4D points
typedef k3d::typed_array<k3d::point4> points4_t;

/// Convenient storage for all components and attributes of a single curve
/**
 * Points are represented in homogeneous coordinates, with the 4th coordinate being the weight.
 * If a single point is referenced more than once by the curve, it is duplicated in this representation,
 * so vertex and point attributes both have the same size.
 */
struct curve_arrays
{
	typedef std::vector<k3d::point4> points4_t;
	points4_t points;
	k3d::mesh::knots_t knots;
	k3d::table point_attributes;
	k3d::table curve_attributes;
	k3d::table parameter_attributes;
	k3d::table vertex_attributes;
	k3d::uint_t order;

	/// Encapsulates the complete evaluation of position, weight and attributes for a certain parameter value
	struct curve_value
	{
		/// Homogeneous position
		k3d::point4 weighted_position;
		k3d::table point_attributes;
		k3d::table vertex_attributes;

		inline const k3d::double_t weight() const
		{
			return weighted_position[3];
		}

		inline const k3d::point3 position() const
		{
			const k3d::double_t w = weight();
			return k3d::point3(weighted_position[0]/w, weighted_position[1]/w, weighted_position[2]/w);
		}
	};

	curve_arrays() {}

	/// Sets up arrays with the given number of control points and order, leaving the knot vector empty
	curve_arrays(const k3d::uint_t PointCount, const k3d::uint_t Order);

	/// Extract attributes and components from the given mesh and curve
	curve_arrays(const k3d::mesh& Mesh, const k3d::nurbs_curve::const_primitive& Primitive, const k3d::uint_t Curve, const k3d::bool_t NormalizeKnots = false);

	/// Verifies correct array lengths
	const k3d::bool_t validate() const;

	/// Adds the represented curve to a mesh
	void add_curve(k3d::mesh& Mesh, k3d::nurbs_curve::primitive& Primitive);

	/// Resize all arrays, ensuring knot length = Size+Order
	void resize(const k3d::uint_t Size, const k3d::uint_t Order);

	/// Evaluate the curve at parameter value U
	const curve_value evaluate(const k3d::double_t U) const;

	/// Normalized tangent at the given parameter value
	const k3d::vector3 tangent(const k3d::double_t U, const k3d::double_t DeltaU = 0.00001) const;

	/// 3D point coordinates
	void points3(k3d::mesh::points_t& Points) const;

	/// Extract the weights
	void weights(k3d::mesh::weights_t& Weights) const;

	/// Append another curve
	void append(const curve_arrays& Other);
};

/// Copy structure and attributes between curves
/**
 * Convenient copying class that treats points, point attributes and vertex attributes in one go.
 */
class curve_copier
{
public:
	curve_copier(const curve_arrays& Source, curve_arrays& Destination);

	/// Appends the given index value from each source array to each corresponding target array.
	void push_back(const k3d::uint_t Index);
	/// Computes a weighted sum of N values from each source array and appends the result to the corresponding target array.
	void push_back(const k3d::uint_t Count, const k3d::uint_t* Indices, const k3d::double_t* Weights);
	/// Copies the given source index value from each source array to the TargetIndex in each corresponding target array.
	void copy(const k3d::uint_t SourceIndex, const k3d::uint_t TargetIndex);
	/// Computes a weighted sum of N values from each source array and copies the result to the corresponding target array at the given TargetIndex.
	void copy(const k3d::uint_t Count, const k3d::uint_t* Indices, const k3d::double_t* Weights, const k3d::uint_t TargetIndex);

private:
	const curve_arrays& m_source;
	curve_arrays& m_destination;
	boost::scoped_ptr<k3d::table_copier> m_point_attribute_copier;
	boost::scoped_ptr<k3d::table_copier> m_vertex_attribute_copier;
};

/// Copies a curve from one primitive to another
void copy_curve(k3d::mesh& OutputMesh, k3d::nurbs_curve::primitive& OutputCurves, const k3d::mesh& InputMesh, const k3d::nurbs_curve::const_primitive& InputCurves, const k3d::uint_t Curve);

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
 * \param Curve The curve
 * \param u The u-value where to insert the knot
 * \param r The multiplicity of the new knot
 */
void insert_knot(curve_arrays& Curve, const k3d::double_t u, const k3d::uint_t r);

/// Knot insertion for a mesh
void insert_knot(k3d::mesh& OutputMesh, k3d::nurbs_curve::primitive& OutputCurves, const k3d::mesh& InputMesh, const k3d::nurbs_curve::const_primitive& InputCurves, k3d::uint_t Curve, const k3d::double_t u, const k3d::uint_t r);

/// Splits a curve at the given u parameter value
void split_curve(k3d::mesh& OutputMesh, k3d::nurbs_curve::primitive& OutputCurves, const k3d::mesh& InputMesh, const k3d::nurbs_curve::const_primitive& InputCurves, k3d::uint_t Curve, const k3d::double_t u);

/// Returns the multiplicity of the given curve
/**
 * \param Knots a collection of knot vectors
 * \param Begin The first knot of the knot vector to consider
 * \param Count The number of knots to consider
 */
const k3d::uint_t multiplicity(const k3d::mesh::knots_t& Knots, const k3d::double_t u, const k3d::uint_t Begin, const k3d::uint_t Count);

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

/// Calculate the non-zero values of the B-spline basis functions at the given parameter value
void basis_functions(k3d::mesh::knots_t& BasisFunctions, const k3d::mesh::knots_t& Knots, const k3d::uint_t Order, const k3d::double_t U);

/// Least squares approximation of the given sample points.
void approximate(k3d::mesh::points_t& Points, k3d::mesh::weights_t& Weights, const k3d::mesh::knots_t& SampleParameters, const points4_t& SamplePoints, const k3d::uint_t Order, const k3d::mesh::knots_t& Knots);

/// Polygonize a curve
void polygonize(k3d::mesh& OutputMesh, k3d::linear_curve::primitive& OutputCurve, const k3d::mesh& InputMesh, const k3d::nurbs_curve::const_primitive& InputCurves, const k3d::uint_t Curve, const k3d::uint_t Samples);

/// Samples the supplied knot value SampleCount + 1 times on each interval between unique knots
void sample(k3d::mesh::knots_t& Samples, const k3d::mesh::knots_t& Knots, const k3d::uint_t SampleCount);

} //namespace nurbs

} //namespace module

#endif // !MODULES_NURBS_NURBS_CURVES_H
