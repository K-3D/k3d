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
		\brief 2D geometry routines implementation
		\author Tim Shead (tshead@k-3d.com)
*/

#include "algebra.h"
#include "basic_math.h"
#include "geometry.h"
#include "log.h"
#include "result.h"

namespace k3d
{

/*
double Area(const point2& Point1, const point2& Point2, const point2& Point3)
{
	double result = 0;

	result += (Point1[0] * Point2[1]) - (Point2[0] * Point1[1]);
	result += (Point2[0] * Point3[1]) - (Point3[0] * Point2[1]);
	result += (Point3[0] * Point1[1]) - (Point1[0] * Point3[1]);

	// For robustness ...
	//if(std::fabs(result) < FLT_EPSILON)
	//	return 0;

	return result * 0.5;
}
*/

/////////////////////////////////////////////////////////////////////////////
// rectangle

rectangle::rectangle(const double Left, const double Right, const double Top, const double Bottom) :
	left(Left),
	right(Right),
	top(Top),
	bottom(Bottom)
{
}

rectangle::rectangle(const point2& TopLeft, const point2& BottomRight) :
	left(TopLeft[0]),
	right(BottomRight[0]),
	top(TopLeft[1]),
	bottom(BottomRight[1])
{
}

double rectangle::width() const
{
	return std::fabs(right - left);
}

double rectangle::height() const
{
	return std::fabs(bottom - top);
}

const point2 rectangle::top_left() const
{
	return point2(left, top);
}

const point2 rectangle::bottom_right() const
{
	return point2(right, bottom);
}

const point2 rectangle::center() const
{
	return point2(left+right, top+bottom) * 0.5;
}

bool rectangle::contains(const point2& Point) const
{
	return (left <= Point[0] && Point[0] <= right && top <= Point[1] && Point[1] <= bottom) ? true : false;
}

const rectangle normalize(const rectangle& Rectangle)
{
	return rectangle(
		std::min(Rectangle.left, Rectangle.right),
		std::max(Rectangle.left, Rectangle.right),
		std::min(Rectangle.top, Rectangle.bottom),
		std::max(Rectangle.top, Rectangle.bottom));
}

std::ostream& operator<<(std::ostream& Stream, const rectangle& Arg)
{
	Stream << Arg.left << " " << Arg.right << " " << Arg.top << " " << Arg.bottom;
	return Stream;
}

std::istream& operator>>(std::istream& Stream, rectangle& Arg)
{
	Stream >> Arg.left >> Arg.right >> Arg.top >> Arg.bottom;
	return Stream;
}

/////////////////////////////////////////////////////////////////////////////
// plane

plane::plane(const vector3& Normal, const double Distance) :
	normal(normalize(Normal)),
	distance(Distance)
{
}

plane::plane(const vector3& Normal, const point3& Point) :
	normal(normalize(Normal)),
	distance(-to_vector(Point) * normal)
{
}

plane::plane(const point3& PointA, const point3& PointB, const point3& PointC) :
	normal(normalize(to_vector(PointA - PointB) ^ to_vector(PointC - PointB))),
	distance(-to_vector(PointA) * normal)
{
}

const plane operator*(const matrix4& Matrix, const plane& Plane)
{
	return plane(Matrix * Plane.normal, Matrix * to_point(Plane.distance * Plane.normal));
}

const plane operator*(const plane& Plane, const matrix4& Matrix)
{
	return Matrix * Plane;
}

std::ostream& operator<<(std::ostream& Stream, const plane& RHS)
{
	Stream << RHS.normal << " " << RHS.distance;
	return Stream;
}

std::istream& operator>>(std::istream& Stream, plane& RHS)
{
	Stream >> RHS.normal >> RHS.distance;
	return Stream;
}

/////////////////////////////////////////////////////////////////////////////
// line2

line2::line2(const vector2& Direction, const point2& Point) :
	direction(Direction),
	point(Point)
{
}

line2::line2(const point2& Point1, const point2& Point2) :
	direction(to_vector(Point2 - Point1)),
	point(Point1)
{
}

/////////////////////////////////////////////////////////////////////////////
// line3

line3::line3(const vector3& Direction, const point3& Point) :
	direction(Direction),
	point(Point)
{
}

line3::line3(const point3& Point1, const point3& Point2) :
	direction(to_vector(Point2 - Point1)),
	point(Point1)
{
}

/////////////////////////////////////////////////////////////////////////////
// intersect

bool intersect(const plane& Plane, const line3& Line, point3& Intersection)
{
	// Calculate the angle (dot product) between line and plane ...
	const double theta = Plane.normal * Line.direction;

	// Make sure the line & plane aren't parallel ...
	if(0.0 == theta)
		return false;

	const double v0 = -(Plane.normal * to_vector(Line.point) + Plane.distance);
	const double t = v0 / theta;

	Intersection = Line.point + (t * Line.direction);
	return true;
}

/////////////////////////////////////////////////////////////////////////////
// distance

const double distance(const point2& Point, const line2& Line)
{
	const double x0 = Point[0];
	const double y0 = Point[1];
	const double x1 = Line.point[0];
	const double y1 = Line.point[1];
	const double x2 = x1 + Line.direction[0];
	const double y2 = y1 + Line.direction[1];

	const double denominator = sqrt((x2-x1) * (x2-x1) + (y2-y1) * (y2-y1));
	return_val_if_fail(denominator, 0.0); // Happens if the line direction is undefined

	const double numerator = fabs((x2-x1) * (y1-y0) - (x1-x0) * (y2-y1));
	return numerator / denominator;
}

/*
bool LineIntersection(const point2& A1, const point2& A2, const point2& B1, const point2& B2, point2& Result)
{
	const point2 a = (A2 - A1).Normalize();
	const point2 b = (B2 - B1).Normalize();
	const point2 c = B1 - A1;
	const point2 bperp = b.Perpendicular();

	const double denominator = bperp * a;
	if(0 == denominator)
		return false;

	const double t = (bperp * c) / denominator;

	Result = A1 + (t * a);

	return true;
}

line_segment_intersection_type LineSegmentIntersection(const point2& A1, const point2& A2, const point2& B1, const point2& B2)
{
	const double sign1 = sign(Area(A1, A2, B1));
	const double sign2 = sign(Area(A1, A2, B2));

	// The three most likely possibilities ...
	if(sign1 && sign2)
	{
		// No intersection ...
		if(sign1 == sign2)
			return SDP_NOINTERSECTION;

		point2 intersection;
		if(!LineIntersection(A1, A2, B1, B2, intersection))
		{
			log() << error << "LineIntersection 1 failed." << std::endl;
			return SDP_ERROR;
		}

		const double sign3 = sign(Area(B1, intersection, A1));
		const double sign4 = sign(Area(B1, intersection, A2));

		if(sign3 != sign4)
			return SDP_INTERIORINTERSECTION;

		return SDP_EXTERIORINTERSECTION;
	}
	// Two variations with single-point intersections ...
	else if(sign1 || sign2)
	{
		point2 intersection;
		if(!LineIntersection(A1, A2, B1, B2, intersection))
		{
			log() << error << "LineIntersection 2 failed." << std::endl;
			return SDP_ERROR;
		}

		const double sign3 = sign(Area(B1, intersection, A1));
		const double sign4 = sign(Area(B1, intersection, A2));

		if(sign3 != sign4)
			return SDP_INTERIORCOLINEARPOINT;

		return SDP_EXTERIORCOLINEARPOINT;
	}
	// The four colinear possibilities ...
	else
	{
		// This ray is getting crowded!  Find something somewhere else
		point2 start = B1 + (B2 - B1).Perpendicular();

		const double sign3 = sign(Area(start, B1, A1));
		const double sign4 = sign(Area(start, B1, A2));
		const double sign5 = sign(Area(start, B2, A1));
		const double sign6 = sign(Area(start, B2, A2));

		if((sign3 != sign4) && (sign5 != sign6))
			return SDP_INTERIORCOLINEARPOINTS;

		if((sign3 == sign4) && (sign4 == sign5) && (sign5 == sign6))
			return SDP_EXTERIORCOLINEARPOINTSNOINTERSECTION;

		if((sign3 == sign4) && (sign4 != sign5) && (sign5 == sign6))
			return SDP_EXTERIORCOLINEARPOINTS;

		return SDP_INTERIOREXTERIORCOLINEARPOINTS;
	}
}
*/

double BernsteinBasis(const unsigned long Order, const unsigned long ControlPoint, const double Parameter)
{
	// Sanity checks ...
	assert(Order > 1);
	assert(ControlPoint <= Order);

	const int n = Order - 1;
	const int i = ControlPoint;
	const double t = Parameter;

	const double ni = factorial(n) / (factorial(i) * factorial(n - i));

	return ni * pow(t, i) * pow((1 - t), (n - i));
}

bool intersect_lines(const point3& P1, const vector3& T1, const point3& P2, const vector3& T2, point3& Result)
{
	// Code originally from Aqsis, http://www.aqsis.com
	vector3 px = T1 ^ to_vector( P1 - T2 );
	vector3 v = px ^ T1;

	double t = to_vector( P1 - P2 ) * v;
	double vw = v * T2;
	if ( ( vw * vw ) < 1.0e-07 )
		return false;

	t /= vw;
	Result = P2 + ( ( to_vector( P1 - P2 ) * v ) / vw ) * T2 ;
	return true;
}

} // namespace k3d

