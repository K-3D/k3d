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

/** \file
	\author Timothy M. Shead (tshead@k-3d.com)
*/

#include "table_python.h"
#include "const_table_python.h"
#include "const_typed_array_python.h"
#include "instance_wrapper_python.h"
#include "mesh_python.h"
#include "typed_array_python.h"
#include "utility_python.h"

#include <k3dsdk/color.h>
#include <k3dsdk/mesh.h>
#include <k3dsdk/type_registry.h>

#include <boost/python.hpp>
#include <boost/python/detail/api_placeholder.hpp>
using namespace boost::python;

namespace k3d
{

namespace python
{

namespace detail
{

///////////////////////////////////////////////////////////////////////////////////////
// wrap_const_object

template<typename return_type, typename value_type>
object wrap_const_object(const pipeline_data<value_type>& Data)
{
	if(!Data)
		return object();

	return object(return_type(*Data));
}

////////////////////////////////////////////////////////////////////////////////////////
// wrap_non_const_object

template<typename value_type>
object wrap_non_const_object(pipeline_data<value_type>& Data)
{
	if(!Data)
		return object();

	return object(wrap(Data.writable()));
}

////////////////////////////////////////////////////////////////////////////////////////
// create_object

template<typename return_type, typename value_type>
object create_object(pipeline_data<value_type>& Data)
{
	return object(return_type(Data.create()));
}

///////////////////////////////////////////////////////////////////////////////////////////////
// wrap_const_array

template<typename array_type>
object wrap_const_array(const pipeline_data<array_type>& Data)
{
	if(!Data)
		return object();

	return object(instance_wrapper<const array_type>(*Data));
}

///////////////////////////////////////////////////////////////////////////////////////////////
// wrap_non_const_array

template<typename array_type>
object wrap_non_const_array(pipeline_data<array_type>& Data)
{
	if(!Data)
		return object();

	return object(instance_wrapper<array_type>(Data.writable()));
}

////////////////////////////////////////////////////////////////////////////////////////////////
// create_array

template<typename array_type>
object create_array(pipeline_data<array_type>& Data)
{
	return object(instance_wrapper<array_type>(Data.create()));
}

} // namespace detail

///////////////////////////////////////////////////////////////////////////////////////////////////
// mesh

static void mesh_copy_const(mesh_wrapper& Self, const_mesh_wrapper& RHS)
{
	Self.wrapped() = RHS.wrapped();
}

static void mesh_copy_non_const(mesh_wrapper& Self, mesh_wrapper& RHS)
{
	Self.wrapped() = RHS.wrapped();
}

static object mesh_create_point_selection(mesh_wrapper& Self) { return detail::create_array(Self.wrapped().point_selection); }
static object mesh_create_points(mesh_wrapper& Self) { return detail::create_array(Self.wrapped().points); }
static object mesh_point_selection(mesh_wrapper& Self) { return detail::wrap_non_const_array(Self.wrapped().point_selection); }
static object mesh_points(mesh_wrapper& Self) { return detail::wrap_non_const_array(Self.wrapped().points); }
static object mesh_point_attributes(mesh_wrapper& Self) { return wrap(Self.wrapped().point_attributes); }
static object mesh_primitives(mesh_wrapper& Self) { return wrap(Self.wrapped().primitives); }

static const string_t mesh_str(mesh_wrapper& Self)
{
	std::ostringstream buffer;
	buffer << Self.wrapped();
	return buffer.str();
}


///////////////////////////////////////////////////////////////////////////////////////////////////
// const_mesh

static object const_mesh_point_selection(const_mesh_wrapper& Self) { return detail::wrap_const_array(Self.wrapped().point_selection); }
static object const_mesh_points(const_mesh_wrapper& Self) { return detail::wrap_const_array(Self.wrapped().points); }
static object const_mesh_point_attributes(const_mesh_wrapper& Self) { return wrap(Self.wrapped().point_attributes); }
static object const_mesh_primitives(const_mesh_wrapper& Self) { return wrap(Self.wrapped().primitives); }

static const string_t const_mesh_str(const_mesh_wrapper& Self)
{
	std::ostringstream buffer;
	buffer << Self.wrapped();
	return buffer.str();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// const_mesh_primitive 

static const string_t const_mesh_primitive_get_type(const_mesh_primitive_wrapper& Self)
{
	return Self.wrapped().type;
}

static object const_mesh_primitive_get_structure(const_mesh_primitive_wrapper& Self)
{
	return wrap(Self.wrapped().structure);
}

static object const_mesh_primitive_get_attributes(const_mesh_primitive_wrapper& Self)
{
	return wrap(Self.wrapped().attributes);
}

static const string_t const_mesh_primitive_str(const_mesh_primitive_wrapper& Self)
{
	std::ostringstream buffer;
	buffer << Self.wrapped();
	return buffer.str();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// mesh_primitive 

static const string_t mesh_primitive_get_type(mesh_primitive_wrapper& Self)
{
	return Self.wrapped().type;
}

static void mesh_primitive_set_type(mesh_primitive_wrapper& Self, const string_t& Type)
{
	Self.wrapped().type = Type;
}

static object mesh_primitive_get_structure(mesh_primitive_wrapper& Self)
{
	return wrap(Self.wrapped().structure);
}

static object mesh_primitive_get_attributes(mesh_primitive_wrapper& Self)
{
	return wrap(Self.wrapped().attributes);
}

static const string_t mesh_primitive_str(mesh_primitive_wrapper& Self)
{
	std::ostringstream buffer;
	buffer << Self.wrapped();
	return buffer.str();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// const_mesh_primitives_t

typedef instance_wrapper<const k3d::mesh::primitives_t> const_mesh_primitives_t_wrapper;

static object const_mesh_primitives_t_get_item(const_mesh_primitives_t_wrapper& Self, int Item)
{
	if(Item < 0 || Item >= Self.wrapped().size())
		throw std::out_of_range("index out-of-range");

	return wrap(Self.wrapped().at(Item).get());
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// mesh_primitives_t

typedef instance_wrapper<k3d::mesh::primitives_t> mesh_primitives_t_wrapper;

static object mesh_primitives_t_get_item(mesh_primitives_t_wrapper& Self, int Item)
{
	if(Item < 0 || Item >= Self.wrapped().size())
		throw std::out_of_range("index out-of-range");

	return detail::wrap_non_const_object(Self.wrapped().at(Item));
}

static object mesh_primitives_t_create(mesh_primitives_t_wrapper& Self, const string_t& Type)
{
	return wrap(Self.wrapped().create(Type));
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// define_class_mesh 

void define_class_mesh()
{
	scope outer = class_<mesh_wrapper>("mesh", 
		"Stores a heterogeneous collection of geometric mesh primitives.", no_init)
		.def("point_attributes", &mesh_point_attributes,
			"Returns a L{table} object containing a collection of mutable per-vertex data, or None.")
		.def("point_selection", &mesh_point_selection,
			"Returns an mutable L{double_array} object containing the selection state of every vertex in the mesh, or None.")
		.def("points", &mesh_points,
			"Returns an mutable L{const_point3_array} object containing the geometric coordinates of every vertex in the mesh, or None.")
		.def("copy", &mesh_copy_non_const,
			"Store a shallow copy of the given L{mesh}.")
		.def("copy", &mesh_copy_const,
			"Store a shallow copy of the given L{const_mesh}.")
		.def("create_point_selection", &mesh_create_point_selection,
			"Creates and returns a new L{double_array} object used to store the selection state of every vertex in the mesh.")
		.def("create_points", &mesh_create_points,
			"Creates and returns a new L{point3_array} object used to store the geometric coordinates of every vertex in the mesh.")
		.def("primitives", &mesh_primitives,
			"Returns the set of L{primitive} objects in the mesh.")
		.def("__str__", &mesh_str);

	class_<mesh_primitive_wrapper>("primitive", no_init)
		.def("type", &mesh_primitive_get_type)
		.def("structure", &mesh_primitive_get_structure)
		.def("attributes", &mesh_primitive_get_attributes)
		.def("__str__", &mesh_primitive_str)
		;

	class_<mesh_primitives_t_wrapper>("primitives_t", no_init)
		.def("__len__", &utility::wrapped_len<mesh_primitives_t_wrapper>)
		.def("__getitem__", &mesh_primitives_t_get_item)
		.def("create", &mesh_primitives_t_create)
		;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// define_class_mesh

void define_class_const_mesh()
{
	scope outer = class_<const_mesh_wrapper>("const_mesh",
		"Stores a heterogeneous, immutable collection of geometric mesh primitives.", no_init)
		.def("point_attributes", &const_mesh_point_attributes,
			"Returns a L{const_named_arrays} object containing a collection of immutable (read-only) per-vertex data, or None.")
		.def("point_selection", &const_mesh_point_selection,
			"Returns an immutable (read-only) L{const_double_array} object containing the selection state of every vertex in the mesh, or None.")
		.def("points", &const_mesh_points,
			"Returns an immutable (read-only) L{const_point3_array} object containing the geometric coordinates of every vertex in the mesh, or None.")
		.def("primitives", &const_mesh_primitives,
			"Returns the set of L{const_primitive} objects in the mesh.")
		.def("__str__", &const_mesh_str);

	class_<const_mesh_primitive_wrapper>("const_primitive", no_init)
		.def("type", &const_mesh_primitive_get_type)
		.def("structure", &const_mesh_primitive_get_structure)
		.def("attributes", &const_mesh_primitive_get_attributes)
		.def("__str__", &const_mesh_primitive_str)
		;

	class_<const_mesh_primitives_t_wrapper>("const_primitives_t", no_init)
		.def("__len__", &utility::wrapped_len<const_mesh_primitives_t_wrapper>)
		.def("__getitem__", &const_mesh_primitives_t_get_item)
		;
}

} // namespace python

} // namespace k3d

