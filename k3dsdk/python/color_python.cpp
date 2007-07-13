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

#include "color_python.h"

#include <k3dsdk/color.h>
#include <boost/python.hpp>
using namespace boost::python;

namespace k3d
{

namespace python
{

void export_color()
{
	class_<k3d::color>("color",
		"Stores a color using half-precision floating-point RGB values.\n\n"
		"@note: The values [0, 1] are mapped to the values [0, 255] in integer color types. "
		"Values outside the range [0, 1] are explicitly allowed.")
		.def(init<double, double, double>())
		.def_readwrite("red", &k3d::color::red,
			"Stores red component of the color.")
		.def_readwrite("green", &k3d::color::green,
			"Stores green component of the color.")
		.def_readwrite("blue", &k3d::color::blue,
			"Stores blue component of the color.")
		.def(self + self)
		.def(self - self)
		.def(self * double())
		.def(double() * self)
		.def(self == self)
		.def(self != self)
		.def(self_ns::str(self));
}

} // namespace python

} // namespace k3d

