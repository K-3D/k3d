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

#include "selection_python.h"

#include <k3dsdk/iomanip.h>

#include <boost/python.hpp>
#include <boost/python/detail/api_placeholder.hpp>
using namespace boost::python;

namespace k3d
{

namespace python
{

///////////////////////////////////////////////////////////////////////////////////////////////////
// selection 

class selection
{
public:
};

///////////////////////////////////////////////////////////////////////////////////////////////////
// storage 

typedef instance_wrapper<k3d::selection::storage> selection_storage_wrapper;

static const string_t selection_storage_get_type(selection_storage_wrapper& Self)
{
	return Self.wrapped().type;
}

static object selection_storage_get_structure(selection_storage_wrapper& Self)
{
	return wrap(Self.wrapped().structure);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// set

static int selection_set_len(k3d::selection::set& Self)
{
	return Self.size();
}

static object selection_set_get_item(k3d::selection::set& Self, int Item)
{
	if(Item < 0 || Item >= Self.size())
		throw std::out_of_range("index out-of-range");

	return wrap(Self[Item]);
}

static string_t selection_set_str(k3d::selection::set& Self)
{
	std::ostringstream buffer;
	buffer << "set: (" << Self.size() << ")\n";
	buffer << push_indent << Self << pop_indent;

	return buffer.str();
}

static object selection_set_create(k3d::selection::set& Self, const string_t& Type)
{
	return wrap(Self.create(Type));
}

void define_namespace_selection()
{
	scope outer = class_<selection>("selection", no_init)
		;

	enum_<k3d::selection::type>("type")
		.value("NONE", k3d::selection::NONE)
		.value("NODE", k3d::selection::NODE)
		.value("MESH", k3d::selection::MESH)
		.value("PRIMITIVE", k3d::selection::PRIMITIVE)
		.value("CONSTANT", k3d::selection::CONSTANT)
		.value("UNIFORM", k3d::selection::UNIFORM)
		.value("VARYING", k3d::selection::VARYING)
		.value("FACE_VARYING", k3d::selection::FACE_VARYING)
		.value("SPLIT_EDGE", k3d::selection::SPLIT_EDGE)
		.value("POINT", k3d::selection::POINT)
		.attr("__module__") = "k3d";

	class_<selection_storage_wrapper>("storage", no_init)
		.def("type", &selection_storage_get_type)
		.def("structure", &selection_storage_get_structure)
		;

	class_<k3d::selection::set>("set")
		.def("__len__", &selection_set_len)
		.def("__getitem__", &selection_set_get_item)
		.def("__str__", &selection_set_str)
		.def("create", &selection_set_create)
		;
}

} // namespace python

} // namespace k3d

