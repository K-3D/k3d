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
#include "interface_wrapper_python.h"
#include "iproperty_collection_python.h"

#include <k3dsdk/idocument.h>
#include <k3dsdk/inode.h>
#include <k3dsdk/iproperty.h>
#include <k3dsdk/iproperty_collection.h>
#include <k3dsdk/iwritable_property.h>
#include <k3dsdk/properties.h>
#include <k3dsdk/property_types.h>
#include <k3dsdk/property_types_ri.h>
#include <k3dsdk/type_registry.h>

#include <boost/python.hpp>
using namespace boost::python;

namespace k3d
{

namespace python
{

typedef interface_wrapper<k3d::iproperty_collection> iproperty_collection_wrapper;

static list properties(iproperty_collection_wrapper& Self)
{
	list results;
	const k3d::iproperty_collection::properties_t& properties = Self.wrapped().properties();
	for(k3d::iproperty_collection::properties_t::const_iterator property = properties.begin(); property != properties.end(); ++property)
		results.append(interface_wrapper<k3d::iproperty>(*property));
	return results;
}

static object get_property(iproperty_collection_wrapper& Self, const string_t& Name)
{
	return wrap(k3d::property::get(Self.wrapped(), Name));
}

static object create_property(iproperty_collection_wrapper& Self, const string_t& Type, const string_t& Name, const string_t& Label, const string_t& Description)
{
	k3d::inode* const node = dynamic_cast<k3d::inode*>(Self.wrapped_ptr());
	if(!node)
		throw std::runtime_error("missing node");

	k3d::iproperty* const result = k3d::property::create(*node, Type, Name, Label, Description);
	if(!result)
		throw std::invalid_argument("unknown user property type: " + Type);

	return wrap(result);
}

static object create_renderman_attribute(iproperty_collection_wrapper& Self, const string_t& Type, const string_t& AttributeName, const string_t& Name, const string_t& Label, const string_t& Description)
{
	k3d::inode* const node = dynamic_cast<k3d::inode*>(Self.wrapped_ptr());
	if(!node)
		throw std::runtime_error("missing node");

	k3d::iproperty* const result = k3d::property::ri::create_attribute(*node, Type, AttributeName, Name, Label, Description);
	if(!result)
		throw std::invalid_argument("unknown attribute type: " + Type);

	return wrap(result);
}

static object create_renderman_option(iproperty_collection_wrapper& Self, const string_t& Type, const string_t& OptionName, const string_t& Name, const string_t& Label, const string_t& Description)
{
	k3d::inode* const node = dynamic_cast<k3d::inode*>(Self.wrapped_ptr());
	if(!node)
		throw std::runtime_error("missing node");

	k3d::iproperty* const result = k3d::property::ri::create_option(*node, Type, OptionName, Name, Label, Description);
	if(!result)
		throw std::invalid_argument("unknown option type: " + Type);

	return wrap(result);
}

static bool has_property(iproperty_collection_wrapper& Self, const string_t& Name)
{
	if (k3d::property::get(Self.wrapped(), Name))
		return true;
	return false;
}

static object getattr(iproperty_collection_wrapper& Self, const string_t& Name)
{
	if(k3d::iproperty* property = k3d::property::get(Self.wrapped(), Name))
		return any_to_python(k3d::property::pipeline_value(*property));

	throw std::invalid_argument("unknown property: " + Name);
}

static void setattr(iproperty_collection_wrapper& Self, const string_t& Name, const object& Value)
{
	if(k3d::iproperty* const property = k3d::property::get(Self.wrapped(), Name))
	{
		if(k3d::iwritable_property* const writable = dynamic_cast<k3d::iwritable_property*>(property))
		{
			writable->property_set_value(python_to_any(Value, property->property_type()));
			return;
		}

		throw std::invalid_argument("read-only property: " + Name);
	}

	throw std::invalid_argument("unknown property: " + Name);
}

void define_class_iproperty_collection()
{
	class_<iproperty_collection_wrapper>("iproperty_collection",
		"Abstract interface for a collection of L{iproperty} objects.", no_init)
		.def("properties", &properties,
			"Returns the set of all properties held within this collection.\n\n"
			"@return: A list of L{iproperty} objects.")
		.def("get_property", &get_property,
			"Returns a single property by name.\n\n"
			"@rtype: L{iproperty}\n"
			"@return: The property object if it exists, or None.")
		.def("create_property", &create_property,
			"Adds a custom user property to the collection.\n\n"
			"@rtype: L{iproperty}\n"
			"@return: The newly-created property.\n\n")
		.def("create_renderman_attribute", &create_renderman_attribute,
			"Adds a custom RenderMan attribute property to the collection.")
		.def("create_renderman_option", &create_renderman_option,
			"Adds a custom RenderMan option property to the collection.")
		.def("__getattr__", &getattr,
			"Returns the pipeline value of an L{iproperty} by name.")
		.def("__setattr__", &setattr,
			"Sets the internal value of an L{iproperty} value by name.")
		.def("has_property", &has_property,
			"True if the named property is registered");
}

} // namespace python

} // namespace k3d

