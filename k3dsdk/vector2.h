#ifndef K3DSDK_VECTOR2_H
#define K3DSDK_VECTOR2_H

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

#include <cmath>
#include <iomanip>

namespace k3d
{

/////////////////////////////////////////////////////////////////////////////
// vector2

/// Encapsulates a direction vector in two-dimensional space
class vector2
{
public:
	/// Stores the vector values
	double n[2];

	vector2()
	{
		n[0] = n[1] = 0.0;
	}

	vector2(const double x, const double y)
	{
		n[0] = x;
		n[1] = y;
	}

	vector2& operator+=(const vector2& v)
	{
		n[0] += v.n[0];
		n[1] += v.n[1];
		return *this;
	}

	vector2& operator-=(const vector2& v)
	{
		n[0] -= v.n[0];
		n[1] -= v.n[1];
		return *this;
	}

	vector2& operator*=(const double d)
	{
		n[0] *= d;
		n[1] *= d;
		return *this;
	}

	vector2& operator/=(const double d)
	{
		return_val_if_fail(d, *this);

		const double d_inv = 1./d;
		n[0] *= d_inv;
		n[1] *= d_inv;
		return *this;
	}

	double& operator[](const unsigned int i)
	{
		return n[i];
	}

	double operator[](const unsigned int i) const
	{
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
		return n[0] * n[0] + n[1] * n[1];
	}

	friend std::ostream& operator<<(std::ostream& Stream, const vector2& RHS)
	{
		boost::io::ios_flags_saver stream_state(Stream);
		Stream << std::setprecision(17) << RHS.n[0] << " " << RHS.n[1];
		return Stream;
	}

	friend std::istream& operator>>(std::istream& Stream, vector2& RHS)
	{
		Stream >> RHS.n[0] >> RHS.n[1];
		return Stream;
	}
};

/// Negation
inline const vector2 operator-(const vector2& v)
{
	return vector2(-v.n[0], -v.n[1]);
}

/// Addition
inline const vector2 operator+(const vector2& a, const vector2& b)
{
	return vector2(a.n[0] + b.n[0], a.n[1] + b.n[1]);
}

/// Subtraction
inline const vector2 operator-(const vector2& a, const vector2& b)
{
	return vector2(a.n[0] - b.n[0], a.n[1] - b.n[1]);
}

/// Multiplication by a constant
inline const vector2 operator*(const vector2& a, const double d)
{
	return vector2(a.n[0] * d, a.n[1] * d);
}

/// Multiplication by a constant
inline const vector2 operator*(const double d, const vector2& a)
{
	return vector2(a.n[0] * d, a.n[1] * d);
}

/// Returns the dot product of two vectors
inline double operator*(const vector2& a, const vector2& b)
{
	return a.n[0] * b.n[0] + a.n[1] * b.n[1];
}

/// Division by a constant
inline const vector2 operator/(const vector2& a, const double d)
{
	return_val_if_fail(d, vector2());
	return vector2(a.n[0] / d, a.n[1] / d);
}

/// Equality
inline bool operator==(const vector2& a, const vector2& b)
{
	return a.n[0] == b.n[0] && a.n[1] == b.n[1];
}

/// Inequality
inline bool operator!=(const vector2& a, const vector2& b)
{
	return a.n[0] != b.n[0] || a.n[1] != b.n[1];
}

/// Returns the length of a vector
inline double length(const vector2& Vector)
{
	return Vector.length();
}

/// Returns the normalized form of a vector
inline const vector2 normalize(const vector2& Vector)
{
	const double length = Vector.length();
	return_val_if_fail(length, Vector);
	return Vector / length;
}

/// Returns a vector perpendicular to the given vector
inline const vector2 perpendicular(const vector2& Vector)
{
	return vector2(Vector[1], -Vector[0]);
}

/// Specialization of almost_equal that tests two vector2 objects for near-equality
template<>
class almost_equal<vector2>
{
	typedef vector2 T;
public:
	almost_equal(const boost::uint64_t Threshold) : threshold(Threshold) { }
	inline bool_t operator()(const T& A, const T& B) const
	{
		return std::equal(A.n, A.n + 2, B.n, almost_equal<double_t>(threshold));
	}

private:
	const boost::uint64_t threshold;
};

} // namespace k3d

#endif // !K3DSDK_VECTOR2_H

