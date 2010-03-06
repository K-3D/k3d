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

#include <k3dsdk/python/mesh_python.h>
#include <k3dsdk/python/owned_instance_wrapper_python.h>
#include <k3dsdk/python/linear_curve_python.h>

#include <k3dsdk/linear_curve.h>

#include <boost/python.hpp>
#include <boost/python/detail/api_placeholder.hpp>
using namespace boost::python;
#include <boost/scoped_ptr.hpp>

namespace k3d
{

namespace python
{

class linear_curve
{
public:
	class const_primitive
	{
	public:
		typedef owned_instance_wrapper<k3d::linear_curve::const_primitive> wrapper;

		static object periodic(wrapper& Self) { return wrap(Self.wrapped().periodic); }
		static object material(wrapper& Self) { return wrap(Self.wrapped().material); }
		static object curve_first_points(wrapper& Self) { return wrap(Self.wrapped().curve_first_points); }
		static object curve_point_counts(wrapper& Self) { return wrap(Self.wrapped().curve_point_counts); }
		static object curve_selections(wrapper& Self) { return wrap(Self.wrapped().curve_selections); }
		static object curve_points(wrapper& Self) { return wrap(Self.wrapped().curve_points); }
		static object constant_attributes(wrapper& Self) { return wrap(Self.wrapped().constant_attributes); }
		static object curve_attributes(wrapper& Self) { return wrap(Self.wrapped().curve_attributes); }
		static object parameter_attributes(wrapper& Self) { return wrap(Self.wrapped().parameter_attributes); }
		static object vertex_attributes(wrapper& Self) { return wrap(Self.wrapped().vertex_attributes); }
	};

	class primitive
	{
	public:
		typedef owned_instance_wrapper<k3d::linear_curve::primitive> wrapper;

		static object periodic(wrapper& Self) { return wrap(Self.wrapped().periodic); }
		static object material(wrapper& Self) { return wrap(Self.wrapped().material); }
		static object curve_first_points(wrapper& Self) { return wrap(Self.wrapped().curve_first_points); }
		static object curve_point_counts(wrapper& Self) { return wrap(Self.wrapped().curve_point_counts); }
		static object curve_selections(wrapper& Self) { return wrap(Self.wrapped().curve_selections); }
		static object curve_points(wrapper& Self) { return wrap(Self.wrapped().curve_points); }
		static object constant_attributes(wrapper& Self) { return wrap(Self.wrapped().constant_attributes); }
		static object curve_attributes(wrapper& Self) { return wrap(Self.wrapped().curve_attributes); }
		static object parameter_attributes(wrapper& Self) { return wrap(Self.wrapped().parameter_attributes); }
		static object vertex_attributes(wrapper& Self) { return wrap(Self.wrapped().vertex_attributes); }
	};


	static object create(mesh_wrapper& Mesh)
	{
		return wrap_owned(k3d::linear_curve::create(Mesh.wrapped()));
	}

	static object validate(mesh_wrapper& Mesh, mesh_primitive_wrapper& Primitive)
	{
		return wrap_owned(k3d::linear_curve::validate(Mesh.wrapped(), Primitive.wrapped()));
	}

	static object validate_const(const_mesh_wrapper& Mesh, const_mesh_primitive_wrapper& Primitive)
	{
		return wrap_owned(k3d::linear_curve::validate(Mesh.wrapped(), Primitive.wrapped()));
	}
};

void define_namespace_linear_curve()
{
	scope outer = class_<linear_curve>("linear_curve", no_init)
		.def("create", &linear_curve::create)
		.staticmethod("create")
		.def("validate", &linear_curve::validate)
		.def("validate", &linear_curve::validate_const)
		.staticmethod("validate")
		;

	class_<linear_curve::const_primitive::wrapper>("const_primitive", no_init)
		.def("periodic", &linear_curve::const_primitive::periodic)
		.def("material", &linear_curve::const_primitive::material)
		.def("curve_first_points", &linear_curve::const_primitive::curve_first_points)
		.def("curve_point_counts", &linear_curve::const_primitive::curve_point_counts)
		.def("curve_selections", &linear_curve::const_primitive::curve_selections)
		.def("curve_points", &linear_curve::const_primitive::curve_points)
		.def("constant_attributes", &linear_curve::const_primitive::constant_attributes)
		.def("curve_attributes", &linear_curve::const_primitive::curve_attributes)
		.def("parameter_attributes", &linear_curve::const_primitive::parameter_attributes)
		.def("vertex_attributes", &linear_curve::const_primitive::vertex_attributes)
		;

	class_<linear_curve::primitive::wrapper>("primitive", no_init)
		.def("periodic", &linear_curve::primitive::periodic)
		.def("material", &linear_curve::primitive::material)
		.def("curve_first_points", &linear_curve::primitive::curve_first_points)
		.def("curve_point_counts", &linear_curve::primitive::curve_point_counts)
		.def("curve_selections", &linear_curve::primitive::curve_selections)
		.def("curve_points", &linear_curve::primitive::curve_points)
		.def("constant_attributes", &linear_curve::primitive::constant_attributes)
		.def("curve_attributes", &linear_curve::primitive::curve_attributes)
		.def("parameter_attributes", &linear_curve::primitive::parameter_attributes)
		.def("vertex_attributes", &linear_curve::primitive::vertex_attributes)
		;
}

} // namespace python

} // namespace k3d

