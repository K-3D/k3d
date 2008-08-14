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

#include "const_array_python.h"
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

static boost::python::object get_item_imaterial(interface_wrapper<const k3d::typed_array<k3d::imaterial*> >& Self, int Item)
{
	if(Item < 0 || Item >= Self.wrapped().size())
		throw std::out_of_range("index out-of-range");

	return wrap(Self.wrapped().at(Item));
}

static boost::python::object get_item_inode(interface_wrapper<const k3d::typed_array<k3d::inode*> >& Self, int Item)
{
	if(Item < 0 || Item >= Self.wrapped().size())
		throw std::out_of_range("index out-of-range");

	return wrap(Self.wrapped().at(Item));
}

//////////////////////////////////////////////////////////////////////////////////////////////
// define_class_const_array

template<typename array_type>
static void define_class_const_array(const char* const ClassName, const char* const DocString)
{
	boost::python::class_<interface_wrapper<array_type> >(ClassName, DocString, boost::python::no_init)
		.def("__len__", &len<array_type>)
		.def("__getitem__", &get_item<array_type>);
}

template<>
void define_class_const_array<const k3d::typed_array<k3d::imaterial*> >(const char* const ClassName, const char* const DocString)
{
	typedef const k3d::typed_array<k3d::imaterial*> array_type;

	boost::python::class_<interface_wrapper<array_type> >(ClassName, DocString, boost::python::no_init)
		.def("__len__", &len<array_type>)
		.def("__getitem__", &get_item_imaterial);
}

template<>
void define_class_const_array<const k3d::typed_array<k3d::inode*> >(const char* const ClassName, const char* const DocString)
{
	typedef const k3d::typed_array<k3d::inode*> array_type;

	boost::python::class_<interface_wrapper<array_type> >(ClassName, DocString, boost::python::no_init)
		.def("__len__", &len<array_type>)
		.def("__getitem__", &get_item_inode);
}

//////////////////////////////////////////////////////////////////////////////////////////////
// define_const_array_classes

void define_const_array_classes()
{
	define_class_const_array<const k3d::typed_array<k3d::bool_t> >("const_k3d_bool_t_array",
		"Stores an immutable (read-only) collection of boolean values.");
	define_class_const_array<const k3d::typed_array<k3d::double_t> >("const_k3d_double_t_array",
		"Stores an immutable (read-only) collection of floating-point values.");
	define_class_const_array<const k3d::typed_array<k3d::int8_t> >("const_k3d_int8_t_array",
		"Stores an immutable (read-only) collection of 8-bit integer values.");
	define_class_const_array<const k3d::typed_array<k3d::int16_t> >("const_k3d_int16_t_array",
		"Stores an immutable (read-only) collection of 16-bit integer values.");
	define_class_const_array<const k3d::typed_array<k3d::int32_t> >("const_k3d_int32_t_array",
		"Stores an immutable (read-only) collection of 32-bit integer values.");
	define_class_const_array<const k3d::typed_array<k3d::int64_t> >("const_k3d_int64_t_array",
		"Stores an immutable (read-only) collection of 64-bit integer values.");
	define_class_const_array<const k3d::typed_array<k3d::mesh::polyhedra_t::polyhedron_type> >("const_k3d_mesh_polyhedra_t_polyhedron_type_array",
		"Stores an immutable (read-only) collection of polyhedron type values.");
	define_class_const_array<const k3d::typed_array<k3d::mesh::blobbies_t::operator_type> >("const_k3d_mesh_blobbies_t_operator_type_array",
		"Stores an immutable (read-only) collection of blobby operator type values.");
	define_class_const_array<const k3d::typed_array<k3d::mesh::blobbies_t::primitive_type> >("const_k3d_mesh_blobbies_t_primitive_type_array",
		"Stores an immutable (read-only) collection of blobby primitive type values.");
	define_class_const_array<const k3d::typed_array<k3d::imaterial*> >("const_k3d_imaterial_array",
		"Stores an immutable (read-only) collection of L{imaterial} objects.");
	define_class_const_array<const k3d::typed_array<k3d::inode*> >("const_k3d_inode_array",
		"Stores an immutable (read-only) collection of L{inode} objects.");
	define_class_const_array<const k3d::typed_array<k3d::color> >("const_k3d_color_array",
		"Stores an immutable (read-only) collection of L{color} values.");
	define_class_const_array<const k3d::typed_array<k3d::matrix4> >("const_k3d_matrix4_array",
		"Stores an immutable (read-only) collection of L{matrix4} values.");
	define_class_const_array<const k3d::typed_array<k3d::normal3> >("const_k3d_normal3_array",
		"Stores an immutable (read-only) collection of L{normal3} values.");
	define_class_const_array<const k3d::typed_array<k3d::point2> >("const_k3d_point2_array",
		"Stores an immutable (read-only) collection of L{point2} values.");
	define_class_const_array<const k3d::typed_array<k3d::point3> >("const_k3d_point3_array",
		"Stores an immutable (read-only) collection of L{point3} values.");
	define_class_const_array<const k3d::typed_array<k3d::point4> >("const_k3d_point4_array",
		"Stores an immutable (read-only) collection of L{point4} values.");
	define_class_const_array<const k3d::typed_array<k3d::string_t> >("const_k3d_string_t_array",
		"Stores an immutable (read-only) collection of string values.");
	define_class_const_array<const k3d::typed_array<k3d::texture3> >("const_k3d_texture3_array",
		"Stores an immutable (read-only) collection of L{texture3} values.");
	define_class_const_array<const k3d::typed_array<k3d::uint8_t> >("const_k3d_uint8_t_array",
		"Stores an immutable (read-only) collection of unsigned 8-bit integer values.");
	define_class_const_array<const k3d::typed_array<k3d::uint16_t> >("const_k3d_uint16_t_array",
		"Stores an immutable (read-only) collection of unsigned 16-bit integer values.");
	define_class_const_array<const k3d::typed_array<k3d::uint32_t> >("const_k3d_uint32_t_array",
		"Stores an immutable (read-only) collection of unsigned 32-bit integer values.");
	define_class_const_array<const k3d::typed_array<k3d::uint64_t> >("const_k3d_uint64_t_array",
		"Stores an immutable (read-only) collection of unsigned 64-bit integer values.");
	define_class_const_array<const k3d::typed_array<k3d::vector2> >("const_k3d_vector2_array",
		"Stores an immutable (read-only) collection of L{vector2} values.");
	define_class_const_array<const k3d::typed_array<k3d::vector3> >("const_k3d_vector3_array",
		"Stores an immutable (read-only) collection of L{vector3} values.");
}

} // namespace python

} // namespace k3d

