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
	\author Tim Shead (tshead@k-3d.com)
*/

#include "algebra.h"
#include "plane.h"

namespace k3d
{

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

const plane operator*(const matrix4& Matrix, const plane& Plane)
{
	return plane(Matrix * Plane.normal, Matrix * to_point(Plane.distance * Plane.normal));
}

const plane operator*(const plane& Plane, const matrix4& Matrix)
{
	return Matrix * Plane;
}

} // namespace k3d

