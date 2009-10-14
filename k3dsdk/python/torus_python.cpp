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
#include <k3dsdk/python/torus_python.h>

#include <k3dsdk/torus.h>

#include <boost/python.hpp>
#include <boost/python/detail/api_placeholder.hpp>
using namespace boost::python;
#include <boost/scoped_ptr.hpp>

namespace k3d
{

namespace python
{

class torus
{
public:
	class const_primitive
	{
	public:
		typedef owned_instance_wrapper<k3d::torus::const_primitive> wrapper;

		static object matrices(wrapper& Self) { return wrap(Self.wrapped().matrices); }
		static object materials(wrapper& Self) { return wrap(Self.wrapped().materials); }
		static object major_radii(wrapper& Self) { return wrap(Self.wrapped().major_radii); }
		static object minor_radii(wrapper& Self) { return wrap(Self.wrapped().minor_radii); }
		static object phi_min(wrapper& Self) { return wrap(Self.wrapped().phi_min); }
		static object phi_max(wrapper& Self) { return wrap(Self.wrapped().phi_max); }
		static object sweep_angles(wrapper& Self) { return wrap(Self.wrapped().sweep_angles); }
		static object selections(wrapper& Self) { return wrap(Self.wrapped().selections); }
		static object constant_attributes(wrapper& Self) { return wrap(Self.wrapped().constant_attributes); }
		static object surface_attributes(wrapper& Self) { return wrap(Self.wrapped().surface_attributes); }
		static object varying_attributes(wrapper& Self) { return wrap(Self.wrapped().varying_attributes); }
	};

	class primitive
	{
	public:
		typedef owned_instance_wrapper<k3d::torus::primitive> wrapper;

		static object matrices(wrapper& Self) { return wrap(Self.wrapped().matrices); }
		static object materials(wrapper& Self) { return wrap(Self.wrapped().materials); }
		static object major_radii(wrapper& Self) { return wrap(Self.wrapped().major_radii); }
		static object minor_radii(wrapper& Self) { return wrap(Self.wrapped().minor_radii); }
		static object phi_min(wrapper& Self) { return wrap(Self.wrapped().phi_min); }
		static object phi_max(wrapper& Self) { return wrap(Self.wrapped().phi_max); }
		static object sweep_angles(wrapper& Self) { return wrap(Self.wrapped().sweep_angles); }
		static object selections(wrapper& Self) { return wrap(Self.wrapped().selections); }
		static object constant_attributes(wrapper& Self) { return wrap(Self.wrapped().constant_attributes); }
		static object surface_attributes(wrapper& Self) { return wrap(Self.wrapped().surface_attributes); }
		static object varying_attributes(wrapper& Self) { return wrap(Self.wrapped().varying_attributes); }
	};


	static object create(mesh_wrapper& Mesh)
	{
		return wrap_owned(k3d::torus::create(Mesh.wrapped()));
	}

	static object validate(mesh_wrapper& Mesh, mesh_primitive_wrapper& Primitive)
	{
		return wrap_owned(k3d::torus::validate(Mesh.wrapped(), Primitive.wrapped()));
	}

	static object validate_const(const_mesh_wrapper& Mesh, const_mesh_primitive_wrapper& Primitive)
	{
		return wrap_owned(k3d::torus::validate(Mesh.wrapped(), Primitive.wrapped()));
	}
};

void define_namespace_torus()
{
	scope outer = class_<torus>("torus", no_init)
		.def("create", &torus::create)
		.staticmethod("create")
		.def("validate", &torus::validate)
		.def("validate", &torus::validate_const)
		.staticmethod("validate")
		;

	class_<torus::const_primitive::wrapper>("const_primitive", no_init)
		.def("matrices", &torus::const_primitive::matrices)
		.def("materials", &torus::const_primitive::materials)
		.def("major_radii", &torus::const_primitive::major_radii)
		.def("minor_radii", &torus::const_primitive::minor_radii)
		.def("phi_min", &torus::const_primitive::phi_min)
		.def("phi_max", &torus::const_primitive::phi_max)
		.def("sweep_angles", &torus::const_primitive::sweep_angles)
		.def("selections", &torus::const_primitive::selections)
		.def("constant_attributes", &torus::const_primitive::constant_attributes)
		.def("surface_attributes", &torus::const_primitive::surface_attributes)
		.def("varying_attributes", &torus::const_primitive::varying_attributes)
		;

	class_<torus::primitive::wrapper>("primitive", no_init)
		.def("matrices", &torus::primitive::matrices)
		.def("materials", &torus::primitive::materials)
		.def("major_radii", &torus::primitive::major_radii)
		.def("minor_radii", &torus::primitive::minor_radii)
		.def("phi_min", &torus::primitive::phi_min)
		.def("phi_max", &torus::primitive::phi_max)
		.def("sweep_angles", &torus::primitive::sweep_angles)
		.def("selections", &torus::primitive::selections)
		.def("constant_attributes", &torus::primitive::constant_attributes)
		.def("surface_attributes", &torus::primitive::surface_attributes)
		.def("varying_attributes", &torus::primitive::varying_attributes)
		;
}

} // namespace python

} // namespace k3d

