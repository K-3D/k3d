// K-3D
// Copyright (c) 2004-2005, Romain Behar
//
// Contact: romainbehar@yahoo.com
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public
// License as published by the Free Software Foundation; either
// version 2 of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Public License for more details.
//
// You should have received a copy of the GNU General Public
// License along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

/** \file
		\author Romain Behar (romainbehar@yahoo.com)
*/

#include <k3dsdk/color.h>
#include <k3dsdk/document_plugin_factory.h>
#include <k3d-i18n-config.h>
#include <k3dsdk/imaterial.h>
#include <k3dsdk/types_ri.h>
#include <k3dsdk/material.h>
#include <k3dsdk/material_client.h>
#include <k3dsdk/measurement.h>
#include <k3dsdk/legacy_mesh_source.h>
#include <k3dsdk/node.h>
#include <k3dsdk/persistent.h>

namespace k3d
{

inline bool operator < (const k3d::point3& a, const k3d::point3& b)
{
	if(a[0] < b[0])
		return true;
	if(a[0] > b[0])
		return false;
	if(a[1] < b[1])
		return true;
	if(a[1] > b[1])
		return false;
	if(a[2] < b[2])
		return true;

	return false;
}

} // namespace k3d

namespace libk3dprimitives
{

namespace detail
{

// This file is part of a program called sphereEversion:
// http://www.dgp.utoronto.ca/~mjmcguff/eversion/
//
// sphereEversion was created by Michael McGuffin.
// The code in this file was almost entirely taken
// (with slight adaptations) from the source code of
// a program called "evert", which was written by
// Nathaniel Thurston.
// evert's source code can be down loaded from
// http://www.geom.umn.edu/docs/outreach/oi/software.html
//
// Michael McGuffin extends many thanks to Nathaniel Thurston,
// Silvio Levy, and the Geometry Center (University of Minnesota)
// for making evert's source code freely available to the public.

// Axis convention: right handed system
class eversion_point
{
public:
	// Location (x, y, z) and normal vector(nx, ny, nz)
	double vertex[3];
	double normal[3];
};

typedef std::vector<eversion_point> geometry_vector_t;
typedef std::vector<geometry_vector_t> geometry_matrix_t;


class TwoJet D(const class ThreeJet x, int index);
class TwoJet
{
public:
	double f;
	double fu, fv;
	double fuv;

	TwoJet() {}
	TwoJet(double d, double du, double dv)
	{ f = d; fu = du; fv = dv; fuv = 0; }
	TwoJet(double d, double du, double dv, double duv)
	{ f = d; fu = du; fv = dv; fuv = duv; }

	bool operator < (double d) { return f < d; }
	bool operator > (double d) { return f > d; }
	bool operator <= (double d) { return f <= d; }
	bool operator >= (double d) { return f >= d; }
	double df_du() { return fu; }
	double df_dv() { return fv; }
	double d2f_dudv() { return fuv; }
	void operator += (TwoJet x)
	{ f += x.f; fu += x.fu; fv += x.fv; fuv += x.fuv; }
	void operator += (double d)
	{ f += d; }
	void operator *= (TwoJet x)
	{
		fuv = f*x.fuv + fu*x.fv + fv*x.fu + fuv*x.f;
		fu = f*x.fu + fu*x.f;
		fv = f*x.fv + fv*x.f;
		f *= x.f;
	}
	void operator *=(double d)
	{ f *= d; fu *= d; fv *= d; fuv *= d; }
	void operator %=(double d)
	{ f = fmod(f, d); if(f < 0) f += d; }
	void operator ^=(double n)
	{
		if(f > 0)
		{
			double x0 = pow(f, n);
			double x1 = n * x0/f;
			double x2 = (n-1)*x1/f;
			fuv = x1*fuv + x2*fu*fv;
			fu = x1*fu;
			fv = x1*fv;
			f = x0;
		}
	}
	void Annihilate(int index)
	{
		if(index == 0)
			fu = 0;
		else if(index == 1)
			fv = 0;

		fuv = 0;
	}
	void TakeSin()
	{
		*this *= 2*k3d::pi();
		double s = sin(f), c = cos(f);
		f = s; fu = fu*c; fv = fv*c; fuv = c*fuv - s*fu*fv;
	}
	void TakeCos()
	{
		*this *= 2*k3d::pi();
		double s = cos(f), c = -sin(f);
		f = s; fu = fu*c; fv = fv*c; fuv = c*fuv - s*fu*fv;
	}

	friend TwoJet operator+(const TwoJet x, const TwoJet y);
	friend TwoJet operator*(const TwoJet x, const TwoJet y);
	friend TwoJet operator+(const TwoJet x, double d);
	friend TwoJet operator*(const TwoJet x, double d);
	friend TwoJet Sin(const TwoJet x);
	friend TwoJet Cos(const TwoJet x);
	friend TwoJet operator^(const TwoJet x, double n);
	friend TwoJet Annihilate(const TwoJet x, int index);
	friend TwoJet Interpolate(const TwoJet v1, const TwoJet v2, const TwoJet weight);
	friend class TwoJet D(const class ThreeJet x, int index);
	friend class ThreeJet;
};

// ----------------------------------------

TwoJet operator+(const TwoJet x, const TwoJet y)
{ return TwoJet(x.f+y.f, x.fu+y.fu, x.fv+y.fv, x.fuv + y.fuv); }

TwoJet operator*(const TwoJet x, const TwoJet y)
{
	return TwoJet(
		x.f*y.f,
		x.f*y.fu + x.fu*y.f,
		x.f*y.fv + x.fv*y.f,
		x.f*y.fuv + x.fu*y.fv + x.fv*y.fu + x.fuv*y.f
	);
}

TwoJet operator+(const TwoJet x, double d)
{ return TwoJet(x.f + d, x.fu, x.fv, x.fuv); }

TwoJet operator*(const TwoJet x, double d)
{ return TwoJet(d*x.f, d*x.fu, d*x.fv, d*x.fuv); }

TwoJet Sin(const TwoJet x)
{
	TwoJet t = x*(2*k3d::pi());
	double s = sin(t.f);
	double c = cos(t.f);
	return TwoJet(s, c*t.fu, c*t.fv, c*t.fuv - s*t.fu*t.fv);
}

TwoJet Cos(const TwoJet x)
{
	TwoJet t = x*(2*k3d::pi());
	double s = cos(t.f);
	double c = -sin(t.f);
	return TwoJet(s, c*t.fu, c*t.fv, c*t.fuv - s*t.fu*t.fv);
}

TwoJet operator^(const TwoJet x, double n)
{
	double x0 = pow(x.f, n);
	double x1 = (x.f == 0) ? 0 : n * x0/x.f;
	double x2 = (x.f == 0) ? 0 : (n-1)*x1/x.f;
	return TwoJet(x0, x1*x.fu, x1*x.fv, x1*x.fuv + x2*x.fu*x.fv);
}

TwoJet Annihilate(const TwoJet x, int index)
{ return TwoJet(x.f, index == 1 ? x.fu : 0, index == 0 ? x.fv : 0, 0); }

TwoJet Interpolate(const TwoJet v1, const TwoJet v2, const TwoJet weight)
{ return (v1) * ((weight) * (-1) + 1) + v2*weight; }

// ----------------------------------------

class ThreeJet
{
public:
	double f;

private:
	double fu, fv;
	double fuu, fuv, fvv;
	double fuuv, fuvv;

	ThreeJet(double d, double du, double dv, double duu, double duv, double dvv,
		double duuv, double duvv)
	{ f = d; fu = du; fv = dv; fuu = duu; fuv = duv; fvv = dvv;
		fuuv = duuv; fuvv = duvv; }

public:
	ThreeJet() {}
	ThreeJet(double d, double du, double dv)
	{ f = d; fu = du; fv = dv; fuu = fuv = fvv = fuuv = fuvv = 0;}
	operator TwoJet() { return TwoJet(f, fu, fv, fuv); }

	bool operator<(double d) { return f < d; }
	bool operator>(double d) { return f > d; }
	bool operator<=(double d) { return f <= d; }
	bool operator>=(double d) { return f >= d; }
	void operator%=(double d) { f = fmod(f, d); if(f < 0) f += d; }
	friend ThreeJet operator+(const ThreeJet x, const ThreeJet y);
	friend ThreeJet operator*(const ThreeJet x, const ThreeJet y);
	friend ThreeJet operator+(const ThreeJet x, double d);
	friend ThreeJet operator*(const ThreeJet x, double d);
	friend ThreeJet Sin(const ThreeJet x);
	friend ThreeJet Cos(const ThreeJet x);
	friend ThreeJet operator^(const ThreeJet x, double n);
	friend ThreeJet Annihilate(const ThreeJet x, int index);
	friend ThreeJet Interpolate(const ThreeJet v1, const ThreeJet v2, const ThreeJet weight);
	friend class TwoJet D(const class ThreeJet x, int index);
};

// ----------------------------------------

ThreeJet operator+(const ThreeJet x, const ThreeJet y)
{
	ThreeJet result;
	result.f = x.f + y.f;
	result.fu = x.fu + y.fu;
	result.fv = x.fv + y.fv;
	result.fuu = x.fuu + y.fuu;
	result.fuv = x.fuv + y.fuv;
	result.fvv = x.fvv + y.fvv;
	result.fuuv = x.fuuv + y.fuuv;
	result.fuvv = x.fuvv + y.fuvv;

	return result;
}

ThreeJet operator*(const ThreeJet x, const ThreeJet y)
{
	ThreeJet result;
	result.f = x.f*y.f;
	result.fu = x.f*y.fu + x.fu*y.f;
	result.fv = x.f*y.fv + x.fv*y.f;
	result.fuu = x.f*y.fuu + 2*x.fu*y.fu + x.fuu*y.f;
	result.fuv = x.f*y.fuv + x.fu*y.fv + x.fv*y.fu + x.fuv*y.f;
	result.fvv = x.f*y.fvv + 2*x.fv*y.fv + x.fvv*y.f;
	result.fuuv = x.f*y.fuuv + 2*x.fu*y.fuv + x.fv*y.fuu + 2*x.fuv*y.fu + x.fuu*y.fv + x.fuuv*y.f;
	result.fuvv = x.f*y.fuvv + 2*x.fv*y.fuv + x.fu*y.fvv + 2*x.fuv*y.fv + x.fvv*y.fu + x.fuvv*y.f;
	return result;
}

ThreeJet operator+(const ThreeJet x, double d)
{
	ThreeJet result;
	result = x;
	result.f += d;
	return result;
}

ThreeJet operator*(const ThreeJet x, double d)
{
	ThreeJet result;
	result.f = d*x.f;
	result.fu = d*x.fu;
	result.fv = d*x.fv;
	result.fuu = d*x.fuu;
	result.fuv = d*x.fuv;
	result.fvv = d*x.fvv;
	result.fuuv = d*x.fuuv;
	result.fuvv = d*x.fuvv;
	return result;
}

ThreeJet Sin(const ThreeJet x)
{
	ThreeJet result;
	ThreeJet t = x*(2*k3d::pi());
	double s = sin(t.f);
	double c = cos(t.f);
	result.f = s;
	result.fu = c*t.fu;
	result.fv = c*t.fv;
	result.fuu = c*t.fuu - s*t.fu*t.fu;
	result.fuv = c*t.fuv - s*t.fu*t.fv;
	result.fvv = c*t.fvv - s*t.fv*t.fv;
	result.fuuv = c*t.fuuv - s*(2*t.fu*t.fuv + t.fv*t.fuu) - c*t.fu*t.fu*t.fv;
	result.fuvv = c*t.fuvv - s*(2*t.fv*t.fuv + t.fu*t.fvv) - c*t.fu*t.fv*t.fv;
	return result;
}

ThreeJet Cos(const ThreeJet x)
{
	ThreeJet result;
	ThreeJet t = x*(2*k3d::pi());
	double s = cos(t.f);
	double c = -sin(t.f);
	result.f = s;
	result.fu = c*t.fu;
	result.fv = c*t.fv;
	result.fuu = c*t.fuu - s*t.fu*t.fu;
	result.fuv = c*t.fuv - s*t.fu*t.fv;
	result.fvv = c*t.fvv - s*t.fv*t.fv;
	result.fuuv = c*t.fuuv - s*(2*t.fu*t.fuv + t.fv*t.fuu) - c*t.fu*t.fu*t.fv;
	result.fuvv = c*t.fuvv - s*(2*t.fv*t.fuv + t.fu*t.fvv) - c*t.fu*t.fv*t.fv;
	return result;
}

ThreeJet operator^(const ThreeJet x, double n)
{
	double x0 = pow(x.f, n);
	double x1 = (x.f == 0) ? 0 : n * x0/x.f;
	double x2 = (x.f == 0) ? 0 : (n-1) * x1/x.f;
	double x3 = (x.f == 0) ? 0 : (n-2) * x2/x.f;
	ThreeJet result;
	result.f = x0;
	result.fu = x1*x.fu;
	result.fv = x1*x.fv;
	result.fuu = x1*x.fuu + x2*x.fu*x.fu;
	result.fuv = x1*x.fuv + x2*x.fu*x.fv;
	result.fvv = x1*x.fvv + x2*x.fv*x.fv;
	result.fuuv = x1*x.fuuv + x2*(2*x.fu*x.fuv + x.fv*x.fuu) + x3*x.fu*x.fu*x.fv;
	result.fuvv = x1*x.fuvv + x2*(2*x.fv*x.fuv + x.fu*x.fvv) + x3*x.fu*x.fv*x.fv;
	return result;
}

TwoJet D(const ThreeJet x, int index)
{
	TwoJet result;
	if(index == 0)
	{
		result.f = x.fu;
		result.fu = x.fuu;
		result.fv = x.fuv;
		result.fuv = x.fuuv;
	}
	else if(index == 1)
	{
		result.f = x.fv;
		result.fu = x.fuv;
		result.fv = x.fvv;
		result.fuv = x.fuvv;
	}
	else
	{
		result.f = result.fu = result.fv =
		result.fuv = 0;
	}

	return result;
}

ThreeJet Annihilate(const ThreeJet x, int index)
{
	ThreeJet result = ThreeJet(x.f,0,0);
	if(index == 0)
	{
		result.fv = x.fv;
		result.fvv = x.fvv;
	}
	else if(index == 1)
	{
		result.fu = x.fu;
		result.fuu = x.fuu;
	}

	return result;
}

ThreeJet Interpolate(const ThreeJet v1, const ThreeJet v2, const ThreeJet weight)
{ return (v1) * ((weight) * (-1) + 1) + v2*weight; }

// ----------------------------------------

struct TwoJetVec
{
	TwoJet x;
	TwoJet y;
	TwoJet z;
	TwoJetVec() {}
	TwoJetVec(TwoJet a, TwoJet b, TwoJet c) { x = a; y = b; z = c; }
};

TwoJetVec operator+(TwoJetVec v, TwoJetVec w);
TwoJetVec operator*(TwoJetVec v, TwoJet a);
TwoJetVec operator*(TwoJetVec v, double a);
TwoJetVec AnnihilateVec(TwoJetVec v, int index);
TwoJetVec Cross(TwoJetVec v, TwoJetVec w);
TwoJet Dot(TwoJetVec v, TwoJetVec w);
TwoJetVec Normalize(TwoJetVec v);
TwoJetVec RotateZ(TwoJetVec v, TwoJet angle);
TwoJetVec RotateY(TwoJetVec v, TwoJet angle);
TwoJetVec RotateX(TwoJetVec v, TwoJet angle);
TwoJetVec InterpolateVec(TwoJetVec v1, TwoJetVec v2, TwoJet weight);
TwoJet Length(TwoJetVec v);

// ----------------------------------------

TwoJetVec operator+(TwoJetVec v, TwoJetVec w)
{
	TwoJetVec result;
	result.x = v.x + w.x;
	result.y = v.y + w.y;
	result.z = v.z + w.z;
	return result;
}

TwoJetVec operator*(TwoJetVec v, TwoJet a)
{
	TwoJetVec result;
	result.x = v.x*a;
	result.y = v.y*a;
	result.z = v.z*a;
	return result;
}

TwoJetVec operator*(TwoJetVec v, double a)
{
	TwoJetVec result;
	result.x = v.x*a;
	result.y = v.y*a;
	result.z = v.z*a;
	return result;
}

TwoJetVec AnnihilateVec(TwoJetVec v, int index)
{
	TwoJetVec result;
	result.x = Annihilate(v.x, index);
	result.y = Annihilate(v.y, index);
	result.z = Annihilate(v.z, index);
	return result;
}

TwoJetVec Cross(TwoJetVec v, TwoJetVec w)
{
	TwoJetVec result;
	result.x = v.y*w.z + v.z*w.y*-1;
	result.y = v.z*w.x + v.x*w.z*-1;
	result.z = v.x*w.y + v.y*w.x*-1;
	return result;
}

TwoJet Dot(TwoJetVec v, TwoJetVec w)
{ return v.x*w.x + v.y*w.y + v.z*w.z; }

TwoJetVec Normalize(TwoJetVec v)
{
	TwoJet a;
	a = Dot(v,v);
	if(a > 0)
		a = a^-0.5;
	else
		a = TwoJet(0, 0, 0);

	return v*a;
}

TwoJetVec RotateZ(TwoJetVec v, TwoJet angle)
{
	TwoJetVec result;
	TwoJet s,c;
	s = Sin (angle);
	c = Cos (angle);
	result.x = v.x*c + v.y*s;
	result.y = v.x*s*-1 + v.y*c;
	result.z = v.z;
	return result;
}

TwoJetVec RotateY(TwoJetVec v, TwoJet angle)
{
	TwoJetVec result;
	TwoJet s, c;
	s = Sin (angle);
	c = Cos (angle);
	result.x = v.x*c + v.z*s*-1;
	result.y = v.y;
	result.z = v.x*s + v.z*c;
	return result;
}

TwoJetVec RotateX(TwoJetVec v, TwoJet angle)
{
	TwoJetVec result;
	TwoJet s,c;
	s = Sin (angle);
	c = Cos (angle);
	result.x = v.x;
	result.y = v.y*c + v.z*s;
	result.z = v.y*s*-1 + v.z*c;
	return result;
}

TwoJetVec InterpolateVec(TwoJetVec v1, TwoJetVec v2, TwoJet weight)
{
	return (v1) * (weight*-1 + 1) + v2*weight;
}

TwoJet Length(TwoJetVec v)
{
	return (TwoJet(v.x^2) + TwoJet(v.y^2)) ^ (.5);
}

// ----------------------------------------

struct ThreeJetVec
{
	ThreeJet x;
	ThreeJet y;
	ThreeJet z;
	operator TwoJetVec() { return TwoJetVec(x,y,z); }
};

ThreeJetVec operator+(ThreeJetVec v, ThreeJetVec w);
ThreeJetVec operator*(ThreeJetVec v, ThreeJet a);
ThreeJetVec operator*(ThreeJetVec v, double a);
ThreeJetVec AnnihilateVec(ThreeJetVec v, int index);
ThreeJetVec Cross(ThreeJetVec v, ThreeJetVec w);
ThreeJet Dot(ThreeJetVec v, ThreeJetVec w);
TwoJetVec D(ThreeJetVec x, int index);
ThreeJetVec Normalize(ThreeJetVec v);
ThreeJetVec RotateZ(ThreeJetVec v, ThreeJet angle);
ThreeJetVec RotateY(ThreeJetVec v, ThreeJet angle);
ThreeJetVec RotateX(ThreeJetVec v, ThreeJet angle);
ThreeJetVec InterpolateVec(ThreeJetVec v1, ThreeJetVec v2, ThreeJet weight);
ThreeJet Length(ThreeJetVec v);

// ----------------------------------------

ThreeJetVec operator+(ThreeJetVec v, ThreeJetVec w)
{
	ThreeJetVec result;
	result.x = v.x + w.x;
	result.y = v.y + w.y;
	result.z = v.z + w.z;
	return result;
}

ThreeJetVec operator*(ThreeJetVec v, ThreeJet a)
{
	ThreeJetVec result;
	result.x = v.x*a;
	result.y = v.y*a;
	result.z = v.z*a;
	return result;
}

ThreeJetVec operator*(ThreeJetVec v, double a)
{
	ThreeJetVec result;
	result.x = v.x*a;
	result.y = v.y*a;
	result.z = v.z*a;
	return result;
}

ThreeJetVec AnnihilateVec(ThreeJetVec v, int index)
{
	ThreeJetVec result;
	result.x = Annihilate(v.x, index);
	result.y = Annihilate(v.y, index);
	result.z = Annihilate(v.z, index);
	return result;
}

TwoJetVec D(ThreeJetVec x, int index)
{
	TwoJetVec result;
	result.x = D(x.x, index);
	result.y = D(x.y, index);
	result.z = D(x.z, index);
	return result;
}

ThreeJetVec Cross(ThreeJetVec v, ThreeJetVec w)
{
	ThreeJetVec result;
	result.x = v.y*w.z + v.z*w.y*-1;
	result.y = v.z*w.x + v.x*w.z*-1;
	result.z = v.x*w.y + v.y*w.x*-1;
	return result;
}

ThreeJet Dot(ThreeJetVec v, ThreeJetVec w)
{ return v.x*w.x + v.y*w.y + v.z*w.z; }

ThreeJetVec Normalize(ThreeJetVec v)
{
	ThreeJet a;
	a = Dot(v,v);
	if(a > 0)
		a = a^-0.5;
	else
		a = ThreeJet(0, 0, 0);

	return v*a;
}

ThreeJetVec RotateZ(ThreeJetVec v, ThreeJet angle)
{
	ThreeJetVec result;
	ThreeJet s,c;
	s = Sin (angle);
	c = Cos (angle);
	result.x = v.x*c + v.y*s;
	result.y = v.x*s*-1 + v.y*c;
	result.z = v.z;
	return result;
}

ThreeJetVec RotateY(ThreeJetVec v, ThreeJet angle)
{
	ThreeJetVec result;
	ThreeJet s, c;
	s = Sin (angle);
	c = Cos (angle);
	result.x = v.x*c + v.z*s*-1;
	result.y = v.y;
	result.z = v.x*s + v.z*c;
	return result;
}

ThreeJetVec RotateX(ThreeJetVec v, ThreeJet angle)
{
	ThreeJetVec result;
	ThreeJet s,c;
	s = Sin (angle);
	c = Cos (angle);
	result.x = v.x;
	result.y = v.y*c + v.z*s;
	result.z = v.y*s*-1 + v.z*c;
	return result;
}

ThreeJetVec InterpolateVec(ThreeJetVec v1, ThreeJetVec v2, ThreeJet weight)
{
	return (v1) * (weight*-1 + 1) + v2*weight;
}

ThreeJet Length(ThreeJetVec v)
{
	return (ThreeJet(v.x^2) + ThreeJet(v.y^2)) ^ (.5);
}

// ----------------------------------------

TwoJetVec FigureEight(TwoJetVec w, TwoJetVec h, TwoJetVec bend, TwoJet form, TwoJet v)
{
	TwoJet height;
	v %= 1;
	height = (Cos (v*2) + -1) * (-1);
	if(v > 0.25 && v < 0.75)
		height = height*-1 + 4;

	height = height*0.6;
	h = h + bend*(height*height*(1/64.0));

	return w*Sin (v*2) + (h) * (Interpolate((Cos (v) + -1) * (-2), height, form)) ;
}

TwoJetVec AddFigureEight(ThreeJetVec p, ThreeJet u, TwoJet v, ThreeJet form, ThreeJet scale, int numStrips)
{
	ThreeJet size = form * scale;
	form = form*2 + form*form*-1;
	TwoJetVec dv = AnnihilateVec(D(p, 1), 1);
	p = AnnihilateVec(p, 1);
	TwoJetVec du = Normalize(D(p, 0));
	TwoJetVec h = Normalize(Cross(du, dv))*TwoJet(size);
	TwoJetVec w = Normalize(Cross(h, du))*(TwoJet(size)*1.1);

	return RotateZ(
	TwoJetVec(p) +
	FigureEight(w, h, du*D(size, 0)*(D(u, 0)^(-1)), form, v),
	v*(1.0/numStrips)
	);
}

// ----------------------------------------

ThreeJetVec Arc(ThreeJet u, ThreeJet v, double xsize, double ysize, double zsize)
{
	ThreeJetVec result;
	u = u*0.25;
	result.x = Sin (u) * Sin (v) * xsize;
	result.y = Sin (u) * Cos (v) * ysize;
	result.z = Cos (u) * zsize;
	return result;
}

ThreeJetVec Straight(ThreeJet u, ThreeJet v, double xsize, double ysize, double zsize)
{
	ThreeJetVec result;
	u = u*0.25;

	result.x = Sin (v) * xsize;
	result.y = Cos (v) * ysize;
	result.z = Cos (u) * zsize;
	return result;
}

ThreeJet Param1(ThreeJet x)
{
	double offset = 0;
	x %= 4;
	if(x > 2)
	{
		x = x+(-2);
		offset = 2;
	}

	if(x <= 1)
		return x*2 + (x^2)*(-1) + offset;
	else
		return (x^2) + x*(-2) + (2 + offset);
}

ThreeJet Param2(ThreeJet x)
{
	double offset = 0;
	x %= 4;
	if(x > 2)
	{
		x = x+(-2);
		offset = 2;
	}

	if(x <= 1)
		return (x^2) + offset;
	else
		return (x^2)*(-1) + x*4 + (-2 + offset);
}

static inline ThreeJet TInterp(double x)
{ return ThreeJet(x,0,0); }

ThreeJet UInterp(ThreeJet x)
{
	x %= 2;
	if(x > 1)
		x = x*(-1) + 2;
	return (x^2)*3 + (x^3) * (-2);
}

const double FFPOW = 3;
ThreeJet FFInterp(ThreeJet x)
{
	x %= 2;
	if(x > 1)
		x = x*(-1) + 2;
	x = x*1.06 + -0.05;
	if(x < 0)
		return ThreeJet(0, 0, 0);
	else if(x > 1)
		return ThreeJet(0, 0, 0) + 1;
	else
		return (x ^ (FFPOW-1)) * (FFPOW) + (x^FFPOW) * (-FFPOW+1);
}

const double FSPOW = 3;
ThreeJet FSInterp(ThreeJet x)
{
	x %= 2;
	if(x > 1)
		x = x*(-1) + 2;

	return ((x ^ (FSPOW-1)) * (FSPOW) + (x^FSPOW) * (-FSPOW+1)) * (-0.2);
}

ThreeJetVec Stage0(ThreeJet u, ThreeJet v)
{ return Straight(u, v, 1, 1, 1); }

ThreeJetVec Stage1(ThreeJet u, ThreeJet v)
{ return Arc(u, v, 1, 1, 1); }

ThreeJetVec Stage2(ThreeJet u, ThreeJet v)
{
	return InterpolateVec(
		Arc(Param1(u), v, 0.9, 0.9, -1),
		Arc(Param2(u), v, 1, 1, 0.5),
		UInterp(u)
	);
}

ThreeJetVec Stage3(ThreeJet u, ThreeJet v)
{
	return InterpolateVec(
		Arc(Param1(u), v,-0.9,-0.9,-1),
		Arc(Param2(u), v,-1, 1,-0.5),
		UInterp(u)
	);
}

ThreeJetVec Stage4(ThreeJet u, ThreeJet v)
{ return Arc(u, v, -1,-1, -1); }

ThreeJetVec Scene01(ThreeJet u, ThreeJet v, double t)
{ return InterpolateVec(Stage0(u,v), Stage1(u,v), TInterp(t)); }

ThreeJetVec Scene12(ThreeJet u, ThreeJet v, double t)
{ return InterpolateVec(Stage1(u,v), Stage2(u,v), TInterp(t)); }

ThreeJetVec Scene23(ThreeJet u, ThreeJet v, double t)
{
	ThreeJet tmp = TInterp(t);
	t = tmp.f * 0.5;
	double tt = (u <= 1) ? t : -t;

	return InterpolateVec(
		RotateZ(Arc(Param1(u), v, 0.9, 0.9,-1), ThreeJet(tt,0,0)),
		RotateY(Arc(Param2(u), v, 1, 1, 0.5), ThreeJet(t,0,0)),
		UInterp(u)
	);
}

ThreeJetVec Scene34(ThreeJet u, ThreeJet v, double t)
{ return InterpolateVec(Stage3(u,v), Stage4(u,v), TInterp(t)); }

TwoJetVec BendIn(ThreeJet u, ThreeJet v, double t, int numStrips)
{
	ThreeJet tmp = TInterp(t);
	t = tmp.f;

	return AddFigureEight(
		Scene01(u, ThreeJet(0, 0, 1), t),
		u, v, ThreeJet(0, 0, 0), FSInterp(u),
		numStrips
	);
}

TwoJetVec Corrugate(ThreeJet u, ThreeJet v, double t, int numStrips)
{
	ThreeJet tmp = TInterp(t);
	t = tmp.f;

	return AddFigureEight(
		Stage1(u, ThreeJet(0, 0, 1)),
		u, v, FFInterp(u) * ThreeJet(t,0,0), FSInterp(u),
		numStrips
	);
}

TwoJetVec PushThrough(ThreeJet u, ThreeJet v, double t, int numStrips)
{
	return AddFigureEight(
		Scene12(u,ThreeJet(0, 0, 1),t),
		u, v, FFInterp(u), FSInterp(u),
		numStrips
	);
}

TwoJetVec Twist(ThreeJet u, ThreeJet v, double t, int numStrips)
{
	return AddFigureEight(
		Scene23(u,ThreeJet(0, 0, 1),t),
		u, v, FFInterp(u), FSInterp(u),
		numStrips
	);
}

TwoJetVec UnPush(ThreeJet u, ThreeJet v, double t, int numStrips)
{
	return AddFigureEight(
		Scene34(u,ThreeJet(0, 0, 1),t),
		u, v, FFInterp(u), FSInterp(u),
		numStrips
	);
}

TwoJetVec UnCorrugate(ThreeJet u, ThreeJet v, double t, int numStrips)
{
	ThreeJet tmp;
	tmp = TInterp((t) * (-1) + 1);
	t = tmp.f;

	return AddFigureEight(
		Stage4(u,ThreeJet(0, 0, 1)),
		u, v, FFInterp(u) * ThreeJet(t,0,0), FSInterp(u),
		numStrips
	);
}

// ----------------------------------------

typedef std::map<k3d::point3, k3d::legacy::point*> point_map_t;
point_map_t point_map;
k3d::imaterial* current_material;
double current_radius;

k3d::legacy::point* get_spline_point(k3d::legacy::mesh* mesh, TwoJetVec p, double ps, double pus, double pvs, double puvs)
{
	k3d::point3 v;
	v[0] = (p.x.f)*ps + (p.x.df_du()*pus + p.x.df_dv()*pvs)/3.0 + p.x.d2f_dudv()*puvs/9.0;
	v[1] = (p.y.f)*ps + (p.y.df_du()*pus + p.y.df_dv()*pvs)/3.0 + p.y.d2f_dudv()*puvs/9.0;
	v[2] = (p.z.f)*ps + (p.z.df_du()*pus + p.z.df_dv()*pvs)/3.0 + p.z.d2f_dudv()*puvs/9.0;
	// Handedness change
	v[0] = -v[0];

	k3d::legacy::point* point;
	point_map_t::const_iterator mapped_point = point_map.find(v);
	if(mapped_point != point_map.end())
	{
		point = mapped_point->second;
	}
	else
	{
		point = new k3d::legacy::point(v * current_radius);
		point_map[v] = point;
		mesh->points.push_back(point);
	}

	return point;
}

void get_patch(k3d::legacy::mesh* mesh, TwoJetVec v00, TwoJetVec v01, TwoJetVec v10, TwoJetVec v11, double us, double vs, double s0, double s1, double t0, double t1)
{
	k3d::legacy::bicubic_patch* const patch = new k3d::legacy::bicubic_patch();

	patch->control_points[0] = get_spline_point(mesh, v00, 1, 0, 0, 0);
	patch->control_points[1] = get_spline_point(mesh, v00, 1, us, 0, 0);
	patch->control_points[2] = get_spline_point(mesh, v10, 1,-us, 0, 0);
	patch->control_points[3] = get_spline_point(mesh, v10, 1, 0, 0, 0);

	patch->control_points[4] = get_spline_point(mesh, v00, 1, 0, vs, 0);
	patch->control_points[5] = get_spline_point(mesh, v00, 1, us, vs, us*vs);
	patch->control_points[6] = get_spline_point(mesh, v10, 1,-us, vs,-us*vs);
	patch->control_points[7] = get_spline_point(mesh, v10, 1, 0, vs, 0);

	patch->control_points[8] = get_spline_point(mesh, v01, 1, 0,-vs, 0);
	patch->control_points[9] = get_spline_point(mesh, v01, 1, us,-vs,-us*vs);
	patch->control_points[10] = get_spline_point(mesh, v11, 1,-us,-vs, us*vs);
	patch->control_points[11] = get_spline_point(mesh, v11, 1, 0,-vs, 0);

	patch->control_points[12] = get_spline_point(mesh, v01, 1, 0, 0, 0);
	patch->control_points[13] = get_spline_point(mesh, v01, 1, us, 0, 0);
	patch->control_points[14] = get_spline_point(mesh, v11, 1,-us, 0, 0);
	patch->control_points[15] = get_spline_point(mesh, v11, 1, 0, 0, 0);

	// Texture coordinates?
	//s0,t0, s1,t0, s0,t1, s1,t1;

	mesh->bicubic_patches.push_back(patch);
	patch->material = current_material;
}

void get_polygon_point(TwoJetVec p, eversion_point& point)
{
	double x = p.x.f ;
	double y = p.y.f ;
	double z = p.z.f ;
	double nx = p.y.df_du() * p.z.df_dv() - p.z.df_du() * p.y.df_dv();
	double ny = p.z.df_du() * p.x.df_dv() - p.x.df_du() * p.z.df_dv();
	double nz = p.x.df_du() * p.y.df_dv() - p.y.df_du() * p.x.df_dv();

	double s = nx*nx + ny*ny + nz*nz;
	if(s > 0)
		s = sqrt(1/s);

	point.vertex[0] = x;
	point.vertex[1] = y;
	point.vertex[2] = z;
	point.normal[0] = -nx*s;
	point.normal[1] = -ny*s;
	point.normal[2] = -nz*s;
}

// ----------------------------------------

typedef TwoJetVec SurfaceTimeFunction(ThreeJet u, ThreeJet v, double t, int numStrips);

inline double sqr(double x)
{
	return x*x;
}
inline double calcSpeedV(TwoJetVec v)
{
	return sqrt(sqr(v.x.df_dv()) + sqr(v.y.df_dv()) + sqr(v.z.df_dv()));
}
inline double calcSpeedU(TwoJetVec v)
{
	return sqrt(sqr(v.x.df_du()) + sqr(v.y.df_du()) + sqr(v.z.df_du()));
}

void get_eversion(k3d::legacy::mesh* mesh, bool polygonal, SurfaceTimeFunction* func, double umin, double umax, int ucount, double vmin, double vmax, int vcount, double t, geometry_matrix_t& points, int numStrips)
{
	static TwoJetVec** values;

	if(ucount <= 0 || vcount <= 0)
		return;

	double delta_u = (umax-umin) / ucount;
	double delta_v = (vmax-vmin) / vcount;
	values = (TwoJetVec**)calloc(ucount+1, sizeof(TwoJetVec*));
	//double* speedv = (double*)calloc(ucount+1, sizeof(double));
	//double** speedu = (double**)calloc(ucount+1, sizeof(double*));
	for(int j = 0; j <= ucount; j++)
	{
		double u = umin + j*delta_u;
		values[j] = (TwoJetVec*)calloc(vcount+1, sizeof(TwoJetVec));
		//speedu[j] = (double*)calloc(vcount+1, sizeof(double));
		//speedv[j] = calcSpeedV((*func)(ThreeJet(u, 1, 0), ThreeJet(0, 0, 1), t, numStrips));
		double speed_v = calcSpeedV((*func)(ThreeJet(u, 1, 0), ThreeJet(0, 0, 1), t, numStrips));
		if(speed_v == 0)
		//if(speedv[j] == 0)
		{
			// Perturb a bit, hoping to avoid degeneracy
			u += (u < 1) ? 1e-9 : -1e-9;
			//speedv[j] = calcSpeedV((*func)(ThreeJet(u, 1, 0), ThreeJet(0, 0, 1), t, numStrips));
		}

		for(int k = 0; k <= vcount; k++)
		{
			double v = vmin + k*delta_v;
			values[j][k] = (*func)(ThreeJet(u, 1, 0), ThreeJet(v, 0, 1), t, numStrips);
			//speedu[j][k] = calcSpeedU(values[j][k]);
		}
	}

	if(!polygonal)
	{
		// Bezier patches
		for(int j = 0; j < ucount; ++j)
			//double u = umin + j*delta_u;
			for(int k = 0; k < vcount; ++k)
				//double v = vmin + k*delta_v;
				get_patch(mesh, values[j][k], values[j][k+1], values[j+1][k], values[j+1][k+1], delta_u, delta_v, umin + j*delta_u, umin + (j+1)*delta_u, vmin + k*delta_v, vmin + (k+1)*delta_v);
	}
	else
	{
		// Quadrilaterals
		for(int j = 0; j <= ucount; ++j)
		{
			geometry_vector_t point_vector;
			for(int k = 0; k <= vcount; ++k)
			{
				eversion_point new_point;
				get_polygon_point(values[j][k], new_point);
				point_vector.push_back(new_point);
			}

			points.push_back(point_vector);
		}
	}

	// Clean up
	for(int j = 0; j <= ucount; j++)
	{
		free(values[j]);
		//free(speedu[j]);
	}

	free(values);
	//free(speedu);
	//free(speedv);
}

// Generate an eversion strip
void generate_strip(
	// Mesh
	k3d::legacy::mesh* mesh,
	// Return Bezier patches (false) or polygons (true)
	bool polygonal,
	// An array of (1 + u_count) arrays of (1 + v_count) elements
	geometry_matrix_t& points,
	// Eversion time, must be between 0.0 and 1.0
	double time = 0,
	// If this is too small, there will be pinches in the eversion
	int numStrips = 8,
	// The u parameter corresponds to latitude, v to longitude:
	// - parallels [90 degrees north, 90 degrees south] are mapped to [0.0, 2.0]
	double u_min = 0.0,	// set to 0.0 to start at north pole
	int u_count = 12,	// recommended value: 12*(u_max-u_min)
	double u_max = 1.0,	// set to 1.0 to stop at equator, or 2.0 to stop at south pole
	// - meridians [0 degrees, 180 degrees west] are mapped to [0, numStrips]
	double v_min = 0.0,
	int v_count = 12,	// recommended value: 12*(v_max-v_min)
	double v_max = 1.0,

	// -1 means don't do bendtime at all
	double bendtime = -1.0,
	double corrStart = 0.00,	// start of corrugation
	double pushStart = 0.10,	// start of push (poles are pushed through each other)
	double twistStart = 0.23,	// start of twist (poles rotate in opposite directions)
	double unpushStart = 0.60,	// start of unpush (poles held fixed while corrugations pushed through center)
	double uncorrStart = 0.93)	// start of uncorrugation
{
	point_map.clear();
	if(bendtime >= 0.0)
	{
		get_eversion(mesh, polygonal, BendIn, u_min, u_max, u_count, v_min, v_max, v_count, bendtime, points, numStrips);
		return;
	}

	// time = (time - howfar) / chunk

	if(time >= uncorrStart)
		get_eversion(mesh, polygonal, UnCorrugate, u_min, u_max, u_count, v_min, v_max, v_count,
			(time - uncorrStart) / (1.0 - uncorrStart), points, numStrips);
	else if(time >= unpushStart)
		get_eversion(mesh, polygonal, UnPush, u_min, u_max, u_count, v_min, v_max, v_count,
			(time - unpushStart) / (uncorrStart - unpushStart), points, numStrips);
	else if(time >= twistStart)
		get_eversion(mesh, polygonal, Twist, u_min, u_max, u_count, v_min, v_max, v_count,
			(time - twistStart) / (unpushStart - twistStart), points, numStrips);
	else if(time >= pushStart)
		get_eversion(mesh, polygonal, PushThrough, u_min, u_max, u_count, v_min, v_max, v_count,
			(time - pushStart) / (twistStart - pushStart), points, numStrips);
	else if(time >= corrStart)
		get_eversion(mesh, polygonal, Corrugate, u_min, u_max, u_count, v_min, v_max, v_count,
			(time - corrStart) / (pushStart - corrStart), points, numStrips);
}

} // namespace detail

/////////////////////////////////////////////////////////////////////////////
// sphere_eversion_implementation

class sphere_eversion_implementation :
	public k3d::material_client<k3d::legacy::mesh_source<k3d::persistent<k3d::node> > >
{
	typedef k3d::material_client<k3d::legacy::mesh_source<k3d::persistent<k3d::node> > > base;

public:
	sphere_eversion_implementation(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_document(Document),
		m_time(init_owner(*this) + init_name("time") + init_label(_("Time")) + init_description(_("Eversion Time : between 0 and 1")) + init_value(0.0) + init_constraint(constraint::minimum(0.0, constraint::maximum(1.0))) + init_step_increment(0.01) + init_units(typeid(k3d::measurement::scalar))),
		m_radius(init_owner(*this) + init_name("radius") + init_label(_("Radius")) + init_description(_("Sphere radius")) + init_value(5.0) + init_step_increment(0.1) + init_units(typeid(k3d::measurement::scalar))),
		m_latitudinal_patches(init_owner(*this) + init_name("latitudinal_patches") + init_label(_("Latitudinal patches")) + init_description(_("Latitudinal Patches Per Hemisphere")) + init_value(6) + init_constraint(constraint::minimum(2L)) + init_step_increment(1) + init_units(typeid(k3d::measurement::scalar))),
		m_longitudinal_patches(init_owner(*this) + init_name("longitudinal_patches") + init_label(_("Longitudinal patches")) + init_description(_("Longitudinal Patches Per Strip")) + init_value(4) + init_constraint(constraint::minimum(2L)) + init_step_increment(1) + init_units(typeid(k3d::measurement::scalar))),
		m_strips(init_owner(*this) + init_name("strips") + init_label(_("Strips")) + init_description(_("Strip Number")) + init_value(8) + init_constraint(constraint::minimum(1L)) + init_step_increment(1) + init_units(typeid(k3d::measurement::scalar))),
		m_display_strips(init_owner(*this) + init_name("display_strips") + init_label(_("Display strips")) + init_description(_("Display strip Number")) + init_value(8) + init_constraint(constraint::minimum(1L)) + init_step_increment(1) + init_units(typeid(k3d::measurement::scalar))),
		m_half_strips(init_owner(*this) + init_name("half_strips") + init_label(_("Half strips")) + init_description(_("Output half strips")) + init_value(false)),
		m_lower_hemisphere(init_owner(*this) + init_name("lower_hemisphere") + init_label(_("Lower hemisphere")) + init_description(_("Output lower hemisphere")) + init_value(true)),
		m_cylinder_test(init_owner(*this) + init_name("cylinder_test") + init_label(_("Cylinder test")) + init_description(_("Cylinder bending into sphere")) + init_value(false)),
		m_polygonal(init_owner(*this) + init_name("polygonal") + init_label(_("Polygonal")) + init_description(_("Output polygons (true) or patches (false)")) + init_value(false))
	{
		m_material.changed_signal().connect(make_reset_mesh_slot());
		m_time.changed_signal().connect(make_reset_mesh_slot());
		m_radius.changed_signal().connect(make_reset_mesh_slot());
		m_latitudinal_patches.changed_signal().connect(make_reset_mesh_slot());
		m_longitudinal_patches.changed_signal().connect(make_reset_mesh_slot());
		m_strips.changed_signal().connect(make_reset_mesh_slot());
		m_display_strips.changed_signal().connect(make_reset_mesh_slot());
		m_half_strips.changed_signal().connect(make_reset_mesh_slot());
		m_lower_hemisphere.changed_signal().connect(make_reset_mesh_slot());
		m_cylinder_test.changed_signal().connect(make_reset_mesh_slot());
		m_polygonal.changed_signal().connect(make_reset_mesh_slot());
	}

	void on_initialize_mesh(k3d::legacy::mesh& Mesh)
	{
		const double time = m_time.pipeline_value();
		const double radius = m_radius.pipeline_value();
		const unsigned long latitudinal = m_latitudinal_patches.pipeline_value();
		const unsigned long longitudinal = m_longitudinal_patches.pipeline_value();
		const unsigned long strip_number = m_strips.pipeline_value();
		const unsigned long display_strips_number = m_display_strips.pipeline_value();
		const bool half_strips = m_half_strips.pipeline_value();
		const bool lower_hemisphere = m_lower_hemisphere.pipeline_value();
		const bool cylinder_test = m_cylinder_test.pipeline_value();
		const bool polygonal = m_polygonal.pipeline_value();

		k3d::legacy::mesh* const strip_mesh = new k3d::legacy::mesh();
		detail::current_material = m_material.pipeline_value();
		detail::current_radius = radius;
		detail::geometry_matrix_t sphere_points;
		detail::generate_strip(strip_mesh, polygonal, sphere_points, time, strip_number,
			0.0,
			latitudinal,
			1.0,
			0.0,
			longitudinal,
			half_strips ? 0.5 : 1.0,
			cylinder_test ? time : -1.0
		);

		if(polygonal)
		{
			// Create polygonal strip mesh ...
			std::vector< std::vector<k3d::legacy::point*> > points;
			for(unsigned long j = 0; j <= latitudinal; ++j)
			{
				std::vector<k3d::legacy::point*> vpoints;
				for(unsigned long k = 0; k <= longitudinal; ++k)
				{
					double* p = sphere_points[j][k].vertex;
					double x = p[0] * radius;
					double y = p[1] * radius;
					double z = p[2] * radius;

					// Fix handedness
					k3d::legacy::point* point = new k3d::legacy::point(x, y, -z);

					// Set normal
					p = sphere_points[j][k].normal;
					x = p[0];
					y = p[1];
					z = p[2];
					point->vertex_data["N"] = k3d::ri::normal(x, y, -z);

					strip_mesh->points.push_back(point);
					vpoints.push_back(point);
				}

				points.push_back(vpoints);
			}

			strip_mesh->polyhedra.push_back(new k3d::legacy::polyhedron());
			k3d::legacy::polyhedron& polyhedron = *strip_mesh->polyhedra.back();

			for(unsigned long j = 0; j < latitudinal; ++j)
			{
				k3d::legacy::point* p1 = 0;
				k3d::legacy::point* p2 = 0;
				k3d::legacy::point* p3 = 0;
				bool strip_toggle = false;

				for(unsigned long k = 0; k <= longitudinal; ++k)
				for(unsigned long n = 0; n < 2; n++)
				{
					p1 = p2;
					p2 = p3;
					p3 = points[j+n][k];
					if(!p1)
						continue;

					std::vector<k3d::legacy::split_edge*> edges;
					edges.push_back(new k3d::legacy::split_edge(p1));
					edges.push_back(new k3d::legacy::split_edge(p2));
					edges.push_back(new k3d::legacy::split_edge(p3));
					if(strip_toggle)
						std::reverse(edges.begin(), edges.end());
					strip_toggle = !strip_toggle;

					k3d::legacy::loop_edges(edges.begin(), edges.end());

					k3d::legacy::face* new_face = new k3d::legacy::face(edges.front(), detail::current_material);
					polyhedron.faces.push_back(new_face);
				}
			}

			assert_warning(is_valid(polyhedron));
		}

		// Duplicate strip to strip number
		unsigned long original_points = strip_mesh->points.size();

		double strip_rotation = 2*k3d::pi() / static_cast<double>(strip_number);
		for(unsigned long n = 0; n < display_strips_number && n < strip_number; n++)
		{
			k3d::legacy::deep_copy(*strip_mesh, Mesh);

			k3d::matrix4 rotation(k3d::rotation3D(k3d::point3(0, 0, static_cast<double>(n) * strip_rotation)));
			if(n > 0)
				for(k3d::legacy::mesh::points_t::iterator p = Mesh.points.end() - original_points; p != Mesh.points.end(); p++)
				{
					(*p)->position = rotation * (*p)->position;
					if(polygonal)
						(*p)->vertex_data["N"] = k3d::ri::normal(rotation * boost::any_cast<k3d::ri::normal>((*p)->vertex_data["N"]));
				}

			if(!lower_hemisphere)
				continue;

			if(!polygonal)
			{
				k3d::legacy::deep_copy(*strip_mesh, Mesh);
				k3d::matrix4 rotation2(k3d::rotation3D(k3d::point3(k3d::pi(), 0, 0)));
				if(n > 0)
					rotation = rotation2 * rotation;
				else
					rotation = rotation2;

				for(k3d::legacy::mesh::points_t::iterator p = Mesh.points.end() - original_points; p != Mesh.points.end(); p++)
					(*p)->position = rotation * (*p)->position;
			}
			else
			{
				k3d::legacy::deep_copy(*strip_mesh, Mesh);
				for(k3d::legacy::mesh::points_t::iterator p = Mesh.points.end() - original_points; p != Mesh.points.end(); p++)
				{
					k3d::point3 position = rotation * (*p)->position;
					k3d::ri::normal normal = k3d::ri::normal(rotation * boost::any_cast<k3d::ri::normal>((*p)->vertex_data["N"]));

					position.n[2] = -position.n[2];
					normal.n[2] = -normal.n[2];

					(*p)->position = position;
					(*p)->vertex_data["N"] = k3d::ri::normal(normal);
				}

				// Invert triangles
				k3d::legacy::polyhedron& polyhedron = *Mesh.polyhedra.back();
				for(k3d::legacy::polyhedron::faces_t::iterator face = polyhedron.faces.begin(); face != polyhedron.faces.end(); face++)
				{
					k3d::legacy::split_edge* first = (*face)->first_edge;
					k3d::legacy::split_edge* second = first->face_clockwise;
					k3d::legacy::split_edge* third = second->face_clockwise;

					second->face_clockwise = first;
					third->face_clockwise = second;
					first->face_clockwise = third;
				}
			}
		}

		delete strip_mesh;
	}

	void on_update_mesh(k3d::legacy::mesh& Mesh)
	{
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<sphere_eversion_implementation, k3d::interface_list<k3d::imesh_source > > factory(
			k3d::uuid(0x2e885acd, 0x9e914078, 0xb4fcb4a7, 0x415231c5),
			"SphereEversion",
			_("Generates sphere eversion at any stage"),
			"Patches Polygons",
			k3d::iplugin_factory::STABLE);

		return factory;
	}

private:
	k3d::idocument& m_document;
	k3d_data(double, immutable_name, change_signal, with_undo, local_storage, with_constraint, measurement_property, with_serialization) m_time;
	k3d_data(double, immutable_name, change_signal, with_undo, local_storage, no_constraint, measurement_property, with_serialization) m_radius;
	k3d_data(long, immutable_name, change_signal, with_undo, local_storage, with_constraint, measurement_property, with_serialization) m_latitudinal_patches;
	k3d_data(long, immutable_name, change_signal, with_undo, local_storage, with_constraint, measurement_property, with_serialization) m_longitudinal_patches;
	k3d_data(long, immutable_name, change_signal, with_undo, local_storage, with_constraint, measurement_property, with_serialization) m_strips;
	k3d_data(long, immutable_name, change_signal, with_undo, local_storage, with_constraint, measurement_property, with_serialization) m_display_strips;
	k3d_data(bool, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_half_strips;
	k3d_data(bool, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_lower_hemisphere;
	k3d_data(bool, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_cylinder_test;
	k3d_data(bool, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_polygonal;
};

/////////////////////////////////////////////////////////////////////////////
// sphere_eversion_factory

k3d::iplugin_factory& sphere_eversion_factory()
{
	return sphere_eversion_implementation::get_factory();
}

} // namespace libk3dprimitives

