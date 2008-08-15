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
#include "utility_python.h"

#include <k3dsdk/algebra.h>

#include <boost/python.hpp>
using namespace boost::python;

namespace k3d
{

namespace python
{

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

void define_class_matrix4()
{
	class_<k3d::matrix4>("matrix4",
		"Stores a 4x4 transformation matrix.")
		.def("__len__", &utility::constant_len_len<k3d::matrix4, 4>)
		.def("__getitem__", &utility::constant_len_get_item<k3d::matrix4, 4, k3d::point4>)
		.def("__setitem__", &utility::constant_len_set_item<k3d::matrix4, 4, k3d::point4>)
		.def("row_major_list", matrix4_row_major_list,
			"Returns the contents of the matrix as a list of floating-point values in row-major order.")
		.def("column_major_list", matrix4_column_major_list,
			"Returns the contents of the matrix as a list of floating-point values in column-major order.")
		.def(self * k3d::normal3())
		.def(self * k3d::point3())
		.def(self * k3d::vector3())
		.def(self * self)
		.def(self == self)
		.def(self != self)
		.def(self - self)
		.def(self + self)
		.def(self * double())
		.def(double() * self)
		.def(self_ns::str(self));
}

} // namespace python

} // namespace k3d

