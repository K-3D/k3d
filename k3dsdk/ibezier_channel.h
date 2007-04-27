#ifndef K3DSDK_IBEZIER_CHANNEL_H
#define K3DSDK_IBEZIER_CHANNEL_H

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
		\brief Declares ibezier_channel, an abstract interface for a third-order two-dimensional Bezier control spline
		\author Tim Shead (tshead@k-3d.com)
*/

#include "iunknown.h"
#include "vectors.h"

namespace k3d
{

/// Abstract interface for a two-dimensional Bezier control spline
template<typename value_t>
class ibezier_channel :
	public virtual iunknown
{
public:
	/// Defines a set of two dimensional control points
	typedef std::vector<k3d::point2> control_points_t;
	/// Defines a set of values
	typedef std::vector<value_t> values_t;

	/** \brief Assigns a new 3rd-order Bezier curve
	*/
	virtual void set_curve(const control_points_t ControlPoints, const values_t Values) = 0;
	/** \brief Returns data about the curve
	*/
	virtual void get_curve(control_points_t& ControlPoints, values_t& Values) = 0;

protected:
	ibezier_channel() {}
	ibezier_channel(const ibezier_channel& RHS) {}
	ibezier_channel& operator=(const ibezier_channel& RHS) { return *this; }
	virtual ~ibezier_channel() {}
};

/// Specialization of ibezier_channel for scalar channels
template<>
class ibezier_channel<void> :
	public virtual iunknown
{
public:
	/// Defines a set of two dimensional control points
	typedef std::vector<k3d::point2> control_points_t;

	/** \brief Assigns a new 3rd-order Bezier curve
	*/
	virtual void set_curve(const control_points_t ControlPoints) = 0;
	/** \brief Returns data about the curve
	*/
	virtual void get_curve(control_points_t& ControlPoints) = 0;

protected:
	ibezier_channel() {}
	ibezier_channel(const ibezier_channel& RHS) {}
	ibezier_channel& operator=(const ibezier_channel& RHS) { return *this; }
	virtual ~ibezier_channel() {}
};

} // namespace k3d

#endif // K3DSDK_IBEZIER_CHANNEL_H


