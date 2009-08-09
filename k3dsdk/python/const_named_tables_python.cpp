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

#include "const_named_tables_python.h"
#include "utility_python.h"

#include <boost/python.hpp>
using namespace boost::python;

namespace k3d
{

namespace python
{

static object const_named_tables_get_item(const_named_tables_wrapper& Self, int Item)
{
	if(Item < 0 || Item >= Self.wrapped().size())
		throw std::out_of_range("index out-of-range");

	k3d::named_tables::const_iterator item(Self.wrapped().begin());
	std::advance(item, Item);

	return wrap(item->second);
}

static list keys(const_named_tables_wrapper& Self)
{
	list results;

	for(k3d::named_tables::const_iterator table = Self.wrapped().begin(); table != Self.wrapped().end(); ++table)
		results.append(table->first);

	return results;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// define_class_const_named_tables 

void define_class_const_named_tables()
{
	class_<const_named_tables_wrapper>("const_named_tables", no_init)
		.def("__len__", &utility::wrapped_len<const_named_tables_wrapper>)
		.def("__getitem__", &utility::wrapped_get_wrapped_item_by_key<const_named_tables_wrapper>)
		.def("__getitem__", &const_named_tables_get_item)
		.def("keys", &keys, "Returns a list containing names for all the tables in the collection.")
		;
}

} // namespace python

} // namespace k3d

