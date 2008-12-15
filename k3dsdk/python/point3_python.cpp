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

#include "point3_python.h"
#include "utility_python.h"

#include <k3dsdk/vectors.h>

#include <boost/python.hpp>
#include <boost/python/detail/api_placeholder.hpp>
using namespace boost::python;

namespace k3d
{

namespace python
{

void define_class_point3()
{
	class_<k3d::point3>("point3",
		"Stores a position in three-dimensional space", no_init)
		.def(init<double_t, double_t, double_t>())
		.def(init<const k3d::point3&>())
		.def("__len__", &utility::constant_len_len<k3d::point3, 3>)
		.def("__getitem__", &utility::constant_len_get_item<k3d::point3, 3, k3d::double_t>)
		.def("__setitem__", &utility::constant_len_set_item<k3d::point3, 3, k3d::double_t>)
		.def(self == self)
		.def(self != self)
		.def(self + self)
		.def(self - self)
		.def(self * double_t())
		.def(double_t() * self)
		.def(self *= double_t())
		.def(self /= double_t())
		.def(self += k3d::vector3())
		.def(self -= k3d::vector3())
		.def(self_ns::str(self));
}

} // namespace python

} // namespace k3d

