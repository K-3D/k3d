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
		static object constant_attributes(wrapper& Self) { return wrap(Self.wrapped().constant_attributes); }
		static object uniform_attributes(wrapper& Self) { return wrap(Self.wrapped().uniform_attributes); }
		static object varying_attributes(wrapper& Self) { return wrap(Self.wrapped().varying_attributes); }
		static object vertex_attributes(wrapper& Self) { return wrap(Self.wrapped().vertex_attributes); }
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
		static object constant_attributes(wrapper& Self) { return wrap(Self.wrapped().constant_attributes); }
		static object uniform_attributes(wrapper& Self) { return wrap(Self.wrapped().uniform_attributes); }
		static object varying_attributes(wrapper& Self) { return wrap(Self.wrapped().varying_attributes); }
		static object vertex_attributes(wrapper& Self) { return wrap(Self.wrapped().vertex_attributes); }
	};


	static object create(mesh_wrapper& Mesh)
	{
		return wrap_owned(k3d::bezier_triangle_patch::create(Mesh.wrapped()));
	}

	static object validate(mesh_wrapper& Mesh, mesh_primitive_wrapper& Primitive)
	{
		return wrap_owned(k3d::bezier_triangle_patch::validate(Mesh.wrapped(), Primitive.wrapped()));
	}

	static object validate_const(const_mesh_wrapper& Mesh, const_mesh_primitive_wrapper& Primitive)
	{
		return wrap_owned(k3d::bezier_triangle_patch::validate(Mesh.wrapped(), Primitive.wrapped()));
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
		.def("constant_attributes", &bezier_triangle_patch::const_primitive::constant_attributes)
		.def("uniform_attributes", &bezier_triangle_patch::const_primitive::uniform_attributes)
		.def("varying_attributes", &bezier_triangle_patch::const_primitive::varying_attributes)
		.def("vertex_attributes", &bezier_triangle_patch::const_primitive::vertex_attributes)
		;

	class_<bezier_triangle_patch::primitive::wrapper>("primitive", no_init)
		.def("patch_first_points", &bezier_triangle_patch::primitive::patch_first_points)
		.def("patch_orders", &bezier_triangle_patch::primitive::patch_orders)
		.def("patch_selections", &bezier_triangle_patch::primitive::patch_selections)
		.def("patch_materials", &bezier_triangle_patch::primitive::patch_materials)
		.def("patch_points", &bezier_triangle_patch::primitive::patch_points)
		.def("patch_point_weights", &bezier_triangle_patch::primitive::patch_point_weights)
		.def("constant_attributes", &bezier_triangle_patch::primitive::constant_attributes)
		.def("uniform_attributes", &bezier_triangle_patch::primitive::uniform_attributes)
		.def("varying_attributes", &bezier_triangle_patch::primitive::varying_attributes)
		.def("vertex_attributes", &bezier_triangle_patch::primitive::vertex_attributes)
		;
}

} // namespace python

} // namespace k3d

