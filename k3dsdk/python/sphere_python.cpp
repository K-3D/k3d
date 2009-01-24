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
#include "sphere_python.h"

#include <k3dsdk/sphere.h>

#include <boost/python.hpp>
#include <boost/python/detail/api_placeholder.hpp>
using namespace boost::python;
#include <boost/scoped_ptr.hpp>

namespace k3d
{

namespace python
{

class sphere
{
public:
	class const_primitive
	{
	public:
		typedef owned_instance_wrapper<k3d::sphere::const_primitive> wrapper;

		static object matrices(wrapper& Self) { return wrap(Self.wrapped().matrices); }
		static object materials(wrapper& Self) { return wrap(Self.wrapped().materials); }
		static object radii(wrapper& Self) { return wrap(Self.wrapped().radii); }
		static object z_min(wrapper& Self) { return wrap(Self.wrapped().z_min); }
		static object z_max(wrapper& Self) { return wrap(Self.wrapped().z_max); }
		static object sweep_angles(wrapper& Self) { return wrap(Self.wrapped().sweep_angles); }
		static object selections(wrapper& Self) { return wrap(Self.wrapped().selections); }
		static object constant_data(wrapper& Self) { return wrap(Self.wrapped().constant_data); }
		static object uniform_data(wrapper& Self) { return wrap(Self.wrapped().uniform_data); }
		static object varying_data(wrapper& Self) { return wrap(Self.wrapped().varying_data); }
	};

	class primitive
	{
	public:
		typedef owned_instance_wrapper<k3d::sphere::primitive> wrapper;

		static object matrices(wrapper& Self) { return wrap(Self.wrapped().matrices); }
		static object materials(wrapper& Self) { return wrap(Self.wrapped().materials); }
		static object radii(wrapper& Self) { return wrap(Self.wrapped().radii); }
		static object z_min(wrapper& Self) { return wrap(Self.wrapped().z_min); }
		static object z_max(wrapper& Self) { return wrap(Self.wrapped().z_max); }
		static object sweep_angles(wrapper& Self) { return wrap(Self.wrapped().sweep_angles); }
		static object selections(wrapper& Self) { return wrap(Self.wrapped().selections); }
		static object constant_data(wrapper& Self) { return wrap(Self.wrapped().constant_data); }
		static object uniform_data(wrapper& Self) { return wrap(Self.wrapped().uniform_data); }
		static object varying_data(wrapper& Self) { return wrap(Self.wrapped().varying_data); }
	};


	static object create(mesh& Mesh)
	{
		return wrap_owned(k3d::sphere::create(Mesh.wrapped()));
	}

	static object validate(mesh_primitive_wrapper& Primitive)
	{
		return wrap_owned(k3d::sphere::validate(Primitive.wrapped()));
	}

	static object validate_const(const_mesh_primitive_wrapper& Primitive)
	{
		return wrap_owned(k3d::sphere::validate(Primitive.wrapped()));
	}
};

void define_namespace_sphere()
{
	scope outer = class_<sphere>("sphere", no_init)
		.def("create", &sphere::create)
		.staticmethod("create")
		.def("validate", &sphere::validate)
		.def("validate", &sphere::validate_const)
		.staticmethod("validate")
		;

	class_<sphere::const_primitive::wrapper>("const_primitive", no_init)
		.def("matrices", &sphere::const_primitive::matrices)
		.def("materials", &sphere::const_primitive::materials)
		.def("radii", &sphere::const_primitive::radii)
		.def("z_min", &sphere::const_primitive::z_min)
		.def("z_max", &sphere::const_primitive::z_max)
		.def("sweep_angles", &sphere::const_primitive::sweep_angles)
		.def("selections", &sphere::const_primitive::selections)
		.def("constant_data", &sphere::const_primitive::constant_data)
		.def("uniform_data", &sphere::const_primitive::uniform_data)
		.def("varying_data", &sphere::const_primitive::varying_data)
		;

	class_<sphere::primitive::wrapper>("primitive", no_init)
		.def("matrices", &sphere::primitive::matrices)
		.def("materials", &sphere::primitive::materials)
		.def("radii", &sphere::primitive::radii)
		.def("z_min", &sphere::primitive::z_min)
		.def("z_max", &sphere::primitive::z_max)
		.def("sweep_angles", &sphere::primitive::sweep_angles)
		.def("selections", &sphere::primitive::selections)
		.def("constant_data", &sphere::primitive::constant_data)
		.def("uniform_data", &sphere::primitive::uniform_data)
		.def("varying_data", &sphere::primitive::varying_data)
		;
}

} // namespace python

} // namespace k3d

