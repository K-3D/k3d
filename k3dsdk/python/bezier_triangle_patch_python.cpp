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
#include "bezier_triangle_patch_python.h"

#include <k3dsdk/bezier_triangle_patch.h>

#include <boost/python.hpp>
#include <boost/python/detail/api_placeholder.hpp>
using namespace boost::python;
#include <boost/scoped_ptr.hpp>

namespace k3d
{

namespace python
{

class bezier_triangle_patch
{
public:
	class const_primitive
	{
	public:
		typedef owned_instance_wrapper<k3d::bezier_triangle_patch::const_primitive> wrapper;

		static object patch_first_points(wrapper& Self) { return wrap(Self.wrapped().patch_first_points); }
		static object patch_orders(wrapper& Self) { return wrap(Self.wrapped().patch_orders); }
		static object patch_selections(wrapper& Self) { return wrap(Self.wrapped().patch_selections); }
		static object patch_materials(wrapper& Self) { return wrap(Self.wrapped().patch_materials); }
		static object patch_points(wrapper& Self) { return wrap(Self.wrapped().patch_points); }
		static object patch_point_weights(wrapper& Self) { return wrap(Self.wrapped().patch_point_weights); }
		static object constant_data(wrapper& Self) { return wrap(Self.wrapped().constant_data); }
		static object uniform_data(wrapper& Self) { return wrap(Self.wrapped().uniform_data); }
		static object varying_data(wrapper& Self) { return wrap(Self.wrapped().varying_data); }
	};

	class primitive
	{
	public:
		typedef owned_instance_wrapper<k3d::bezier_triangle_patch::primitive> wrapper;

		static object patch_first_points(wrapper& Self) { return wrap(Self.wrapped().patch_first_points); }
		static object patch_orders(wrapper& Self) { return wrap(Self.wrapped().patch_orders); }
		static object patch_selections(wrapper& Self) { return wrap(Self.wrapped().patch_selections); }
		static object patch_materials(wrapper& Self) { return wrap(Self.wrapped().patch_materials); }
		static object patch_points(wrapper& Self) { return wrap(Self.wrapped().patch_points); }
		static object patch_point_weights(wrapper& Self) { return wrap(Self.wrapped().patch_point_weights); }
		static object constant_data(wrapper& Self) { return wrap(Self.wrapped().constant_data); }
		static object uniform_data(wrapper& Self) { return wrap(Self.wrapped().uniform_data); }
		static object varying_data(wrapper& Self) { return wrap(Self.wrapped().varying_data); }
	};


	static object create(mesh& Mesh)
	{
		return wrap_owned(k3d::bezier_triangle_patch::create(Mesh.wrapped()));
	}

	static object validate(mesh_primitive_wrapper& Primitive)
	{
		return wrap_owned(k3d::bezier_triangle_patch::validate(Primitive.wrapped()));
	}

	static object validate_const(const_mesh_primitive_wrapper& Primitive)
	{
		return wrap_owned(k3d::bezier_triangle_patch::validate(Primitive.wrapped()));
	}
};

void define_namespace_bezier_triangle_patch()
{
	scope outer = class_<bezier_triangle_patch>("bezier_triangle_patch", no_init)
		.def("create", &bezier_triangle_patch::create)
		.staticmethod("create")
		.def("validate", &bezier_triangle_patch::validate)
		.def("validate", &bezier_triangle_patch::validate_const)
		.staticmethod("validate")
		;

	class_<bezier_triangle_patch::const_primitive::wrapper>("const_primitive", no_init)
		.def("patch_first_points", &bezier_triangle_patch::const_primitive::patch_first_points)
		.def("patch_orders", &bezier_triangle_patch::const_primitive::patch_orders)
		.def("patch_selections", &bezier_triangle_patch::const_primitive::patch_selections)
		.def("patch_materials", &bezier_triangle_patch::const_primitive::patch_materials)
		.def("patch_points", &bezier_triangle_patch::const_primitive::patch_points)
		.def("patch_point_weights", &bezier_triangle_patch::const_primitive::patch_point_weights)
		.def("constant_data", &bezier_triangle_patch::const_primitive::constant_data)
		.def("uniform_data", &bezier_triangle_patch::const_primitive::uniform_data)
		.def("varying_data", &bezier_triangle_patch::const_primitive::varying_data)
		;

	class_<bezier_triangle_patch::primitive::wrapper>("primitive", no_init)
		.def("patch_first_points", &bezier_triangle_patch::primitive::patch_first_points)
		.def("patch_orders", &bezier_triangle_patch::primitive::patch_orders)
		.def("patch_selections", &bezier_triangle_patch::primitive::patch_selections)
		.def("patch_materials", &bezier_triangle_patch::primitive::patch_materials)
		.def("patch_points", &bezier_triangle_patch::primitive::patch_points)
		.def("patch_point_weights", &bezier_triangle_patch::primitive::patch_point_weights)
		.def("constant_data", &bezier_triangle_patch::primitive::constant_data)
		.def("uniform_data", &bezier_triangle_patch::primitive::uniform_data)
		.def("varying_data", &bezier_triangle_patch::primitive::varying_data)
		;
}

} // namespace python

} // namespace k3d

