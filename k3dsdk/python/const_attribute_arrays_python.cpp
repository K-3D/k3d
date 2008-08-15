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

#include "const_attribute_arrays_python.h"
#include "utility_python.h"

#include <boost/python.hpp>
using namespace boost::python;

namespace k3d
{

namespace python
{

static list array_names(const_attribute_arrays_wrapper& Self)
{
	list results;

	for(k3d::attribute_arrays::const_iterator array = Self.wrapped().begin(); array != Self.wrapped().end(); ++array)
		results.append(array->first);

	return results;
}

static object array(const_attribute_arrays_wrapper& Self, const string_t& Name)
{
	if(!Self.wrapped().count(Name))
		return object();

	return wrap(Self.wrapped().find(Name)->second.get());
}

static object get_item(const_attribute_arrays_wrapper& Self, int Item)
{
	if(Item < 0 || Item >= Self.wrapped().size())
		throw std::out_of_range("index out-of-range");

	k3d::attribute_arrays::const_iterator iterator = Self.wrapped().begin();
	std::advance(iterator, Item);

	return wrap(iterator->second.get());
}

void define_class_const_attribute_arrays()
{
	class_<const_attribute_arrays_wrapper>("const_attribute_arrays", 
		"Stores an immutable (read-only) collection of attribute arrays (named arrays with identical lengths).", no_init)
		.def("array_names", &array_names,
			"Returns a list containing names for all the arrays in the collection.")
		.def("array", &array,
			"Returns the array with the given name if it exists, or None.")
		.def("__len__", &utility::wrapped_len<const_attribute_arrays_wrapper>)
		.def("__getitem__", &get_item);
}

} // namespace python

} // namespace k3d

