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
#include "iunknown_python.h"
#include "utility_python.h"

#include <k3dsdk/imetadata.h>

#include <boost/python.hpp>
using namespace boost::python;

namespace k3d
{

namespace python
{

static void set_metadata_value(iunknown_wrapper& Self, const string_t& Name, const string_t& Value)
{
	Self.wrapped<k3d::imetadata>().set_metadata_value(Name, Value);
}

static boost::python::dict get_metadata(iunknown_wrapper& Self)
{
	boost::python::dict result;

	const imetadata::metadata_t metadata = Self.wrapped<k3d::imetadata>().get_metadata();
	for(imetadata::metadata_t::const_iterator pair = metadata.begin(); pair != metadata.end(); ++pair)
		result[pair->first] = pair->second;

	return result;
}

static string_t get_metadata_value(iunknown_wrapper& Self, const string_t& Name)
{
	return Self.wrapped<k3d::imetadata>().get_metadata_value(Name);
}

static void erase_metadata_value(iunknown_wrapper& Self, const string_t& Name)
{
	Self.wrapped<k3d::imetadata>().erase_metadata_value(Name);
}

void define_methods_imetadata(iunknown& Interface, boost::python::object& Instance)
{
	utility::add_method(utility::make_function(&set_metadata_value), "set_metadata_value", Instance);
	utility::add_method(utility::make_function(&get_metadata), "get_metadata", Instance);
	utility::add_method(utility::make_function(&get_metadata_value), "get_metadata_value", Instance);
	utility::add_method(utility::make_function(&erase_metadata_value), "erase_metadata_value", Instance);
}

} // namespace python

} // namespace k3d

