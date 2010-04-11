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

#include <k3dsdk/python/const_table_python.h>
#include <k3dsdk/python/const_typed_array_python.h>
#include <k3dsdk/python/utility_python.h>

#include <boost/python.hpp>
using namespace boost::python;

namespace k3d
{

namespace python
{

static list keys(const_table_wrapper& Self)
{
	list results;

	for(k3d::table::const_iterator array = Self.wrapped().begin(); array != Self.wrapped().end(); ++array)
		results.append(array->first);

	return results;
}

static object get_item1(const_table_wrapper& Self, const string_t& Key)
{
	k3d::table::const_iterator iterator = Self.wrapped().find(Key);
	if(iterator == Self.wrapped().end())
		throw std::runtime_error("unknown key: " + Key);

	return wrap_array(iterator->second.get());
}

static object get_item2(const_table_wrapper& Self, int Item)
{
	if(Item < 0 || Item >= Self.wrapped().column_count())
		throw std::out_of_range("index out-of-range");

	k3d::table::const_iterator iterator = Self.wrapped().begin();
	std::advance(iterator, Item);

	return wrap_array(iterator->second.get());
}

static uint_t row_count(const_table_wrapper& Self)
{
	return Self.wrapped().row_count();
}

void define_class_const_table()
{
	class_<const_table_wrapper>("const_table", 
		"Stores an immutable (read-only) collection of attribute arrays (named arrays with identical lengths).", no_init)
		.def("keys", &keys,
			"Returns a list containing names for all the arrays in the collection.")
		.def("__len__", &row_count)
		.def("__getitem__", &get_item1)
		.def("__getitem__", &get_item2)
		;
}

} // namespace python

} // namespace k3d

