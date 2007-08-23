#ifndef K3DSDK_VECTORS_H
#define K3DSDK_VECTORS_H

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
	\brief Vector (points, vectors and normals) routines
	\author Timothy M. Shead (tshead@k-3d.com)
*/

/****************************************************************
*
* C++ Vector and Matrix Algebra routines
* Author: Jean-Francois DOUE
* Version 3.1 --- October 1993
*
****************************************************************/

//
//	From "Graphics Gems IV / Edited by Paul S. Heckbert
//	Academic Press, 1994, ISBN 0-12-336156-9
//	"You are free to use and modify this code in any way
//	you like." (p. xv)
//
//	Modified by J. Nagle, March 1997
//	-	All functions are inline.
//	-	All functions are const-correct.
//	-	All checking is via the standard "assert" macro.
//

// Modified by Tim Shead for use with K-3D, January 1998

#include "almost_equal.h"
#include "normal3.h"
#include "point2.h"
#include "point3.h"
#include "point4.h"
#include "vector2.h"
#include "vector3.h"

#include <algorithm>
#include <cmath>
#include <iostream>

namespace k3d
{

/////////////////////////////////////////////////////////////////////////////
// vector2 / point2 operations

/// Add a point and a vector, returning the moved point
inline const point2 operator+(const point2& a, const vector2& b)
{
	return point2(a.n[0] + b.n[0], a.n[1] + b.n[1]);
}

/// Add a vector and a point, returning the moved point
inline const point2 operator+(const vector2& a, const point2& b)
{
	return point2(a.n[0] + b.n[0], a.n[1] + b.n[1]);
}

/// Subtracts a vector from a point, returning the modified point
inline const point2 operator-(const point2& a, const vector2& b)
{
	return point2(a.n[0] - b.n[0], a.n[1] - b.n[1]);
}

/// Returns the vector difference between two points
inline const vector2 operator-(const point2& a, const point2& b)
{
	return vector2(a.n[0] - b.n[0], a.n[1] - b.n[1]);
}

inline point2& point2::operator+=(const vector2& v)
{
	n[0] += v.n[0];
	n[1] += v.n[1];
	return *this;
}

inline point2& point2::operator-=(const vector2& v)
{
	n[0] -= v.n[0];
	n[1] -= v.n[1];
	return *this;
}

/////////////////////////////////////////////////////////////////////////////
// vector3 / point3 operations

/// Add a point and a vector, returning the modified point
inline const point3 operator+(const point3& a, const vector3& b)
{
	return point3(a.n[0] + b.n[0], a.n[1] + b.n[1], a.n[2] + b.n[2]);
}

/// Adds a vector and a point, returning the modified point
inline const point3 operator+(const vector3& a, const point3& b)
{
	return point3(a.n[0] + b.n[0], a.n[1] + b.n[1], a.n[2] + b.n[2]);
}

/// Subtracts a vector from a point, returning the modified point
inline const point3 operator-(const point3& a, const vector3& b)
{
	return point3(a.n[0] - b.n[0], a.n[1] - b.n[1], a.n[2] - b.n[2]);
}

/// Returns the vector difference between two points
inline const vector3 operator-(const point3& a, const point3& b)
{
	return vector3(a.n[0] - b.n[0], a.n[1] - b.n[1], a.n[2] - b.n[2]);
}

inline point3& point3::operator+=(const vector3& v)
{
	n[0] += v.n[0]; n[1] += v.n[1]; n[2] += v.n[2]; return *this;
}

inline point3& point3::operator-=(const vector3& v)
{
	n[0] -= v.n[0]; n[1] -= v.n[1]; n[2] -= v.n[2]; return *this;
}

//////////////////////////////////////////////////////////////////////////////
// vector3 / normal3 operations

/// Returns the dot product of a vector and a normal
inline const double operator*(const vector3& a, const normal3& b)
{
	return a.n[0] * b.n[0] + a.n[1] * b.n[1] + a.n[2] * b.n[2];
}

/// Returns the dot product of a normal and a vector
inline const double operator*(const normal3& a, const vector3& b)
{
	return a.n[0] * b.n[0] + a.n[1] * b.n[1] + a.n[2] * b.n[2];
}

/// Returns the cross product of a vector and a normal
inline const vector3 operator^(const vector3& a, const normal3& b)
{
	return vector3(a.n[1] * b.n[2] - a.n[2] * b.n[1], a.n[2] * b.n[0] - a.n[0] * b.n[2], a.n[0] * b.n[1] - a.n[1] * b.n[0]);
}

/// Returns the cross product of a normal and a vector
inline const vector3 operator^(const normal3& a, const vector3& b)
{
	return vector3(a.n[1] * b.n[2] - a.n[2] * b.n[1], a.n[2] * b.n[0] - a.n[0] * b.n[2], a.n[0] * b.n[1] - a.n[1] * b.n[0]);
}

//////////////////////////////////////////////////////////////////////////
// Odds-and-ends

/// Returns the distance between two points
inline const double distance(const point2& P1, const point2& P2)
{
	return length(P2 - P1);
}

/// Returns the distance between two points
inline const double distance(const point3& P1, const point3& P2)
{
	return length(P2 - P1);
}

/// Explicit conversion
inline const vector2 to_vector(const point2& v)
{
	return vector2(v.n[0], v.n[1]);
}

/// Explicit conversion
inline const point3 to_point(const vector3& v)
{
	return point3(v.n[0], v.n[1], v.n[2]);
}

/// Explicit conversion
inline const point3 to_point(const normal3& v)
{
	return point3(v.n[0], v.n[1], v.n[2]);
}

/// Explicit conversion
inline const vector3 to_vector(const point3& v)
{
	return vector3(v.n[0], v.n[1], v.n[2]);
}

/// Explicit conversion
inline const vector3 to_vector(const normal3& v)
{
	return vector3(v.n[0], v.n[1], v.n[2]);
}

/// Explicit conversion
inline const normal3 to_normal(const point3& v)
{
	return normal3(v.n[0], v.n[1], v.n[2]);
}

/// Explicit conversion
inline const normal3 to_normal(const vector3& v)
{
	return normal3(v.n[0], v.n[1], v.n[2]);
}

} // namespace k3d

#endif // K3DSDK_VECTORS_H

