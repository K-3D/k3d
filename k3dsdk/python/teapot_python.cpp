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
#include "teapot_python.h"

#include <k3dsdk/teapot.h>

#include <boost/python.hpp>
#include <boost/python/detail/api_placeholder.hpp>
using namespace boost::python;
#include <boost/scoped_ptr.hpp>

namespace k3d
{

namespace python
{

class teapot
{
public:
	class const_primitive
	{
	public:
		typedef owned_instance_wrapper<k3d::teapot::const_primitive> wrapper;

		static object matrices(wrapper& Self) { return wrap(Self.wrapped().matrices); }
		static object materials(wrapper& Self) { return wrap(Self.wrapped().materials); }
		static object constant_data(wrapper& Self) { return wrap(Self.wrapped().constant_data); }
		static object uniform_data(wrapper& Self) { return wrap(Self.wrapped().uniform_data); }
	};

	class primitive
	{
	public:
		typedef owned_instance_wrapper<k3d::teapot::primitive> wrapper;

		static object matrices(wrapper& Self) { return wrap(Self.wrapped().matrices); }
		static object materials(wrapper& Self) { return wrap(Self.wrapped().materials); }
		static object constant_data(wrapper& Self) { return wrap(Self.wrapped().constant_data); }
		static object uniform_data(wrapper& Self) { return wrap(Self.wrapped().uniform_data); }
	};


	static object create(mesh& Mesh)
	{
		return wrap_owned(k3d::teapot::create(Mesh.wrapped()));
	}

	static object validate(mesh_primitive_wrapper& Primitive)
	{
		return wrap_owned(k3d::teapot::validate(Primitive.wrapped()));
	}

	static object validate_const(const_mesh_primitive_wrapper& Primitive)
	{
		return wrap_owned(k3d::teapot::validate(Primitive.wrapped()));
	}
};

void define_namespace_teapot()
{
	scope outer = class_<teapot>("teapot", no_init)
		.def("create", &teapot::create)
		.staticmethod("create")
		.def("validate", &teapot::validate)
		.def("validate", &teapot::validate_const)
		.staticmethod("validate")
		;

	class_<teapot::const_primitive::wrapper>("const_primitive", no_init)
		.def("matrices", &teapot::const_primitive::matrices)
		.def("materials", &teapot::const_primitive::materials)
		.def("constant_data", &teapot::const_primitive::constant_data)
		.def("uniform_data", &teapot::const_primitive::uniform_data)
		;

	class_<teapot::primitive::wrapper>("primitive", no_init)
		.def("matrices", &teapot::primitive::matrices)
		.def("materials", &teapot::primitive::materials)
		.def("constant_data", &teapot::primitive::constant_data)
		.def("uniform_data", &teapot::primitive::uniform_data)
		;
}

} // namespace python

} // namespace k3d

