// K-3D
// Copyright (c) 1995-2007, Timothy M. Shead
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

#include "bounding_box3_python.h"

#include <k3dsdk/bounding_box3.h>

#include <boost/python.hpp>
using namespace boost::python;

#include <sstream>

namespace k3d
{

namespace python
{

void define_class_bounding_box3()
{
	class_<k3d::bounding_box3>("bounding_box3",
		"Stores a three-dimensional bounding box.")
		.def(init<double, double, double, double, double, double>())
		.def("empty", &k3d::bounding_box3::empty,
			"Returns True if the bounding box is empty.\n\n"
			"@note: An empty bounding box has a non-existent volume, "
			"whereas a bounding box that contains a single point will have zero volume.")
		.def("width", &k3d::bounding_box3::width,
			"Returns the width of the bounding box (its dimension along X).")
		.def("height", &k3d::bounding_box3::height,
			"Returns the height of the bounding box (its dimension along Y).")
		.def("depth", &k3d::bounding_box3::depth,
			"Returns the depth of the bounding box (its dimension along Z).")
		.def(self == self)
		.def(self != self)
		.def(self_ns::str(self));
}

} // namespace python

} // namespace k3d

