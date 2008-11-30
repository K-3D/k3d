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

#include "imetadata_python.h"

#include <boost/python.hpp>
using namespace boost::python;

namespace k3d
{

namespace python
{

static void set_metadata_value(imetadata_wrapper& Self, const string_t& Name, const string_t& Value)
{
	Self.wrapped().set_metadata_value(Name, Value);
}

static boost::python::dict get_metadata(imetadata_wrapper& Self)
{
	boost::python::dict result;

	const imetadata::metadata_t metadata = Self.wrapped().get_metadata();
	for(imetadata::metadata_t::const_iterator pair = metadata.begin(); pair != metadata.end(); ++pair)
		result[pair->first] = pair->second;

	return result;
}

static string_t get_metadata_value(imetadata_wrapper& Self, const string_t& Name)
{
	return Self.wrapped().get_metadata_value(Name);
}

static void erase_metadata_value(imetadata_wrapper& Self, const string_t& Name)
{
	Self.wrapped().erase_metadata_value(Name);
}

void define_class_imetadata()
{
	class_<imetadata_wrapper>("imetadata", 
		"Abstract interface for objects that can store user-defined metadata.",
		no_init)
		.def("set_metadata_value", &set_metadata_value)
		.def("get_metadata", &get_metadata)
		.def("get_metadata_value", &get_metadata_value)
		.def("erase_metadata_value", &erase_metadata_value)
		;
}

} // namespace python

} // namespace k3d

