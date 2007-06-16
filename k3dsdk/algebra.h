#ifndef K3DSDK_ALGEBRA_H
#define K3DSDK_ALGEBRA_H

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
		\brief Algebra classes
		\author Tim Shead (tshead@k-3d.com)
*/

#include "basic_math.h"
#include "log.h"
#include "vectors.h"

#include <cfloat>

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

namespace k3d
{

class matrix4;
class quaternion;
class angle_axis;
class euler_angles;

/////////////////////////////////////////////////////////////////////////////
// point3

/// Matrix multiplication
point3 operator*(const matrix4& a, const point3& v);
/// Matrix multiplication
point3 operator*(const point3& v, const matrix4& a);

/////////////////////////////////////////////////////////////////////////////
// point4

/// Matrix multiplication
point4 operator*(const matrix4& a, const point4& v);
/// Matrix multiplication
point4 operator*(const point4& v, const matrix4& a);
/// Linear transformation
point3 operator*(const matrix4& a, const point3& v);
/// Matrix multiplication
matrix4 operator*(const matrix4& a, const matrix4& b);

/////////////////////////////////////////////////////////////////////////////
// matrix4

/// A 4x4 matrix
class matrix4
{
public:
	/// Stores the matrix elements
	point4 v[4];
	// Constructors
	matrix4();
	matrix4(const point4& v0, const point4& v1, const point4& v2, const point4& v3);
	matrix4(const double d);
	matrix4(euler_angles Angles);
	/// Copy constructor
	matrix4(const matrix4& m);
	/// Assignment of an matrix4
	matrix4& operator=(const matrix4& m);
	/// Assignment of a C/C++ array
	matrix4& operator=(const double d[4][4]);
	/// Assignment of a C/C++ array
	matrix4& operator=(const double d[16]);
	/// Addition
	matrix4& operator+=(const matrix4& m);
	/// Subtraction
	matrix4& operator-=(const matrix4& m);
	/// Multiplication by a constant
	matrix4& operator*=(const double d);
	/// Division by a constant
	matrix4& operator/=(const double d);
	/// Returns a vector by index
	point4& operator[](int i);
	/// Returns a vector by index
	const point4& operator[](int i) const;
	/// Copies the matrix contents into a C/C++ style array
	void CopyArray(float m[16]) const;
	/// Copies the matrix contents into a C/C++ style array
	void CopyArray(double m[16]) const;
	/// Casts the matrix to a C/C++ style array
	operator double*() { return &v[0][0]; }
};

/// Negation
matrix4 operator-(const matrix4& a);
/// Addition
matrix4 operator+(const matrix4& a, const matrix4& b);
/// Subtraction
matrix4 operator-(const matrix4& a, const matrix4& b);
/// Matrix multiplication
matrix4 operator*(const matrix4& a, const matrix4& b);
/// Multiplication by a constant
matrix4 operator*(const matrix4& a, const double d);
/// Multiplication by a constant
matrix4 operator*(const double d, const matrix4& a);
/// Division by a constant
matrix4 operator/(const matrix4& a, const double d);
/// Equality
bool operator==(const matrix4& a, const matrix4& b);
/// Non-equality
bool operator!=(const matrix4& a, const matrix4& b);
/// Linear transformation
point4 operator*(const matrix4& a, const point4& v);
/// Linear transformation
point3 operator*(const matrix4& a, const point3& v);
/// Returns a three-dimensional identity matrix
inline matrix4 identity3D()
{
	return matrix4(
		point4(1, 0, 0, 0),
		point4(0, 1, 0, 0),
		point4(0, 0, 1, 0),
		point4(0, 0, 0, 1));
}
/// Returns the transposition of a matrix
inline matrix4 transpose(const matrix4& v)
{
	return matrix4(
		point4(v[0][0], v[1][0], v[2][0], v[3][0]),
		point4(v[0][1], v[1][1], v[2][1], v[3][1]),
		point4(v[0][2], v[1][2], v[2][2], v[3][2]),
		point4(v[0][3], v[1][3], v[2][3], v[3][3]));
}
/// Returns the inverse of a matrix
inline matrix4 inverse(const matrix4& v)
{
	matrix4 a(v),	 // As a evolves from original mat into identity
	b(identity3D()); // b evolves from identity into inverse(a)

	// Loop over cols of a from left to right, eliminating above and below diag
	for(int j=0; j<4; ++j) { // Find largest pivot in column j among rows j..3
		int i1 = j;		 // Row with largest pivot candidate

		for(int i=j+1; i<4; ++i)
		{
			if (fabs(a.v[i].n[j]) > fabs(a.v[i1].n[j]))
				i1 = i;
		}

		// Swap rows i1 and j in a and b to put pivot on diagonal
		std::swap(a.v[i1], a.v[j]);
		std::swap(b.v[i1], b.v[j]);

		// Scale row j to have a unit diagonal
		if(!a.v[j].n[j])
		{
			// Singular matrix - can't invert
			log() << error << "Can't invert singular matrix!" << std::endl;
			return b;
		}

		b.v[j] /= a.v[j].n[j];
		a.v[j] /= a.v[j].n[j];

		// Eliminate off-diagonal elems in col j of a, doing identical ops to b
		for(int i=0; i<4; ++i)
		{
			if(i!=j)
			{
				b.v[i] -= a.v[i].n[j]*b.v[j];
				a.v[i] -= a.v[i].n[j]*a.v[j];
			}
		}
	}
	return b;
}

/// Returns true if a matrix contains negative scale factors that will flip an object "inside-out"
inline const bool inside_out(const matrix4& m)
{
	const vector3 a(m[0][0], m[0][1], m[0][2]);
	const vector3 b(m[1][0], m[1][1], m[1][2]);
	const vector3 c(m[2][0], m[2][1], m[2][2]);

	return ((a ^ b) * c) < 0 ? true : false;
}

/// Linear transformation
inline const vector3 operator*(const matrix4& a, const vector3& v)
{
	const point4 temp((a * point4(v[0], v[1], v[2], 1)) - (a * point4(0, 0, 0, 1)));
	return vector3(temp.n[0], temp.n[1], temp.n[2]);
}

/// Linear transformation
inline const vector3 operator*(const vector3& v, const matrix4& a)
{
	return transpose(a) * v;
}

/// Linear transformation
inline const normal3 operator*(const matrix4& a, const normal3& v)
{
	const point4 temp((a * point4(v[0], v[1], v[2], 1)) - (a * point4(0, 0, 0, 1)));
	return normal3(temp.n[0], temp.n[1], temp.n[2]);
}

/// Linear transformation
inline const normal3 operator*(const normal3& v, const matrix4& a)
{
	return transpose(a) * v;
}

/// Output inserter
std::ostream& operator<<(std::ostream& Stream, const matrix4& Arg);
/// Input extractor
std::istream& operator>>(std::istream& Stream, matrix4& Arg);

/////////////////////////////////////////////////////////////////////////////
// quaternion

/// Encapsulates a quaternion
class quaternion
{
public:
	// The scalar+vector representation is used
	double w;
	vector3 v;

	// Constructors
	quaternion();
	quaternion(const double u, const point3& t);
	quaternion(const double u, const vector3& t);
	quaternion(const double u, const double x, const double y, const double z);
	quaternion(const angle_axis& AngleAxis);
	quaternion(euler_angles Angles);
	/// Assignment of a quaternion
	quaternion& operator=(const quaternion& q);
	/// Assignment of an axis/angle
	quaternion& operator=(const angle_axis& AngleAxis);
	/// Multiplication by a constant
	quaternion& operator*=(const double d);
	/// Division by a constant
	quaternion& operator/=(const double d);
	/// Addition
	quaternion& operator+=(const quaternion& q);
	/// Substraction
	quaternion& operator-=(const quaternion& q);
	/// Multiplication
	quaternion& operator*=(const quaternion& q);
	/// Division
	quaternion& operator/=(const quaternion& q);
	/// Magnitude
	double Magnitude() const;
	/// Normalization
	quaternion& Normalize();
	/// Conjugate
	quaternion& Conjugate();
	/// Inversion in place
	quaternion& Inverse();
	/// Squares the quaternion in place
	quaternion& Square();
	/// Axis rotation
	quaternion& AxisRotation(const double phi);
};

/// Negation
quaternion operator-(quaternion& q);
/// Addition
quaternion operator+(const quaternion& q, const quaternion& r);
/// Substraction
quaternion operator-(const quaternion& q, const quaternion& r);
/// Multiplication by a quaternion
quaternion operator*(const quaternion& q, const quaternion& r);
/// Multiplication by a constant
quaternion operator*(const quaternion& q, const double d);
/// Multiplication by a constant
quaternion operator*(double d, const quaternion& q);
/// Division by a quaternion
quaternion operator/(const quaternion& q, const quaternion& r);
/// Division by a constant
quaternion operator/(const quaternion& q, const double d);
/// Equality test
bool operator==(const quaternion& q, const quaternion& r);
/// Non-equality test
bool operator!=(const quaternion& q, const quaternion& r);

/// Output inserter
std::ostream& operator<<(std::ostream& Stream, const quaternion& Arg);
/// Input extractor
std::istream& operator>>(std::istream& Stream, quaternion& Arg);

/////////////////////////////////////////////////////////////////////////////
// angle_axis

/// Encapsulates an angle/axis representation of an object's orientation
class angle_axis
{
public:
	double angle;
	vector3 axis;

	// Constructors ...
	angle_axis();
	angle_axis(const double Angle, const point3& Axis);
	angle_axis(const double Angle, const vector3& Axis);
	angle_axis(const double Angle, const double X, const double Y, const double Z);
	angle_axis(const double AngleAxis[4]);
	angle_axis(const quaternion& Quaternion);
	/// Copy constructor
	angle_axis(const angle_axis& AngleAxis);
	/// Assignment of an angle_axis
	angle_axis& operator=(const angle_axis& AngleAxis);
	/// Assignment of a quaternion
	angle_axis& operator=(const quaternion& Quaternion);
	/// Assignment of a C/C++ style array
	angle_axis& operator=(const double AngleAxis[4]);
	/// Casting to a C/C++ style array pointer
	operator double*() { return &angle; }
	/// Copies the vector into a C/C++ style array
	void CopyArray(float AngleAxis[4]) const;
	/// Copies the vector into a C/C++ style array
	void CopyArray(double AngleAxis[4]) const;
};

/// Equality test
bool operator==(const angle_axis& a, const angle_axis& b);
/// Non-equality test
bool operator!=(const angle_axis& a, const angle_axis& b);

/// Output inserter
std::ostream& operator<<(std::ostream& Stream, const angle_axis& Arg);
/// Input extractor
std::istream& operator>>(std::istream& Stream, angle_axis& Arg);

#define SDPEULERANGLEORDER(initialaxis, parity, repetition, frame) ((((((initialaxis << 1) + parity) << 1) + repetition) << 1) + frame)

/// Encapsulates a set of Euler angles, including the order in which they should be applied
class euler_angles
{
public:

	/// Enumerates what gets rotated - the axes themselves, or the frame they represent
	typedef enum
	{
		StaticFrame = 0,
		RotatingFrame = 1,
	} OrderFrame;

	/// Enumerates whether the last axis will be the same as the first
	typedef enum
	{
		NoRepetition = 0,
		Repeats = 1,
	} OrderRepetition;

	/// Enumerates which axes will become the second axis
	typedef enum
	{
		EvenParity = 0,
		OddParity = 1,
	} OrderParity;

	/// Enumerates all possible axis order permutations
	typedef enum
	{
		XYZstatic = SDPEULERANGLEORDER(0, EvenParity, NoRepetition, StaticFrame),
		XYXstatic = SDPEULERANGLEORDER(0, EvenParity, Repeats, StaticFrame),
		XZYstatic = SDPEULERANGLEORDER(0, OddParity, NoRepetition, StaticFrame),
		XZXstatic = SDPEULERANGLEORDER(0, OddParity, Repeats, StaticFrame),
		YZXstatic = SDPEULERANGLEORDER(1, EvenParity, NoRepetition, StaticFrame),
		YZYstatic = SDPEULERANGLEORDER(1, EvenParity, Repeats, StaticFrame),
		YXZstatic = SDPEULERANGLEORDER(1, OddParity, NoRepetition, StaticFrame),
		YXYstatic = SDPEULERANGLEORDER(1, OddParity, Repeats, StaticFrame),
		ZXYstatic = SDPEULERANGLEORDER(2, EvenParity, NoRepetition, StaticFrame),
		ZXZstatic = SDPEULERANGLEORDER(2, EvenParity, Repeats, StaticFrame),
		ZYXstatic = SDPEULERANGLEORDER(2, OddParity, NoRepetition, StaticFrame),
		ZYZstatic = SDPEULERANGLEORDER(2, OddParity, Repeats, StaticFrame),
		ZYXrotating = SDPEULERANGLEORDER(0, EvenParity, NoRepetition, RotatingFrame),
		XYXrotating = SDPEULERANGLEORDER(0, EvenParity, Repeats, RotatingFrame),
		YZXrotating = SDPEULERANGLEORDER(0, OddParity, NoRepetition, RotatingFrame),
		XZXrotating = SDPEULERANGLEORDER(0, OddParity, Repeats, RotatingFrame),
		XZYrotating = SDPEULERANGLEORDER(1, EvenParity, NoRepetition, RotatingFrame),
		YZYrotating = SDPEULERANGLEORDER(1, EvenParity, Repeats, RotatingFrame),
		ZXYrotating = SDPEULERANGLEORDER(1, OddParity, NoRepetition, RotatingFrame),
		YXYrotating = SDPEULERANGLEORDER(1, OddParity, Repeats, RotatingFrame),
		YXZrotating = SDPEULERANGLEORDER(2, EvenParity, NoRepetition, RotatingFrame),
		ZXZrotating = SDPEULERANGLEORDER(2, EvenParity, Repeats, RotatingFrame),
		XYZrotating = SDPEULERANGLEORDER(2, OddParity, NoRepetition, RotatingFrame),
		ZYZrotating = SDPEULERANGLEORDER(2, OddParity, Repeats, RotatingFrame),
	} AngleOrder;

	/// Stores the three axis angles
	double n[3];
	/// Stores the axis order in packed format
	AngleOrder order;

	euler_angles();
	euler_angles(point3 Angles, AngleOrder Order);
	euler_angles(double X, double Y, double Z, AngleOrder Order);
	/// Conversion from a quaternion
	euler_angles(const quaternion& Quaternion, AngleOrder Order);
	/// Conversion from a 4x4 matrix
	euler_angles(const matrix4& Matrix, AngleOrder Order);

	/// Returns the frame type from a packed order representation
	static OrderFrame Frame(AngleOrder& Order) { return OrderFrame(Order & 1); }
	/// Returns the repetition type from a packed order representation
	static OrderRepetition Repetition(AngleOrder& Order) { return OrderRepetition((Order >> 1) & 1); }
	/// Returns the parity type from a packed order representation
	static OrderParity Parity(AngleOrder& Order) { return OrderParity((Order >> 2) & 1); }
	/// Returns the axes in order, from a packed order representation
	static void Axes(AngleOrder& Order, int& i, int& j, int& k);

	/// Returns the frame type of this instance
	OrderFrame Frame() { return Frame(order); }
	/// Returns the repetition type of this instance
	OrderRepetition Repetition() { return Repetition(order); }
	/// Returns the parity type of this instance
	OrderParity Parity() { return Parity(order); }
	/// Returns the axes in order, for this instance
	void Axes(int& i, int& j, int& k) { Axes(order, i, j, k); }

	/// Returns the given angle by reference
	double& operator[](int i);
	/// Returns the given angle by value
	double operator[](int i) const;
};

/// Output inserter
std::ostream& operator<<(std::ostream& Stream, const euler_angles& Arg);
/// Input extractor
std::istream& operator>>(std::istream& Stream, euler_angles& Arg);

/// Returns a three-dimensional translation matrix
inline const matrix4 translation3D(const point3& v)
{
	return matrix4(
		point4(1, 0, 0, v[0]),
		point4(0, 1, 0, v[1]),
		point4(0, 0, 1, v[2]),
		point4(0, 0, 0, 1));
}
/// Returns a three-dimensional translation matrix
inline const matrix4 translation3D(const vector3& v)
{
	return matrix4(
		point4(1, 0, 0, v[0]),
		point4(0, 1, 0, v[1]),
		point4(0, 0, 1, v[2]),
		point4(0, 0, 0, 1));
}
/// Returns a three-dimensional rotation matrix about an arbitrary axis
inline const matrix4 rotation3D(const double Angle, vector3 Axis)
{
	const double c = cos(Angle), s = sin(Angle), t = 1.0 - c;

	Axis = normalize(Axis);

	return matrix4(
		point4(t * Axis[0] * Axis[0] + c, t * Axis[0] * Axis[1] - s * Axis[2], t * Axis[0] * Axis[2] + s * Axis[1], 0),
		point4(t * Axis[0] * Axis[1] + s * Axis[2], t * Axis[1] * Axis[1] + c, t * Axis[1] * Axis[2] - s * Axis[0], 0),
		point4(t * Axis[0] * Axis[2] - s * Axis[1], t * Axis[1] * Axis[2] + s * Axis[0], t * Axis[2] * Axis[2] + c, 0),
		point4(0, 0, 0, 1));
}
/// Returns a three-dimensional rotation matrix about an arbitrary axis
inline const matrix4 rotation3D(const angle_axis& AngleAxis)
{
	return rotation3D(AngleAxis.angle, AngleAxis.axis);
}
/// Returns a three-dimensional rotation matrix based on a set of Euler angles
inline const matrix4 rotation3D(const point3& EulerAngles)
{
	matrix4 matrix = identity3D();
	matrix = matrix * rotation3D(EulerAngles[1], vector3(0, 1, 0));
	matrix = matrix * rotation3D(EulerAngles[0], vector3(1, 0, 0));
	matrix = matrix * rotation3D(EulerAngles[2], vector3(0, 0, 1));

	return matrix;
}
/// Returns a three-dimensional rotation matrix based on a quaternion
inline const matrix4 rotation3D(const quaternion& Quaternion)
{
	return rotation3D(angle_axis(Quaternion));
}
/// Returns a quaternion based on a three-dimensional rotation matrix
inline const quaternion rotation3D(const matrix4& m)
{
	double d0 = m[0][0];
	double d1 = m[1][1];
	double d2 = m[2][2];

	double trace = d0 + d1 + d2 + 1;
	if(trace > 0.0)
	{
		double s = 0.5 / std::sqrt(trace);
		return quaternion(0.25 / s, s * point3(m[2][1] - m[1][2], m[0][2] - m[2][0], m[1][0] - m[0][1]));
	}

	// Identify the major diagonal element with greatest value
	if(d0 >= d1 && d0 >= d2)
	{
		double s = std::sqrt(1.0 + d0 - d1 - d2) * 2.0;
		return quaternion(m[1][2] - m[2][1], point3(0.5, m[0][1] - m[1][0], m[0][2] - m[2][0])) / s;
	}
	else if(d1 >= d0 && d1 >= d2)
	{
		double s = std::sqrt(1.0 + d1 - d0 - d2) * 2.0;
		return quaternion(m[0][2] - m[2][0], point3(m[0][1] - m[1][0], 0.5, m[1][2] - m[2][1])) / s;
	}
	else
	{
		double s = std::sqrt(1.0 + d2 - d0 - d1) * 2.0;
		return quaternion(m[0][1] - m[1][0], point3(m[0][2] - m[2][0], m[1][2] - m[2][1], 0.5)) / s;
	}
}

/// Returns a three-dimensional scaling matrix
inline const matrix4 scaling3D(const point3& v)
{
	return matrix4(
		point4(v[0], 0, 0, 0),
		point4(0, v[1], 0, 0),
		point4(0, 0, v[2], 0),
		point4(0, 0, 0, 1));
}
/// Returns a three-dimensional perspective matrix
inline const matrix4 perspective3D(const double d)
{
	return matrix4(
		point4(1, 0, 0, 0),
		point4(0, 1, 0, 0),
		point4(0, 0, 1, 0),
		point4(0, 0, 1/d, 0));
}
/// Returns a three-dimensional shearing matrix
inline const matrix4 shearing3D(const double XY, const double XZ, const double YX, const double YZ, const double ZX, const double ZY)
{
	return matrix4(
		point4(1, XY, XZ, 0),
		point4(YX, 1, YZ, 0),
		point4(ZX, ZY, 1, 0),
		point4(0, 0, 0, 1));
}

/// Extract a "look" vector from a view matrix
inline const vector3 look_vector(const matrix4& Matrix)
{
	return normalize(Matrix * vector3(0, 0, 1));
}

/// Extract an "up" vector from a view matrix
inline const vector3 up_vector(const matrix4& Matrix)
{
	return normalize(Matrix * vector3(0, 1, 0));
}

/// Extract a "right" vector from a view matrix
inline const vector3 right_vector(const matrix4& Matrix)
{
	return normalize(Matrix * vector3(1, 0, 0));
}

/// Extract position from a view matrix
inline const point3 position(const matrix4& Matrix)
{
	return point3(Matrix[0][3], Matrix[1][3], Matrix[2][3]);
}

/// Construct a view matrix from look, up, right, and position data
inline const matrix4 view_matrix(const vector3& Look, const vector3& Up, const point3& Position)
{
	const vector3 look = normalize(Look);
	const vector3 right = normalize(Up ^ look);
	const vector3 up = normalize(look ^ right);

	return matrix4(
		point4(right[0], up[0], look[0], Position[0]),
		point4(right[1], up[1], look[1], Position[1]),
		point4(right[2], up[2], look[2], Position[2]),
		point4(0, 0, 0, 1));
}

//	Matrix utilities for affine matrices.
//
//	The input matrix must be affine, but need not be orthogonal.
//	In other words, it may contain scaling, rotation, and translation,
//	but not perspective projection.
//	Ref. Foley and Van Dam, 2nd ed, p. 217.
//
/// Extracts translation from a three-dimensional matrix
inline const vector3 extract_translation(const matrix4& m)
{
	return(vector3(m[0][3], m[1][3], m[2][3]));
}
/// Extracts scaling from a matrix
inline const matrix4 extract_scaling(const matrix4& m)
{
	return matrix4(
		point4(sqrt(m[0][0] * m[0][0] + m[1][0] * m[1][0] + m[2][0] * m[2][0]), 0, 0, 0),
		point4(0, sqrt(m[0][1] * m[0][1] + m[1][1] * m[1][1] + m[2][1] * m[2][1]), 0, 0),
		point4(0, 0, sqrt(m[0][2] * m[0][2] + m[1][2] * m[1][2] + m[2][2] * m[2][2]), 0),
		point4(0, 0, 0, 1));
}
/// Extracts rotation from a matrix
inline const matrix4 extract_rotation(const matrix4& m)
{
	// Get scaling ...
	const double scale_x = sqrt(m[0][0] * m[0][0] + m[1][0] * m[1][0] + m[2][0] * m[2][0]);
	const double scale_y = sqrt(m[0][1] * m[0][1] + m[1][1] * m[1][1] + m[2][1] * m[2][1]);
	const double scale_z = sqrt(m[0][2] * m[0][2] + m[1][2] * m[1][2] + m[2][2] * m[2][2]);
	return_val_if_fail(scale_x && scale_y && scale_z, identity3D());

	// Compute the inverse of scaling ...
	point3 invscale(1.0 / scale_x, 1.0 / scale_y, 1.0 / scale_z);

	// Apply inverse of scaling as a transformation, to get unit scale ...
	matrix4 unscaled(m * scaling3D(invscale));

	return matrix4(
		point4(unscaled[0][0], unscaled[0][1], unscaled[0][2], 0),
		point4(unscaled[1][0], unscaled[1][1], unscaled[1][2], 0),
		point4(unscaled[2][0], unscaled[2][1], unscaled[2][2], 0),
		point4(0, 0, 0, 1));
}

/////////////////////////////////////////////////////////////////////////////
// point3 implementation

inline point3 operator*(const matrix4& a, const point3& v)
{
	const point4 result = a * point4(v, 1);
	return point3(result[0] / result[3], result[1] / result[3], result[2] / result[3]);
}

inline point3 operator*(const point3& v, const matrix4& a)
{
	return transpose(a) * v;
}

/////////////////////////////////////////////////////////////////////////////
// point4 implementation

inline point4 operator*(const matrix4& a, const point4& v)
{
#define ROWCOL(i) a.v[i].n[0]*v.n[0] + a.v[i].n[1]*v.n[1] + a.v[i].n[2]*v.n[2] + a.v[i].n[3]*v.n[3]
	return point4(ROWCOL(0), ROWCOL(1), ROWCOL(2), ROWCOL(3));
#undef ROWCOL // (i)
}

inline point4 operator*(const point4& v, const matrix4& a)
{
	return transpose(a) * v;
}

/////////////////////////////////////////////////////////////////////////////
// matrix4 implementation

inline matrix4::matrix4() {}

inline matrix4::matrix4(const point4& v0, const point4& v1, const point4& v2, const point4& v3)
{ v[0] = v0; v[1] = v1; v[2] = v2; v[3] = v3; }

inline matrix4::matrix4(const double d)
{ v[0] = v[1] = v[2] = v[3] = point4(d, d, d, d); }

inline matrix4::matrix4(const matrix4& m)
{ v[0] = m.v[0]; v[1] = m.v[1]; v[2] = m.v[2]; v[3] = m.v[3]; }

inline matrix4::matrix4(euler_angles Angles)
{
	const euler_angles::OrderFrame frame = Angles.Frame();
	const euler_angles::OrderParity parity = Angles.Parity();
	const euler_angles::OrderRepetition repetition = Angles.Repetition();
	int i, j, k;
	Angles.Axes(i, j, k);

	if(frame == euler_angles::RotatingFrame)
		std::swap(Angles.n[0], Angles.n[2]);

	if(parity == euler_angles::OddParity)
	{
		Angles.n[0] = -Angles.n[0];
		Angles.n[1] = -Angles.n[1];
		Angles.n[2] = -Angles.n[2];
	}

	const double ti = Angles.n[0], tj = Angles.n[1], th = Angles.n[2];
	const double ci = cos(ti), cj = cos(tj), ch = cos(th);
	const double si = sin(ti), sj = sin(tj), sh = sin(th);
	const double cc = ci*ch, cs = ci*sh;
	const double sc = si*ch, ss = si*sh;

	matrix4 m = identity3D();
	if(repetition == euler_angles::Repeats)
	{
		m[i][i] =  cj;		m[i][j] =  sj*si;	m[i][k] =  sj*ci;
		m[j][i] =  sj*sh;	m[j][j] = -cj*ss+cc;	m[j][k] = -cj*cs-sc;
		m[k][i] = -sj*ch;	m[k][j] =  cj*sc+cs;	m[k][k] =  cj*cc-ss;
	}
	else
	{
		m[i][i] =  cj*ch;	m[i][j] = sj*sc-cs;	m[i][k] = sj*cc+ss;
		m[j][i] =  cj*sh;	m[j][j] = sj*ss+cc;	m[j][k] = sj*cs-sc;
		m[k][i] = -sj;		m[k][j] = cj*si;	m[k][k] = cj*ci;
	}

	*this = m;
}

inline matrix4& matrix4::operator=(const matrix4& m)
{ v[0] = m.v[0]; v[1] = m.v[1]; v[2] = m.v[2]; v[3] = m.v[3];
return *this; }

inline matrix4& matrix4::operator=(const double d[4][4])
{ v[0] = d[0]; v[1] = d[1]; v[2] = d[2]; v[3] = d[3]; return *this; }

inline matrix4& matrix4::operator=(const double d[16])
{ memcpy(&v[0][0], &d[0], 16 * sizeof(double)); return *this; }

inline matrix4& matrix4::operator+=(const matrix4& m)
{ v[0] += m.v[0]; v[1] += m.v[1]; v[2] += m.v[2]; v[3] += m.v[3];
return *this; }

inline matrix4& matrix4::operator-=(const matrix4& m)
{ v[0] -= m.v[0]; v[1] -= m.v[1]; v[2] -= m.v[2]; v[3] -= m.v[3];
return *this; }

inline matrix4& matrix4::operator*=(const double d)
{ v[0] *= d; v[1] *= d; v[2] *= d; v[3] *= d; return *this; }

inline matrix4& matrix4::operator/=(const double d)
{
	return_val_if_fail(d, *this);

	const double inv = 1./d;
	v[0] *= inv;
	v[1] *= inv;
	v[2] *= inv;
	v[3] *= inv;
	return *this;
}

inline point4& matrix4::operator[](int i)
{
	return_val_if_fail((i >= 0 && i <= 3), v[0]);
	return v[i];
}

inline const point4& matrix4::operator[](int i) const
{
	return_val_if_fail((i >= 0 && i <= 3), v[0]);
	return v[i];
}

inline void matrix4::CopyArray(float m[16]) const
{
	unsigned long index = 0;
	for(unsigned long i = 0; i < 4; ++i)
		for(unsigned long j = 0; j < 4; ++j)
			m[index++] = float(v[i][j]);
}

inline void matrix4::CopyArray(double m[16]) const
{
	unsigned long index = 0;
	for(unsigned long i = 0; i < 4; ++i)
		for(unsigned long j = 0; j < 4; ++j)
			m[index++] = v[i][j];
}

inline matrix4 operator-(const matrix4& a)
{ return matrix4(-a.v[0], -a.v[1], -a.v[2], -a.v[3]); }

inline matrix4 operator+(const matrix4& a, const matrix4& b)
{ return matrix4(a.v[0] + b.v[0], a.v[1] + b.v[1], a.v[2] + b.v[2], a.v[3] + b.v[3]); }

inline matrix4 operator-(const matrix4& a, const matrix4& b)
{ return matrix4(a.v[0] - b.v[0], a.v[1] - b.v[1], a.v[2] - b.v[2], a.v[3] - b.v[3]); }

inline matrix4 operator*(const matrix4& a, const matrix4& b)
{
#define ROWCOL(i, j) a.v[i].n[0]*b.v[0][j] + a.v[i].n[1]*b.v[1][j] + a.v[i].n[2]*b.v[2][j] + a.v[i].n[3]*b.v[3][j]
	return matrix4(
		point4(ROWCOL(0,0), ROWCOL(0,1), ROWCOL(0,2), ROWCOL(0,3)),
		point4(ROWCOL(1,0), ROWCOL(1,1), ROWCOL(1,2), ROWCOL(1,3)),
		point4(ROWCOL(2,0), ROWCOL(2,1), ROWCOL(2,2), ROWCOL(2,3)),
		point4(ROWCOL(3,0), ROWCOL(3,1), ROWCOL(3,2), ROWCOL(3,3)));
}

inline matrix4 operator*(const matrix4& a, const double d)
{ return matrix4(a.v[0] * d, a.v[1] * d, a.v[2] * d, a.v[3] * d); }

inline matrix4 operator*(const double d, const matrix4& a)
{ return a*d; }

inline matrix4 operator/(const matrix4& a, const double d)
{
	return_val_if_fail(d, matrix4());

	const double inv = 1./d;
	return matrix4(a.v[0] * inv, a.v[1] * inv, a.v[2] * inv, a.v[3] * inv);
}

inline bool operator==(const matrix4& a, const matrix4& b)
{ return ((a.v[0] == b.v[0]) && (a.v[1] == b.v[1]) && (a.v[2] == b.v[2]) && (a.v[3] == b.v[3])); }

inline bool operator!=(const matrix4& a, const matrix4& b)
{ return !(a == b); }

inline std::ostream& operator<<(std::ostream& Stream, const matrix4& Arg)
{
	Stream << Arg[0] << " " << Arg[1] << " " << Arg[2] << " " << Arg[3];
	return Stream;
}

inline std::istream& operator>>(std::istream& Stream, matrix4& Arg)
{
	Stream >> Arg[0] >> Arg[1] >> Arg[2] >> Arg[3];
	return Stream;
}

/////////////////////////////////////////////////////////////////////////////
// quaternion implementation

inline quaternion::quaternion()
{ w = 1.0; v = vector3(0, 0, 0); }

inline quaternion::quaternion(const double u, const point3& t)
{ w = u; v = to_vector(t); }

inline quaternion::quaternion(const double u, const vector3& t)
{ w = u; v = t; }

inline quaternion::quaternion(const double u, const double x, const double y, const double z)
{ w = u; v = vector3(x, y, z); }

inline quaternion::quaternion(const angle_axis& AngleAxis)
{ w = cos(AngleAxis.angle * 0.5); vector3 axis = k3d::normalize(AngleAxis.axis); v = sin(AngleAxis.angle * 0.5) * axis; }

inline quaternion::quaternion(euler_angles Angles)
{
	const euler_angles::OrderFrame frame = Angles.Frame();
	const euler_angles::OrderParity parity = Angles.Parity();
	const euler_angles::OrderRepetition repetition = Angles.Repetition();
	int i, j, k;
	Angles.Axes(i, j, k);

	if(frame == euler_angles::RotatingFrame)
		std::swap(Angles.n[0], Angles.n[2]);

	if(parity == euler_angles::OddParity)
		Angles.n[1] = -Angles.n[1];

	const double ti = Angles.n[0]*0.5, tj = Angles.n[1]*0.5, th = Angles.n[2]*0.5;
	const double ci = cos(ti), cj = cos(tj), ch = cos(th);
	const double si = sin(ti), sj = sin(tj), sh = sin(th);
	const double cc = ci*ch, cs = ci*sh;
	const double sc = si*ch, ss = si*sh;

	if(repetition == euler_angles::Repeats)
	{
		v[i] = cj*(cs + sc);
		v[j] = sj*(cc + ss);
		v[k] = sj*(cs - sc);
		w = cj*(cc - ss);
	}
	else
	{
		v[i] = cj*sc - sj*cs;
		v[j] = cj*ss + sj*cc;
		v[k] = cj*cs - sj*sc;
		w = cj*cc + sj*ss;
	}

	if(parity == euler_angles::OddParity)
		v[j] = -v[j];
}

inline quaternion& quaternion::operator=(const quaternion& q)
{ w = q.w; v = q.v; return *this; }

inline quaternion& quaternion::operator=(const angle_axis& AngleAxis)
{ w = cos(AngleAxis.angle * 0.5); vector3 axis = k3d::normalize(AngleAxis.axis); v = sin(AngleAxis.angle * 0.5) * axis; return *this; }

inline quaternion& quaternion::operator*=(const double d)
{ w *= d; v *= d; return *this; }

inline quaternion& quaternion::operator/=(const double d)
{
	return_val_if_fail(d, *this);

	const double inv = 1./d;
	w *= inv;
	v *= inv;
	return *this;
}

inline quaternion& quaternion::operator+=(const quaternion& q)
{ w += q.w; v += q.v; return *this; }

inline quaternion& quaternion::operator-=(const quaternion& q)
{ w -= q.w; v -= q.v; return *this; }

inline quaternion& quaternion::operator*=(const quaternion& q)
{ w = w*q.w - (v*q.v); v = q.w*v + w*q.v + (v^q.v); return *this; }

inline quaternion& quaternion::operator/=(const quaternion& q)
{ quaternion t = q; (*this) *= t.Inverse(); return *this; }

inline double quaternion::Magnitude() const
{ return std::sqrt(w*w + v.length2()); }

inline quaternion& quaternion::Normalize()
{
	if(const double magnitude = Magnitude())
		*this /= magnitude;

	return *this;
}

inline quaternion& quaternion::Conjugate()
{ v = -v; return *this; }

inline quaternion& quaternion::Inverse()
{ if(Magnitude() != 0.0) { const double i = 1.0 / Magnitude(); w *= i; v *= -i; } return *this; }

inline quaternion& quaternion::Square()
{ const double t = 2*w; w = w*w - v.length2(); v *= t; return *this; }

inline quaternion& quaternion::AxisRotation(const double phi)
{ v = k3d::normalize(v); v *= sin(phi/2.0); w = cos(phi/2); return *this; }

inline quaternion operator-(const quaternion& q)
{ return quaternion(-q.w,-q.v); }

inline quaternion operator+(const quaternion& q, const quaternion& r)
{ return quaternion(q.w + r.w, q.v + r.v); }

inline quaternion operator-(const quaternion& q, const quaternion& r)
{ return quaternion(q.w - r.w, q.v - r.v); }

inline quaternion operator*(const quaternion& q, const quaternion& r)
{ return quaternion(q.w*r.w - (q.v*r.v), q.w*r.v + r.w*q.v + (q.v^r.v)); }

inline quaternion operator*(const quaternion& q, const double d)
{ return quaternion(q.w*d, q.v*d); }

inline quaternion operator*(const double d, const quaternion& q)
{ return quaternion(q.w*d, q.v*d); }

inline quaternion operator/(const quaternion& q, const quaternion& r)
{ quaternion t = r; return q*t.Inverse(); }

inline quaternion operator/(const quaternion& q, const double d)
{
	return_val_if_fail(d, quaternion());

	const double inv = 1./d;
	return quaternion(q.w * inv, q.v * inv);
}

inline bool operator==(const quaternion& q, const quaternion& r)
{ return ((q.w == r.w) && (q.v == r.v)); }

inline bool operator!=(const quaternion& q, const quaternion& r)
{ return !(q == r); }

inline std::ostream& operator<<(std::ostream& Stream, const quaternion& Arg)
{
	Stream << Arg.w << " " << Arg.v[0] << " " << Arg.v[1] << " " << Arg.v[2];
	return Stream;
}

inline std::istream& operator>>(std::istream& Stream, quaternion& Arg)
{
	Stream >> Arg.w >> Arg.v[0] >> Arg.v[1] >> Arg.v[2];
	return Stream;
}

/////////////////////////////////////////////////////////////////////////////
// angle_axis implementation ...

inline angle_axis::angle_axis()
{ angle = 0.0; axis = vector3(0, 0, 1); }

inline angle_axis::angle_axis(const double Angle, const point3& Axis)
{ angle = Angle; axis = normalize(to_vector(Axis)); }

inline angle_axis::angle_axis(const double Angle, const vector3& Axis)
{ angle = Angle; axis = normalize(Axis); }

inline angle_axis::angle_axis(const double Angle, const double X, const double Y, const double Z)
{ angle = Angle; axis[0] = X; axis[1] = Y; axis[2] = Z; axis = normalize(axis); }

inline angle_axis::angle_axis(const double AngleAxis[4])
{ angle = AngleAxis[0]; axis[0] = AngleAxis[1]; axis[1] = AngleAxis[2]; axis[2] = AngleAxis[3]; axis = normalize(axis); }

inline angle_axis::angle_axis(const quaternion& Quaternion)
{
	quaternion q = Quaternion;
	q.Normalize();
	const double halftheta = acos(q.w);
	const double sinhalftheta = sin(halftheta);

	angle = 2.0 * halftheta;
	if(sinhalftheta != 0.0)
		axis = q.v / sinhalftheta;
	else
		axis = vector3(0, 0, 1);
}

inline angle_axis::angle_axis(const angle_axis& AngleAxis)
{ angle = AngleAxis.angle; axis = AngleAxis.axis; }

inline angle_axis& angle_axis::operator=(const angle_axis& AngleAxis)
{ angle = AngleAxis.angle; axis = AngleAxis.axis; return *this; }

inline angle_axis& angle_axis::operator=(const quaternion& Quaternion)
{
	const double halftheta = acos(Quaternion.w);
	const double sinhalftheta = sin(halftheta);

	angle = 2.0 * halftheta;
	if(sinhalftheta != 0.0)
		axis = Quaternion.v / sinhalftheta;
	else
		axis = vector3(0, 0, 1);

	return *this;
}

inline angle_axis& angle_axis::operator=(const double AngleAxis[4])
{ angle = AngleAxis[0]; axis[0] = AngleAxis[1]; axis[1] = AngleAxis[2]; axis[2] = AngleAxis[3]; return *this; }

inline void angle_axis::CopyArray(float AngleAxis[4]) const
{	AngleAxis[0] = float(angle); AngleAxis[1] = float(axis[0]); AngleAxis[2] = float(axis[1]); AngleAxis[3] = float(axis[2]); }

inline void angle_axis::CopyArray(double AngleAxis[4]) const
{	AngleAxis[0] = angle; AngleAxis[1] = axis[0]; AngleAxis[2] = axis[1]; AngleAxis[3] = axis[2]; }

inline bool operator==(const angle_axis& a, const angle_axis& b)
{	return ((a.angle == b.angle) && (a.axis == b.axis)); }
inline bool operator!=(const angle_axis& a, const angle_axis& b)
{	return !(a == b); }

inline std::ostream& operator<<(std::ostream& Stream, const angle_axis& Arg)
{
	Stream << Arg.angle << " " << Arg.axis[0] << " " << Arg.axis[1] << " " << Arg.axis[2];
	return Stream;
}

inline std::istream& operator>>(std::istream& Stream, angle_axis& Arg)
{
	Stream >> Arg.angle >> Arg.axis[0] >> Arg.axis[1] >> Arg.axis[2];
	return Stream;
}

//	Unit quaternion utilities.
//
//	Those special functions deal with rotations, thus require unit quaternions.
//

//	Euler/Matrix/Quaternion conversion functions are based on Ken Shoemake code,
//	from Graphic Gems IV

/// Returns the spherical linear interpolation between two quaternions
inline quaternion Slerp(const quaternion& q1, const quaternion& q2, double t)
{
	// Calculate the angle between the two quaternions ...
	double cosangle = q1.w * q2.w + q1.v * q2.v;

	if(cosangle < -1.0)
		cosangle = -1.0;
	if(cosangle > 1.0)
		cosangle = 1.0;

	// If the angle is reasonably large, do the spherical interpolation
	if(1.0 - cosangle > 16 * FLT_EPSILON)
	{
		const double angle = acos(cosangle);
		return (sin((1.0 - t)*angle)*q1 + sin(t*angle)*q2) / sin(angle);
	}

	// The angle is too close to zero - do a linear interpolation
	return mix(q1, q2, t);
}

/////////////////////////////////////////////////////////////////////////////
// euler_angles implementation

inline euler_angles::euler_angles()
{
	n[0] = n[1] = n[2] = 0.0;
	order = YXZstatic;
}

inline euler_angles::euler_angles(point3 Angles, AngleOrder Order)
{
	n[0] = Angles[0];
	n[1] = Angles[1];
	n[2] = Angles[2];
	order = Order;
}

inline euler_angles::euler_angles(double X, double Y, double Z, AngleOrder Order)
{
	n[0] = X;
	n[1] = Y;
	n[2] = Z;
	order = Order;
}

inline euler_angles::euler_angles(const quaternion& Quaternion, AngleOrder Order)
{
	const double NQuaternion = Quaternion.Magnitude();
	const double s = (NQuaternion > 0.0) ? (2.0 / NQuaternion) : 0.0;

	const double xs = Quaternion.v[0]*s, ys = Quaternion.v[1]*s, zs = Quaternion.v[2]*s;
	const double wx = Quaternion.w*xs, wy = Quaternion.w*ys, wz = Quaternion.w*zs;
	const double xx = Quaternion.v[0]*xs, xy = Quaternion.v[0]*ys, xz = Quaternion.v[0]*zs;
	const double yy = Quaternion.v[1]*ys, yz = Quaternion.v[1]*zs, zz = Quaternion.v[2]*zs;

	matrix4 matrix = identity3D();
	matrix[0][0] = 1.0 - (yy + zz);
	matrix[0][1] = xy - wz;
	matrix[0][2] = xz + wy;
	matrix[1][0] = xy + wz;
	matrix[1][1] = 1.0 - (xx + zz);
	matrix[1][2] = yz - wx;
	matrix[2][0] = xz - wy;
	matrix[2][1] = yz + wx;
	matrix[2][2] = 1.0 - (xx + yy);

	*this = euler_angles(matrix, Order);
}

inline euler_angles::euler_angles(const matrix4& Matrix, AngleOrder Order)
{
	OrderRepetition repetition = Repetition(Order);
	OrderParity parity = Parity(Order);
	OrderFrame frame = Frame(Order);
	int i, j, k;
	Axes(Order, i, j, k);

	if(repetition == euler_angles::Repeats)
	{
		const double sy = sqrt(Matrix[i][j]*Matrix[i][j] + Matrix[i][k]*Matrix[i][k]);
		if(sy > 16*FLT_EPSILON)
		{
			n[0] = atan2(Matrix[i][j], Matrix[i][k]);
			n[1] = atan2(sy, Matrix[i][i]);
			n[2] = atan2(Matrix[j][i], -Matrix[k][i]);
		}
		else
		{
			n[0] = atan2(-Matrix[j][k], Matrix[j][j]);
			n[1] = atan2(sy, Matrix[i][i]);
			n[2] = 0;
		}
	}
	else
	{
		const double cy = sqrt(Matrix[i][i]*Matrix[i][i] + Matrix[j][i]*Matrix[j][i]);
		if(cy > 16*FLT_EPSILON)
		{
			n[0] = atan2(Matrix[k][j], Matrix[k][k]);
			n[1] = atan2(-Matrix[k][i], cy);
			n[2] = atan2(Matrix[j][i], Matrix[i][i]);
		}
		else
		{
			n[0] = atan2(-Matrix[j][k], Matrix[j][j]);
			n[1] = atan2(-Matrix[k][i], cy);
			n[2] = 0;
		}
	}

	if(parity == euler_angles::OddParity)
	{
		n[0] = -n[0];
		n[1] = -n[1];
		n[2] = -n[2];
	}

	if(frame == euler_angles::RotatingFrame)
		std::swap(n[0], n[2]);

	order = Order;
}

inline void euler_angles::Axes(AngleOrder& Order, int& i, int& j, int& k)
{
	const int safe[4] = {0, 1, 2, 0};
	const int next[4] = {1, 2, 0, 1};

	i = safe[(Order >> 3) & 3];
	j = next[i + Parity(Order)];
	k = next[i + 1 - Parity(Order)];
}

inline double& euler_angles::operator[](int i)
{
	return_val_if_fail((i >= 0 && i <= 2), n[0]);
	return n[i];
}

inline double euler_angles::operator[](int i) const
{
	return_val_if_fail((i >= 0 && i <= 2), n[0]);
	return n[i];
}

inline std::ostream& operator<<(std::ostream& Stream, const euler_angles& Arg)
{
	Stream << Arg.n[0] << " " << Arg.n[1] << " " << Arg.n[2] << " " << int(Arg.order);
	return Stream;
}

inline std::istream& operator>>(std::istream& Stream, euler_angles& Arg)
{
	int order;
	Stream >> Arg.n[0] >> Arg.n[1] >> Arg.n[2] >> order;

	Arg.order = euler_angles::AngleOrder(order);

	return Stream;
}

} // namespace k3d

#endif // K3DSDK_ALGEBRA_H

