// K-3D
// Copyright (c) 1995-2006, Timothy M. Shead
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
	\author Timothy M. Shead (tshead@k-3d.com)
*/

#include "angle_axis_python.h"

#include <k3dsdk/algebra.h>

#include <boost/python.hpp>
using namespace boost::python;

#include <sstream>

namespace k3d
{

namespace python
{

void define_class_angle_axis()
{
	class_<k3d::angle_axis>("angle_axis",
		"Encodes a change in orientation as a rotation around an arbitrary axis.\n"
		"@note: This is the preferred datatype for storing orientation "
		"in K-3D because it does not suffer from gimbal-lock.", no_init)
		.def(init<double, const k3d::vector3&>())
		.def(init<double, double, double, double>())
		.def_readwrite("angle", &k3d::angle_axis::angle,
			"Stores the rotation angle in radians.")
		.def_readwrite("axis", &k3d::angle_axis::axis,
			"Stores the axis of rotation as a L{vector3}.")
		.def(self == self)
		.def(self != self)
		.def(self_ns::str(self));
}

} // namespace python

} // namespace k3d

