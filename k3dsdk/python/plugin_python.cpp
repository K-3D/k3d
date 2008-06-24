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

#include "interface_wrapper_python.h"
#include "plugin_python.h"

#include <k3dsdk/plugins.h>

#include <boost/python.hpp>
using namespace boost::python;

namespace k3d
{

namespace python
{

class plugin
{
public:
	static object create(const string_t& Type)
	{
		return wrap(k3d::plugin::create(Type));
	}

	class factory
	{
	public:
		static const list lookup()
		{
			list plugins;

			const k3d::plugin::factory::collection_t& factories = k3d::plugin::factory::lookup();
			for(k3d::plugin::factory::collection_t::const_iterator factory = factories.begin(); factory != factories.end(); ++factory)
				plugins.append(interface_wrapper<k3d::iplugin_factory>(*factory));

			return plugins;
		}

		static object lookup_by_name(const string_t& Name)
		{
			return wrap(k3d::plugin::factory::lookup(Name));
		}
	};
};

void define_plugin_namespace()
{
	scope outer = class_<plugin>("plugin", no_init)
		.def("create", plugin::create,
			"Returns a new application plugin instance, or None.")
		.staticmethod("create");

	class_<plugin::factory>("factory", no_init)
		.def("lookup", plugin::factory::lookup,
			"Returns the list of all available plugin factories.")
		.def("lookup", plugin::factory::lookup_by_name,
			"Returns the plugin factory that matches the given name, or None.")
		.staticmethod("lookup");
}

} // namespace python

} // namespace k3d

