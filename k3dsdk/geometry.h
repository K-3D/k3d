#ifndef K3DSDK_GEOMETRY_H
#define K3DSDK_GEOMETRY_H

// K-3D
// Copyright (c) 1995-2006, Timothy M. Shead
//
// Contact: tshead@k-3d.com
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public
// License as published by the Free Software Foundation; either
// version 2 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Public License for more details.
//
// You should have received a copy of the GNU General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

/** \file
		\brief 2D geometry routines
		\author Tim Shead (tshead@k-3d.com)
*/

#include "vectors.h"

#include <cassert>
#include <vector>

namespace k3d
{

class matrix4;

/////////////////////////////////////////////////////////////////////////////
// rectangle

/// A two-dimensional rectangle particularly suitable for use with window systems i.e. Y coordinates are reversed so that top <= bottom
class rectangle
{
public:
	rectangle(const double Left, const double Right, const double Top, const double Bottom);
	rectangle(const point2& TopLeft, const point2& BottomRight);
	/// Returns the rectangle width
	double width() const;
	/// Returns the rectangle height
	double height() const;
	/// Returns the top-left corner of the rectangle
	const point2 top_left() const;
	/// Returns the bottom-right corner of the rectangle
	const point2 bottom_right() const;
	/// Returns the center of the rectangle
	const point2 center() const;
	/// Tests a point to see if it is contained within the rectangle
	bool contains(const point2& Point) const;
	/// Stores the rectangle's left edge
	double left;
	/// Stores the rectangle's right edge
	double right;
	/// Stores the rectangle's top edge
	double top;
	/// Stores the rectangle's bottom edge
	double bottom;
};

/// Returns a rectangle where left <= right and top <= bottom
const rectangle normalize(const rectangle& Rectangle);

/// Output inserter
std::ostream& operator<<(std::ostream& Stream, const rectangle& Arg);
/// Input extractor
std::istream& operator>>(std::istream& Stream, rectangle& Arg);

/// Encapsulates a 3D plane surface
class plane
{
public:
	/// Constructs a plane from its normal and distance from the origin
	plane(const vector3& Normal, const double Distance);
	/// Constructs a plane from its normal and one point located within the plane
	plane(const vector3& Normal, const point3& Point);
	/// Constructs a plane from three points contained within the plane
	plane(const point3& PointA, const point3& PointB, const point3& PointC);

	vector3 normal;
	double distance;
};

/// Linear transformation if a plane by a transformation matrix
const plane operator*(const matrix4& Matrix, const plane& Plane);
/// Linear transformation if a plane by a transformation matrix
const plane operator*(const plane& Plane, const matrix4& Matrix);

std::ostream& operator<<(std::ostream& Stream, const plane& RHS);
std::istream& operator>>(std::istream& Stream, plane& RHS);

/////////////////////////////////////////////////////////////////////////////
// line2

/// Encapsulates a 2D line
class line2
{
public:
	/// Constructs a line from its direction vector and a point on the line
	line2(const vector2& Direction, const point2& Point);
	/// Constructs a line from two points on the line
	line2(const point2& Point1, const point2& Point2);

	vector2 direction;
	point2 point;
};

/////////////////////////////////////////////////////////////////////////////
// line3

/// Encapsulates a 3D line
class line3
{
public:
	/// Constructs a line from its direction vector and a point on the line
	line3(const vector3& Direction, const point3& Point);
	/// Constructs a line from two points on the line
	line3(const point3& Point1, const point3& Point2);

	vector3 direction;
	point3 point;
};

/*
/// Enumerates the ways in which two line segments might (or might not) intersect
typedef enum
{
	SDP_NOINTERSECTION,
	SDP_EXTERIORINTERSECTION,
	SDP_INTERIORINTERSECTION,

	SDP_EXTERIORCOLINEARPOINT,
	SDP_INTERIORCOLINEARPOINT,

	SDP_INTERIORCOLINEARPOINTS,
	SDP_INTERIOREXTERIORCOLINEARPOINTS,
	SDP_EXTERIORCOLINEARPOINTSNOINTERSECTION,
	SDP_EXTERIORCOLINEARPOINTS,

	SDP_ERROR
} line_segment_intersection_type;
/// Determines whether two line segments intersect, and if so, how
line_segment_intersection_type LineSegmentIntersection(const point2& A1, const point2& A2, const point2& B1, const point2& B2);
/// Calculates the intersection of two lines, specified by two contained points each
bool LineIntersection(const point2& A1, const point2& A2, const point2& B1, const point2& B2, point2& Result);
*/

/// Calculates the intersection of a plane with a line, returns false if the line and plane are parallel
bool intersect(const plane& Plane, const line3& Line, point3& Intersection);
/// Returns the (minimum) distance between a point and a line in two dimensions
const double distance(const point2& Point, const line2& Line);

/// Returns a Bezier / Bernstein basis for the given order, control point number, and parameter value
double BernsteinBasis(const unsigned long Order, const unsigned long ControlPoint, const double Parameter);

/// Computes a Bezier curve value with given order,  control points, and parameter value
template<class Type>
Type Bezier(const std::vector<Type>& ControlPoints, const double Parameter)
{
	// Sanity checks ...
	assert(ControlPoints.size() > 1);

	Type result = BernsteinBasis(ControlPoints.size(), 0, Parameter) * ControlPoints[0];
	for(unsigned long i = 1; i < ControlPoints.size(); i++)
		result += BernsteinBasis(ControlPoints.size(), i, Parameter) * ControlPoints[i];

	return result;
}

/** Find the point at which two infinite lines intersect. The algorithm generates a plane from one of the lines and finds the intersection point between this plane and the other line.
	\param P1 A point that lies on the first line
	\param T1 Direction vector for the first line
	\param P2 A point that lies on the second line
	\param T2 Direction vector for the second line
	\param Result Returns the intersection between the two lines
	\result Returns true iff the lines intersect, false if they are parallel
	\note Code originally from Aqsis, http://www.aqsis.com
*/
bool intersect_lines(const point3& P1, const vector3& T1, const point3& P2, const vector3& T2, point3& Result);

/** Calculates a 3rd-order NURBS curve that creates a circular arc centered at the origin with unit radius
	\param X Defines the X axis of the plane containing the arc
	\param Y Defines the Y axis of the plane containing the arc
	\param StartAngle Start angle of the arc in radians
	\param EndAngle End angle of the arc in radians
	\param Segments The number of NURBS segments in the resulting arc
	\param Knots Output container for the resulting arc knot vector
	\param Weights Output container for the resulting arc control point weights
	\param ControlPoints Output container for the resulting arc control point positions
*/
template<typename knots_t, typename weights_t, typename control_points_t>
void nurbs_arc(const point3& X, const point3& Y, const double StartAngle, const double EndAngle, const unsigned long Segments, knots_t& Knots, weights_t& Weights, control_points_t& ControlPoints)
{
	const double start_angle = std::min(StartAngle, EndAngle);
	const double end_angle = std::max(StartAngle, EndAngle);
	const double theta = (end_angle - start_angle) / static_cast<double>(Segments);
	const double weight = std::cos(theta * 0.5);

	Knots.clear();
	Knots.insert(Knots.end(), 3, 0);
	for(unsigned long i = 1; i != Segments; ++i)
		Knots.insert(Knots.end(), 2, i);
	Knots.insert(Knots.end(), 3, Segments);

	Weights.clear();
	Weights.push_back(1.0);
	for(unsigned long i = 0; i != Segments; ++i)
	{
		Weights.push_back(weight);
		Weights.push_back(1);
	}

	ControlPoints.clear();
	ControlPoints.push_back(std::cos(start_angle) * X + std::sin(start_angle) * Y);
	for(unsigned long i = 0; i != Segments; ++i)
	{
		const double a0 = start_angle + (theta * (i));
		const double a2 = start_angle + (theta * (i + 1));

		const point3 p0(std::cos(a0) * X + std::sin(a0) * Y);
		const point3 p2(std::cos(a2) * X + std::sin(a2) * Y);

		const point3 t0(-std::sin(a0) * X + std::cos(a0) * Y);
		const point3 t2(-std::sin(a2) * X + std::cos(a2) * Y);

		point3 p1;
		intersect_lines(p0, to_vector(t0), p2, to_vector(t2), p1);

		ControlPoints.push_back(p1);
		ControlPoints.push_back(p2);
	}
}

} // namespace k3d

#endif // K3DSDK_GEOMETRY_H

