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
#include <k3dsdk/python/bicubic_patch_python.h>

#include <k3dsdk/bicubic_patch.h>

#include <boost/python.hpp>
#include <boost/python/detail/api_placeholder.hpp>
using namespace boost::python;
#include <boost/scoped_ptr.hpp>

namespace k3d
{

namespace python
{

class bicubic_patch
{
public:
	class const_primitive
	{
	public:
		typedef owned_instance_wrapper<k3d::bicubic_patch::const_primitive> wrapper;

		static object patch_selections(wrapper& Self) { return wrap(Self.wrapped().patch_selections); }
		static object patch_materials(wrapper& Self) { return wrap(Self.wrapped().patch_materials); }
		static object patch_points(wrapper& Self) { return wrap(Self.wrapped().patch_points); }
		static object constant_attributes(wrapper& Self) { return wrap(Self.wrapped().constant_attributes); }
		static object patch_attributes(wrapper& Self) { return wrap(Self.wrapped().patch_attributes); }
		static object parameter_attributes(wrapper& Self) { return wrap(Self.wrapped().parameter_attributes); }
	};

	class primitive
	{
	public:
		typedef owned_instance_wrapper<k3d::bicubic_patch::primitive> wrapper;

		static object patch_selections(wrapper& Self) { return wrap(Self.wrapped().patch_selections); }
		static object patch_materials(wrapper& Self) { return wrap(Self.wrapped().patch_materials); }
		static object patch_points(wrapper& Self) { return wrap(Self.wrapped().patch_points); }
		static object constant_attributes(wrapper& Self) { return wrap(Self.wrapped().constant_attributes); }
		static object patch_attributes(wrapper& Self) { return wrap(Self.wrapped().patch_attributes); }
		static object parameter_attributes(wrapper& Self) { return wrap(Self.wrapped().parameter_attributes); }
	};


	static object create(mesh_wrapper& Mesh)
	{
		return wrap_owned(k3d::bicubic_patch::create(Mesh.wrapped()));
	}

	static object validate(mesh_wrapper& Mesh, mesh_primitive_wrapper& Primitive)
	{
		return wrap_owned(k3d::bicubic_patch::validate(Mesh.wrapped(), Primitive.wrapped()));
	}

	static object validate_const(const_mesh_wrapper& Mesh, const_mesh_primitive_wrapper& Primitive)
	{
		return wrap_owned(k3d::bicubic_patch::validate(Mesh.wrapped(), Primitive.wrapped()));
	}
};

void define_namespace_bicubic_patch()
{
	scope outer = class_<bicubic_patch>("bicubic_patch", no_init)
		.def("create", &bicubic_patch::create)
		.staticmethod("create")
		.def("validate", &bicubic_patch::validate)
		.def("validate", &bicubic_patch::validate_const)
		.staticmethod("validate")
		;

	class_<bicubic_patch::const_primitive::wrapper>("const_primitive", no_init)
		.def("patch_selections", &bicubic_patch::const_primitive::patch_selections)
		.def("patch_materials", &bicubic_patch::const_primitive::patch_materials)
		.def("patch_points", &bicubic_patch::const_primitive::patch_points)
		.def("constant_attributes", &bicubic_patch::const_primitive::constant_attributes)
		.def("patch_attributes", &bicubic_patch::const_primitive::patch_attributes)
		.def("parameter_attributes", &bicubic_patch::const_primitive::parameter_attributes)
		;

	class_<bicubic_patch::primitive::wrapper>("primitive", no_init)
		.def("patch_selections", &bicubic_patch::primitive::patch_selections)
		.def("patch_materials", &bicubic_patch::primitive::patch_materials)
		.def("patch_points", &bicubic_patch::primitive::patch_points)
		.def("constant_attributes", &bicubic_patch::primitive::constant_attributes)
		.def("patch_attributes", &bicubic_patch::primitive::patch_attributes)
		.def("parameter_attributes", &bicubic_patch::primitive::parameter_attributes)
		;
}

} // namespace python

} // namespace k3d

