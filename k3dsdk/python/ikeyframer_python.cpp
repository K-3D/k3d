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

#include "any_python.h"
#include "iproperty_collection_python.h"
#include "iunknown_python.h"
#include "utility_python.h"

#include <k3dsdk/ikeyframer.h>

#include <boost/python.hpp>
using namespace boost::python;

namespace k3d
{

namespace python
{

static void keyframe(iunknown_wrapper& Self)
{
	Self.wrapped<k3d::ikeyframer>().keyframe();
}

static void delete_key(iunknown_wrapper& Self, iunknown_wrapper& TimeProperty)
{
	k3d::iproperty* property = TimeProperty.wrapped_ptr<iproperty>();
	Self.wrapped<k3d::ikeyframer>().delete_key(property);
}

static object input_property(iunknown_wrapper& Self)
{
	return wrap_unknown(Self.wrapped<k3d::ikeyframer>().input_property());
}

static list get_keys(iunknown_wrapper& Self)
{
	list results;
	const k3d::ikeyframer::keys_t keys = Self.wrapped<k3d::ikeyframer>().get_keys();
	for(k3d::ikeyframer::keys_t::const_iterator key = keys.begin(); key != keys.end(); ++key)
		results.append(wrap_unknown(*key));
	return results;
}

void define_methods_ikeyframer(iunknown& Interface, boost::python::object& Instance)
{
	if(!dynamic_cast<k3d::ikeyframer*>(&Interface))
		return;

	utility::add_method(utility::make_function(&keyframe,
		"Set a keyframe. Inputs are implementation-dependent, and may come from properties."), "keyframe", Instance);
	utility::add_method(utility::make_function(&delete_key,
		"Delete the keyframe referenced by the given time property."), "delete_key", Instance);
	utility::add_method(utility::make_function(&input_property,
		"Get the property used to input the key values.\n\n"
		"@rtype: L{iproperty}\n"
		"@return: The input property.\n\n"), "input_property", Instance);
	utility::add_method(utility::make_function(&get_keys,
		"Returns a list with all the time properties for the keyframes."), "get_keys", Instance);
}

} // namespace python

} // namespace k3d

