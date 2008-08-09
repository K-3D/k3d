// K-3D
// Copyright (c) 1995-2006, Timothy M. Shead
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

#include "const_named_arrays_python.h"

using namespace boost::python;

namespace k3d
{

namespace python
{

//////////////////////////////////////////////////////////////////////////////////////////////////
// const_named_arrays

const_named_arrays::const_named_arrays(const k3d::mesh::attribute_arrays_t& NamedArrays) :
	wrapped(NamedArrays)
{
}

list const_named_arrays::array_names()
{
	list results;

	for(k3d::mesh::attribute_arrays_t::const_iterator array = wrapped.begin(); array != wrapped.end(); ++array)
		results.append(array->first);

	return results;
}

object const_named_arrays::array(const std::string& Name)
{
	if(!wrapped.count(Name))
		throw std::runtime_error("Unknown array name: " + Name);

	return wrap_array(wrapped.find(Name)->second.get());
}

int const_named_arrays::len()
{
	return wrapped.size();
}

object const_named_arrays::get_item(int item)
{
	if(item < 0 || item >= wrapped.size())
		throw std::out_of_range("index out-of-range");

	k3d::mesh::attribute_arrays_t::const_iterator array_iterator = wrapped.begin();
	std::advance(array_iterator, item);

	return wrap_array(array_iterator->second.get());
}

object const_named_arrays::wrap_array(const k3d::array* const Array)
{
	return object();
}

void const_named_arrays::define_class()
{
	class_<const_named_arrays>("const_named_arrays", 
		"Stores an immutable (read-only) collection of named arrays, typically those stored as part of a L{mesh}.", no_init)
		.def("array_names", &const_named_arrays::array_names,
			"Returns a list containing names for all the arrays in the collection.")
		.def("array", &const_named_arrays::array,
			"Returns the array with the given name, or throws an exception.")
		.def("__len__", &const_named_arrays::len)
		.def("__getitem__", &const_named_arrays::get_item);
}

} // namespace python

} // namespace k3d

