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

#include <boost/python.hpp>

#include <k3dsdk/python/mesh_python.h>
#include <k3dsdk/python/owned_instance_wrapper_python.h>
#include <k3dsdk/python/hyperboloid_python.h>

#include <k3dsdk/hyperboloid.h>

#include <boost/python/detail/api_placeholder.hpp>
using namespace boost::python;
#include <boost/scoped_ptr.hpp>

namespace k3d
{

namespace python
{

class hyperboloid
{
public:
	class const_primitive
	{
	public:
		typedef owned_instance_wrapper<k3d::hyperboloid::const_primitive> wrapper;

		static object matrices(wrapper& Self) { return wrap(Self.wrapped().matrices); }
		static object materials(wrapper& Self) { return wrap(Self.wrapped().materials); }
		static object start_points(wrapper& Self) { return wrap(Self.wrapped().start_points); }
		static object end_points(wrapper& Self) { return wrap(Self.wrapped().end_points); }
		static object sweep_angles(wrapper& Self) { return wrap(Self.wrapped().sweep_angles); }
		static object selections(wrapper& Self) { return wrap(Self.wrapped().selections); }
		static object constant_attributes(wrapper& Self) { return wrap(Self.wrapped().constant_attributes); }
		static object surface_attributes(wrapper& Self) { return wrap(Self.wrapped().surface_attributes); }
		static object parameter_attributes(wrapper& Self) { return wrap(Self.wrapped().parameter_attributes); }
	};

	class primitive
	{
	public:
		typedef owned_instance_wrapper<k3d::hyperboloid::primitive> wrapper;

		static object matrices(wrapper& Self) { return wrap(Self.wrapped().matrices); }
		static object materials(wrapper& Self) { return wrap(Self.wrapped().materials); }
		static object start_points(wrapper& Self) { return wrap(Self.wrapped().start_points); }
		static object end_points(wrapper& Self) { return wrap(Self.wrapped().end_points); }
		static object sweep_angles(wrapper& Self) { return wrap(Self.wrapped().sweep_angles); }
		static object selections(wrapper& Self) { return wrap(Self.wrapped().selections); }
		static object constant_attributes(wrapper& Self) { return wrap(Self.wrapped().constant_attributes); }
		static object surface_attributes(wrapper& Self) { return wrap(Self.wrapped().surface_attributes); }
		static object parameter_attributes(wrapper& Self) { return wrap(Self.wrapped().parameter_attributes); }
	};


	static object create(mesh_wrapper& Mesh)
	{
		return wrap_owned(k3d::hyperboloid::create(Mesh.wrapped()));
	}

	static object validate(mesh_wrapper& Mesh, mesh_primitive_wrapper& Primitive)
	{
		return wrap_owned(k3d::hyperboloid::validate(Mesh.wrapped(), Primitive.wrapped()));
	}

	static object validate_const(const_mesh_wrapper& Mesh, const_mesh_primitive_wrapper& Primitive)
	{
		return wrap_owned(k3d::hyperboloid::validate(Mesh.wrapped(), Primitive.wrapped()));
	}
};

void define_namespace_hyperboloid()
{
	scope outer = class_<hyperboloid>("hyperboloid", no_init)
		.def("create", &hyperboloid::create)
		.staticmethod("create")
		.def("validate", &hyperboloid::validate)
		.def("validate", &hyperboloid::validate_const)
		.staticmethod("validate")
		;

	class_<hyperboloid::const_primitive::wrapper>("const_primitive", no_init)
		.def("matrices", &hyperboloid::const_primitive::matrices)
		.def("materials", &hyperboloid::const_primitive::materials)
		.def("start_points", &hyperboloid::const_primitive::start_points)
		.def("end_points", &hyperboloid::const_primitive::end_points)
		.def("sweep_angles", &hyperboloid::const_primitive::sweep_angles)
		.def("selections", &hyperboloid::const_primitive::selections)
		.def("constant_attributes", &hyperboloid::const_primitive::constant_attributes)
		.def("surface_attributes", &hyperboloid::const_primitive::surface_attributes)
		.def("parameter_attributes", &hyperboloid::const_primitive::parameter_attributes)
		;

	class_<hyperboloid::primitive::wrapper>("primitive", no_init)
		.def("matrices", &hyperboloid::primitive::matrices)
		.def("materials", &hyperboloid::primitive::materials)
		.def("start_points", &hyperboloid::primitive::start_points)
		.def("end_points", &hyperboloid::primitive::end_points)
		.def("sweep_angles", &hyperboloid::primitive::sweep_angles)
		.def("selections", &hyperboloid::primitive::selections)
		.def("constant_attributes", &hyperboloid::primitive::constant_attributes)
		.def("surface_attributes", &hyperboloid::primitive::surface_attributes)
		.def("parameter_attributes", &hyperboloid::primitive::parameter_attributes)
		;
}

} // namespace python

} // namespace k3d

