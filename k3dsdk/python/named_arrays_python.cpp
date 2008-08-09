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

#include "named_arrays_python.h"

#include <k3dsdk/named_array_types.h>
#include <k3dsdk/type_registry.h>

using namespace boost::python;

namespace k3d
{

namespace python
{

//////////////////////////////////////////////////////////////////////////////////////////////////
// named_arrays::array_factory

class named_arrays::array_factory
{
public:
	array_factory(const k3d::string_t& Name, const k3d::string_t& Type, boost::python::object& Array, k3d::attribute_arrays& Arrays) :
		name(Name),
		type(Type),
		array(Array),
		arrays(Arrays)
	{
	}

	template<typename T>
	void operator()(T) const
	{
		if(array != boost::python::object())
			return;

		if(type != k3d::type_string<T>())
			return;

		k3d::typed_array<T>* const new_array = new k3d::typed_array<T>();
		arrays[name].reset(new_array);
		array = boost::python::object(k3d::python::array<k3d::typed_array<T> >(*new_array));
	}

private:
	k3d::string_t name;
	k3d::string_t type;
	boost::python::object& array;
	k3d::attribute_arrays& arrays;
};

//////////////////////////////////////////////////////////////////////////////////////////////////
// named_arrays

named_arrays::named_arrays(k3d::attribute_arrays& NamedArrays) :
	wrapped(NamedArrays)
{
}

list named_arrays::array_names()
{
	list results;

	for(k3d::attribute_arrays::const_iterator array = wrapped.begin(); array != wrapped.end(); ++array)
		results.append(array->first);

	return results;
}

object named_arrays::array(const k3d::string_t& Name)
{
	if(!wrapped.count(Name))
		throw std::runtime_error("Unknown array name: " + Name);

	return wrap_array(wrapped.find(Name)->second.get());
}

object named_arrays::create_array(const k3d::string_t& Name, const k3d::string_t& Type)
{
	if(Name.empty())
		throw std::runtime_error("Empty array name");

	boost::python::object result;
	boost::mpl::for_each<k3d::named_array_types>(array_factory(Name, Type, result, wrapped));
	if(result == boost::python::object())
		throw std::runtime_error("Cannot create array [" + Name + "] with unknown type [" + Name + "]");

	return result;
}

int named_arrays::len()
{
	return wrapped.size();
}

object named_arrays::get_item(int item)
{
	if(item < 0 || item >= wrapped.size())
		throw std::out_of_range("index out-of-range");

	k3d::attribute_arrays::const_iterator array_iterator = wrapped.begin();
	std::advance(array_iterator, item);

	return wrap_array(array_iterator->second.get());
}

object named_arrays::wrap_array(const k3d::array* const Array)
{
	return object();
}

void named_arrays::define_class()
{
	class_<named_arrays>("named_arrays", 
		"Stores a mutable (read-write) collection of named arrays, typically those stored as part of a L{mesh}.", no_init)
		.def("array_names", &named_arrays::array_names,
			"Returns a list containing names for all the arrays in the collection.")
		.def("array", &named_arrays::array,
			"Returns the array with the given name, or throws an exception.")
		.def("create_array", &named_arrays::create_array,
			"Creates an array with given name and type.")
		.def("__len__", &named_arrays::len)
		.def("__getitem__", &named_arrays::get_item);
}

} // namespace python

} // namespace k3d

