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

#include "array_python.h"
#include "imaterial_python.h"
#include "inode_python.h"

#include <k3dsdk/mesh.h>
#include <k3dsdk/named_array_types.h>
#include <k3dsdk/typed_array.h>

#include <boost/python.hpp>

namespace k3d
{

namespace python
{

template<typename array_type>
static int len(interface_wrapper<array_type>& Self)
{
	return Self.wrapped().size();
}

template<typename array_type>
static typename array_type::value_type get_item(interface_wrapper<array_type>& Self, int Item)
{
	if(Item < 0 || Item >= Self.wrapped().size())
		throw std::out_of_range("index out-of-range");

	return Self.wrapped().at(Item);
}

static boost::python::object get_item_imaterial(interface_wrapper<k3d::typed_array<k3d::imaterial*> >& Self, int Item)
{
	if(Item < 0 || Item >= Self.wrapped().size())
		throw std::out_of_range("index out-of-range");

	return wrap(Self.wrapped().at(Item));
}

static boost::python::object get_item_inode(interface_wrapper<k3d::typed_array<k3d::inode*> >& Self, int Item)
{
	if(Item < 0 || Item >= Self.wrapped().size())
		throw std::out_of_range("index out-of-range");

	return wrap(Self.wrapped().at(Item));
}

template<typename array_type>
static void set_item(interface_wrapper<array_type>& Self, int Item, const typename array_type::value_type& Value)
{
	if(Item < 0)
		throw std::out_of_range("index out-of-range");

	if(static_cast<uint_t>(Item) >= Self.wrapped().size())
		Self.wrapped().resize(Item + 1);

	Self.wrapped()[Item] = Value;
}

static void set_item_imaterial(interface_wrapper<k3d::typed_array<k3d::imaterial*> >& Self, int Item, const boost::python::object& Value)
{
	if(Item < 0)
		throw std::out_of_range("index out-of-range");

	if(static_cast<uint_t>(Item) >= Self.wrapped().size())
		Self.wrapped().resize(Item + 1);

	if(Value)
	{
		imaterial_wrapper material = boost::python::extract<imaterial_wrapper>(Value);
		Self.wrapped()[Item] = &material.wrapped();
	}
	else
	{
		Self.wrapped()[Item] = static_cast<k3d::imaterial*>(0);
	}
}

static void set_item_inode(interface_wrapper<k3d::typed_array<k3d::inode*> >& Self, int Item, const boost::python::object& Value)
{
	if(Item < 0)
		throw std::out_of_range("index out-of-range");

	if(static_cast<uint_t>(Item) >= Self.wrapped().size())
		Self.wrapped().resize(Item + 1);

	if(Value)
	{
		inode_wrapper node = boost::python::extract<inode_wrapper>(Value);
		Self.wrapped()[Item] = &node.wrapped();
	}
	else
	{
		Self.wrapped()[Item] = static_cast<k3d::inode*>(0);
	}
}

template<typename array_type>
static void append(interface_wrapper<array_type>& Self, const typename array_type::value_type& Value)
{
	Self.wrapped().push_back(Value);
}

static void append_imaterial(interface_wrapper<k3d::typed_array<k3d::imaterial*> >& Self, const boost::python::object& Value)
{
	if(Value)
	{
		imaterial_wrapper material = boost::python::extract<imaterial_wrapper>(Value);
		Self.wrapped().push_back(&material.wrapped());
	}
	else
	{
		Self.wrapped().push_back(static_cast<k3d::imaterial*>(0));
	}
}

static void append_inode(interface_wrapper<k3d::typed_array<k3d::inode*> >& Self, const boost::python::object& Value)
{
	if(Value)
	{
		inode_wrapper node = boost::python::extract<inode_wrapper>(Value);
		Self.wrapped().push_back(&node.wrapped());
	}
	else
	{
		Self.wrapped().push_back(static_cast<k3d::inode*>(0));
	}
}

template<typename array_type>
static void assign(interface_wrapper<array_type>& Self, const boost::python::list& Value)
{
	array_type& storage = Self.wrapped();

	const uint_t count = boost::python::len(Value);
	storage.resize(count);
	for(uint_t i = 0; i != count; ++i)
		storage[i] = boost::python::extract<typename array_type::value_type>(Value[i]);
}

void assign_imaterial(interface_wrapper<k3d::typed_array<k3d::imaterial*> >& Self, const boost::python::list& Value)
{
	k3d::typed_array<k3d::imaterial*>& storage = Self.wrapped();

	const uint_t count = boost::python::len(Value);
	storage.resize(count);
	for(uint_t i = 0; i != count; ++i)
	{
		if(Value[i])
		{
			imaterial_wrapper material = boost::python::extract<imaterial_wrapper>(Value[i]);
			storage[i] = &material.wrapped();
		}
		else
		{
			storage[i] = static_cast<k3d::imaterial*>(0);
		}
	}
}

void assign_inode(interface_wrapper<k3d::typed_array<k3d::inode*> >& Self, const boost::python::list& Value)
{
	k3d::typed_array<k3d::inode*>& storage = Self.wrapped();

	const uint_t count = boost::python::len(Value);
	storage.resize(count);
	for(uint_t i = 0; i != count; ++i)
	{
		if(Value[i])
		{
			inode_wrapper node = boost::python::extract<inode_wrapper>(Value[i]);
			storage[i] = &node.wrapped();
		}
		else
		{
			storage[i] = static_cast<k3d::inode*>(0);
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////
// define_class_array

template<typename array_type>
void define_class_array(const char* const ClassName, const char* const DocString)
{
	boost::python::class_<interface_wrapper<array_type> >(ClassName, DocString, boost::python::no_init)
		.def("__len__", &len<array_type>)
		.def("__getitem__", &get_item<array_type>)
		.def("__setitem__", &set_item<array_type>)
		.def("append", &append<array_type>,
			"Append a value to the end of the array, growing its size by one.")
		.def("assign", &assign<array_type>,
			"Replace the contents of the array with a list of values.");
}

template<>
void define_class_array<k3d::typed_array<k3d::imaterial*> >(const char* const ClassName, const char* const DocString)
{
	typedef k3d::typed_array<k3d::imaterial*> array_type;

	boost::python::class_<interface_wrapper<array_type> >(ClassName, DocString, boost::python::no_init)
		.def("__len__", &len<array_type>)
		.def("__getitem__", &get_item_imaterial)
		.def("__setitem__", &set_item_imaterial)
		.def("append", &append_imaterial,
			"Append a value to the end of the array, growing its size by one.")
		.def("assign", &assign_imaterial,
			"Replace the contents of the array with a list of values.");
}

template<>
void define_class_array<k3d::typed_array<k3d::inode*> >(const char* const ClassName, const char* const DocString)
{
	typedef k3d::typed_array<k3d::inode*> array_type;

	boost::python::class_<interface_wrapper<array_type> >(ClassName, DocString, boost::python::no_init)
		.def("__len__", &len<array_type>)
		.def("__getitem__", &get_item_inode)
		.def("__setitem__", &set_item_inode)
		.def("append", &append_inode,
			"Append a value to the end of the array, growing its size by one.")
		.def("assign", &assign_inode,
			"Replace the contents of the array with a list of values.");
}

////////////////////////////////////////////////////////////////////////////////////////
// define_array_classes

void define_array_classes()
{
	define_class_array<k3d::typed_array<k3d::bool_t> >("k3d_bool_t_array",
		"Stores a mutable (read-write) collection of boolean values.");
	define_class_array<k3d::typed_array<k3d::double_t> >("k3d_double_t_array",
		"Stores a mutable (read-write) collection of floating-point values.");
	define_class_array<k3d::typed_array<k3d::int8_t> >("k3d_int8_t_array",
		"Stores a mutable (read-write) collection of 8-bit integer values.");
	define_class_array<k3d::typed_array<k3d::int16_t> >("k3d_int16_t_array",
		"Stores a mutable (read-write) collection of 16-bit integer values.");
	define_class_array<k3d::typed_array<k3d::int32_t> >("k3d_int32_t_array",
		"Stores a mutable (read-write) collection of 32-bit integer values.");
	define_class_array<k3d::typed_array<k3d::int64_t> >("k3d_int64_t_array",
		"Stores a mutable (read-write) collection of 64-bit integer values.");
	define_class_array<k3d::typed_array<k3d::mesh::polyhedra_t::polyhedron_type> >("k3d_mesh_polyheda_t_polyhedron_type_array",
		"Stores a mutable (read-write) collection of polyhedron type values.");
	define_class_array<k3d::typed_array<k3d::mesh::blobbies_t::operator_type> >("k3d_mesh_blobbies_t_operator_type_array",
		"Stores a mutable (read-write) collection of blobby operator type values.");
	define_class_array<k3d::typed_array<k3d::mesh::blobbies_t::primitive_type> >("k3d_mesh_blobbies_t_primitive_type_array",
		"Stores a mutable (read-write) collection of blobby primitive type values.");
	define_class_array<k3d::typed_array<k3d::imaterial*> >("k3d_imaterial_array",
		"Stores a mutable (read-write) collection of L{imaterial} objects.");
	define_class_array<k3d::typed_array<k3d::inode*> >("k3d_inode_array",
		"Stores a mutable (read-write) collection of L{inode} objects.");
	define_class_array<k3d::typed_array<k3d::color> >("k3d_color_array",
		"Stores a mutable (read-write) collection of L{color} values.");
	define_class_array<k3d::typed_array<k3d::matrix4> >("k3d_matrix4_array",
		"Stores a mutable (read-write) collection of L{matrix4} values.");
	define_class_array<k3d::typed_array<k3d::normal3> >("k3d_normal3_array",
		"Stores a mutable (read-write) collection of L{normal3} values.");
	define_class_array<k3d::typed_array<k3d::point2> >("k3d_point2_array",
		"Stores a mutable (read-write) collection of L{point2} values.");
	define_class_array<k3d::typed_array<k3d::point3> >("k3d_point3_array",
		"Stores a mutable (read-write) collection of L{point3} values.");
	define_class_array<k3d::typed_array<k3d::point4> >("k3d_point4_array",
		"Stores a mutable (read-write) collection of L{point4} values.");
	define_class_array<k3d::typed_array<k3d::string_t> >("k3d_string_t_array",
		"Stores a mutable (read-write) collection of string values.");
	define_class_array<k3d::typed_array<k3d::texture3> >("k3d_texture3_array",
		"Stores a mutable (read-write) collection of L{texture} values.");
	define_class_array<k3d::typed_array<k3d::uint8_t> >("k3d_uint8_t_array",
		"Stores a mutable (read-write) collection of unsigned 8-bit integer values.");
	define_class_array<k3d::typed_array<k3d::uint16_t> >("k3d_uint16_t_array",
		"Stores a mutable (read-write) collection of unsigned 16-bit integer values.");
	define_class_array<k3d::typed_array<k3d::uint32_t> >("k3d_uint32_t_array",
		"Stores a mutable (read-write) collection of unsigned 32-bit integer values.");
	define_class_array<k3d::typed_array<k3d::uint64_t> >("k3d_uint64_t_array",
		"Stores a mutable (read-write) collection of unsigned 64-bit integer values.");
	define_class_array<k3d::typed_array<k3d::vector2> >("k3d_vector2_array",
		"Stores a mutable (read-write) collection of L{vector2} values.");
	define_class_array<k3d::typed_array<k3d::vector3> >("k3d_vector3_array",
		"Stores a mutable (read-write) collection of L{vector3} values.");
}

} // namespace python

} // namespace k3d

