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
#include "cone_python.h"

#include <k3dsdk/cone.h>

#include <boost/python.hpp>
#include <boost/python/detail/api_placeholder.hpp>
using namespace boost::python;
#include <boost/scoped_ptr.hpp>

namespace k3d
{

namespace python
{

class cone
{
public:
	class const_primitive
	{
	public:
		typedef owned_instance_wrapper<k3d::cone::const_primitive> wrapper;

		static object matrices(wrapper& Self) { return wrap(Self.wrapped().matrices); }
		static object materials(wrapper& Self) { return wrap(Self.wrapped().materials); }
		static object heights(wrapper& Self) { return wrap(Self.wrapped().heights); }
		static object radii(wrapper& Self) { return wrap(Self.wrapped().radii); }
		static object sweep_angles(wrapper& Self) { return wrap(Self.wrapped().sweep_angles); }
		static object selections(wrapper& Self) { return wrap(Self.wrapped().selections); }
		static object constant_data(wrapper& Self) { return wrap(Self.wrapped().constant_data); }
		static object uniform_data(wrapper& Self) { return wrap(Self.wrapped().uniform_data); }
		static object varying_data(wrapper& Self) { return wrap(Self.wrapped().varying_data); }
	};

	class primitive
	{
	public:
		typedef owned_instance_wrapper<k3d::cone::primitive> wrapper;

		static object matrices(wrapper& Self) { return wrap(Self.wrapped().matrices); }
		static object materials(wrapper& Self) { return wrap(Self.wrapped().materials); }
		static object heights(wrapper& Self) { return wrap(Self.wrapped().heights); }
		static object radii(wrapper& Self) { return wrap(Self.wrapped().radii); }
		static object sweep_angles(wrapper& Self) { return wrap(Self.wrapped().sweep_angles); }
		static object selections(wrapper& Self) { return wrap(Self.wrapped().selections); }
		static object constant_data(wrapper& Self) { return wrap(Self.wrapped().constant_data); }
		static object uniform_data(wrapper& Self) { return wrap(Self.wrapped().uniform_data); }
		static object varying_data(wrapper& Self) { return wrap(Self.wrapped().varying_data); }
	};


	static object create(mesh& Mesh)
	{
		return wrap_owned(k3d::cone::create(Mesh.wrapped()));
	}

	static object validate(mesh_primitive_wrapper& Primitive)
	{
		return wrap_owned(k3d::cone::validate(Primitive.wrapped()));
	}

	static object validate_const(const_mesh_primitive_wrapper& Primitive)
	{
		return wrap_owned(k3d::cone::validate(Primitive.wrapped()));
	}
};

void define_namespace_cone()
{
	scope outer = class_<cone>("cone", no_init)
		.def("create", &cone::create)
		.staticmethod("create")
		.def("validate", &cone::validate)
		.def("validate", &cone::validate_const)
		.staticmethod("validate")
		;

	class_<cone::const_primitive::wrapper>("const_primitive", no_init)
		.def("matrices", &cone::const_primitive::matrices)
		.def("materials", &cone::const_primitive::materials)
		.def("heights", &cone::const_primitive::heights)
		.def("radii", &cone::const_primitive::radii)
		.def("sweep_angles", &cone::const_primitive::sweep_angles)
		.def("selections", &cone::const_primitive::selections)
		.def("constant_data", &cone::const_primitive::constant_data)
		.def("uniform_data", &cone::const_primitive::uniform_data)
		.def("varying_data", &cone::const_primitive::varying_data)
		;

	class_<cone::primitive::wrapper>("primitive", no_init)
		.def("matrices", &cone::primitive::matrices)
		.def("materials", &cone::primitive::materials)
		.def("heights", &cone::primitive::heights)
		.def("radii", &cone::primitive::radii)
		.def("sweep_angles", &cone::primitive::sweep_angles)
		.def("selections", &cone::primitive::selections)
		.def("constant_data", &cone::primitive::constant_data)
		.def("uniform_data", &cone::primitive::uniform_data)
		.def("varying_data", &cone::primitive::varying_data)
		;
}

} // namespace python

} // namespace k3d

