#ifndef K3DSDK_GEOMETRIC_OPERATIONS_H
#define K3DSDK_GEOMETRIC_OPERATIONS_H

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

#include "types.h"

namespace k3d
{

class line2;
class line3;
class plane;
class point2;
class point3;
class vector3;

/// Returns the (minimum) distance between a point and a line in two dimensions
double_t distance(const point2& Point, const line2& Line);
/// Calculates the intersection of a plane with a line, returns false if the line and plane are parallel
bool_t intersect(const plane& Plane, const line3& Line, point3& Intersection);

/** Find the point at which two infinite lines intersect. The algorithm generates a plane from one of the lines and finds the intersection point between this plane and the other line.
	\param P1 A point that lies on the first line
	\param T1 Direction vector for the first line
	\param P2 A point that lies on the second line
	\param T2 Direction vector for the second line
	\param Result Returns the intersection between the two lines
	\result Returns true iff the lines intersect, false if they are parallel
	\note Code originally from Aqsis, http://www.aqsis.com
*/
bool_t intersect_lines(const point3& P1, const vector3& T1, const point3& P2, const vector3& T2, point3& Result);

} // namespace k3d

#endif // !K3DSDK_GEOMETRIC_OPERATIONS_H

