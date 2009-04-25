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

#include "attribute_arrays_python.h"
#include "const_attribute_arrays_python.h"
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

template<typename return_type, typename value_type>
object wrap_non_const_object(pipeline_data<value_type>& Data)
{
	if(!Data)
		return object();

	return object(return_type(Data.writable()));
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

mesh::mesh() :
	base()
{
}

mesh::mesh(k3d::mesh* Mesh) :
	base(Mesh)
{
}

void mesh::copy(const mesh& RHS)
{
	wrapped() = RHS.wrapped();
}

object mesh::create_point_selection() { return detail::create_array(wrapped().point_selection); } 
object mesh::create_points() { return detail::create_array(wrapped().points); }
object mesh::point_selection() { return detail::wrap_const_array(wrapped().point_selection); } 
object mesh::points() { return detail::wrap_const_array(wrapped().points); } 
object mesh::vertex_data() { return wrap(wrapped().vertex_data); } 
object mesh::writable_point_selection() { return detail::wrap_non_const_array(wrapped().point_selection); } 
object mesh::writable_points() { return detail::wrap_non_const_array(wrapped().points); } 
object mesh::writable_vertex_data() { return wrap(wrapped().vertex_data); } 

static object mesh_primitives(mesh& Self)
{
	return wrap(Self.wrapped().primitives);
}

const string_t mesh::repr()
{
	std::ostringstream buffer;
	buffer << wrapped();
	return buffer.str();
}

const string_t mesh::str()
{
	std::ostringstream buffer;
	buffer << "<k3d.mesh object wrapping mesh " << &wrapped() << ">";
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

	return wrap(Self.wrapped().at(Item).get());
}

static object mesh_primitives_t_create(mesh_primitives_t_wrapper& Self, const string_t& Type)
{
	return wrap(Self.wrapped().create(Type));
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// define_class_mesh 

void define_class_mesh()
{
	scope outer = class_<mesh>("mesh", 
		"Stores a heterogeneous collection of geometric mesh primitives.", no_init)
		.def("vertex_data", &mesh::vertex_data,
			"Returns a L{const_named_arrays} object containing a collection of immutable (read-only) per-vertex data, or None.")
		.def("point_selection", &mesh::point_selection,
			"Returns an immutable (read-only) L{const_double_array} object containing the selection state of every vertex in the mesh, or None.")
		.def("points", &mesh::points,
			"Returns an immutable (read-only) L{const_point3_array} object containing the geometric coordinates of every vertex in the mesh, or None.")
		.def("writable_vertex_data", &mesh::writable_vertex_data,
			"Returns a L{named_arrays} object containing a collection of mutable (read-write) per-vertex data, or None.")
		.def("writable_point_selection", &mesh::writable_point_selection,
			"Returns a mutable (read-write) L{double_array} object containing the selection state of every vertex in the mesh, or None.")
		.def("writable_points", &mesh::writable_points,
			"Returns a mutable (read-write) L{point3_array} object containing the geometric coordinates of every vertex in the mesh, or None.")
		.def("copy", &mesh::copy,
			"Store a shallow copy of the given L{mesh}.")
		.def("create_point_selection", &mesh::create_point_selection,
			"Creates and returns a new L{double_array} object used to store the selection state of every vertex in the mesh.")
		.def("create_points", &mesh::create_points,
			"Creates and returns a new L{point3_array} object used to store the geometric coordinates of every vertex in the mesh.")
		.def("primitives", &mesh_primitives,
			"Returns the set of L{const_primitive} objects in the mesh.")
		.def("__repr__", &mesh::repr)
		.def("__str__", &mesh::str);

	class_<const_mesh_primitive_wrapper>("const_primitive", no_init)
		.def("type", &const_mesh_primitive_get_type)
		.def("structure", &const_mesh_primitive_get_structure)
		.def("attributes", &const_mesh_primitive_get_attributes)
		;

	class_<mesh_primitive_wrapper>("primitive", no_init)
		.def("type", &mesh_primitive_get_type)
		.def("structure", &mesh_primitive_get_structure)
		.def("attributes", &mesh_primitive_get_attributes)
		;

	class_<const_mesh_primitives_t_wrapper>("const_primitives_t", no_init)
		.def("__len__", &utility::wrapped_len<const_mesh_primitives_t_wrapper>)
		.def("__getitem__", &const_mesh_primitives_t_get_item)
		;

	class_<mesh_primitives_t_wrapper>("primitives_t", no_init)
		.def("__len__", &utility::wrapped_len<mesh_primitives_t_wrapper>)
		.def("__getitem__", &mesh_primitives_t_get_item)
		.def("create", &mesh_primitives_t_create)
		;
}

} // namespace python

} // namespace k3d

