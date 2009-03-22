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
#include "nurbs_curve_python.h"
#include "utility_python.h"

#include <k3dsdk/nurbs_curve.h>

#include <boost/python.hpp>
#include <boost/python/detail/api_placeholder.hpp>
using namespace boost::python;
#include <boost/scoped_ptr.hpp>

namespace k3d
{

namespace python
{

class nurbs_curve
{
public:
	class const_primitive
	{
	public:
		typedef owned_instance_wrapper<k3d::nurbs_curve::const_primitive> wrapper;

		static object material(wrapper& Self) { return wrap(Self.wrapped().material); }
		static object curve_first_points(wrapper& Self) { return wrap(Self.wrapped().curve_first_points); }
		static object curve_point_counts(wrapper& Self) { return wrap(Self.wrapped().curve_point_counts); }
		static object curve_orders(wrapper& Self) { return wrap(Self.wrapped().curve_orders); }
		static object curve_first_knots(wrapper& Self) { return wrap(Self.wrapped().curve_first_knots); }
		static object curve_selections(wrapper& Self) { return wrap(Self.wrapped().curve_selections); }
		static object curve_points(wrapper& Self) { return wrap(Self.wrapped().curve_points); }
		static object curve_point_weights(wrapper& Self) { return wrap(Self.wrapped().curve_point_weights); }
		static object curve_knots(wrapper& Self) { return wrap(Self.wrapped().curve_knots); }
		static object constant_data(wrapper& Self) { return wrap(Self.wrapped().constant_data); }
		static object uniform_data(wrapper& Self) { return wrap(Self.wrapped().uniform_data); }
		static object varying_data(wrapper& Self) { return wrap(Self.wrapped().varying_data); }
	};

	class primitive
	{
	public:
		typedef owned_instance_wrapper<k3d::nurbs_curve::primitive> wrapper;

		static object material(wrapper& Self) { return wrap(Self.wrapped().material); }
		static object curve_first_points(wrapper& Self) { return wrap(Self.wrapped().curve_first_points); }
		static object curve_point_counts(wrapper& Self) { return wrap(Self.wrapped().curve_point_counts); }
		static object curve_orders(wrapper& Self) { return wrap(Self.wrapped().curve_orders); }
		static object curve_first_knots(wrapper& Self) { return wrap(Self.wrapped().curve_first_knots); }
		static object curve_selections(wrapper& Self) { return wrap(Self.wrapped().curve_selections); }
		static object curve_points(wrapper& Self) { return wrap(Self.wrapped().curve_points); }
		static object curve_point_weights(wrapper& Self) { return wrap(Self.wrapped().curve_point_weights); }
		static object curve_knots(wrapper& Self) { return wrap(Self.wrapped().curve_knots); }
		static object constant_data(wrapper& Self) { return wrap(Self.wrapped().constant_data); }
		static object uniform_data(wrapper& Self) { return wrap(Self.wrapped().uniform_data); }
		static object varying_data(wrapper& Self) { return wrap(Self.wrapped().varying_data); }
	};


	static object create(mesh& Mesh)
	{
		return wrap_owned(k3d::nurbs_curve::create(Mesh.wrapped()));
	}

	static object validate(mesh_primitive_wrapper& Primitive)
	{
		return wrap_owned(k3d::nurbs_curve::validate(Primitive.wrapped()));
	}

	static object validate_const(const_mesh_primitive_wrapper& Primitive)
	{
		return wrap_owned(k3d::nurbs_curve::validate(Primitive.wrapped()));
	}
};

void define_namespace_nurbs_curve()
{
	scope outer = class_<nurbs_curve>("nurbs_curve", no_init)
		.def("create", &nurbs_curve::create)
		.staticmethod("create")
		.def("validate", &nurbs_curve::validate)
		.def("validate", &nurbs_curve::validate_const)
		.staticmethod("validate")
		;

	class_<nurbs_curve::const_primitive::wrapper>("const_primitive", no_init)
		.def("material", &nurbs_curve::const_primitive::material)
		.def("curve_first_points", &nurbs_curve::const_primitive::curve_first_points)
		.def("curve_point_counts", &nurbs_curve::const_primitive::curve_point_counts)
		.def("curve_orders", &nurbs_curve::const_primitive::curve_orders)
		.def("curve_first_knots", &nurbs_curve::const_primitive::curve_first_knots)
		.def("curve_selections", &nurbs_curve::const_primitive::curve_selections)
		.def("curve_points", &nurbs_curve::const_primitive::curve_points)
		.def("curve_point_weights", &nurbs_curve::const_primitive::curve_point_weights)
		.def("curve_knots", &nurbs_curve::const_primitive::curve_knots)
		.def("constant_data", &nurbs_curve::const_primitive::constant_data)
		.def("uniform_data", &nurbs_curve::const_primitive::uniform_data)
		.def("varying_data", &nurbs_curve::const_primitive::varying_data)
		;

	class_<nurbs_curve::primitive::wrapper>("primitive", no_init)
		.def("material", &nurbs_curve::primitive::material)
		.def("curve_first_points", &nurbs_curve::primitive::curve_first_points)
		.def("curve_point_counts", &nurbs_curve::primitive::curve_point_counts)
		.def("curve_orders", &nurbs_curve::primitive::curve_orders)
		.def("curve_first_knots", &nurbs_curve::primitive::curve_first_knots)
		.def("curve_selections", &nurbs_curve::primitive::curve_selections)
		.def("curve_points", &nurbs_curve::primitive::curve_points)
		.def("curve_point_weights", &nurbs_curve::primitive::curve_point_weights)
		.def("curve_knots", &nurbs_curve::primitive::curve_knots)
		.def("constant_data", &nurbs_curve::primitive::constant_data)
		.def("uniform_data", &nurbs_curve::primitive::uniform_data)
		.def("varying_data", &nurbs_curve::primitive::varying_data)
		;
}

} // namespace python

} // namespace k3d

