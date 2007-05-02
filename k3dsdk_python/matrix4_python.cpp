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

#include "matrix4_python.h"
#include <k3dsdk/algebra.h>
#include <boost/python.hpp>
using namespace boost::python;

namespace k3d
{

namespace python
{

int matrix4_len(const k3d::matrix4& lhs)
{
	return 4;
}

const k3d::point4 matrix4_getitem(const k3d::matrix4& lhs, int item)
{
	if(item < 0 || item > 3)
		throw std::out_of_range("index out-of-range");

	return lhs[item];
}

void matrix4_setitem(k3d::matrix4& lhs, int item, const k3d::point4& value)
{
	if(item < 0 || item > 3)
		throw std::out_of_range("index out-of-range");

	lhs[item] = value;
}

list matrix4_row_major_list(const k3d::matrix4& lhs)
{
	list results;
	for(size_t i = 0; i != 4; ++i)
	{
		for(size_t j = 0; j != 4; ++j)
		{
			results.append(lhs[i][j]);
		}
	}

	return results;
}

list matrix4_column_major_list(const k3d::matrix4& lhs)
{
	list results;
	for(size_t i = 0; i != 4; ++i)
	{
		for(size_t j = 0; j != 4; ++j)
		{
			results.append(lhs[j][i]);
		}
	}

	return results;
}

void export_matrix4()
{
	class_<k3d::matrix4>("matrix4")
		.def("__len__", matrix4_len)
		.def("__getitem__", matrix4_getitem)
		.def("__setitem__", matrix4_setitem)
		.def("row_major_list", matrix4_row_major_list)
		.def("column_major_list", matrix4_column_major_list)
		.def(self * k3d::normal3())
		.def(self * k3d::point3())
		.def(self * k3d::vector3())
		.def(self * self)
		.def(self == self)
		.def(self != self)
		.def(self_ns::str(self));
}

} // namespace python

} // namespace k3d

