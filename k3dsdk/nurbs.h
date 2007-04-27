#ifndef K3DSDK_NURBS_H
#define K3DSDK_NURBS_H

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

#include "result.h"
#include "vectors.h"

#include <iosfwd>
#include <vector>

namespace k3d
{

namespace nurbs
{

/// Storage for a NURBS curve
struct curve3
{
	curve3(const unsigned int Order) : order(Order) {}
	
	/// Stores the curve order (note - the curve order is defined as the curve degree plus 1)
	unsigned int order;
	/// Defines a collection of knots
	typedef std::vector<double> knots_t;
	/// The set of knots that define the curve's knot vector
	knots_t knots;
	/// Defines a control point - a combination of a value and a weight
	struct control_point
	{
		control_point() : weight(0) {}
		control_point(const point3& Position, const double Weight = 1.0) : position(Position), weight(Weight) {}

		point3 position;
		double weight;
	};
	/// Defines a collection of control points
	typedef std::vector<control_point> control_points_t;
	/// The set of control points that define this curve
	control_points_t control_points;

};

std::ostream& operator<<(std::ostream& Stream, const curve3::control_point& RHS);
std::istream& operator>>(std::istream& Stream, curve3::control_point& RHS);
std::ostream& operator<<(std::ostream& Stream, const curve3&);
std::istream& operator>>(std::istream& Stream, curve3& RHS);

/// Tests the given NURBS curve to ensure that it meets validity requirements
bool is_valid(const curve3& Curve);

/// Evaluates the given NURBS curve for a specific parameter value
const point3 evaluate(const curve3& Curve, const double T);

} // namespace nurbs

} // namespace k3d

#endif // !K3DSDK_NURBS_H
