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

#include <k3dsdk/iplugin_factory.h>
#include <k3dsdk/mime_types.h>
#include <k3dsdk/plugin.h>
#include <k3dsdk/python/idocument_python.h>
#include <k3dsdk/python/iunknown_python.h>
#include <k3dsdk/python/plugin_python.h>

#include <boost/python.hpp>

namespace k3d
{

namespace python
{

class plugin
{
public:
	static boost::python::list wrap_factories(const k3d::plugin::factory::collection_t& Factories)
	{
		boost::python::list python_factories;

		for(k3d::plugin::factory::collection_t::const_iterator factory = Factories.begin(); factory != Factories.end(); ++factory)
			python_factories.append(wrap_unknown(*factory));

		return python_factories;
	}

	static boost::python::object create_by_uuid(const uuid& ID)
	{
		return wrap_unknown(k3d::plugin::create(ID));
	}

	static boost::python::object create_by_name(const string_t& Type)
	{
		return wrap_unknown(k3d::plugin::create(Type));
	}

	static boost::python::object create_by_factory(const iunknown_wrapper& Factory)
	{
		return wrap_unknown(k3d::plugin::create(dynamic_cast<k3d::iplugin_factory&>(Factory.wrapped())));
	}

	static boost::python::object document_create_by_uuid(const uuid& ID, const idocument_wrapper& Document)
	{
		return wrap_unknown(k3d::plugin::create(ID, Document.wrapped()));
	}

	static boost::python::object document_create_by_name(const string_t& Type, const idocument_wrapper& Document)
	{
		return wrap_unknown(k3d::plugin::create(Type, Document.wrapped()));
	}

	static boost::python::object document_create_by_factory(const iunknown_wrapper& Factory, const idocument_wrapper& Document)
	{
		return wrap_unknown(k3d::plugin::create(dynamic_cast<k3d::iplugin_factory&>(Factory.wrapped()), Document.wrapped()));
	}

	class factory
	{
	public:
		static boost::python::list lookup()
		{
			return wrap_factories(k3d::plugin::factory::lookup());
		}

		static boost::python::object lookup_by_uuid(const uuid& ID)
		{
			return wrap_unknown(k3d::plugin::factory::lookup(ID));
		}

		static boost::python::object lookup_by_name(const string_t& Name)
		{
			return wrap_unknown(k3d::plugin::factory::lookup(Name));
		}

		static boost::python::list lookup_by_metadata(const string_t& MetadataName, const string_t& MetadataValue)
		{
			return wrap_factories(k3d::plugin::factory::lookup(MetadataName, MetadataValue));
		}

		static boost::python::list lookup_by_mime_type(const k3d::mime::type& Type)
		{
			return wrap_factories(k3d::plugin::factory::lookup(Type));
		}
	};
};

void define_namespace_plugin()
{
	boost::python::scope outer = boost::python::class_<plugin>("plugin", boost::python::no_init)
		.def("create", plugin::create_by_name,
			"Returns a new application plugin instance, or None.")
		.def("create", plugin::create_by_uuid,
			"Returns a new application plugin instance, or None.")
		.def("create", plugin::create_by_factory,
			"Returns a new application plugin instance, or None.")
		.def("create", plugin::document_create_by_name,
			"Returns a new document plugin instance, or None.")
		.def("create", plugin::document_create_by_uuid,
			"Returns a new document plugin instance, or None.")
		.def("create", plugin::document_create_by_factory,
			"Returns a new document plugin instance, or None.")
		.staticmethod("create");

	boost::python::class_<plugin::factory>("factory", boost::python::no_init)
		.def("lookup", plugin::factory::lookup,
			"Returns a list containing all available plugin factories.")
		.def("lookup", plugin::factory::lookup_by_uuid,
			"Returns the plugin factory that matches the given id, or None.")
		.def("lookup", plugin::factory::lookup_by_name,
			"Returns the plugin factory that matches the given name, or None.")
		.def("lookup", plugin::factory::lookup_by_metadata,
			"Returns a list containing all plugin factories that match the given metadata name and value.")
		.def("lookup", plugin::factory::lookup_by_mime_type,
			"Returns a list containing all plugin factories that match the given MIME type.")
		.staticmethod("lookup");
}

} // namespace python

} // namespace k3d

