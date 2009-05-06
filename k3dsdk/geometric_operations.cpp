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
#include "geometric_operations.h"
#include "line2.h"
#include "line3.h"
#include "log.h"
#include "plane.h"
#include "point2.h"
#include "result.h"
#include "vectors.h"

namespace k3d
{

/////////////////////////////////////////////////////////////////////////////
// intersect

bool_t intersect(const plane& Plane, const line3& Line, point3& Intersection)
{
	// Calculate the angle (dot product) between line and plane ...
	const double_t theta = Plane.normal * Line.direction;

	// Make sure the line & plane aren't parallel ...
	if(0.0 == theta)
		return false;

	const double_t v0 = -(Plane.normal * to_vector(Line.point) + Plane.distance);
	const double_t t = v0 / theta;

	Intersection = Line.point + (t * Line.direction);
	return true;
}

/////////////////////////////////////////////////////////////////////////////
// distance

double_t distance(const point2& Point, const line2& Line)
{
	const double_t x0 = Point[0];
	const double_t y0 = Point[1];
	const double_t x1 = Line.point[0];
	const double_t y1 = Line.point[1];
	const double_t x2 = x1 + Line.direction[0];
	const double_t y2 = y1 + Line.direction[1];

	const double_t denominator = sqrt((x2-x1) * (x2-x1) + (y2-y1) * (y2-y1));
	return_val_if_fail(denominator, 0.0); // Happens if the line direction is undefined

	const double_t numerator = fabs((x2-x1) * (y1-y0) - (x1-x0) * (y2-y1));
	return numerator / denominator;
}

bool_t intersect_lines(const point3& P1, const vector3& T1, const point3& P2, const vector3& T2, point3& Result)
{
	// Code originally from Aqsis, http://www.aqsis.com
	vector3 px = T1 ^ to_vector( P1 - T2 );
	vector3 v = px ^ T1;

	double_t t = (P1 - P2) * v;
	double_t vw = v * T2;
	if ( ( vw * vw ) < 1.0e-07 )
		return false;

	t /= vw;
	Result = P2 + (((P1 - P2) * v) / vw) * T2;
	return true;
}

} // namespace k3d

