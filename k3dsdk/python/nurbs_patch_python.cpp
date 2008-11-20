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
#include "nurbs_patch_python.h"
#include "utility_python.h"

#include <k3dsdk/nurbs_patch.h>

#include <boost/python.hpp>
#include <boost/python/detail/api_placeholder.hpp>
using namespace boost::python;
#include <boost/scoped_ptr.hpp>

namespace k3d
{

namespace python
{

class nurbs_patch
{
public:
	class const_primitive
	{
	public:
		typedef owned_instance_wrapper<k3d::nurbs_patch::const_primitive> wrapper;

		static object patch_first_points(wrapper& Self) { return wrap(Self.wrapped().patch_first_points); }
		static object patch_u_point_counts(wrapper& Self) { return wrap(Self.wrapped().patch_u_point_counts); }
		static object patch_v_point_counts(wrapper& Self) { return wrap(Self.wrapped().patch_v_point_counts); }
		static object patch_u_orders(wrapper& Self) { return wrap(Self.wrapped().patch_u_orders); }
		static object patch_v_orders(wrapper& Self) { return wrap(Self.wrapped().patch_v_orders); }
		static object patch_u_first_knots(wrapper& Self) { return wrap(Self.wrapped().patch_u_first_knots); }
		static object patch_v_first_knots(wrapper& Self) { return wrap(Self.wrapped().patch_v_first_knots); }
		static object patch_selections(wrapper& Self) { return wrap(Self.wrapped().patch_selections); }
		static object patch_materials(wrapper& Self) { return wrap(Self.wrapped().patch_materials); }
		static object patch_points(wrapper& Self) { return wrap(Self.wrapped().patch_points); }
		static object patch_point_weights(wrapper& Self) { return wrap(Self.wrapped().patch_point_weights); }
		static object patch_u_knots(wrapper& Self) { return wrap(Self.wrapped().patch_u_knots); }
		static object patch_v_knots(wrapper& Self) { return wrap(Self.wrapped().patch_v_knots); }
		static object patch_trim_loop_counts(wrapper& Self) { return wrap(Self.wrapped().patch_trim_loop_counts); }
		static object patch_first_trim_loops(wrapper& Self) { return wrap(Self.wrapped().patch_first_trim_loops); }
		static object trim_loop_first_curves(wrapper& Self) { return wrap(Self.wrapped().trim_loop_first_curves); }
		static object trim_loop_curve_counts(wrapper& Self) { return wrap(Self.wrapped().trim_loop_curve_counts); }
		static object trim_loop_selections(wrapper& Self) { return wrap(Self.wrapped().trim_loop_selections); }
		static object curve_first_points(wrapper& Self) { return wrap(Self.wrapped().curve_first_points); }
		static object curve_point_counts(wrapper& Self) { return wrap(Self.wrapped().curve_point_counts); }
		static object curve_orders(wrapper& Self) { return wrap(Self.wrapped().curve_orders); }
		static object curve_first_knots(wrapper& Self) { return wrap(Self.wrapped().curve_first_knots); }
		static object curve_selections(wrapper& Self) { return wrap(Self.wrapped().curve_selections); }
		static object curve_points(wrapper& Self) { return wrap(Self.wrapped().curve_points); }
		static object curve_point_weights(wrapper& Self) { return wrap(Self.wrapped().curve_point_weights); }
		static object curve_knots(wrapper& Self) { return wrap(Self.wrapped().curve_knots); }
		static object points(wrapper& Self) { return wrap(Self.wrapped().points); }
		static object point_selections(wrapper& Self) { return wrap(Self.wrapped().point_selections); }
		static object constant_data(wrapper& Self) { return wrap(Self.wrapped().constant_data); }
		static object uniform_data(wrapper& Self) { return wrap(Self.wrapped().uniform_data); }
		static object varying_data(wrapper& Self) { return wrap(Self.wrapped().varying_data); }
	};

	class primitive
	{
	public:
		typedef owned_instance_wrapper<k3d::nurbs_patch::primitive> wrapper;

		static object patch_first_points(wrapper& Self) { return wrap(Self.wrapped().patch_first_points); }
		static object patch_u_point_counts(wrapper& Self) { return wrap(Self.wrapped().patch_u_point_counts); }
		static object patch_v_point_counts(wrapper& Self) { return wrap(Self.wrapped().patch_v_point_counts); }
		static object patch_u_orders(wrapper& Self) { return wrap(Self.wrapped().patch_u_orders); }
		static object patch_v_orders(wrapper& Self) { return wrap(Self.wrapped().patch_v_orders); }
		static object patch_u_first_knots(wrapper& Self) { return wrap(Self.wrapped().patch_u_first_knots); }
		static object patch_v_first_knots(wrapper& Self) { return wrap(Self.wrapped().patch_v_first_knots); }
		static object patch_selections(wrapper& Self) { return wrap(Self.wrapped().patch_selections); }
		static object patch_materials(wrapper& Self) { return wrap(Self.wrapped().patch_materials); }
		static object patch_points(wrapper& Self) { return wrap(Self.wrapped().patch_points); }
		static object patch_point_weights(wrapper& Self) { return wrap(Self.wrapped().patch_point_weights); }
		static object patch_u_knots(wrapper& Self) { return wrap(Self.wrapped().patch_u_knots); }
		static object patch_v_knots(wrapper& Self) { return wrap(Self.wrapped().patch_v_knots); }
		static object patch_trim_loop_counts(wrapper& Self) { return wrap(Self.wrapped().patch_trim_loop_counts); }
		static object patch_first_trim_loops(wrapper& Self) { return wrap(Self.wrapped().patch_first_trim_loops); }
		static object trim_loop_first_curves(wrapper& Self) { return wrap(Self.wrapped().trim_loop_first_curves); }
		static object trim_loop_curve_counts(wrapper& Self) { return wrap(Self.wrapped().trim_loop_curve_counts); }
		static object trim_loop_selections(wrapper& Self) { return wrap(Self.wrapped().trim_loop_selections); }
		static object curve_first_points(wrapper& Self) { return wrap(Self.wrapped().curve_first_points); }
		static object curve_point_counts(wrapper& Self) { return wrap(Self.wrapped().curve_point_counts); }
		static object curve_orders(wrapper& Self) { return wrap(Self.wrapped().curve_orders); }
		static object curve_first_knots(wrapper& Self) { return wrap(Self.wrapped().curve_first_knots); }
		static object curve_selections(wrapper& Self) { return wrap(Self.wrapped().curve_selections); }
		static object curve_points(wrapper& Self) { return wrap(Self.wrapped().curve_points); }
		static object curve_point_weights(wrapper& Self) { return wrap(Self.wrapped().curve_point_weights); }
		static object curve_knots(wrapper& Self) { return wrap(Self.wrapped().curve_knots); }
		static object points(wrapper& Self) { return wrap(Self.wrapped().points); }
		static object point_selections(wrapper& Self) { return wrap(Self.wrapped().point_selections); }
		static object constant_data(wrapper& Self) { return wrap(Self.wrapped().constant_data); }
		static object uniform_data(wrapper& Self) { return wrap(Self.wrapped().uniform_data); }
		static object varying_data(wrapper& Self) { return wrap(Self.wrapped().varying_data); }
	};


	static object create(mesh& Mesh)
	{
		return wrap_owned(k3d::nurbs_patch::create(Mesh.wrapped()));
	}

/*
	static object validate(mesh_primitive_wrapper& Primitive)
	{
		return wrap_owned(k3d::nurbs_patch::validate(Primitive.wrapped()));
	}

	static object validate_const(const_mesh_primitive_wrapper& Primitive)
	{
		return wrap_owned(k3d::nurbs_patch::validate(Primitive.wrapped()));
	}
*/
};

void define_namespace_nurbs_patch()
{
	scope outer = class_<nurbs_patch>("nurbs_patch", no_init)
		.def("create", &nurbs_patch::create)
		.staticmethod("create")
/*
		.def("validate", &nurbs_patch::validate)
		.def("validate", &nurbs_patch::validate_const)
		.staticmethod("validate")
*/
		;

	class_<nurbs_patch::const_primitive::wrapper>("const_primitive", no_init)
		.def("patch_first_points", &nurbs_patch::const_primitive::patch_first_points)
		.def("patch_u_point_counts", &nurbs_patch::const_primitive::patch_u_point_counts)
		.def("patch_v_point_counts", &nurbs_patch::const_primitive::patch_v_point_counts)
		.def("patch_u_orders", &nurbs_patch::const_primitive::patch_u_orders)
		.def("patch_v_orders", &nurbs_patch::const_primitive::patch_v_orders)
		.def("patch_u_first_knots", &nurbs_patch::const_primitive::patch_u_first_knots)
		.def("patch_v_first_knots", &nurbs_patch::const_primitive::patch_v_first_knots)
		.def("patch_selections", &nurbs_patch::const_primitive::patch_selections)
		.def("patch_materials", &nurbs_patch::const_primitive::patch_materials)
		.def("patch_points", &nurbs_patch::const_primitive::patch_points)
		.def("patch_point_weights", &nurbs_patch::const_primitive::patch_point_weights)
		.def("patch_u_knots", &nurbs_patch::const_primitive::patch_u_knots)
		.def("patch_v_knots", &nurbs_patch::const_primitive::patch_v_knots)
		.def("patch_trim_loop_counts", &nurbs_patch::const_primitive::patch_trim_loop_counts)
		.def("patch_first_trim_loops", &nurbs_patch::const_primitive::patch_first_trim_loops)
		.def("trim_loop_first_curves", &nurbs_patch::const_primitive::trim_loop_first_curves)
		.def("trim_loop_curve_counts", &nurbs_patch::const_primitive::trim_loop_curve_counts)
		.def("trim_loop_selections", &nurbs_patch::const_primitive::trim_loop_selections)
		.def("curve_first_points", &nurbs_patch::const_primitive::curve_first_points)
		.def("curve_point_counts", &nurbs_patch::const_primitive::curve_point_counts)
		.def("curve_orders", &nurbs_patch::const_primitive::curve_orders)
		.def("curve_first_knots", &nurbs_patch::const_primitive::curve_first_knots)
		.def("curve_selections", &nurbs_patch::const_primitive::curve_selections)
		.def("curve_points", &nurbs_patch::const_primitive::curve_points)
		.def("curve_point_weights", &nurbs_patch::const_primitive::curve_point_weights)
		.def("curve_knots", &nurbs_patch::const_primitive::curve_knots)
		.def("points", &nurbs_patch::const_primitive::points)
		.def("point_selections", &nurbs_patch::const_primitive::point_selections)
		.def("constant_data", &nurbs_patch::const_primitive::constant_data)
		.def("uniform_data", &nurbs_patch::const_primitive::uniform_data)
		.def("varying_data", &nurbs_patch::const_primitive::varying_data)
		;

	class_<nurbs_patch::primitive::wrapper>("primitive", no_init)
		.def("patch_first_points", &nurbs_patch::primitive::patch_first_points)
		.def("patch_u_point_counts", &nurbs_patch::primitive::patch_u_point_counts)
		.def("patch_v_point_counts", &nurbs_patch::primitive::patch_v_point_counts)
		.def("patch_u_orders", &nurbs_patch::primitive::patch_u_orders)
		.def("patch_v_orders", &nurbs_patch::primitive::patch_v_orders)
		.def("patch_u_first_knots", &nurbs_patch::primitive::patch_u_first_knots)
		.def("patch_v_first_knots", &nurbs_patch::primitive::patch_v_first_knots)
		.def("patch_selections", &nurbs_patch::primitive::patch_selections)
		.def("patch_materials", &nurbs_patch::primitive::patch_materials)
		.def("patch_points", &nurbs_patch::primitive::patch_points)
		.def("patch_point_weights", &nurbs_patch::primitive::patch_point_weights)
		.def("patch_u_knots", &nurbs_patch::primitive::patch_u_knots)
		.def("patch_v_knots", &nurbs_patch::primitive::patch_v_knots)
		.def("patch_trim_loop_counts", &nurbs_patch::primitive::patch_trim_loop_counts)
		.def("patch_first_trim_loops", &nurbs_patch::primitive::patch_first_trim_loops)
		.def("trim_loop_first_curves", &nurbs_patch::primitive::trim_loop_first_curves)
		.def("trim_loop_curve_counts", &nurbs_patch::primitive::trim_loop_curve_counts)
		.def("trim_loop_selections", &nurbs_patch::primitive::trim_loop_selections)
		.def("curve_first_points", &nurbs_patch::primitive::curve_first_points)
		.def("curve_point_counts", &nurbs_patch::primitive::curve_point_counts)
		.def("curve_orders", &nurbs_patch::primitive::curve_orders)
		.def("curve_first_knots", &nurbs_patch::primitive::curve_first_knots)
		.def("curve_selections", &nurbs_patch::primitive::curve_selections)
		.def("curve_points", &nurbs_patch::primitive::curve_points)
		.def("curve_point_weights", &nurbs_patch::primitive::curve_point_weights)
		.def("curve_knots", &nurbs_patch::primitive::curve_knots)
		.def("points", &nurbs_patch::primitive::points)
		.def("point_selections", &nurbs_patch::primitive::point_selections)
		.def("constant_data", &nurbs_patch::primitive::constant_data)
		.def("uniform_data", &nurbs_patch::primitive::uniform_data)
		.def("varying_data", &nurbs_patch::primitive::varying_data)
		;
}

} // namespace python

} // namespace k3d

