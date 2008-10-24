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

#include "const_typed_array_python.h"
#include "imaterial_python.h"
#include "inode_python.h"
#include "utility_python.h"

#include <k3dsdk/mesh.h>
#include <k3dsdk/named_array_types.h>
#include <k3dsdk/typed_array.h>
#include <k3dsdk/uint_t_array.h>

#include <boost/python.hpp>

namespace k3d
{

namespace python
{

static boost::python::object get_item_imaterial(instance_wrapper<const k3d::typed_array<k3d::imaterial*> >& Self, int Item)
{
	if(Item < 0 || Item >= Self.wrapped().size())
		throw std::out_of_range("index out-of-range");

	return wrap(Self.wrapped().at(Item));
}

static boost::python::object get_item_inode(instance_wrapper<const k3d::typed_array<k3d::inode*> >& Self, int Item)
{
	if(Item < 0 || Item >= Self.wrapped().size())
		throw std::out_of_range("index out-of-range");

	return wrap(Self.wrapped().at(Item));
}

template<typename self_t>
static string_t get_metadata_value(const self_t& Self, const string_t& Name)
{
	return Self.wrapped().get_metadata_value(Name);
}

template<typename self_t>
static boost::python::dict get_metadata(const self_t& Self)
{
	boost::python::dict result;

	const array::metadata_t metadata = Self.wrapped().get_metadata();
	for(array::metadata_t::const_iterator pair = metadata.begin(); pair != metadata.end(); ++pair)
		result[pair->first] = pair->second;

	return result;
}

//////////////////////////////////////////////////////////////////////////////////////////////
// define_class_const_typed_array

template<typename array_type>
static void define_class_const_typed_array(const char* const ClassName, const char* const DocString)
{
	typedef instance_wrapper<array_type> wrapper_type;

	boost::python::class_<wrapper_type>(ClassName, DocString, boost::python::no_init)
		.def("__len__", &utility::wrapped_len<wrapper_type>)
		.def("__getitem__", &utility::wrapped_get_item<wrapper_type, typename array_type::value_type>)
		.def("get_metadata_value", &get_metadata_value<wrapper_type>)
		.def("get_metadata", &get_metadata<wrapper_type>)
		;
}

template<>
void define_class_const_typed_array<const k3d::typed_array<k3d::imaterial*> >(const char* const ClassName, const char* const DocString)
{
	typedef const k3d::typed_array<k3d::imaterial*> array_type;
	typedef instance_wrapper<array_type> wrapper_type;

	boost::python::class_<wrapper_type>(ClassName, DocString, boost::python::no_init)
		.def("__len__", &utility::wrapped_len<wrapper_type>)
		.def("__getitem__", &get_item_imaterial)
		.def("get_metadata_value", &get_metadata_value<wrapper_type>)
		.def("get_metadata", &get_metadata<wrapper_type>)
		;
}

template<>
void define_class_const_typed_array<const k3d::typed_array<k3d::inode*> >(const char* const ClassName, const char* const DocString)
{
	typedef const k3d::typed_array<k3d::inode*> array_type;
	typedef instance_wrapper<array_type> wrapper_type;

	boost::python::class_<wrapper_type>(ClassName, DocString, boost::python::no_init)
		.def("__len__", &utility::wrapped_len<wrapper_type>)
		.def("__getitem__", &get_item_inode)
		.def("get_metadata_value", &get_metadata_value<wrapper_type>)
		.def("get_metadata", &get_metadata<wrapper_type>)
		;
}

//////////////////////////////////////////////////////////////////////////////////////////////
// const_typed_array_array_factory

class const_typed_array_array_factory
{
public:
	const_typed_array_array_factory(const k3d::array& Input, boost::python::object& Output) :
		input(Input),
		output(Output)
	{
		output = boost::python::object();
	}

	template<typename T>
	void operator()(T) const
	{
		if(output != boost::python::object())
			return;

		typedef const k3d::typed_array<T> array_type;

		if(array_type* const array = dynamic_cast<array_type*>(&input))
			output = wrap(array);
	}

private:
	const k3d::array& input;
	boost::python::object& output;
};

//////////////////////////////////////////////////////////////////////////////////////////////
// wrap_array

boost::python::object wrap_array(const k3d::array* Wrapped)
{
	if(Wrapped)
		return wrap_array(*Wrapped);

	return boost::python::object();
}

boost::python::object wrap_array(const k3d::array& Wrapped)
{
	boost::python::object result;
	boost::mpl::for_each<k3d::named_array_types>(const_typed_array_array_factory(Wrapped, result));
	return result;
}

//////////////////////////////////////////////////////////////////////////////////////////////
// define_const_typed_array_classes

void define_const_typed_array_classes()
{
	define_class_const_typed_array<const k3d::typed_array<k3d::bool_t> >("const_typed_array_bool_t",
		"Stores an immutable (read-only) collection of boolean values.");
	define_class_const_typed_array<const k3d::typed_array<k3d::double_t> >("const_typed_array_double_t",
		"Stores an immutable (read-only) collection of floating-point values.");
	define_class_const_typed_array<const k3d::typed_array<k3d::int8_t> >("const_typed_array_int8_t",
		"Stores an immutable (read-only) collection of 8-bit integer values.");
	define_class_const_typed_array<const k3d::typed_array<k3d::int16_t> >("const_typed_array_int16_t",
		"Stores an immutable (read-only) collection of 16-bit integer values.");
	define_class_const_typed_array<const k3d::typed_array<k3d::int32_t> >("const_typed_array_int32_t",
		"Stores an immutable (read-only) collection of 32-bit integer values.");
	define_class_const_typed_array<const k3d::typed_array<k3d::int64_t> >("const_typed_array_int64_t",
		"Stores an immutable (read-only) collection of 64-bit integer values.");
	define_class_const_typed_array<const k3d::typed_array<k3d::mesh::polyhedra_t::polyhedron_type> >("const_typed_array_mesh_polyhedra_t_polyhedron_type",
		"Stores an immutable (read-only) collection of polyhedron type values.");
	define_class_const_typed_array<const k3d::typed_array<k3d::mesh::blobbies_t::operator_type> >("const_typed_array_mesh_blobbies_t_operator_type",
		"Stores an immutable (read-only) collection of blobby operator type values.");
	define_class_const_typed_array<const k3d::typed_array<k3d::mesh::blobbies_t::primitive_type> >("const_typed_array_mesh_blobbies_t_primitive_type",
		"Stores an immutable (read-only) collection of blobby primitive type values.");
	define_class_const_typed_array<const k3d::typed_array<k3d::imaterial*> >("const_typed_array_imaterial",
		"Stores an immutable (read-only) collection of L{imaterial} objects.");
	define_class_const_typed_array<const k3d::typed_array<k3d::inode*> >("const_typed_array_inode",
		"Stores an immutable (read-only) collection of L{inode} objects.");
	define_class_const_typed_array<const k3d::typed_array<k3d::color> >("const_typed_array_color",
		"Stores an immutable (read-only) collection of L{color} values.");
	define_class_const_typed_array<const k3d::typed_array<k3d::matrix4> >("const_typed_array_matrix4",
		"Stores an immutable (read-only) collection of L{matrix4} values.");
	define_class_const_typed_array<const k3d::typed_array<k3d::normal3> >("const_typed_array_normal3",
		"Stores an immutable (read-only) collection of L{normal3} values.");
	define_class_const_typed_array<const k3d::typed_array<k3d::point2> >("const_typed_array_point2",
		"Stores an immutable (read-only) collection of L{point2} values.");
	define_class_const_typed_array<const k3d::typed_array<k3d::point3> >("const_typed_array_point3",
		"Stores an immutable (read-only) collection of L{point3} values.");
	define_class_const_typed_array<const k3d::typed_array<k3d::point4> >("const_typed_array_point4",
		"Stores an immutable (read-only) collection of L{point4} values.");
	define_class_const_typed_array<const k3d::typed_array<k3d::string_t> >("const_typed_array_string_t",
		"Stores an immutable (read-only) collection of string values.");
	define_class_const_typed_array<const k3d::typed_array<k3d::texture3> >("const_typed_array_texture3",
		"Stores an immutable (read-only) collection of L{texture3} values.");
	define_class_const_typed_array<const k3d::typed_array<k3d::uint8_t> >("const_typed_array_uint8_t",
		"Stores an immutable (read-only) collection of unsigned 8-bit integer values.");
	define_class_const_typed_array<const k3d::typed_array<k3d::uint16_t> >("const_typed_array_uint16_t",
		"Stores an immutable (read-only) collection of unsigned 16-bit integer values.");
	define_class_const_typed_array<const k3d::typed_array<k3d::uint32_t> >("const_typed_array_uint32_t",
		"Stores an immutable (read-only) collection of unsigned 32-bit integer values.");
	define_class_const_typed_array<const k3d::typed_array<k3d::uint64_t> >("const_typed_array_uint64_t",
		"Stores an immutable (read-only) collection of unsigned 64-bit integer values.");
	define_class_const_typed_array<const k3d::typed_array<k3d::vector2> >("const_typed_array_vector2",
		"Stores an immutable (read-only) collection of L{vector2} values.");
	define_class_const_typed_array<const k3d::typed_array<k3d::vector3> >("const_typed_array_vector3",
		"Stores an immutable (read-only) collection of L{vector3} values.");

	define_class_const_typed_array<const k3d::uint_t_array>("const_uint_t_array",
		"Stores an immutable (read-only) collection of 32- or 64-bit values (depending on platform).");
}

} // namespace python

} // namespace k3d

