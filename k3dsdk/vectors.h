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

#include "result.h"

#include <cmath>
#include <iostream>

namespace k3d
{

class point2;
class point3;
class point4;
class vector2;
class vector3;
class normal3;

/////////////////////////////////////////////////////////////////////////////
// point2

/// Encapsulates a two-dimensional location
class point2
{
public:
	/// Stores the point values
	double n[2];

	// Constructors
	point2()
	{
		n[0] = n[1] = 0.0;
	}
	
	point2(const double x, const double y)
	{
		n[0] = x;
		n[1] = y;
	}

	point2(const double d[2])
	{
		n[0] = d[0];
		n[1] = d[1];
	}
	
	point2(const point2& v)
	{
		n[0] = v.n[0];
		n[1] = v.n[1];
	}

	/// Assignment
	point2& operator=(const point2& v)
	{
		n[0] = v.n[0];
		n[1] = v.n[1];
		return *this;
	}

	/// Assigns a C/C++ style array
	point2& operator=(const double d[2])
	{
		n[0] = d[0];
		n[1] = d[1];
		return *this;
	}

	/// Addition
	point2& operator+=(const point2& v)
	{
		n[0] += v.n[0];
		n[1] += v.n[1];
		return *this;
	}

	/// Addition
	point2& operator+=(const vector2& v);

	/// Subtraction
	point2& operator-=(const point2& v)
	{
		n[0] -= v.n[0];
		n[1] -= v.n[1];
		return *this;
	}

	/// Subtraction
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
	
	/// Integrate with legacy array-oriented APIs
	const double* data() const
	{
		return &n[0];
	}
	
	/// Integrate with legacy array-oriented APIs
	double* data()
	{
		return &n[0];
	}

	friend std::ostream& operator<<(std::ostream& Stream, const point2& RHS)
	{
		Stream << RHS.n[0] << " " << RHS.n[1];
		return Stream;
	}

	friend std::istream& operator>>(std::istream& Stream, point2& RHS)
	{
		Stream >> RHS.n[0] >> RHS.n[1];
		return Stream;
	}
};

/////////////////////////////////////////////////////////////////////////////
// point3

/// Encapsulates a three-dimensional location
class point3
{
public:
	/// Stores the point values
	double n[3];

	point3()
	{
		n[0] = n[1] = n[2] = 0.0;
	}

	point3(const double x, const double y, const double z)
	{
		n[0] = x;
		n[1] = y;
		n[2] = z;
	}

	point3(const double d[3])
	{
		n[0] = d[0];
		n[1] = d[1];
		n[2] = d[2];
	}

	point3(const point3& v)
	{
		n[0] = v.n[0];
		n[1] = v.n[1];
		n[2] = v.n[2];
	}

	/// Casts a point2 to a point3 with the given third dimension
	point3(const point2& v, double d)
	{
		n[0] = v.n[0];
		n[1] = v.n[1];
		n[2] = d;
	}

	/// Assignment
	point3& operator=(const point3& v)
	{
		n[0] = v.n[0];
		n[1] = v.n[1];
		n[2] = v.n[2];
		return *this;
	}

	/// Assignment of a C/C++ style array
	point3& operator=(const double d[3])
	{
		n[0] = d[0];
		n[1] = d[1];
		n[2] = d[2];
		return *this;
	}

	/// Addition
	point3& operator+=(const point3& v)
	{
		n[0] += v.n[0];
		n[1] += v.n[1];
		n[2] += v.n[2];
		return *this;
	}

	/// Addition
	point3& operator+=(const vector3& v);

	/// Subtraction
	point3& operator-=(const point3& v)
	{
		n[0] -= v.n[0];
		n[1] -= v.n[1];
		n[2] -= v.n[2];
		return *this;
	}

	/// Subtraction
	point3& operator-=(const vector3& v);

	/// Multiplication by a constant
	point3& operator*=(const double d)
	{
		n[0] *= d;
		n[1] *= d;
		n[2] *= d;
		return *this; 
	}

	/// Division by a constant
	point3& operator/=(const double d)
	{
		return_val_if_fail(d, *this);

		double d_inv = 1./d;
		n[0] *= d_inv;
		n[1] *= d_inv;
		n[2] *= d_inv;

		return *this;
	}

	/// Returns the given dimension by reference
	double& operator[](int i)
	{
		return n[i];
	}

	/// Returns the given dimension by value
	double operator[](int i) const
	{
		return n[i];
	}

	/// Integrate with legacy array-oriented APIs
	const double* data() const
	{
		return &n[0];
	}

	/// Integrate with legacy array-oriented APIs
	double* data()
	{
		return &n[0];
	}

	friend std::ostream& operator<<(std::ostream& Stream, const point3& RHS)
	{
		Stream << RHS.n[0] << " " << RHS.n[1] << " " << RHS.n[2];
		return Stream;
	}

	friend std::istream& operator>>(std::istream& Stream, point3& RHS)
	{
		Stream >> RHS.n[0] >> RHS.n[1] >> RHS.n[2];
		return Stream;
	}
};

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

	point4(const double d[4])
	{
		n[0] = d[0];
		n[1] = d[1];
		n[2] = d[2];
		n[3] = d[3];
	}

	point4(const point4& v)
	{
		n[0] = v.n[0];
		n[1] = v.n[1];
		n[2] = v.n[2];
		n[3] = v.n[3];
	}

	/// Casts n point3 to n point4 with the given fourth dimension
	point4(const point3& v, const double d)
	{
		n[0] = v.n[0];
		n[1] = v.n[1];
		n[2] = v.n[2];
		n[3] = d;
	}

	/// Assignment
	point4& operator=(const point4& v)
	{
		n[0] = v.n[0];
		n[1] = v.n[1];
		n[2] = v.n[2];
		n[3] = v.n[3];
		return *this;
	}

	/// Assignment of a C/C++ array
	point4& operator=(const double d[4])
	{
		n[0] = d[0];
		n[1] = d[1];
		n[2] = d[2];
		n[3] = d[3];
		return *this;
	}

	/// Addition
	point4& operator+=(const point4& v)
	{
		n[0] += v.n[0];
		n[1] += v.n[1];
		n[2] += v.n[2];
		n[3] += v.n[3];
		return *this;
	}

	/// Subtraction
	point4& operator-=(const point4& v)
	{
		n[0] -= v.n[0];
		n[1] -= v.n[1];
		n[2] -= v.n[2];
		n[3] -= v.n[3];
		return *this;
	}

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

	/// Returns a vector element by reference
	double& operator[](int i)
	{
		return n[i];
	}

	/// Returns a vector element by value
	double operator[](int i) const
	{
		return n[i];
	}

	/// Integrate with legacy array-oriented APIs
	const double* data() const
	{
		return &n[0];
	}

	/// Integrate with legacy array-oriented APIs
	double* data()
	{
		return &n[0];
	}

	friend std::ostream& operator<<(std::ostream& Stream, const point4& RHS)
	{
		Stream << RHS.n[0] << " " << RHS.n[1] << " " << RHS.n[2] << " " << RHS.n[3];
		return Stream;
	}

	friend std::istream& operator>>(std::istream& Stream, point4& RHS)
	{
		Stream >> RHS.n[0] >> RHS.n[1] >> RHS.n[2] >> RHS.n[3];
		return Stream;
	}
};

/////////////////////////////////////////////////////////////////////////////
// vector2

/// Encapsulates a 2D direction vector
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
		n[0] += v.n[0];
		n[1] += v.n[1];
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
		assert_warning((i == 0) || (i == 1));
		return n[i];
	}

	double operator[](const unsigned int i) const
	{
		return_val_if_fail((i == 0) || (i == 1), 0);
		return n[i];
	}

	/// Returns the normal length
	double length() const
	{
		return sqrt(length2());
	}

	/// Returns the squared normal length
	double length2() const
	{
		return n[0] * n[0] + n[1] * n[1];
	}

	friend std::ostream& operator<<(std::ostream& Stream, const vector2& RHS)
	{
		Stream << RHS.n[0] << " " << RHS.n[1];
		return Stream;
	}

	friend std::istream& operator>>(std::istream& Stream, vector2& RHS)
	{
		Stream >> RHS.n[0] >> RHS.n[1];
		return Stream;
	}
};

/////////////////////////////////////////////////////////////////////////////
// vector3

/// Encapsulates a 3D direction vector
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

	vector3(const vector3& v)
	{
		n[0] = v.n[0];
		n[1] = v.n[1];
		n[2] = v.n[2];
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
		return sqrt(length2());
	}

	/// Returns the squared normal length
	double length2() const
	{
		return n[0] * n[0] + n[1] * n[1] + n[2] * n[2];
	}

	friend std::ostream& operator<<(std::ostream& Stream, const vector3& RHS)
	{
		Stream << RHS.n[0] << " " << RHS.n[1] << " " << RHS.n[2];
		return Stream;
	}

	friend std::istream& operator>>(std::istream& Stream, vector3& RHS)
	{
		Stream >> RHS.n[0] >> RHS.n[1] >> RHS.n[2];
		return Stream;
	}
};

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

	normal3(const normal3& Normal)
	{
		n[0] = Normal.n[0];
		n[1] = Normal.n[1];
		n[2] = Normal.n[2];
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
		return sqrt(length2());
	}

	/// Returns the squared normal length
	double length2() const
	{
		return n[0] * n[0] + n[1] * n[1] + n[2] * n[2];
	}

	friend std::ostream& operator<<(std::ostream& Stream, const normal3& RHS)
	{
		Stream << RHS.n[0] << " " << RHS.n[1] << " " << RHS.n[2];
		return Stream;
	}

	friend std::istream& operator>>(std::istream& Stream, normal3& RHS)
	{
		Stream >> RHS.n[0] >> RHS.n[1] >> RHS.n[2];
		return Stream;
	}
};

/////////////////////////////////////////////////////////////////////////////
// point2 implementation

inline point2& point2::operator+=(const vector2& v)
{ n[0] += v.n[0]; n[1] += v.n[1]; return *this; }

inline point2& point2::operator-=(const vector2& v)
{ n[0] -= v.n[0]; n[1] -= v.n[1]; return *this; }

inline point2 operator-(const point2& a)
{ return point2(-a.n[0],-a.n[1]); }

inline point2 operator+(const point2& a, const point2& b)
{ return point2(a.n[0]+ b.n[0], a.n[1] + b.n[1]); }

inline point2 operator-(const point2& a, const point2& b)
{ return point2(a.n[0]-b.n[0], a.n[1]-b.n[1]); }

inline point2 operator*(const point2& a, const double d)
{ return point2(d*a.n[0], d*a.n[1]); }

inline point2 operator*(const double d, const point2& a)
{ return a*d; }

inline double operator*(const point2& a, const point2& b)
{ return (a.n[0]*b.n[0] + a.n[1]*b.n[1]); }

inline point2 operator/(const point2& a, const double d)
{
	return_val_if_fail(d, point2());

	double d_inv = 1./d;
	return point2(a.n[0]*d_inv, a.n[1]*d_inv);
}

inline bool operator==(const point2& a, const point2& b)
{ return (a.n[0] == b.n[0]) && (a.n[1] == b.n[1]); }

inline bool operator!=(const point2& a, const point2& b)
{ return !(a == b); }

/////////////////////////////////////////////////////////////////////////////
// point3 implementation

inline point3& point3::operator+=(const vector3& v)
{ n[0] += v.n[0]; n[1] += v.n[1]; n[2] += v.n[2]; return *this; }

inline point3& point3::operator-=(const vector3& v)
{ n[0] -= v.n[0]; n[1] -= v.n[1]; n[2] -= v.n[2]; return *this; }

inline point3 operator-(const point3& a)
{ return point3(-a.n[0],-a.n[1],-a.n[2]); }

inline point3 operator+(const point3& a, const point3& b)
{ return point3(a.n[0]+ b.n[0], a.n[1] + b.n[1], a.n[2] + b.n[2]); }

inline point3 operator-(const point3& a, const point3& b)
{ return point3(a.n[0]-b.n[0], a.n[1]-b.n[1], a.n[2]-b.n[2]); }

inline point3 operator*(const point3& a, const double d)
{ return point3(d*a.n[0], d*a.n[1], d*a.n[2]); }

inline point3 operator*(const double d, const point3& a)
{ return a*d; }

inline double operator*(const point3& a, const point3& b)
{ return (a.n[0]*b.n[0] + a.n[1]*b.n[1] + a.n[2]*b.n[2]); }

inline point3 operator/(const point3& a, const double d)
{
	return_val_if_fail(d, point3());

	double d_inv = 1./d;
	return point3(a.n[0]*d_inv, a.n[1]*d_inv, a.n[2]*d_inv);
}

inline bool operator==(const point3& a, const point3& b)
{ return (a.n[0] == b.n[0]) && (a.n[1] == b.n[1]) && (a.n[2] == b.n[2]);
}

inline bool operator!=(const point3& a, const point3& b)
{ return !(a == b); }

/////////////////////////////////////////////////////////////////////////////
// point4 implementation

inline point4 operator-(const point4& a)
{ return point4(-a.n[0],-a.n[1],-a.n[2],-a.n[3]); }

inline point4 operator+(const point4& a, const point4& b)
{ return point4(a.n[0] + b.n[0], a.n[1] + b.n[1], a.n[2] + b.n[2],
 a.n[3] + b.n[3]); }

inline point4 operator-(const point4& a, const point4& b)
{ return point4(a.n[0] - b.n[0], a.n[1] - b.n[1], a.n[2] - b.n[2],
 a.n[3] - b.n[3]); }

inline point4 operator*(const point4& a, const double d)
{ return point4(d*a.n[0], d*a.n[1], d*a.n[2], d*a.n[3] ); }

inline point4 operator*(const double d, const point4& a)
{ return a*d; }

inline point4 operator/(const point4& a, const double d)
{
	return_val_if_fail(d, point4());

	double d_inv = 1./d;
	return point4(a.n[0]*d_inv, a.n[1]*d_inv, a.n[2]*d_inv, a.n[3]*d_inv);
}

inline bool operator==(const point4& a, const point4& b)
{ return (a.n[0] == b.n[0]) && (a.n[1] == b.n[1]) && (a.n[2] == b.n[2])
 && (a.n[3] == b.n[3]); }

inline bool operator!=(const point4& a, const point4& b)
{ return !(a == b); }

/////////////////////////////////////////////////////////////////////////////
// vector2 implementation

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

/// Subtraction
inline const vector2 operator-(const vector2& a, const vector2& b)
{
	return vector2(a.n[0] - b.n[0], a.n[1] - b.n[1]);
}

/// Subtracts a vector from a point, returning the modified point
inline const point2 operator-(const point2& a, const vector2& b)
{
	return point2(a.n[0] - b.n[0], a.n[1] - b.n[1]);
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
inline const double operator*(const vector2& a, const vector2& b)
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
inline const bool operator==(const vector2& a, const vector2& b)
{
	return a.n[0] == b.n[0] && a.n[1] == b.n[1];
}

/// Non-equality
inline const bool operator!=(const vector2& a, const vector2& b)
{
	return a.n[0] != b.n[0] || a.n[1] != b.n[1];
}

/////////////////////////////////////////////////////////////////////////////
// vector3 implementation

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

/// Add a point and a vector, returning the moved point
inline const point3 operator+(const point3& a, const vector3& b)
{
	return point3(a.n[0] + b.n[0], a.n[1] + b.n[1], a.n[2] + b.n[2]);
}

/// Adds a vector and a point, returning the moved point
inline const point3 operator+(const vector3& a, const point3& b)
{
	return point3(a.n[0] + b.n[0], a.n[1] + b.n[1], a.n[2] + b.n[2]);
}

/// Subtraction
inline const vector3 operator-(const vector3& a, const vector3& b)
{
	return vector3(a.n[0] - b.n[0], a.n[1] - b.n[1], a.n[2] - b.n[2]);
}

/// Subtracts a vector from a point, returning the modified point
inline const point3 operator-(const point3& a, const vector3& b)
{
	return point3(a.n[0] - b.n[0], a.n[1] - b.n[1], a.n[2] - b.n[2]);
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

/// Equality
inline const bool operator==(const vector3& a, const vector3& b)
{
	return a.n[0] == b.n[0] && a.n[1] == b.n[1] && a.n[2] == b.n[2];
}

/// Non-equality
inline const bool operator!=(const vector3& a, const vector3& b)
{
	return a.n[0] != b.n[0] || a.n[1] != b.n[1] || a.n[2] != b.n[2];
}

/////////////////////////////////////////////////////////////////////////////
// normal3 implementation

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

/// Non-equality
inline const bool operator!=(const normal3& a, const normal3& b)
{
	return a.n[0] != b.n[0] || a.n[1] != b.n[1] || a.n[2] != b.n[2];
}

/////////////////////////////////////////////////////////////////////////////
// Miscellaneous functions

/// Returns the length of a vector
inline const double length(const vector2& Vector)
{
	return Vector.length();
}

/// Returns the length of a vector
inline const double length(const vector3& Vector)
{
	return Vector.length();
}

/// Returns the normalized form of a vector
template<typename T>
const T normalize(const T& Vector)
{
	const double length = Vector.length();
	return_val_if_fail(length, Vector);

	return Vector / length;
}

/// Returns the distance between two points
inline const double distance(const point2& P1, const point2& P2)
{
	const point2 v = P2 - P1;
	return sqrt(v.n[0]*v.n[0] + v.n[1]*v.n[1]);
}

/// Returns the distance between two points
inline const double distance(const point3& P1, const point3& P2)
{
	const point3 v = P2 - P1;
	return sqrt(v.n[0]*v.n[0] + v.n[1]*v.n[1] + v.n[2]*v.n[2]);
}

/// Explicit conversion
inline const point2 to_point(const vector2& v)
{
	return point2(v.n[0], v.n[1]);
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

/// Converts Cartesian coordinates to spherical coordinates
inline const vector3 spherical(const vector3& Vector)
{
	return vector3(Vector.length(), atan2(Vector[0], Vector[2]), atan2(Vector[1], sqrt(Vector[0] * Vector[0] + Vector[2] * Vector[2])));
/*
	// Handle the singularity at the poles
	if(0.0 == Vector[0] && 0.0 == Vector[2])
		return point3(n[1] > 0.0 ? sdpPiOver2 : -sdpPiOver2, 0.0, Vector.length());


	return point3(atan2(Vector[1], sqrt(Vector[0] * Vector[0] + Vector[2] * Vector[2])), atan2(Vector[0], Vector[2]), Vector.length());
*/
}

} // namespace k3d

#endif // K3DSDK_VECTORS_H

