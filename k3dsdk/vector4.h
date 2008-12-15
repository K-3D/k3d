#ifndef K3DSDK_VECTOR4_H
#define K3DSDK_VECTOR4_H

// K-3D
// Copyright (c) 1995-2008, Timothy M. Shead
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
// vector4

/// Encapsulates a direction vector in four-dimensional space
class vector4
{
public:
	/// Stores the vector values
	double_t n[4];

	vector4()
	{
		n[0] = n[1] = n[2] = n[3] = 0.0;
	}

	vector4(const double_t x, const double_t y, const double_t z, const double_t w)
	{
		n[0] = x;
		n[1] = y;
		n[2] = z;
		n[3] = w;
	}

	vector4& operator+=(const vector4& v)
	{
		n[0] += v.n[0];
		n[1] += v.n[1];
		n[2] += v.n[2];
		n[3] += v.n[3];
		return *this;
	}

	vector4& operator-=(const vector4& v)
	{
		n[0] -= v.n[0];
		n[1] -= v.n[1];
		n[2] -= v.n[2];
		n[3] -= v.n[3];
		return *this;
	}

	vector4& operator*=(const double_t d)
	{
		n[0] *= d;
		n[1] *= d;
		n[2] *= d;
		n[3] *= d;
		return *this;
	}

	vector4& operator/=(const double_t d)
	{
		return_val_if_fail(d, *this);

		const double_t d_inv = 1.0 / d;
		n[0] *= d_inv;
		n[1] *= d_inv;
		n[2] *= d_inv;
		n[3] *= d_inv;
		return *this;
	}

	double_t& operator[](const unsigned int i)
	{
		return n[i];
	}

	double_t operator[](const unsigned int i) const
	{
		return n[i];
	}

	/// Returns the normal length
	double_t length() const
	{
		return std::sqrt(length2());
	}

	/// Returns the squared normal length
	double_t length2() const
	{
		return n[0] * n[0] + n[1] * n[1] + n[2] * n[2] + n[3] * n[3];
	}

	friend std::ostream& operator<<(std::ostream& Stream, const vector4& Value)
	{
		boost::io::ios_flags_saver stream_state(Stream);
		Stream << std::setprecision(17) << Value.n[0] << " " << Value.n[1] << " " << Value.n[2] << " " << Value.n[3];
		return Stream;
	}

	friend std::istream& operator>>(std::istream& Stream, vector4& Value)
	{
		Stream >> Value.n[0] >> Value.n[1] >> Value.n[2] >> Value.n[3];
		return Stream;
	}
};

/// Negation
inline const vector4 operator-(const vector4& v)
{
	return vector4(-v.n[0], -v.n[1], -v.n[2], -v.n[3]);
}

/// Addition
inline const vector4 operator+(const vector4& a, const vector4& b)
{
	return vector4(a.n[0] + b.n[0], a.n[1] + b.n[1], a.n[2] + b.n[2], a.n[3] + b.n[3]);
}

/// Subtraction
inline const vector4 operator-(const vector4& a, const vector4& b)
{
	return vector4(a.n[0] - b.n[0], a.n[1] - b.n[1], a.n[2] - b.n[2], a.n[3] - b.n[3]);
}

/// Multiplication by a constant
inline const vector4 operator*(const vector4& a, const double_t d)
{
	return vector4(a.n[0] * d, a.n[1] * d, a.n[2] * d, a.n[3] * d);
}

/// Multiplication by a constant
inline const vector4 operator*(const double_t d, const vector4& a)
{
	return vector4(a.n[0] * d, a.n[1] * d, a.n[2] * d, a.n[3] * d);
}

/// Returns the dot product of two vectors
inline const double_t operator*(const vector4& a, const vector4& b)
{
	return a.n[0] * b.n[0] + a.n[1] * b.n[1] + a.n[2] * b.n[2] + a.n[3] * b.n[3];
}

/// Division by a constant
inline const vector4 operator/(const vector4& a, const double_t d)
{
	return_val_if_fail(d, vector4());
	return vector4(a.n[0] / d, a.n[1] / d, a.n[2] / d, a.n[3] / d);
}

/// Equality
inline const bool operator==(const vector4& a, const vector4& b)
{
	return a.n[0] == b.n[0] && a.n[1] == b.n[1] && a.n[2] == b.n[2] && a.n[3] == b.n[3];
}

/// Inequality
inline const bool operator!=(const vector4& a, const vector4& b)
{
	return a.n[0] != b.n[0] || a.n[1] != b.n[1] || a.n[2] != b.n[2] || a.n[3] != b.n[3];
}

/// Returns the length of a vector
inline const double_t length(const vector4& Vector)
{
	return Vector.length();
}

/// Returns the normalized form of a vector
inline const vector4 normalize(const vector4& Vector)
{
	const double_t length = Vector.length();
	return_val_if_fail(length, Vector);
	return Vector / length;
}

/// Specialization of almost_equal that tests two vector4 objects for near-equality
template<>
class almost_equal<vector4>
{
	typedef vector4 T;
public:
	almost_equal(const boost::uint64_t Threshold) : threshold(Threshold) { }
	inline const bool operator()(const T& A, const T& B) const
	{
		return std::equal(A.n, A.n + 4, B.n, almost_equal<double_t>(threshold));
	}

private:
	const boost::uint64_t threshold;
};

} // namespace k3d

#endif // !K3DSDK_VECTOR4_H

