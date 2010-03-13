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

#include <k3dsdk/algebra.h>
#include <k3dsdk/difference.h>
#include <k3dsdk/python/difference_python.h>
#include <k3dsdk/python/mesh_python.h>

#include <boost/python.hpp>

namespace k3d
{

namespace python
{

class difference
{
public:
	static void const_mesh(const_mesh_wrapper& A, const_mesh_wrapper& B, k3d::difference::test_result& Result)
	{
		k3d::difference::test(A.wrapped(), B.wrapped(), Result);
	}

	static void mesh(mesh_wrapper& A, mesh_wrapper& B, k3d::difference::test_result& Result)
	{
		k3d::difference::test(A.wrapped(), B.wrapped(), Result);
	}

	static void matrix4(k3d::matrix4& A, k3d::matrix4& B, k3d::difference::test_result& Result)
	{
		k3d::difference::test(A, B, Result);
	}

	static void double_t(k3d::double_t& A, k3d::double_t& B, k3d::difference::test_result& Result)
	{
		k3d::difference::test(A, B, Result);
	}
};

void define_namespace_difference()
{
	boost::python::scope outer = boost::python::class_<difference>("difference", boost::python::no_init)
		.def("test", &difference::const_mesh,
			"Returns the difference between two meshes using fuzzy-comparisons for floating-point types.")
		.def("test", &difference::mesh,
			"Returns the difference between two meshes using fuzzy-comparisons for floating-point types.")
		.def("test", &difference::matrix4,
			"Returns the difference between two matrices using fuzzy-comparisons for floating-point types.")
		.def("test", &difference::double_t,
			"Returns the difference between two doubles using fuzzy-comparisons for floating-point types.")
		.staticmethod("test")
		;

	boost::python::class_<k3d::difference::test_result>("test_result")
		.def_readonly("count", &k3d::difference::test_result::count)
		.def_readonly("equal", &k3d::difference::test_result::equal)
		.def_readonly("ulps", &k3d::difference::test_result::ulps)
		.def_readonly("relative_error", &k3d::difference::test_result::relative_error)
		;
}

} // namespace python

} // namespace k3d

