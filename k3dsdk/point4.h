#ifndef K3DSDK_POINT4_H
#define K3DSDK_POINT4_H

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
#include <iomanip>

namespace k3d
{

class vector4;

/////////////////////////////////////////////////////////////////////////////
// point4

/// Encapsulates a four-dimensional location
class point4
{
public:
	/// Stores the point values
	double n[4];

	point4()
	{
		n[0] = n[1] = n[2] = n[3] = 0.0;
	}

	point4(const double x, const double y, const double z, const double w)
	{
		n[0] = x;
		n[1] = y;
		n[2] = z;
		n[3] = w;
	}

	/// Addition of a vector
	point4& operator+=(const vector4& v);

	/// Subtraction of a vector
	point4& operator-=(const vector4& v);

	/// Multiplication by a constant
	point4& operator*=(const double d)
	{
		n[0] *= d;
		n[1] *= d;
		n[2] *= d;
		n[3] *= d;
		return *this;
	}

	/// Division by a constant
	point4& operator/=(const double d)
	{
		return_val_if_fail(d, *this);

		double d_inv = 1./d;
		n[0] *= d_inv;
		n[1] *= d_inv;
		n[2] *= d_inv;
		n[3] *= d_inv;
		return *this;
	}

	/// Returns an indexed dimension by reference
	double& operator[](int i)
	{
		return n[i];
	}

	/// Returns an indexed dimension by value
	double operator[](int i) const
	{
		return n[i];
	}

	friend std::ostream& operator<<(std::ostream& Stream, const point4& RHS)
	{
		boost::io::ios_flags_saver stream_state(Stream);
		Stream << std::setprecision(17) << RHS.n[0] << " " << RHS.n[1] << " " << RHS.n[2] << " " << RHS.n[3];
		return Stream;
	}

	friend std::istream& operator>>(std::istream& Stream, point4& RHS)
	{
		Stream >> RHS.n[0] >> RHS.n[1] >> RHS.n[2] >> RHS.n[3];
		return Stream;
	}
};

/// Negation
inline const point4 operator-(const point4& a)
{
	return point4(-a.n[0],-a.n[1],-a.n[2],-a.n[3]);
}

/// Addition
/** \note We implement this as a convenience for the benefit of linear interpolation - adding two points has no geometric meaning */
inline const point4 operator+(const point4& a, const point4& b)
{
	return point4(a.n[0] + b.n[0], a.n[1] + b.n[1], a.n[2] + b.n[2], a.n[3] + b.n[3]);
}

/// Multiplication by a constant
inline const point4 operator*(const point4& a, const double d)
{
	return point4(d*a.n[0], d*a.n[1], d*a.n[2], d*a.n[3] );
}

/// Multiplication by a constant
inline const point4 operator*(const double d, const point4& a)
{
	return a*d;
}

/// Division by a constant
inline const point4 operator/(const point4& a, const double d)
{
	return_val_if_fail(d, point4());

	double d_inv = 1./d;
	return point4(a.n[0]*d_inv, a.n[1]*d_inv, a.n[2]*d_inv, a.n[3]*d_inv);
}

/// Equality
inline const bool operator==(const point4& a, const point4& b)
{
	return (a.n[0] == b.n[0]) && (a.n[1] == b.n[1]) && (a.n[2] == b.n[2]) && (a.n[3] == b.n[3]);
}

/// Inequality
inline bool operator!=(const point4& a, const point4& b)
{
	return !(a == b);
}

/// Specialization of almost_equal that tests two point4 objects for near-equality
template<>
class almost_equal<point4>
{
	typedef point4 T;
public:
	almost_equal(const boost::uint64_t Threshold) : threshold(Threshold) { }
	inline bool_t operator()(const T& A, const T& B) const
	{
		return std::equal(A.n, A.n + 4, B.n, almost_equal<double_t>(threshold));
	}

private:
	const boost::uint64_t threshold;
};

} // namespace k3d

#endif // !K3DSDK_POINT4_H

