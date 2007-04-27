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
		\brief 3D Quadric Error Metrics
		\author Adapted for K-3D: Romain Behar (romainbehar@yahoo.com)
*/

#include "MxQMetric3.h"

inline bool FEQ(double a,double b,double eps=FEQ_EPS2) { return fabs(a-b)<eps; }

void MxQuadric3::init(double a, double b, double c, double d, double area)
{
	a2 = a*a;  ab = a*b;  ac = a*c;  ad = a*d;
	           b2 = b*b;  bc = b*c;  bd = b*d;
	                  c2 = c*c;  cd = c*d;
			             d2 = d*d;

	r = area;
}

Mat3 MxQuadric3::tensor() const
{
	return Mat3(Vec3(a2, ab, ac),
		Vec3(ab, b2, bc),
		Vec3(ac, bc, c2));
}

void MxQuadric3::set_coefficients(const double *v)
{
	a2 = v[0];  ab = v[1];  ac = v[2];  ad = v[3];
	            b2 = v[4];  bc = v[5];  bd = v[6];
		            c2 = v[7];  cd = v[8];
	                                    d2 = v[9];
}

MxQuadric3& MxQuadric3::operator=(const MxQuadric3& Q)
{
	r = Q.r;

	a2 = Q.a2;  ab = Q.ab;  ac = Q.ac;  ad = Q.ad;
	            b2 = Q.b2;  bc = Q.bc;  bd = Q.bd;
		            c2 = Q.c2;  cd = Q.cd;
	                                    d2 = Q.d2;

	return *this;
}

MxQuadric3& MxQuadric3::operator+=(const MxQuadric3& Q)
{
	// Accumulate area
	r += Q.r;

	// Accumulate coefficients
	a2 += Q.a2;  ab += Q.ab;  ac += Q.ac;  ad += Q.ad;
	             b2 += Q.b2;  bc += Q.bc;  bd += Q.bd;
		              c2 += Q.c2;  cd += Q.cd;
			                   d2 += Q.d2;

	return *this;
}

MxQuadric3& MxQuadric3::operator-=(const MxQuadric3& Q)
{
	r -= Q.r;

	a2 -= Q.a2;  ab -= Q.ab;  ac -= Q.ac;  ad -= Q.ad;
	             b2 -= Q.b2;  bc -= Q.bc;  bd -= Q.bd;
		              c2 -= Q.c2;  cd -= Q.cd;
			                   d2 -= Q.d2;

	return *this;
}

MxQuadric3& MxQuadric3::operator*=(double s)
{
	// Scale coefficients
	a2 *= s;  ab *= s;  ac *= s;  ad *= s;
	          b2 *= s;  bc *= s;  bd *= s;
	                    c2 *= s;  cd *= s;
			          d2 *= s;

	return *this;
}

double MxQuadric3::evaluate(double x, double y, double z) const
{
	// Evaluate vAv + 2bv + c
	return x*x*a2 + 2*x*y*ab + 2*x*z*ac + 2*x*ad
	          + y*y*b2   + 2*y*z*bc + 2*y*bd
	                     + z*z*c2   + 2*z*cd
	                                + d2;
}

bool MxQuadric3::optimize(Vec3& v) const
{
	Mat3 Ainv;
	double det = tensor().invert(Ainv);
	if(FEQ(det, 0.0, 1e-12))
		return false;

	v = -(Ainv*vector());

	return true;
}

bool MxQuadric3::optimize(double *x, double *y, double *z) const
{
	Vec3 v;

	bool success = optimize(v);
	if(success)
	{
		*x = (double)v[X];
		*y = (double)v[Y];
		*z = (double)v[Z];
	}
	return success;
}

bool MxQuadric3::optimize(Vec3& v, const Vec3& v1, const Vec3& v2) const
{
	Vec3 d = v1 - v2;
	Mat3 A = tensor();

	Vec3 Av2 = A*v2;
	Vec3 Ad  = A*d;

	double denom = 2*d*Ad;
	if(FEQ(denom, 0.0, 1e-12))
	return false;

	double a =  (-2*(vector()*d) - (d*Av2) - (v2*Ad)) / (2*(d*Ad));

	if(a<0.0) a=0.0; else if(a>1.0) a=1.0;

	v = a*d + v2;
	return true;
}

bool MxQuadric3::optimize(Vec3& v, const Vec3& v1, const Vec3& v2, const Vec3& v3) const
{
	Vec3 d13 = v1 - v3;
	Vec3 d23 = v2 - v3;
	Mat3 A = tensor();
	Vec3 B = vector();

	Vec3 Ad13 = A*d13;
	Vec3 Ad23 = A*d23;
	Vec3 Av3  = A*v3;

	double d13_d23 = (d13*Ad23) + (d23*Ad13);
	double v3_d13  = (d13*Av3) + (v3*Ad13);
	double v3_d23  = (d23*Av3) + (v3*Ad23);

	double d23Ad23 = d23*Ad23;
	double d13Ad13 = d13*Ad13;

	double denom = d13Ad13*d23Ad23 - 2*d13_d23;
	if(FEQ(denom, 0.0, 1e-12))
	return false;

	double a = (d23Ad23*(2*(B*d13) + v3_d13) -
		  d13_d23*(2*(B*d23) + v3_d23)) / -denom;

	double b = (d13Ad13*(2*(B*d23) + v3_d23) -
		  d13_d23*(2*(B*d13) + v3_d13)) / -denom;

	if(a<0.0) a=0.0; else if(a>1.0) a=1.0;
	if(b<0.0) b=0.0; else if(b>1.0) b=1.0;

	v = a*d13 + b*d23 + v3;
	return true;
}


