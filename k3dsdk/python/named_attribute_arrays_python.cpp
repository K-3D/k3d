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

#include "named_attribute_arrays_python.h"
#include "utility_python.h"

#include <boost/python.hpp>
using namespace boost::python;

namespace k3d
{

namespace python
{

///////////////////////////////////////////////////////////////////////////////////////////////////
// named_attribute_arrays

static object named_attribute_arrays_create(named_attribute_arrays_wrapper& Self, const string_t& Name)
{
	if(Name.empty())
		throw std::runtime_error("empty attribute name");

	if(Self.wrapped().count(Name))
		throw std::runtime_error("name already exists");

	Self.wrapped().insert(std::make_pair(Name, k3d::attribute_arrays()));
	return wrap(Self.wrapped()[Name]);
}

static void delete_1(named_attribute_arrays_wrapper& Self, const string_t& Name)
{
	Self.wrapped().erase(Name);	
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// define_class_named_attribute_arrays 

void define_class_named_attribute_arrays()
{
	class_<named_attribute_arrays_wrapper>("named_attribute_arrays", no_init)
		.def("__len__", &utility::wrapped_len<named_attribute_arrays_wrapper>)
		.def("__getitem__", &utility::wrapped_get_wrapped_item_by_key<named_attribute_arrays_wrapper>)
		.def("create", &named_attribute_arrays_create)
		.def("delete", &delete_1,
			"Deletes a set of attribute arrays with the given name, if any.")
		;
}

} // namespace python

} // namespace k3d

