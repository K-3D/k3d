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

#include "texture3_python.h"

#include <k3dsdk/texture3.h>

#include <boost/python.hpp>
#include <boost/python/detail/api_placeholder.hpp>
using namespace boost::python;

namespace k3d
{

namespace python
{

int texture3_len(const k3d::texture3& lhs)
{
	return 3;
}

double texture3_getitem(const k3d::texture3& lhs, int item)
{
	if(item < 0 || item > 2)
		throw std::out_of_range("index out-of-range");

	return lhs[item];
}

void texture3_setitem(k3d::texture3& lhs, int item, double value)
{
	if(item < 0 || item > 2)
		throw std::out_of_range("index out-of-range");

	lhs[item] = value;
}

void export_texture3()
{
	class_<k3d::texture3>("texture3",
		"Stores a set of three-dimensional texture coordinates", no_init)
		.def(init<double, double, double>())
		.def(init<double, double>())
		.def(init<const k3d::texture3&>())
		.def(self += self)
		.def(self *= double())
		.def(self /= double())
		.def("__len__", texture3_len)
		.def("__getitem__", texture3_getitem)
		.def("__setitem__", texture3_setitem)
		.def(self + self)
		.def(self * double())
		.def(double() * self)
		.def(self == self)
		.def(self != self)
		.def(self_ns::str(self));
}

} // namespace python

} // namespace k3d

