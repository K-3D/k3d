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
#include "point_group_python.h"

#include <k3dsdk/point_group.h>

#include <boost/python.hpp>
#include <boost/python/detail/api_placeholder.hpp>
using namespace boost::python;
#include <boost/scoped_ptr.hpp>

namespace k3d
{

namespace python
{

class point_group
{
public:
	class const_primitive
	{
	public:
		typedef owned_instance_wrapper<k3d::point_group::const_primitive> wrapper;

		static object material(wrapper& Self) { return wrap(Self.wrapped().material); }
		static object points(wrapper& Self) { return wrap(Self.wrapped().points); }
		static object constant_attributes(wrapper& Self) { return wrap(Self.wrapped().constant_attributes); }
		static object vertex_attributes(wrapper& Self) { return wrap(Self.wrapped().vertex_attributes); }
	};

	class primitive
	{
	public:
		typedef owned_instance_wrapper<k3d::point_group::primitive> wrapper;

		static object material(wrapper& Self) { return wrap(Self.wrapped().material); }
		static object points(wrapper& Self) { return wrap(Self.wrapped().points); }
		static object constant_attributes(wrapper& Self) { return wrap(Self.wrapped().constant_attributes); }
		static object vertex_attributes(wrapper& Self) { return wrap(Self.wrapped().vertex_attributes); }
	};


	static object create(mesh& Mesh)
	{
		return wrap_owned(k3d::point_group::create(Mesh.wrapped()));
	}

	static object validate(mesh_primitive_wrapper& Primitive)
	{
		return wrap_owned(k3d::point_group::validate(Primitive.wrapped()));
	}

	static object validate_const(const_mesh_primitive_wrapper& Primitive)
	{
		return wrap_owned(k3d::point_group::validate(Primitive.wrapped()));
	}
};

void define_namespace_point_group()
{
	scope outer = class_<point_group>("point_group", no_init)
		.def("create", &point_group::create)
		.staticmethod("create")
		.def("validate", &point_group::validate)
		.def("validate", &point_group::validate_const)
		.staticmethod("validate")
		;

	class_<point_group::const_primitive::wrapper>("const_primitive", no_init)
		.def("material", &point_group::const_primitive::material)
		.def("points", &point_group::const_primitive::points)
		.def("constant_attributes", &point_group::const_primitive::constant_attributes)
		.def("vertex_attributes", &point_group::const_primitive::vertex_attributes)
		;

	class_<point_group::primitive::wrapper>("primitive", no_init)
		.def("material", &point_group::primitive::material)
		.def("points", &point_group::primitive::points)
		.def("constant_attributes", &point_group::primitive::constant_attributes)
		.def("vertex_attributes", &point_group::primitive::vertex_attributes)
		;
}

} // namespace python

} // namespace k3d

