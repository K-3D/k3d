#ifndef MXVEC3_INCLUDED
#define MXVEC3_INCLUDED

// MixKit -- Code library for multiresolution surface modeling
// Copyright (c) 1998, Michael Garland
//
// Contact: http://graphics.cs.uiuc.edu/~garland/software/qslim.html
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Library General Public
// License as published by the Free Software Foundation; either
// version 2 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Library General Public License for more details.
//
// You should have received a copy of the GNU General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

/** \file
		\brief Vectors and Matrices
		\author Adapted for K-3D: Romain Behar (romainbehar@yahoo.com)
*/

#include <cmath>
#include <iostream>

#ifndef FEQ_EPS2
#define FEQ_EPS2 1e-12
#endif

enum Axis {X=0, Y=1, Z=2, W=3};


class Vec3 {
private:
	double elt[3];

protected:
	inline void copy(const Vec3& v);

public:
	Vec3(double x=0, double y=0, double z=0) { elt[0]=x; elt[1]=y; elt[2]=z; }
	Vec3(const Vec3& v) { copy(v); }
	Vec3(const double *v) { elt[0]=v[0]; elt[1]=v[1]; elt[2]=v[2]; }

	// Access methods
	operator double*() { return elt; }
	operator const double*() const { return elt; }

	// Comparison operators
	inline bool operator==(const Vec3& v) const;
	inline bool operator!=(const Vec3& v) const;

	// Assignment and in-place arithmetic methods
	inline Vec3& operator=(const Vec3& v);
	inline Vec3& operator+=(const Vec3& v);
	inline Vec3& operator-=(const Vec3& v);
	inline Vec3& operator*=(double s);
	inline Vec3& operator/=(double s);

	// Binary arithmetic methods
	inline Vec3 operator+(const Vec3& v) const;
	inline Vec3 operator-(const Vec3& v) const;
	inline Vec3 operator-() const;

	inline Vec3 operator*(double s) const;
	inline Vec3 operator/(double s) const;
	inline double operator*(const Vec3& v) const;
	inline Vec3 operator^(const Vec3& v) const;

	// Other
	Vec3& Normalize();
	double Length()
	{
		return std::sqrt(elt[0]*elt[0]+elt[1]*elt[1]+elt[2]*elt[2]);
	}
};

class Vec4
{
private:
	double elt[4];

public:
	Vec4(double x=0, double y=0, double z=0, double w=0)
	{
		elt[0]=x; elt[1]=y; elt[2]=z; elt[3]=w;
	}
	Vec4(const Vec3& v,double w)
	{
		elt[0]=v[0];elt[1]=v[1];elt[2]=v[2];elt[3]=w;
	}

	// Access methods
	operator double*() { return elt; }
	operator const double*() const { return elt; }

	// Assignment and in-place arithmetic methods
	inline Vec4& operator=(const Vec4& v);
};

class Mat3
{
private:
	Vec3 row[3];
	inline Vec3 col(int i) const {return Vec3(row[0][i],row[1][i],row[2][i]);}

public:
	Mat3()
	{
		row[0] = Vec3(0, 0, 0);
		row[1] = Vec3(0, 0, 0);
		row[2] = Vec3(0, 0, 0);
	}

	Mat3(const Vec3& r0,const Vec3& r1,const Vec3& r2)
	{
		row[0] = r0;
		row[1] = r1;
		row[2] = r2;
	}

	inline Mat3 operator/(double s) const;
	inline Vec3 operator*(const Vec3& v) const;

	// Matrix operations
	double det();
	Mat3 transpose();
	Mat3 adjoint();
	double invert(Mat3&);
};


////////////////////////////////////////////////////////////////////////
//
// Method definitions
//

inline void Vec3::copy(const Vec3& v)
{
	elt[0]=v.elt[0]; elt[1]=v.elt[1]; elt[2]=v.elt[2];
}

inline bool Vec3::operator==(const Vec3& v) const
{
	double dx=elt[X]-v[X],  dy=elt[Y]-v[Y],  dz=elt[Z]-v[Z];
	return (dx*dx + dy*dy + dz*dz) < FEQ_EPS2;
}

inline bool Vec3::operator!=(const Vec3& v) const
{
	double dx=elt[X]-v[X],  dy=elt[Y]-v[Y],  dz=elt[Z]-v[Z];
	return (dx*dx + dy*dy + dz*dz) > FEQ_EPS2;
}

inline Vec3& Vec3::operator=(const Vec3& v)
{
	copy(v);
	return *this;
}

inline Vec3& Vec3::operator+=(const Vec3& v)
{
	elt[0] += v[0];   elt[1] += v[1];   elt[2] += v[2];
	return *this;
}

inline Vec3& Vec3::operator-=(const Vec3& v)
{
	elt[0] -= v[0];   elt[1] -= v[1];   elt[2] -= v[2];
	return *this;
}

inline Vec3& Vec3::operator*=(double s)
{
	elt[0] *= s;   elt[1] *= s;   elt[2] *= s;
	return *this;
}

inline Vec3& Vec3::operator/=(double s)
{
	elt[0] /= s;   elt[1] /= s;   elt[2] /= s;
	return *this;
}


inline Vec3 Vec3::operator+(const Vec3& v) const
{
	return Vec3(elt[0]+v[0], elt[1]+v[1], elt[2]+v[2]);
}

inline Vec3 Vec3::operator-(const Vec3& v) const
{
	return Vec3(elt[0]-v[0], elt[1]-v[1], elt[2]-v[2]);
}

inline Vec3 Vec3::operator-() const
{
	return Vec3(-elt[0], -elt[1], -elt[2]);
}

inline Vec3 Vec3::operator*(double s) const
{
	return Vec3(elt[0]*s, elt[1]*s, elt[2]*s);
}

inline Vec3 Vec3::operator/(double s) const
{
	return Vec3(elt[0]/s, elt[1]/s, elt[2]/s);
}

inline double Vec3::operator*(const Vec3& v) const
{
	return elt[0]*v[0] + elt[1]*v[1] + elt[2]*v[2];
}

inline Vec3 Vec3::operator^(const Vec3& v) const
{
	Vec3 w( elt[1]*v[2] - v[1]*elt[2],
	   -elt[0]*v[2] + v[0]*elt[2],
	    elt[0]*v[1] - v[0]*elt[1] );
	return w;
}

// Make scalar multiplication commutative
inline Vec3 operator*(double s, const Vec3& v) { return v*s; }


////////////////////////////////////////////////////////////////////////
//
// Primitive function definitions
//

inline double norm(const Vec3& v)
{
	return std::sqrt(v[0]*v[0] + v[1]*v[1] + v[2]*v[2]);
}

inline double norm2(const Vec3& v)
{
	return v[0]*v[0] + v[1]*v[1] + v[2]*v[2];
}

inline Vec3& Vec3::Normalize()
{
	if(const double length = norm(*this))
		*this /= length;

	return *this;
}

////////////////////////////////////////////////////////////////////////
//
// Misc. function definitions
//

inline std::ostream& operator<<(std::ostream& out, const Vec3& v)
{
	return out << v[0] << " " << v[1] << " " << v[2];
}

inline std::istream& operator>>(std::istream& in, Vec3& v)
{
	return in >> v[0] >> v[1] >> v[2];
}

////////////////////////////////////////////////////////////////////////
//
// Vec4
//

inline Vec4& Vec4::operator=(const Vec4& v)
{
	elt[0]=v.elt[0]; elt[1]=v.elt[1]; elt[2]=v.elt[2]; elt[3]=v.elt[3];
	return *this;
}

////////////////////////////////////////////////////////////////////////
//
// Mat3
//

inline Mat3 Mat3::operator/(double s) const
{
	return Mat3(row[0]/s, row[1]/s, row[2]/s);
}

inline Vec3 Mat3::operator*(const Vec3& v) const
{
	return Vec3(row[0]*v, row[1]*v, row[2]*v);
}

#endif // MXVEC3_INCLUDED

