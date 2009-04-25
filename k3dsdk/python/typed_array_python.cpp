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

#include "iunknown_python.h"
#include "typed_array_python.h"
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

static boost::python::object get_item_imaterial(instance_wrapper<k3d::typed_array<k3d::imaterial*> >& Self, int Item)
{
	if(Item < 0 || Item >= Self.wrapped().size())
		throw std::out_of_range("index out-of-range");

	return wrap(Self.wrapped().at(Item));
}

static boost::python::object get_item_inode(instance_wrapper<k3d::typed_array<k3d::inode*> >& Self, int Item)
{
	if(Item < 0 || Item >= Self.wrapped().size())
		throw std::out_of_range("index out-of-range");

	return wrap(Self.wrapped().at(Item));
}

template<typename array_type>
static void set_item(instance_wrapper<array_type>& Self, int Item, const typename array_type::value_type& Value)
{
	if(Item < 0 || Item >= Self.wrapped().size())
		throw std::out_of_range("index out-of-range");

	Self.wrapped()[Item] = Value;
}

static void set_item_imaterial(instance_wrapper<k3d::typed_array<k3d::imaterial*> >& Self, int Item, const boost::python::object& Value)
{
	if(Item < 0 || Item >= Self.wrapped().size())
		throw std::out_of_range("index out-of-range");

	if(Value)
	{
		Self.wrapped()[Item] = &dynamic_cast<k3d::imaterial&>(boost::python::extract<iunknown_wrapper>(Value)().wrapped());
	}
	else
	{
		Self.wrapped()[Item] = static_cast<k3d::imaterial*>(0);
	}
}

static void set_item_inode(instance_wrapper<k3d::typed_array<k3d::inode*> >& Self, int Item, const boost::python::object& Value)
{
	if(Item < 0 || Item >= Self.wrapped().size())
		throw std::out_of_range("index out-of-range");

	if(Value)
	{
		Self.wrapped()[Item] = &dynamic_cast<k3d::inode&>(boost::python::extract<iunknown_wrapper>(Value)().wrapped());
	}
	else
	{
		Self.wrapped()[Item] = static_cast<k3d::inode*>(0);
	}
}

template<typename array_type>
static void append(instance_wrapper<array_type>& Self, const typename array_type::value_type& Value)
{
	Self.wrapped().push_back(Value);
}

static void append_imaterial(instance_wrapper<k3d::typed_array<k3d::imaterial*> >& Self, const boost::python::object& Value)
{
	if(Value)
	{
		Self.wrapped().push_back(&dynamic_cast<k3d::imaterial&>(boost::python::extract<iunknown_wrapper>(Value)().wrapped()));
	}
	else
	{
		Self.wrapped().push_back(static_cast<k3d::imaterial*>(0));
	}
}

static void append_inode(instance_wrapper<k3d::typed_array<k3d::inode*> >& Self, const boost::python::object& Value)
{
	if(Value)
	{
		Self.wrapped().push_back(&dynamic_cast<k3d::inode&>(boost::python::extract<iunknown_wrapper>(Value)().wrapped()));
	}
	else
	{
		Self.wrapped().push_back(static_cast<k3d::inode*>(0));
	}
}

template<typename array_type>
static void assign(instance_wrapper<array_type>& Self, const boost::python::list& Value)
{
	array_type& storage = Self.wrapped();

	const uint_t count = boost::python::len(Value);
	storage.resize(count);
	for(uint_t i = 0; i != count; ++i)
		storage[i] = boost::python::extract<typename array_type::value_type>(Value[i]);
}

static void assign_imaterial(instance_wrapper<k3d::typed_array<k3d::imaterial*> >& Self, const boost::python::list& Value)
{
	k3d::typed_array<k3d::imaterial*>& storage = Self.wrapped();

	const uint_t count = boost::python::len(Value);
	storage.resize(count);
	for(uint_t i = 0; i != count; ++i)
	{
		if(Value[i])
		{
			storage[i] = &dynamic_cast<k3d::imaterial&>(boost::python::extract<iunknown_wrapper>(Value[i])().wrapped());
		}
		else
		{
			storage[i] = static_cast<k3d::imaterial*>(0);
		}
	}
}

static void assign_inode(instance_wrapper<k3d::typed_array<k3d::inode*> >& Self, const boost::python::list& Value)
{
	k3d::typed_array<k3d::inode*>& storage = Self.wrapped();

	const uint_t count = boost::python::len(Value);
	storage.resize(count);
	for(uint_t i = 0; i != count; ++i)
	{
		if(Value[i])
		{
			storage[i] = &dynamic_cast<k3d::inode&>(boost::python::extract<iunknown_wrapper>(Value[i])().wrapped());
		}
		else
		{
			storage[i] = static_cast<k3d::inode*>(0);
		}
	}
}

template<typename self_t>
static void set_metadata_value(self_t& Self, const string_t& Name, const string_t& Value)
{
	Self.wrapped().set_metadata_value(Name, Value);
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

template<typename self_t>
static void erase_metadata_value(self_t& Self, const string_t& Name)
{
	Self.wrapped().erase_metadata_value(Name);
}

////////////////////////////////////////////////////////////////////////////////////////
// define_class_typed_array

template<typename array_type>
static void define_class_typed_array(const char* const ClassName, const char* const DocString)
{
	typedef instance_wrapper<array_type> wrapper_type;

	boost::python::class_<wrapper_type>(ClassName, DocString, boost::python::no_init)
		.def("__len__", &utility::wrapped_len<wrapper_type>)
		.def("__getitem__", &utility::wrapped_get_item<wrapper_type, typename array_type::value_type>)
		.def("__setitem__", &set_item<array_type>)
		.def("append", &append<array_type>,
			"Append a value to the end of the array, growing its size by one.")
		.def("assign", &assign<array_type>,
			"Replace the contents of the array with a list of values.")
		.def("set_metadata_value", &set_metadata_value<wrapper_type>)
		.def("get_metadata_value", &get_metadata_value<wrapper_type>)
		.def("get_metadata", &get_metadata<wrapper_type>)
		.def("erase_metadata_value", &erase_metadata_value<wrapper_type>)
		;
}

template<>
void define_class_typed_array<k3d::typed_array<k3d::imaterial*> >(const char* const ClassName, const char* const DocString)
{
	typedef k3d::typed_array<k3d::imaterial*> array_type;
	typedef instance_wrapper<array_type> wrapper_type;

	boost::python::class_<wrapper_type>(ClassName, DocString, boost::python::no_init)
		.def("__len__", &utility::wrapped_len<wrapper_type>)
		.def("__getitem__", &get_item_imaterial)
		.def("__setitem__", &set_item_imaterial)
		.def("append", &append_imaterial,
			"Append a value to the end of the array, growing its size by one.")
		.def("assign", &assign_imaterial,
			"Replace the contents of the array with a list of values.")
		.def("set_metadata_value", &set_metadata_value<wrapper_type>)
		.def("get_metadata_value", &get_metadata_value<wrapper_type>)
		.def("get_metadata", &get_metadata<wrapper_type>)
		.def("erase_metadata_value", &erase_metadata_value<wrapper_type>)
		;
}

template<>
void define_class_typed_array<k3d::typed_array<k3d::inode*> >(const char* const ClassName, const char* const DocString)
{
	typedef k3d::typed_array<k3d::inode*> array_type;
	typedef instance_wrapper<array_type> wrapper_type;

	boost::python::class_<wrapper_type>(ClassName, DocString, boost::python::no_init)
		.def("__len__", &utility::wrapped_len<wrapper_type>)
		.def("__getitem__", &get_item_inode)
		.def("__setitem__", &set_item_inode)
		.def("append", &append_inode,
			"Append a value to the end of the array, growing its size by one.")
		.def("assign", &assign_inode,
			"Replace the contents of the array with a list of values.")
		.def("set_metadata_value", &set_metadata_value<wrapper_type>)
		.def("get_metadata_value", &get_metadata_value<wrapper_type>)
		.def("get_metadata", &get_metadata<wrapper_type>)
		.def("erase_metadata_value", &erase_metadata_value<wrapper_type>)
		;
}

//////////////////////////////////////////////////////////////////////////////////////////////
// typed_array_array_factory

class typed_array_array_factory
{
public:
	typed_array_array_factory(k3d::array& Input, boost::python::object& Output) :
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

		typedef k3d::typed_array<T> array_type;

		if(array_type* const array = dynamic_cast<array_type*>(&input))
			output = wrap(array);
	}

private:
	k3d::array& input;
	boost::python::object& output;
};

//////////////////////////////////////////////////////////////////////////////////////////////
// wrap_array

boost::python::object wrap_array(k3d::array* Wrapped)
{
	if(Wrapped)
		return wrap_array(*Wrapped);

	return boost::python::object();
}

boost::python::object wrap_array(k3d::array& Wrapped)
{
	boost::python::object result;
	boost::mpl::for_each<k3d::named_array_types>(typed_array_array_factory(Wrapped, result));
	return result;
}

////////////////////////////////////////////////////////////////////////////////////////
// define_typed_array_classes

void define_typed_array_classes()
{
	define_class_typed_array<k3d::typed_array<k3d::bool_t> >("typed_array_bool_t",
		"Stores a mutable (read-write) collection of boolean values.");
	define_class_typed_array<k3d::typed_array<k3d::double_t> >("typed_array_double_t",
		"Stores a mutable (read-write) collection of floating-point values.");
	define_class_typed_array<k3d::typed_array<k3d::int8_t> >("typed_array_int8_t",
		"Stores a mutable (read-write) collection of 8-bit integer values.");
	define_class_typed_array<k3d::typed_array<k3d::int16_t> >("typed_array_int16_t",
		"Stores a mutable (read-write) collection of 16-bit integer values.");
	define_class_typed_array<k3d::typed_array<k3d::int32_t> >("typed_array_int32_t",
		"Stores a mutable (read-write) collection of 32-bit integer values.");
	define_class_typed_array<k3d::typed_array<k3d::int64_t> >("typed_array_int64_t",
		"Stores a mutable (read-write) collection of 64-bit integer values.");
	define_class_typed_array<k3d::typed_array<k3d::imaterial*> >("typed_array_imaterial",
		"Stores a mutable (read-write) collection of L{imaterial} objects.");
	define_class_typed_array<k3d::typed_array<k3d::inode*> >("typed_array_inode",
		"Stores a mutable (read-write) collection of L{inode} objects.");
	define_class_typed_array<k3d::typed_array<k3d::color> >("typed_array_color",
		"Stores a mutable (read-write) collection of L{color} values.");
	define_class_typed_array<k3d::typed_array<k3d::matrix4> >("typed_array_matrix4",
		"Stores a mutable (read-write) collection of L{matrix4} values.");
	define_class_typed_array<k3d::typed_array<k3d::normal3> >("typed_array_normal3",
		"Stores a mutable (read-write) collection of L{normal3} values.");
	define_class_typed_array<k3d::typed_array<k3d::point2> >("typed_array_point2",
		"Stores a mutable (read-write) collection of L{point2} values.");
	define_class_typed_array<k3d::typed_array<k3d::point3> >("typed_array_point3",
		"Stores a mutable (read-write) collection of L{point3} values.");
	define_class_typed_array<k3d::typed_array<k3d::point4> >("typed_array_point4",
		"Stores a mutable (read-write) collection of L{point4} values.");
	define_class_typed_array<k3d::typed_array<k3d::string_t> >("typed_array_string_t",
		"Stores a mutable (read-write) collection of string values.");
	define_class_typed_array<k3d::typed_array<k3d::texture3> >("typed_array_texture3",
		"Stores a mutable (read-write) collection of L{texture} values.");
	define_class_typed_array<k3d::typed_array<k3d::uint8_t> >("typed_array_uint8_t",
		"Stores a mutable (read-write) collection of unsigned 8-bit integer values.");
	define_class_typed_array<k3d::typed_array<k3d::uint16_t> >("typed_array_uint16_t",
		"Stores a mutable (read-write) collection of unsigned 16-bit integer values.");
	define_class_typed_array<k3d::typed_array<k3d::uint32_t> >("typed_array_uint32_t",
		"Stores a mutable (read-write) collection of unsigned 32-bit integer values.");
	define_class_typed_array<k3d::typed_array<k3d::uint64_t> >("typed_array_uint64_t",
		"Stores a mutable (read-write) collection of unsigned 64-bit integer values.");
	define_class_typed_array<k3d::typed_array<k3d::vector2> >("typed_array_vector2",
		"Stores a mutable (read-write) collection of L{vector2} values.");
	define_class_typed_array<k3d::typed_array<k3d::vector3> >("typed_array_vector3",
		"Stores a mutable (read-write) collection of L{vector3} values.");

	define_class_typed_array<k3d::uint_t_array>("uint_t_array",
		"Stores a mutable (read-write) collection of 32- or 64-bit values (depending on platform).");
}

} // namespace python

} // namespace k3d

