// K-3D
// Copyright (c) 1995-2004, Timothy M. Shead
//
// Contact: tshead@k-3d.com
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
		\brief Implements basic NURBS functions
		\author Tim Shead (tshead@k-3d.com)
*/

#include "nurbs.h"
#include <iterator>

namespace k3d
{

namespace nurbs
{

std::ostream& operator<<(std::ostream& Stream, const curve3::control_point& RHS)
{
	Stream << RHS.position << " " << RHS.weight;
	return Stream;
}

std::istream& operator>>(std::istream& Stream, curve3::control_point& RHS)
{
	Stream >> RHS.position >> RHS.weight;
	return Stream;
}
	
std::ostream& operator<<(std::ostream& Stream, const curve3& RHS)
{
	Stream << RHS.order << " ";
	Stream << RHS.control_points.size() << " ";
	std::copy(RHS.control_points.begin(), RHS.control_points.end(), std::ostream_iterator<curve3::control_point>(Stream, " "));
	std::copy(RHS.knots.begin(), RHS.knots.end(), std::ostream_iterator<double>(Stream, " "));
	
	return Stream;
}

std::istream& operator>>(std::istream& Stream, curve3& RHS)
{
	Stream >> RHS.order;

	unsigned long control_point_count = 0;
	Stream >> control_point_count;

	curve3::control_point control_point(point3(0, 0, 0), 0);
	for(unsigned long i = 0; i != control_point_count; ++i)
	{
		Stream >> control_point;
		RHS.control_points.push_back(control_point);
	}
	
	double knot = 0;
	for(unsigned long i = 0; i != RHS.order + control_point_count; ++i)
	{
		Stream >> knot;
		RHS.knots.push_back(knot);
	}

	return Stream;
}

bool is_valid(const curve3& Curve)
{
	// Order must always be at least 2 (i.e. a linear curve)
	return_val_if_fail(Curve.order >= 2, false);

	// The number of control points must be >= order
	return_val_if_fail(Curve.control_points.size() >= Curve.order, false);

	// The number of knots must be equal to the number of control points plus the order
	return_val_if_fail(Curve.knots.size() == Curve.control_points.size() + Curve.order, false);

	// Knot vector values must always be in ascending order
	for(unsigned long i = 1; i != Curve.knots.size(); ++i)
		return_val_if_fail(Curve.knots[i] >= Curve.knots[i-1], false);

	return true;
}

const double basis(const unsigned long i, const unsigned long k, const double t, const curve3::knots_t& knots)
{
	double ret = 0;
	if(k > 0)
	{
		double a = 0;
		double b = 0;
		double n1 = (t - knots[i]) * basis(i, k - 1, t, knots);
		double d1 = knots[i + k] - knots[i];
		double n2 = (knots[i + k + 1] - t) * basis(i + 1, k - 1, t, knots);
		double d2 = knots[i + k + 1] - knots[i + 1];
		if(d1 > 0.0001 || d1 < -0.0001)
			a = n1 / d1;
		else
			a = 0;
		
		if(d2 > 0.0001 || d2 < -0.0001)
			b = n2 / d2;
		else
			b = 0;

		ret = a + b;
// print "B i = %d, k = %d, ret = %g, a = %g, b = %g\n"%(i,k,ret,a,b)
	}
	else
	{
		if(knots[i] <= t && t <= knots[i + 1])
			ret = 1;
		else
			ret = 0;
	}
	return ret;
}

const point3 evaluate(const curve3& Curve, const double T)
{
	point3 c;
	for(unsigned long i = 0; i != Curve.control_points.size(); ++i)
	{
		const curve3::control_point& control_point = Curve.control_points[i];
		c += to_vector(control_point.position * control_point.weight * basis(i, Curve.order-1, T, Curve.knots));
	}

	return c;
}

} // namespace nurbs

} // namespace k3d

