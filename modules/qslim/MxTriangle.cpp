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
		\brief Handy 3D geometrical primitives
		\author Adapted for K-3D: Romain Behar (romainbehar@yahoo.com)
*/

#include "MxTriangle.h"

Vec3 triangle_raw_normal(const Vec3& v1, const Vec3& v2, const Vec3& v3)
{
	Vec3 a = v2 - v1;
	Vec3 b = v3 - v1;
	return a^b;
}

double triangle_area(const Vec3& v1, const Vec3& v2, const Vec3& v3)
{
	return 0.5 * norm(triangle_raw_normal(v1, v2, v3));
}

Vec3 triangle_normal(const Vec3& v1, const Vec3& v2, const Vec3& v3)
{
	Vec3 n = triangle_raw_normal(v1, v2, v3);
	n.Normalize();

	return n;
}

Vec4 triangle_plane(const Vec3& v1, const Vec3& v2, const Vec3& v3)
{
	Vec3 n = triangle_normal(v1, v2, v3);

	return Vec4(n, -(n*v1));
}

Vec4 triangle_raw_plane(const Vec3& v1, const Vec3& v2, const Vec3& v3)
{
	Vec3 n = triangle_raw_normal(v1, v2, v3);

	return Vec4(n, -(n*v1));
}

const double FOUR_ROOT3 = 6.928203230275509;

double triangle_compactness(const Vec3& v1, const Vec3& v2, const Vec3& v3)
{
	double L1 = norm2(v2 - v1);
	double L2 = norm2(v3 - v2);
	double L3 = norm2(v1 - v3);

	return FOUR_ROOT3 * triangle_area(v1, v2, v3) / (L1+L2+L3);
}


