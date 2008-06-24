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

#include "resource_python.h"

#include <k3dsdk/resource/resource.h>

#include <boost/python.hpp>
using namespace boost::python;

namespace k3d
{

namespace python
{

class resource
{
public:
	static const list keys()
	{
		list python_keys;

		const k3d::resource::keys_t& keys = k3d::resource::keys();
		for(k3d::resource::keys_t::const_iterator key = keys.begin(); key != keys.end(); ++key)
			python_keys.append(*key);

		return python_keys;
	}
};


void define_resource_namespace()
{
	class_<resource>("resource", no_init)
		.def("keys", resource::keys,
			"Returns a list containing the set of all resources.")
		.def("get_string", k3d::resource::get_string,
			"Returns a resource as a string, or empty string if the resource does not exist.")
		.staticmethod("keys")
		.staticmethod("get_string");

}

} // namespace python

} // namespace k3d

