#ifndef K3DSDK_NORMAL3_H
#define K3DSDK_NORMAL3_H

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
// normal3

/// Encapsulates a 3D surface normal
class normal3
{
public:
	/// Stores the normal values
	double n[3];

	normal3()
	{
		n[0] = n[1] = n[2] = 0.0;
	}

	normal3(const double x, const double y, const double z)
	{
		n[0] = x;
		n[1] = y;
		n[2] = z;
	}

	normal3& operator+=(const normal3& v)
	{
		n[0] += v.n[0];
		n[1] += v.n[1];
		n[2] += v.n[2];
		return *this;
	}

	normal3& operator-=(const normal3& v)
	{
		n[0] += v.n[0];
		n[1] += v.n[1];
		n[2] += v.n[2];
		return *this;
	}

	normal3& operator*=(const double d)
	{
		n[0] *= d;
		n[1] *= d;
		n[2] *= d;
		return *this;
	}

	normal3& operator/=(const double d)
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

	friend std::ostream& operator<<(std::ostream& Stream, const normal3& RHS)
	{
		boost::io::ios_flags_saver stream_state(Stream);
		Stream << std::setprecision(17) << RHS.n[0] << " " << RHS.n[1] << " " << RHS.n[2];
		return Stream;
	}

	friend std::istream& operator>>(std::istream& Stream, normal3& RHS)
	{
		Stream >> RHS.n[0] >> RHS.n[1] >> RHS.n[2];
		return Stream;
	}
};

/// Multiplication by a constant
inline const normal3 operator*(const normal3& a, const double d)
{
	return normal3(a.n[0] * d, a.n[1] * d, a.n[2] * d);
}

/// Multiplication by a constant
inline const normal3 operator*(const double d, const normal3& a)
{
	return normal3(a.n[0] * d, a.n[1] * d, a.n[2] * d);
}

/// Division by a constant
inline const normal3 operator/(const normal3& a, const double d)
{
	return_val_if_fail(d, normal3());
	return normal3(a.n[0] / d, a.n[1] / d, a.n[2] / d);
}

/// Returns the dot product of two normals
inline const double operator*(const normal3& a, const normal3& b)
{
	return a.n[0] * b.n[0] + a.n[1] * b.n[1] + a.n[2] * b.n[2];
}

/// Returns the cross product of two normals
inline const normal3 operator^(const normal3& a, const normal3& b)
{
	return normal3(a.n[1] * b.n[2] - a.n[2] * b.n[1], a.n[2] * b.n[0] - a.n[0] * b.n[2], a.n[0] * b.n[1] - a.n[1] * b.n[0]);
}

/// Equality
inline const bool operator==(const normal3& a, const normal3& b)
{
	return a.n[0] == b.n[0] && a.n[1] == b.n[1] && a.n[2] == b.n[2];
}

/// Inequality
inline const bool operator!=(const normal3& a, const normal3& b)
{
	return a.n[0] != b.n[0] || a.n[1] != b.n[1] || a.n[2] != b.n[2];
}

/// Returns the length of a normal
inline const double length(const normal3& Normal)
{
	return Normal.length();
}

/// Returns the normalized form of a normal
inline const normal3 normalize(const normal3& Normal)
{
	const double length = Normal.length();
	return_val_if_fail(length, Normal);
	return Normal / length;
}

/// Specialization of almost_equal that tests two normal3 objects for near-equality
template<>
class almost_equal<normal3>
{
	typedef normal3 T;
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

#endif // K3DSDK_NORMAL3_H

