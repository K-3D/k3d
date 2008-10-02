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

#include "geometric_operations.h"
#include "vectors.h"
#include "result.h"

#include <cmath>
#include <iosfwd>
#include <vector>

namespace k3d
{

namespace nurbs
{

/// Storage for a NURBS curve
struct curve3
{
	curve3(const uint32_t Order) : order(Order) {}
	
	/// Stores the curve order (note - the curve order is defined as the curve degree plus 1)
	uint32_t order;
	/// Defines a collection of knots
	typedef std::vector<double_t> knots_t;
	/// The set of knots that define the curve's knot vector
	knots_t knots;
	/// Defines a control point - a combination of a value and a weight
	struct control_point
	{
		control_point() : weight(0) {}
		control_point(const point3& Position, const double_t Weight = 1.0) : position(Position), weight(Weight) {}

		point3 position;
		double_t weight;
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
const point3 evaluate(const curve3& Curve, const double_t T);

/** \deprecated */
template<typename knots_t, typename weights_t, typename control_points_t>
void circular_arc(const point3& X, const point3& Y, const double_t StartAngle, const double_t EndAngle, const uint32_t Segments, knots_t& Knots, weights_t& Weights, control_points_t& ControlPoints)
{
	const double_t start_angle = std::min(StartAngle, EndAngle);
	const double_t end_angle = std::max(StartAngle, EndAngle);
	const double_t theta = (end_angle - start_angle) / static_cast<double_t>(Segments);
	const double_t weight = std::cos(theta * 0.5);

	Knots.clear();
	Knots.insert(Knots.end(), 3, 0);
	for(uint32_t i = 1; i != Segments; ++i)
		Knots.insert(Knots.end(), 2, i);
	Knots.insert(Knots.end(), 3, Segments);

	Weights.clear();
	Weights.push_back(1.0);
	for(uint32_t i = 0; i != Segments; ++i)
	{
		Weights.push_back(weight);
		Weights.push_back(1);
	}

	ControlPoints.clear();
	ControlPoints.push_back(std::cos(start_angle) * X + std::sin(start_angle) * Y);
	for(uint32_t i = 0; i != Segments; ++i)
	{
		const double_t a0 = start_angle + (theta * (i));
		const double_t a2 = start_angle + (theta * (i + 1));

		const point3 p0(std::cos(a0) * X + std::sin(a0) * Y);
		const point3 p2(std::cos(a2) * X + std::sin(a2) * Y);

		const point3 t0(-std::sin(a0) * X + std::cos(a0) * Y);
		const point3 t2(-std::sin(a2) * X + std::cos(a2) * Y);

		point3 p1;
		intersect_lines(p0, to_vector(t0), p2, to_vector(t2), p1);

		ControlPoints.push_back(p1);
		ControlPoints.push_back(p2);
	}
}

/** Calculates a 3rd-order NURBS curve that creates a circular arc centered at the origin with unit radius
	\param X Defines the X axis of the plane containing the arc
	\param Y Defines the Y axis of the plane containing the arc
	\param StartAngle Start angle of the arc in radians
	\param EndAngle End angle of the arc in radians
	\param Segments The number of NURBS segments in the resulting arc
	\param Knots Output container for the resulting arc knot vector
	\param Weights Output container for the resulting arc control point weights
	\param ControlPoints Output container for the resulting arc control point positions
*/
template<typename knots_t, typename weights_t, typename control_points_t>
void circular_arc(const vector3& X, const vector3& Y, const double_t StartAngle, const double_t EndAngle, const uint32_t Segments, knots_t& Knots, weights_t& Weights, control_points_t& ControlPoints)
{
	circular_arc(to_point(X), to_point(Y), StartAngle, EndAngle, Segments, Knots, Weights, ControlPoints);
}

} // namespace nurbs

} // namespace k3d

#endif // !K3DSDK_NURBS_H
