#ifndef K3DSDK_VECTOR3_H
#define K3DSDK_VECTOR3_H

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
#include "result.h"

#include <boost/io/ios_state.hpp>

#include <cmath>
#include <iomanip>

namespace k3d
{

/////////////////////////////////////////////////////////////////////////////
// vector3

/// Encapsulates a direction vector in three-dimensional space
class vector3
{
public:
	/// Stores the vector values
	double n[3];

	vector3()
	{
		n[0] = n[1] = n[2] = 0.0;
	}

	vector3(const double x, const double y, const double z)
	{
		n[0] = x;
		n[1] = y;
		n[2] = z;
	}

	vector3& operator+=(const vector3& v)
	{
		n[0] += v.n[0];
		n[1] += v.n[1];
		n[2] += v.n[2];
		return *this;
	}

	vector3& operator-=(const vector3& v)
	{
		n[0] += v.n[0];
		n[1] += v.n[1];
		n[2] += v.n[2];
		return *this;
	}

	vector3& operator*=(const double d)
	{
		n[0] *= d;
		n[1] *= d;
		n[2] *= d;
		return *this;
	}

	vector3& operator/=(const double d)
	{
		return_val_if_fail(d, *this);

		const double d_inv = 1./d;
		n[0] *= d_inv;
		n[1] *= d_inv;
		n[2] *= d_inv;
		return *this;
	}

	double& operator[](const unsigned int i)
	{
		assert_warning((i >= 0) && (i <= 2));
		return n[i];
	}

	double operator[](const unsigned int i) const
	{
		return_val_if_fail((i >= 0) && (i <= 2), 0);
		return n[i];
	}

	/// Returns the normal length
	double length() const
	{
		return std::sqrt(length2());
	}

	/// Returns the squared normal length
	double length2() const
	{
		return n[0] * n[0] + n[1] * n[1] + n[2] * n[2];
	}

	friend std::ostream& operator<<(std::ostream& Stream, const vector3& RHS)
	{
		boost::io::ios_flags_saver stream_state(Stream);
		Stream << std::setprecision(17) << RHS.n[0] << " " << RHS.n[1] << " " << RHS.n[2];
		return Stream;
	}

	friend std::istream& operator>>(std::istream& Stream, vector3& RHS)
	{
		Stream >> RHS.n[0] >> RHS.n[1] >> RHS.n[2];
		return Stream;
	}
};

/// Negation
inline const vector3 operator-(const vector3& v)
{
	return vector3(-v.n[0], -v.n[1], -v.n[2]);
}

/// Addition
inline const vector3 operator+(const vector3& a, const vector3& b)
{
	return vector3(a.n[0] + b.n[0], a.n[1] + b.n[1], a.n[2] + b.n[2]);
}

/// Subtraction
inline const vector3 operator-(const vector3& a, const vector3& b)
{
	return vector3(a.n[0] - b.n[0], a.n[1] - b.n[1], a.n[2] - b.n[2]);
}

/// Multiplication by a constant
inline const vector3 operator*(const vector3& a, const double d)
{
	return vector3(a.n[0] * d, a.n[1] * d, a.n[2] * d);
}

/// Multiplication by a constant
inline const vector3 operator*(const double d, const vector3& a)
{
	return vector3(a.n[0] * d, a.n[1] * d, a.n[2] * d);
}

/// Returns the dot product of two vectors
inline const double operator*(const vector3& a, const vector3& b)
{
	return a.n[0] * b.n[0] + a.n[1] * b.n[1] + a.n[2] * b.n[2];
}

/// Division by a constant
inline const vector3 operator/(const vector3& a, const double d)
{
	return_val_if_fail(d, vector3());
	return vector3(a.n[0] / d, a.n[1] / d, a.n[2] / d);
}

/// Returns the cross product of two vectors
inline const vector3 operator^(const vector3& a, const vector3& b)
{
	return vector3(a.n[1] * b.n[2] - a.n[2] * b.n[1], a.n[2] * b.n[0] - a.n[0] * b.n[2], a.n[0] * b.n[1] - a.n[1] * b.n[0]);
}

/// Equality
inline const bool operator==(const vector3& a, const vector3& b)
{
	return a.n[0] == b.n[0] && a.n[1] == b.n[1] && a.n[2] == b.n[2];
}

/// Inequality
inline const bool operator!=(const vector3& a, const vector3& b)
{
	return a.n[0] != b.n[0] || a.n[1] != b.n[1] || a.n[2] != b.n[2];
}

/// Returns the length of a vector
inline const double length(const vector3& Vector)
{
	return Vector.length();
}

/// Returns the normalized form of a vector
inline const vector3 normalize(const vector3& Vector)
{
	const double length = Vector.length();
	return_val_if_fail(length, Vector);
	return Vector / length;
}

/// Converts Cartesian coordinates to spherical coordinates
inline const vector3 spherical(const vector3& Vector)
{
	return vector3(Vector.length(), std::atan2(Vector[0], Vector[2]), std::atan2(Vector[1], sqrt(Vector[0] * Vector[0] + Vector[2] * Vector[2])));
/*
	// Handle the singularity at the poles
	if(0.0 == Vector[0] && 0.0 == Vector[2])
		return point3(n[1] > 0.0 ? sdpPiOver2 : -sdpPiOver2, 0.0, Vector.length());


	return point3(atan2(Vector[1], sqrt(Vector[0] * Vector[0] + Vector[2] * Vector[2])), atan2(Vector[0], Vector[2]), Vector.length());
*/
}

/// Specialization of almost_equal that tests two vector3 objects for near-equality
template<>
class almost_equal<vector3>
{
	typedef vector3 T;
public:
	almost_equal(const boost::uint64_t Threshold) : threshold(Threshold) { }
	inline const bool operator()(const T& A, const T& B) const
	{
		return std::equal(A.n, A.n + 3, B.n, almost_equal<double>(threshold));
	}

private:
	const boost::uint64_t threshold;
};

} // namespace k3d

#endif // K3DSDK_VECTOR3_H

