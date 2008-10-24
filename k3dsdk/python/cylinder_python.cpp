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
#include "cylinder_python.h"

#include <k3dsdk/cylinder.h>

#include <boost/python.hpp>
#include <boost/python/detail/api_placeholder.hpp>
using namespace boost::python;
#include <boost/scoped_ptr.hpp>

namespace k3d
{

namespace python
{

class cylinder
{
public:
	class const_primitive
	{
	public:
		typedef owned_instance_wrapper<k3d::cylinder::const_primitive> wrapper;

		static object matrices(wrapper& Self) { return wrap(Self.wrapped().matrices); }
		static object materials(wrapper& Self) { return wrap(Self.wrapped().materials); }
		static object radii(wrapper& Self) { return wrap(Self.wrapped().radii); }
		static object z_min(wrapper& Self) { return wrap(Self.wrapped().z_min); }
		static object z_max(wrapper& Self) { return wrap(Self.wrapped().z_max); }
		static object sweep_angles(wrapper& Self) { return wrap(Self.wrapped().sweep_angles); }
		static object constant_data(wrapper& Self) { return wrap(Self.wrapped().constant_data); }
		static object uniform_data(wrapper& Self) { return wrap(Self.wrapped().uniform_data); }
		static object varying_data(wrapper& Self) { return wrap(Self.wrapped().varying_data); }
	};

	class primitive
	{
	public:
		typedef owned_instance_wrapper<k3d::cylinder::primitive> wrapper;

		static object matrices(wrapper& Self) { return wrap(Self.wrapped().matrices); }
		static object materials(wrapper& Self) { return wrap(Self.wrapped().materials); }
		static object radii(wrapper& Self) { return wrap(Self.wrapped().radii); }
		static object z_min(wrapper& Self) { return wrap(Self.wrapped().z_min); }
		static object z_max(wrapper& Self) { return wrap(Self.wrapped().z_max); }
		static object sweep_angles(wrapper& Self) { return wrap(Self.wrapped().sweep_angles); }
		static object constant_data(wrapper& Self) { return wrap(Self.wrapped().constant_data); }
		static object uniform_data(wrapper& Self) { return wrap(Self.wrapped().uniform_data); }
		static object varying_data(wrapper& Self) { return wrap(Self.wrapped().varying_data); }
	};


	static object create(mesh& Mesh)
	{
		return wrap_owned(k3d::cylinder::create(Mesh.wrapped()));
	}

	static object validate(mesh_primitive_wrapper& Primitive)
	{
		return wrap_owned(k3d::cylinder::validate(Primitive.wrapped()));
	}

	static object validate_const(const_mesh_primitive_wrapper& Primitive)
	{
		return wrap_owned(k3d::cylinder::validate(Primitive.wrapped()));
	}
};

void define_namespace_cylinder()
{
	scope outer = class_<cylinder>("cylinder", no_init)
		.def("create", &cylinder::create)
		.staticmethod("create")
		.def("validate", &cylinder::validate)
		.def("validate", &cylinder::validate_const)
		.staticmethod("validate")
		;

	class_<cylinder::const_primitive::wrapper>("const_primitive", no_init)
		.def("matrices", &cylinder::const_primitive::matrices)
		.def("materials", &cylinder::const_primitive::materials)
		.def("radii", &cylinder::const_primitive::radii)
		.def("z_min", &cylinder::const_primitive::z_min)
		.def("z_max", &cylinder::const_primitive::z_max)
		.def("sweep_angles", &cylinder::const_primitive::sweep_angles)
		.def("constant_data", &cylinder::const_primitive::constant_data)
		.def("uniform_data", &cylinder::const_primitive::uniform_data)
		.def("varying_data", &cylinder::const_primitive::varying_data)
		;

	class_<cylinder::primitive::wrapper>("primitive", no_init)
		.def("matrices", &cylinder::primitive::matrices)
		.def("materials", &cylinder::primitive::materials)
		.def("radii", &cylinder::primitive::radii)
		.def("z_min", &cylinder::primitive::z_min)
		.def("z_max", &cylinder::primitive::z_max)
		.def("sweep_angles", &cylinder::primitive::sweep_angles)
		.def("constant_data", &cylinder::primitive::constant_data)
		.def("uniform_data", &cylinder::primitive::uniform_data)
		.def("varying_data", &cylinder::primitive::varying_data)
		;
}

} // namespace python

} // namespace k3d

