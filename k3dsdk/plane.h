#ifndef K3DSDK_PLANE_H
#define K3DSDK_PLANE_H

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

#include "vector3.h"

namespace k3d
{

class matrix4;
class point3;

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

	/// Stores the plane's normal vector
	vector3 normal;
	/// Stores the plane's distance from the origin
	double distance;
};

std::ostream& operator<<(std::ostream& Stream, const plane& RHS);
std::istream& operator>>(std::istream& Stream, plane& RHS);

/// Linear transformation if a plane by a transformation matrix
const plane operator*(const matrix4& Matrix, const plane& Plane);
/// Linear transformation if a plane by a transformation matrix
const plane operator*(const plane& Plane, const matrix4& Matrix);

} // namespace k3d

#endif // K3DSDK_PLANE_H

