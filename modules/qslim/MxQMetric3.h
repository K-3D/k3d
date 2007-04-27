#ifndef MXQMETRIC3_INCLUDED
#define MXQMETRIC3_INCLUDED

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
		\brief 3D Quadric Error Metric
		\author Adapted for K-3D: Romain Behar (romainbehar@yahoo.com)
*/

#include "MxMath.h"

class MxQuadric3
{
private:
	double a2, ab, ac, ad;
	double     b2, bc, bd;
	double         c2, cd;
	double             d2;

	double r;

	void init(double a, double b, double c, double d, double area);

public:
	MxQuadric3() { clear(); }
	MxQuadric3(double a, double b, double c, double d, double area=1.0)
	{ init(a, b, c, d, area); }
	MxQuadric3(const double *n, double d, double area=1.0)
	{ init(n[X], n[Y], n[Z], d, area); }
	MxQuadric3(const MxQuadric3& Q) { *this = Q; }

	Mat3 tensor() const;
	Vec3 vector() const { return Vec3(ad, bd, cd); }
	double offset() const { return d2; }
	double area() const { return r; }

	void set_coefficients(const double *);
	void set_area(double a) { r=a; }

	void clear(double val=0.0) { a2=ab=ac=ad=b2=bc=bd=c2=cd=d2=r=val; }
	MxQuadric3& operator=(const MxQuadric3& Q);
	MxQuadric3& operator+=(const MxQuadric3& Q);
	MxQuadric3& operator-=(const MxQuadric3& Q);
	MxQuadric3& operator*=(double s);

	double evaluate(double x, double y, double z) const;
	double evaluate(const double *v) const {return evaluate(v[X], v[Y], v[Z]);}

	bool optimize(Vec3& v) const;
	bool optimize(double *x, double *y, double *z) const;

	bool optimize(Vec3& v, const Vec3& v1, const Vec3& v2) const;
	bool optimize(Vec3& v,const Vec3& v1,const Vec3& v2,const Vec3& v3) const;
};

#endif // MXQMETRIC3_INCLUDED

