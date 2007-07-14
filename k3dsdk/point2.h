#ifndef K3DSDK_POINT2_H
#define K3DSDK_POINT2_H

// K-3D
// Copyright (c) 1995-2007, Timothy M. Shead
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

class vector2;

/////////////////////////////////////////////////////////////////////////////
// point2

/// Encapsulates a location in two-dimensional space
class point2
{
public:
	/// Stores the point values
	double n[2];

	point2()
	{
		n[0] = n[1] = 0.0;
	}
	
	point2(const double x, const double y)
	{
		n[0] = x;
		n[1] = y;
	}

	point2(const point2& v)
	{
		n[0] = v.n[0];
		n[1] = v.n[1];
	}

	/// Addition of a vector
	point2& operator+=(const vector2& v);

	/// Subtraction of a vector
	point2& operator-=(const vector2& v);

	/// Multiplication by a constant
	point2& operator*=(const double d)
	{
		n[0] *= d;
		n[1] *= d;
		return *this;
	}
	
	/// Division by a constant
	point2& operator/=(const double d)
	{
		return_val_if_fail(d, *this);

		double d_inv = 1./d;
		n[0] *= d_inv;
		n[1] *= d_inv;

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

	friend std::ostream& operator<<(std::ostream& Stream, const point2& RHS)
	{
		boost::io::ios_flags_saver stream_state(Stream);
		Stream << std::setprecision(17) << RHS.n[0] << " " << RHS.n[1];
		return Stream;
	}

	friend std::istream& operator>>(std::istream& Stream, point2& RHS)
	{
		Stream >> RHS.n[0] >> RHS.n[1];
		return Stream;
	}
};

/// Negation
inline const point2 operator-(const point2& a)
{
	return point2(-a.n[0], -a.n[1]);
}

/// Addition
/** \note We implement this as a convenience for the benefit of linear interpolation - adding two points has no geometric meaning */
inline const point2 operator+(const point2& a, const point2& b)
{
	return point2(a.n[0] + b.n[0], a.n[1] + b.n[1]);
}

/// Multiplication by a constant
inline const point2 operator*(const point2& a, const double d)
{
	return point2(d * a.n[0], d * a.n[1]);
}

/// Multiplication by a constant
inline const point2 operator*(const double d, const point2& a)
{
	return a * d;
}

/// Division by a constant
inline const point2 operator/(const point2& a, const double d)
{
	return_val_if_fail(d, point2());

	double d_inv = 1./d;
	return point2(a.n[0]*d_inv, a.n[1]*d_inv);
}

/// Equality
inline const bool operator==(const point2& a, const point2& b)
{
	return a.n[0] == b.n[0] && a.n[1] == b.n[1];
}

/// Non-equality
inline const bool operator!=(const point2& a, const point2& b)
{
	return !(a == b);
}

/// Specialization of almost_equal that tests two point2 objects for near-equality
template<>
class almost_equal<point2>
{
	typedef point2 T;
public:
	almost_equal(const boost::uint64_t Threshold) : threshold(Threshold) { }
	inline const bool operator()(const T& A, const T& B) const
	{
		return std::equal(A.n, A.n + 2, B.n, almost_equal<double>(threshold));
	}

private:
	const boost::uint64_t threshold;
};

} // namespace k3d

#endif // K3DSDK_POINT2_H

