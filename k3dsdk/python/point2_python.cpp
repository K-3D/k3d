// K-3D
// Copyright (c) 1995-2008, Timothy M. Shead
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

#include "point2_python.h"

#include <k3dsdk/vectors.h>

#include <boost/python.hpp>
#include <boost/python/detail/api_placeholder.hpp>
using namespace boost::python;

namespace k3d
{

namespace python
{

int point2_len(const k3d::point2& lhs)
{
	return 2;
}

double point2_getitem(const k3d::point2& lhs, int item)
{
	if(item < 0 || item > 1)
		throw std::out_of_range("index out-of-range");

	return lhs[item];
}

void point2_setitem(k3d::point2& lhs, int item, double value)
{
	if(item < 0 || item > 1)
		throw std::out_of_range("index out-of-range");

	lhs[item] = value;
}

void export_point2()
{
	class_<k3d::point2>("point2",
		"Stores a position in two-dimensional space", no_init)
		.def(init<double, double>())
		.def(init<const k3d::point2&>())
		.def("__len__", point2_len)
		.def("__getitem__", point2_getitem)
		.def("__setitem__", point2_setitem)
		.def(self == self)
		.def(self != self)
		.def(self + self)
		.def(self - self)
		.def(self * double())
		.def(double() * self)
		.def(self *= double())
		.def(self /= double())
		.def(self += k3d::vector2())
		.def(self -= k3d::vector2())
		.def(self_ns::str(self));
}

} // namespace python

} // namespace k3d

