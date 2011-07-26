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

#include <boost/python.hpp>

#include <k3dsdk/algebra.h>
#include <k3dsdk/python/matrix4_python.h>
#include <k3dsdk/python/utility_python.h>


#include <vector>

namespace k3d
{

namespace python
{

static k3d::matrix4 row_major_factory(boost::python::object values)
{
	std::vector<double_t> temp;
	if(len(values) == 4)
	{
		for(uint_t i = 0; i != 4; ++i)
		{
			for(uint_t j = 0; j != 4; ++j)
				temp.push_back(boost::python::extract<double_t>(values[i][j])());
		}
	}
	else if(len(values) == 16)
	{
		for(uint_t i = 0; i != 16; ++i)
			temp.push_back(boost::python::extract<double_t>(values[i])());
	}
	else
	{
		throw std::runtime_error("row_major() requires a sequence of 16 values, or four sequences of four values.");
	}

	return k3d::matrix4::row_major(temp.begin(), temp.end());
}

static boost::python::list row_major_values(const k3d::matrix4& Self)
{
	boost::python::list results;
	for(uint_t i = 0; i != 4; ++i)
	{
		for(uint_t j = 0; j != 4; ++j)
		{
			results.append(Self[i][j]);
		}
	}

	return results;
}

static boost::python::list column_major_values(const k3d::matrix4& Self)
{
	boost::python::list results;
	for(uint_t i = 0; i != 4; ++i)
	{
		for(uint_t j = 0; j != 4; ++j)
		{
			results.append(Self[j][i]);
		}
	}

	return results;
}

void define_class_matrix4()
{
	boost::python::class_<k3d::matrix4>("matrix4",
		"Stores a 4x4 transformation matrix.")
		.def(boost::python::init<const k3d::matrix4&>())
		.def(boost::python::init<const k3d::vector4&, const k3d::vector4&, const k3d::vector4&, const k3d::vector4&>())
		.def("row_major", row_major_factory,
			"Constructs a matrix4 from a sequence of values in row-major order.")
		.staticmethod("row_major")
		.def("__len__", &utility::constant_len_len<k3d::matrix4, 4>)
		.def("__getitem__", &utility::constant_len_get_item<k3d::matrix4, 4, k3d::vector4>)
		.def("__setitem__", &utility::constant_len_set_item<k3d::matrix4, 4, k3d::vector4>)
		.def("row_major_values", row_major_values,
			"Returns the contents of the matrix as a list of floating-point values in row-major order.")
		.def("column_major_values", column_major_values,
			"Returns the contents of the matrix as a list of floating-point values in column-major order.")
		.def(boost::python::self * k3d::normal3())
		.def(boost::python::self * k3d::point3())
		.def(boost::python::self * k3d::vector3())
		.def(boost::python::self * boost::python::self)
		.def(boost::python::self == boost::python::self)
		.def(boost::python::self != boost::python::self)
		.def(boost::python::self - boost::python::self)
		.def(boost::python::self + boost::python::self)
		.def(boost::python::self * double())
		.def(double() * boost::python::self)
		.def(boost::python::self_ns::str(boost::python::self));
}

} // namespace python

} // namespace k3d

