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

#include <boost/python.hpp>
using namespace boost::python;

#include <sstream>

namespace k3d
{

namespace python
{

////////////////////////////////////////////////////////////////////////////////////
// angle_axis

const double angle_axis_get_angle(const angle_axis& lhs)
{
	return k3d::degrees(lhs.angle);
}

void angle_axis_set_angle(angle_axis& lhs, const double value)
{
	lhs.angle = k3d::radians(value);
}

const std::string angle_axis_str(const angle_axis& lhs)
{
	std::stringstream buffer;
	buffer << "(" << k3d::degrees(lhs.angle) << ", (" << lhs.axis[0] << ", " << lhs.axis[1] << ", " << lhs.axis[2] << "))";
	return buffer.str();
}

angle_axis::angle_axis() :
	base()
{
}

angle_axis::angle_axis(double Angle, const vector3& Axis) :
	base(k3d::radians(Angle), Axis)
{
}

angle_axis::angle_axis(const k3d::angle_axis& Value) :
	base(Value)
{
}

void angle_axis::define_class()
{
	class_<angle_axis>("angle_axis",
		"Stores a rotation around an arbitrary axis.\n"
		"@note: This is the preferred datatype for storing orientation "
		"in K-3D because it does not suffer from gimbal-lock.")
		.def(init<double, const k3d::vector3&>())
		.add_property("angle", angle_axis_get_angle, angle_axis_set_angle,
			"Stores the rotation angle in degrees.")
		.def_readwrite("axis", &angle_axis::axis,
			"Stores the axis of rotation as a L{vector3}.")
		.def(self == self)
		.def(self != self)
		.def("__str__", angle_axis_str);
}

} // namespace python

} // namespace k3d

