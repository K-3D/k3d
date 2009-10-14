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

#include <k3dsdk/python/named_tables_python.h>
#include <k3dsdk/python/utility_python.h>

#include <boost/python.hpp>
using namespace boost::python;

namespace k3d
{

namespace python
{

///////////////////////////////////////////////////////////////////////////////////////////////////
// named_tables

static object named_tables_get_item(named_tables_wrapper& Self, int Item)
{
	if(Item < 0 || Item >= Self.wrapped().size())
		throw std::out_of_range("index out-of-range");

	k3d::named_tables::iterator item(Self.wrapped().begin());
	std::advance(item, Item);

	return wrap(item->second);
}

static object named_tables_create(named_tables_wrapper& Self, const string_t& Name)
{
	if(Name.empty())
		throw std::runtime_error("empty attribute name");

	if(Self.wrapped().count(Name))
		throw std::runtime_error("name already exists");

	Self.wrapped().insert(std::make_pair(Name, k3d::table()));
	return wrap(Self.wrapped()[Name]);
}

static void delete_1(named_tables_wrapper& Self, const string_t& Name)
{
	Self.wrapped().erase(Name);	
}

static list keys(named_tables_wrapper& Self)
{
	list results;

	for(k3d::named_tables::const_iterator table = Self.wrapped().begin(); table != Self.wrapped().end(); ++table)
		results.append(table->first);

	return results;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// define_class_named_tables 

void define_class_named_tables()
{
	class_<named_tables_wrapper>("named_tables", no_init)
		.def("__len__", &utility::wrapped_len<named_tables_wrapper>)
		.def("__getitem__", &utility::wrapped_get_wrapped_item_by_key<named_tables_wrapper>)
		.def("__getitem__", &named_tables_get_item)
		.def("create", &named_tables_create)
		.def("delete", &delete_1, "Deletes a set of attribute arrays with the given name, if any.")
		.def("keys", &keys, "Returns a list containing names for all the tables in the collection.")
		;
}

} // namespace python

} // namespace k3d

