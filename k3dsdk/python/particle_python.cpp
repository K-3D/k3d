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
#include <k3dsdk/python/particle_python.h>

#include <k3dsdk/particle.h>

#include <boost/python.hpp>
#include <boost/python/detail/api_placeholder.hpp>
using namespace boost::python;
#include <boost/scoped_ptr.hpp>

namespace k3d
{

namespace python
{

class particle
{
public:
	class const_primitive
	{
	public:
		typedef owned_instance_wrapper<k3d::particle::const_primitive> wrapper;

		static object material(wrapper& Self) { return wrap(Self.wrapped().material); }
		static object points(wrapper& Self) { return wrap(Self.wrapped().points); }
		static object constant_attributes(wrapper& Self) { return wrap(Self.wrapped().constant_attributes); }
		static object vertex_attributes(wrapper& Self) { return wrap(Self.wrapped().vertex_attributes); }
	};

	class primitive
	{
	public:
		typedef owned_instance_wrapper<k3d::particle::primitive> wrapper;

		static object material(wrapper& Self) { return wrap(Self.wrapped().material); }
		static object points(wrapper& Self) { return wrap(Self.wrapped().points); }
		static object constant_attributes(wrapper& Self) { return wrap(Self.wrapped().constant_attributes); }
		static object vertex_attributes(wrapper& Self) { return wrap(Self.wrapped().vertex_attributes); }
	};


	static object create(mesh_wrapper& Mesh)
	{
		return wrap_owned(k3d::particle::create(Mesh.wrapped()));
	}

	static object validate(mesh_wrapper& Mesh, mesh_primitive_wrapper& Primitive)
	{
		return wrap_owned(k3d::particle::validate(Mesh.wrapped(), Primitive.wrapped()));
	}

	static object validate_const(const_mesh_wrapper& Mesh, const_mesh_primitive_wrapper& Primitive)
	{
		return wrap_owned(k3d::particle::validate(Mesh.wrapped(), Primitive.wrapped()));
	}
};

void define_namespace_particle()
{
	scope outer = class_<particle>("particle", no_init)
		.def("create", &particle::create)
		.staticmethod("create")
		.def("validate", &particle::validate)
		.def("validate", &particle::validate_const)
		.staticmethod("validate")
		;

	class_<particle::const_primitive::wrapper>("const_primitive", no_init)
		.def("material", &particle::const_primitive::material)
		.def("points", &particle::const_primitive::points)
		.def("constant_attributes", &particle::const_primitive::constant_attributes)
		.def("vertex_attributes", &particle::const_primitive::vertex_attributes)
		;

	class_<particle::primitive::wrapper>("primitive", no_init)
		.def("material", &particle::primitive::material)
		.def("points", &particle::primitive::points)
		.def("constant_attributes", &particle::primitive::constant_attributes)
		.def("vertex_attributes", &particle::primitive::vertex_attributes)
		;
}

} // namespace python

} // namespace k3d

