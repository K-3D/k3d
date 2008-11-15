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

#include "mesh_python.h"
#include "owned_instance_wrapper_python.h"
#include "cubic_curve_python.h"

#include <k3dsdk/cubic_curve.h>

#include <boost/python.hpp>
#include <boost/python/detail/api_placeholder.hpp>
using namespace boost::python;
#include <boost/scoped_ptr.hpp>

namespace k3d
{

namespace python
{

class cubic_curve
{
public:
	class const_primitive
	{
	public:
		typedef owned_instance_wrapper<k3d::cubic_curve::const_primitive> wrapper;

		static object first_curves(wrapper& Self) { return wrap(Self.wrapped().first_curves); }
		static object curve_counts(wrapper& Self) { return wrap(Self.wrapped().curve_counts); }
		static object periodic_curves(wrapper& Self) { return wrap(Self.wrapped().periodic_curves); }
		static object materials(wrapper& Self) { return wrap(Self.wrapped().materials); }
		static object curve_first_points(wrapper& Self) { return wrap(Self.wrapped().curve_first_points); }
		static object curve_point_counts(wrapper& Self) { return wrap(Self.wrapped().curve_point_counts); }
		static object curve_selections(wrapper& Self) { return wrap(Self.wrapped().curve_selections); }
		static object curve_points(wrapper& Self) { return wrap(Self.wrapped().curve_points); }
		static object constant_data(wrapper& Self) { return wrap(Self.wrapped().constant_data); }
		static object uniform_data(wrapper& Self) { return wrap(Self.wrapped().uniform_data); }
		static object varying_data(wrapper& Self) { return wrap(Self.wrapped().varying_data); }
	};

	class primitive
	{
	public:
		typedef owned_instance_wrapper<k3d::cubic_curve::primitive> wrapper;

		static object first_curves(wrapper& Self) { return wrap(Self.wrapped().first_curves); }
		static object curve_counts(wrapper& Self) { return wrap(Self.wrapped().curve_counts); }
		static object periodic_curves(wrapper& Self) { return wrap(Self.wrapped().periodic_curves); }
		static object materials(wrapper& Self) { return wrap(Self.wrapped().materials); }
		static object curve_first_points(wrapper& Self) { return wrap(Self.wrapped().curve_first_points); }
		static object curve_point_counts(wrapper& Self) { return wrap(Self.wrapped().curve_point_counts); }
		static object curve_selections(wrapper& Self) { return wrap(Self.wrapped().curve_selections); }
		static object curve_points(wrapper& Self) { return wrap(Self.wrapped().curve_points); }
		static object constant_data(wrapper& Self) { return wrap(Self.wrapped().constant_data); }
		static object uniform_data(wrapper& Self) { return wrap(Self.wrapped().uniform_data); }
		static object varying_data(wrapper& Self) { return wrap(Self.wrapped().varying_data); }
	};


	static object create(mesh& Mesh)
	{
		return wrap_owned(k3d::cubic_curve::create(Mesh.wrapped()));
	}

	static object validate(mesh_primitive_wrapper& Primitive)
	{
		return wrap_owned(k3d::cubic_curve::validate(Primitive.wrapped()));
	}

	static object validate_const(const_mesh_primitive_wrapper& Primitive)
	{
		return wrap_owned(k3d::cubic_curve::validate(Primitive.wrapped()));
	}
};

void define_namespace_cubic_curve()
{
	scope outer = class_<cubic_curve>("cubic_curve", no_init)
		.def("create", &cubic_curve::create)
		.staticmethod("create")
		.def("validate", &cubic_curve::validate)
		.def("validate", &cubic_curve::validate_const)
		.staticmethod("validate")
		;

	class_<cubic_curve::const_primitive::wrapper>("const_primitive", no_init)
		.def("first_curves", &cubic_curve::const_primitive::first_curves)
		.def("curve_counts", &cubic_curve::const_primitive::curve_counts)
		.def("periodic_curves", &cubic_curve::const_primitive::periodic_curves)
		.def("materials", &cubic_curve::const_primitive::materials)
		.def("curve_first_points", &cubic_curve::const_primitive::curve_first_points)
		.def("curve_point_counts", &cubic_curve::const_primitive::curve_point_counts)
		.def("curve_selections", &cubic_curve::const_primitive::curve_selections)
		.def("curve_points", &cubic_curve::const_primitive::curve_points)
		.def("constant_data", &cubic_curve::const_primitive::constant_data)
		.def("uniform_data", &cubic_curve::const_primitive::uniform_data)
		.def("varying_data", &cubic_curve::const_primitive::varying_data)
		;

	class_<cubic_curve::primitive::wrapper>("primitive", no_init)
		.def("first_curves", &cubic_curve::primitive::first_curves)
		.def("curve_counts", &cubic_curve::primitive::curve_counts)
		.def("periodic_curves", &cubic_curve::primitive::periodic_curves)
		.def("materials", &cubic_curve::primitive::materials)
		.def("curve_first_points", &cubic_curve::primitive::curve_first_points)
		.def("curve_point_counts", &cubic_curve::primitive::curve_point_counts)
		.def("curve_selections", &cubic_curve::primitive::curve_selections)
		.def("curve_points", &cubic_curve::primitive::curve_points)
		.def("constant_data", &cubic_curve::primitive::constant_data)
		.def("uniform_data", &cubic_curve::primitive::uniform_data)
		.def("varying_data", &cubic_curve::primitive::varying_data)
		;
}

} // namespace python

} // namespace k3d

