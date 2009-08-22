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
#include "blobby_python.h"

#include <k3dsdk/blobby.h>

#include <boost/python.hpp>
#include <boost/python/detail/api_placeholder.hpp>
using namespace boost::python;
#include <boost/scoped_ptr.hpp>

namespace k3d
{

namespace python
{

class blobby
{
public:
	class const_primitive
	{
	public:
		typedef owned_instance_wrapper<k3d::blobby::const_primitive> wrapper;

		static object first_primitives(wrapper& Self) { return wrap(Self.wrapped().first_primitives); }
		static object primitive_counts(wrapper& Self) { return wrap(Self.wrapped().primitive_counts); }
		static object first_operators(wrapper& Self) { return wrap(Self.wrapped().first_operators); }
		static object operator_counts(wrapper& Self) { return wrap(Self.wrapped().operator_counts); }
		static object materials(wrapper& Self) { return wrap(Self.wrapped().materials); }
		static object primitives(wrapper& Self) { return wrap(Self.wrapped().primitives); }
		static object primitive_first_floats(wrapper& Self) { return wrap(Self.wrapped().primitive_first_floats); }
		static object primitive_float_counts(wrapper& Self) { return wrap(Self.wrapped().primitive_float_counts); }
		static object operators(wrapper& Self) { return wrap(Self.wrapped().operators); }
		static object operator_first_operands(wrapper& Self) { return wrap(Self.wrapped().operator_first_operands); }
		static object operator_operand_counts(wrapper& Self) { return wrap(Self.wrapped().operator_operand_counts); }
		static object floats(wrapper& Self) { return wrap(Self.wrapped().floats); }
		static object operands(wrapper& Self) { return wrap(Self.wrapped().operands); }
		static object constant_attributes(wrapper& Self) { return wrap(Self.wrapped().constant_attributes); }
		static object surface_attributes(wrapper& Self) { return wrap(Self.wrapped().surface_attributes); }
		static object varying_attributes(wrapper& Self) { return wrap(Self.wrapped().varying_attributes); }
		static object vertex_attributes(wrapper& Self) { return wrap(Self.wrapped().vertex_attributes); }
	};

	class primitive
	{
	public:
		typedef owned_instance_wrapper<k3d::blobby::primitive> wrapper;

		static object first_primitives(wrapper& Self) { return wrap(Self.wrapped().first_primitives); }
		static object primitive_counts(wrapper& Self) { return wrap(Self.wrapped().primitive_counts); }
		static object first_operators(wrapper& Self) { return wrap(Self.wrapped().first_operators); }
		static object operator_counts(wrapper& Self) { return wrap(Self.wrapped().operator_counts); }
		static object materials(wrapper& Self) { return wrap(Self.wrapped().materials); }
		static object primitives(wrapper& Self) { return wrap(Self.wrapped().primitives); }
		static object primitive_first_floats(wrapper& Self) { return wrap(Self.wrapped().primitive_first_floats); }
		static object primitive_float_counts(wrapper& Self) { return wrap(Self.wrapped().primitive_float_counts); }
		static object operators(wrapper& Self) { return wrap(Self.wrapped().operators); }
		static object operator_first_operands(wrapper& Self) { return wrap(Self.wrapped().operator_first_operands); }
		static object operator_operand_counts(wrapper& Self) { return wrap(Self.wrapped().operator_operand_counts); }
		static object floats(wrapper& Self) { return wrap(Self.wrapped().floats); }
		static object operands(wrapper& Self) { return wrap(Self.wrapped().operands); }
		static object constant_attributes(wrapper& Self) { return wrap(Self.wrapped().constant_attributes); }
		static object surface_attributes(wrapper& Self) { return wrap(Self.wrapped().surface_attributes); }
		static object varying_attributes(wrapper& Self) { return wrap(Self.wrapped().varying_attributes); }
		static object vertex_attributes(wrapper& Self) { return wrap(Self.wrapped().vertex_attributes); }
	};


	static object create(mesh_wrapper& Mesh)
	{
		return wrap_owned(k3d::blobby::create(Mesh.wrapped()));
	}

	static object validate(mesh_wrapper& Mesh, mesh_primitive_wrapper& Primitive)
	{
		return wrap_owned(k3d::blobby::validate(Mesh.wrapped(), Primitive.wrapped()));
	}

	static object validate_const(const_mesh_wrapper& Mesh, const_mesh_primitive_wrapper& Primitive)
	{
		return wrap_owned(k3d::blobby::validate(Mesh.wrapped(), Primitive.wrapped()));
	}
};

void define_namespace_blobby()
{
	scope outer = class_<blobby>("blobby", no_init)
		.def("create", &blobby::create)
		.staticmethod("create")
		.def("validate", &blobby::validate)
		.def("validate", &blobby::validate_const)
		.staticmethod("validate")
		;

	enum_<k3d::blobby::primitive_type>("primitive_type")
		.value("CONSTANT", k3d::blobby::CONSTANT)
		.value("ELLIPSOID", k3d::blobby::ELLIPSOID)
		.value("SEGMENT", k3d::blobby::SEGMENT)
		.attr("__module__") = "k3d";

	enum_<k3d::blobby::operator_type>("operator_type")
		.value("ADD", k3d::blobby::ADD)
		.value("MULTIPLY", k3d::blobby::MULTIPLY)
		.value("MAXIMUM", k3d::blobby::MAXIMUM)
		.value("MINIMUM", k3d::blobby::MINIMUM)
		.value("SUBTRACT", k3d::blobby::SUBTRACT)
		.value("DIVIDE", k3d::blobby::DIVIDE)
		.value("NEGATE", k3d::blobby::NEGATE)
		.value("IDENTITY", k3d::blobby::IDENTITY)
		.attr("__module__") = "k3d";

	class_<blobby::const_primitive::wrapper>("const_primitive", no_init)
		.def("first_primitives", &blobby::const_primitive::first_primitives)
		.def("primitive_counts", &blobby::const_primitive::primitive_counts)
		.def("first_operators", &blobby::const_primitive::first_operators)
		.def("operator_counts", &blobby::const_primitive::operator_counts)
		.def("materials", &blobby::const_primitive::materials)
		.def("primitives", &blobby::const_primitive::primitives)
		.def("primitive_first_floats", &blobby::const_primitive::primitive_first_floats)
		.def("primitive_float_counts", &blobby::const_primitive::primitive_float_counts)
		.def("operators", &blobby::const_primitive::operators)
		.def("operator_first_operands", &blobby::const_primitive::operator_first_operands)
		.def("operator_operand_counts", &blobby::const_primitive::operator_operand_counts)
		.def("floats", &blobby::const_primitive::floats)
		.def("operands", &blobby::const_primitive::operands)
		.def("constant_attributes", &blobby::const_primitive::constant_attributes)
		.def("surface_attributes", &blobby::const_primitive::surface_attributes)
		.def("varying_attributes", &blobby::const_primitive::varying_attributes)
		.def("vertex_attributes", &blobby::const_primitive::vertex_attributes)
		;

	class_<blobby::primitive::wrapper>("primitive", no_init)
		.def("first_primitives", &blobby::primitive::first_primitives)
		.def("primitive_counts", &blobby::primitive::primitive_counts)
		.def("first_operators", &blobby::primitive::first_operators)
		.def("operator_counts", &blobby::primitive::operator_counts)
		.def("materials", &blobby::primitive::materials)
		.def("primitives", &blobby::primitive::primitives)
		.def("primitive_first_floats", &blobby::primitive::primitive_first_floats)
		.def("primitive_float_counts", &blobby::primitive::primitive_float_counts)
		.def("operators", &blobby::primitive::operators)
		.def("operator_first_operands", &blobby::primitive::operator_first_operands)
		.def("operator_operand_counts", &blobby::primitive::operator_operand_counts)
		.def("floats", &blobby::primitive::floats)
		.def("operands", &blobby::primitive::operands)
		.def("constant_attributes", &blobby::primitive::constant_attributes)
		.def("surface_attributes", &blobby::primitive::surface_attributes)
		.def("varying_attributes", &blobby::primitive::varying_attributes)
		.def("vertex_attributes", &blobby::primitive::vertex_attributes)
		;
}

} // namespace python

} // namespace k3d

