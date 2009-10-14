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

#include <k3dsdk/python/table_python.h>
#include <k3dsdk/python/typed_array_python.h>
#include <k3dsdk/python/utility_python.h>

#include <k3dsdk/named_array_types.h>
#include <k3dsdk/type_registry.h>
#include <k3dsdk/typed_array.h>
#include <k3dsdk/uint_t_array.h>

#include <boost/python.hpp>
using namespace boost::python;

namespace k3d
{

namespace python
{

class table_array_factory
{
public:
	table_array_factory(const string_t& Name, const string_t& Type, boost::python::object& Array, k3d::table& Arrays) :
		name(Name),
		type(Type),
		array(Array),
		arrays(Arrays)
	{
		// Handle arrays of uint_t as a special-case ...
		if(Type == "k3d::uint_t")
			array = wrap(arrays.create<k3d::uint_t_array>(name));
	}

	template<typename T>
	void operator()(T) const
	{
		if(array != boost::python::object())
			return;

		if(type != k3d::type_string<T>())
			return;

		array = wrap(arrays.create<k3d::typed_array<T> >(name));
	}

private:
	string_t name;
	string_t type;
	boost::python::object& array;
	k3d::table& arrays;
};

static list keys(table_wrapper& Self)
{
	list results;

	for(k3d::table::const_iterator array = Self.wrapped().begin(); array != Self.wrapped().end(); ++array)
		results.append(array->first);

	return results;
}

static object create(table_wrapper& Self, const string_t& Name, const string_t& Type)
{
	if(Name.empty())
		throw std::runtime_error("Empty array name");

	boost::python::object result;
	boost::mpl::for_each<k3d::named_array_types>(table_array_factory(Name, Type, result, Self.wrapped()));
	if(result == boost::python::object())
		throw std::runtime_error("Cannot create array [" + Name + "] with unknown type [" + Type + "]");

	return result;
}

static object create_array(table_wrapper& Self, const string_t& Name, const string_t& Type)
{
	k3d::log() << warning << "create_array() is deprecated, use create() instead." << std::endl;
	return create(Self, Name, Type);
}

static void delete_1(table_wrapper& Self, const string_t& Name)
{
	Self.wrapped().erase(Name);	
}

static uint_t row_count(const table_wrapper& Self)
{
	return Self.wrapped().row_count();
}

static void set_row_count(table_wrapper& Self, const uint_t NewSize)
{
	Self.wrapped().set_row_count(NewSize);
}

static object get_item1(table_wrapper& Self, const string_t& Key)
{
	k3d::table::iterator iterator = Self.wrapped().find(Key);
	if(iterator == Self.wrapped().end())
		throw std::runtime_error("unknown key: " + Key);

	return wrap_array(iterator->second.writable());
}

static object get_item2(table_wrapper& Self, int Item)
{
	if(Item < 0 || Item >= Self.wrapped().column_count())
		throw std::out_of_range("index out-of-range");

	k3d::table::iterator iterator = Self.wrapped().begin();
	std::advance(iterator, Item);

	return wrap_array(iterator->second.writable());
}

void define_class_table()
{
	class_<table_wrapper>("table", 
		"Stores a mutable (read-write) collection of attribute arrays (named arrays of equal length).", no_init)
		.def("keys", &keys,
			"Returns a list containing names for all the arrays in the collection.")
		.def("create", &create,
			"Creates an array with given name and type.")
		.def("create_array", &create_array,
			"Creates an array with given name and type.")
		.def("delete", &delete_1,
			"Deletes an array with given name, if any.")
		.def("set_row_count", &set_row_count,
			"Sets the size of every array in the collection.")
		.def("__len__", &row_count)
		.def("__getitem__", &get_item1)
		.def("__getitem__", &get_item2)
		;
}

} // namespace python

} // namespace k3d

