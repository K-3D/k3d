#ifndef K3DSDK_BEZIER_H
#define K3DSDK_BEZIER_H

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
		\brief Implements basic Bezier functions
		\author Tim Shead (tshead@k-3d.com)
*/

#include "bezier_private.h"
#include "vectors.h"

#include <cassert>
#include <iterator>
#include <vector>

namespace k3d
{

/// Computes a Bezier curve value with given degree, control points, and parameter value
template<int n, typename value_t, typename iterator_t>
value_t bezier(const iterator_t First, const iterator_t Last, const double t)
{
	// Sanity checks ...
	assert(std::distance(First, Last) == n+1);
	
	return detail::bezier_implementation<n, n, value_t, iterator_t>().value(t, Last-1);
}

/// Treats a Bezier curve as a two-dimensional function, returning the curve value Y for a specific X (instead of using the curve parameter t, which is nonlinear)
template<int n, typename value_t, typename iterator_t>
double bezier_function(const iterator_t First, const iterator_t Last, const double X, const double Hint, const double MaxError, const unsigned long MaxIterations, double& Error, unsigned long& Iterations)
{
	assert(MaxIterations);

	double t = Hint;
	value_t result;
	for(Iterations = 1; Iterations <= MaxIterations; ++Iterations)
		{
			result = bezier<n, value_t, iterator_t>(First, Last, t);
			Error = X - result[0];

			if(std::abs(Error) < MaxError)
				return result[1];

			t += 0.4 * Error;
		}

	return result[1];
}

/// Returns a Bezier / Bernstein basis for the given order, control point number, and parameter value
double BernsteinBasis(const unsigned long Order, const unsigned long ControlPoint, const double Parameter);

/// Computes a Bezier curve value with given order,  control points, and parameter value
template<class Type>
Type Bezier(const std::vector<Type>& ControlPoints, const double Parameter)
{
	// Sanity checks ...
	assert(ControlPoints.size() > 1);

	Type result = BernsteinBasis(ControlPoints.size(), 0, Parameter) * ControlPoints[0];
	for(unsigned long i = 1; i < ControlPoints.size(); i++)
		result += to_vector(BernsteinBasis(ControlPoints.size(), i, Parameter) * ControlPoints[i]);

	return result;
}

} // namespace k3d

#endif // !K3DSDK_BEZIER_H
