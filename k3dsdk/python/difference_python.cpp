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
#include <k3dsdk/python/typed_array_python.h>
#include <k3dsdk/python/utility_python.h>

#include <boost/python.hpp>
#include <boost/python/detail/api_placeholder.hpp>
using namespace boost::python;

namespace k3d
{

namespace python
{

class difference
{
public:
	static void const_mesh(const_mesh_wrapper& A, const_mesh_wrapper& B, k3d::difference::accumulator& Result)
	{
		k3d::difference::test(A.wrapped(), B.wrapped(), Result);
	}

	static void mesh(mesh_wrapper& A, mesh_wrapper& B, k3d::difference::accumulator& Result)
	{
		k3d::difference::test(A.wrapped(), B.wrapped(), Result);
	}

	static void matrix4(k3d::matrix4& A, k3d::matrix4& B, k3d::difference::accumulator& Result)
	{
		k3d::difference::test(A, B, Result);
	}

	static void const_points(instance_wrapper<const k3d::mesh::points_t>& A, instance_wrapper<const k3d::mesh::points_t>& B, k3d::difference::accumulator& Result)
	{
		A.wrapped().difference(B.wrapped(), Result);
	}

	static void points(instance_wrapper<k3d::mesh::points_t>& A, instance_wrapper<k3d::mesh::points_t>& B, k3d::difference::accumulator& Result)
	{
		A.wrapped().difference(B.wrapped(), Result);
	}

	static void points_list(instance_wrapper<k3d::mesh::points_t>& A, list B, k3d::difference::accumulator& Result)
	{
		k3d::mesh::points_t b;
		utility::copy(B, b);
		A.wrapped().difference(b, Result);
	}

	static void const_points_list(instance_wrapper<const k3d::mesh::points_t>& A, list B, k3d::difference::accumulator& Result)
	{
		k3d::mesh::points_t b;
		utility::copy(B, b);
		A.wrapped().difference(b, Result);
	}
};

static std::size_t exact_count(k3d::difference::accumulator& Self) { return boost::accumulators::count(Self.exact); }
static k3d::bool_t exact_min(k3d::difference::accumulator& Self) { return boost::accumulators::min(Self.exact); }
static k3d::bool_t exact_max(k3d::difference::accumulator& Self) { return boost::accumulators::max(Self.exact); }
static std::size_t ulps_count(k3d::difference::accumulator& Self) { return boost::accumulators::count(Self.ulps); }
static k3d::double_t ulps_min(k3d::difference::accumulator& Self) { return boost::accumulators::min(Self.ulps); }
static k3d::double_t ulps_max(k3d::difference::accumulator& Self) { return boost::accumulators::max(Self.ulps); }
static k3d::double_t ulps_mean(k3d::difference::accumulator& Self) { return boost::accumulators::mean(Self.ulps); }
static k3d::double_t ulps_median(k3d::difference::accumulator& Self) { return boost::accumulators::median(Self.ulps); }
//static k3d::double_t ulps_variance(k3d::difference::accumulator& Self) { return boost::accumulators::variance(Self.ulps); }
//static k3d::double_t ulps_standard_deviation(k3d::difference::accumulator& Self) { return std::sqrt(boost::accumulators::variance(Self.ulps)); }

void define_namespace_difference()
{
	boost::python::scope outer = boost::python::class_<difference>("difference", boost::python::no_init)
		.def("test", &difference::const_mesh,
			"Computes the difference between two meshes using fuzzy-comparisons for floating-point types.")
		.def("test", &difference::mesh,
			"Computes the difference between two meshes using fuzzy-comparisons for floating-point types.")
		.def("test", &difference::matrix4,
			"Computes the difference between two matrices using fuzzy-comparisons for floating-point types.")
		.def("test", &difference::points,
			"Computes the difference between two point arrays using fuzzy-comparisons for floating-point types.")
		.def("test", &difference::const_points,
			"Computes the difference between two point arrays using fuzzy-comparisons for floating-point types.")
		.def("test", &difference::points_list,
			"Computes the difference between a point array and a list using fuzzy-comparisons for floating-point types.")
		.def("test", &difference::const_points_list,
			"Computes the difference between a point array and a list using fuzzy-comparisons for floating-point types.")
		.staticmethod("test")
		;

	boost::python::class_<k3d::difference::accumulator>("accumulator")
		.def("exact_count", &exact_count)
		.def("exact_min", &exact_min)
		.def("exact_max", &exact_max)

		.def("ulps_count", &ulps_count)
		.def("ulps_min", &ulps_min)
		.def("ulps_max", &ulps_max)
		.def("ulps_mean", &ulps_mean)
		.def("ulps_median", &ulps_median)
		//.def("ulps_variance", &ulps_variance)
		//.def("ulps_standard_deviation", &ulps_standard_deviation)
		;
}

} // namespace python

} // namespace k3d

