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

#include <k3dsdk/idocument.h>
#include <k3dsdk/inode.h>
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

static list properties(iunknown_wrapper& Self)
{
	list results;
	const k3d::iproperty_collection::properties_t& properties = Self.wrapped<k3d::iproperty_collection>().properties();
	for(k3d::iproperty_collection::properties_t::const_iterator property = properties.begin(); property != properties.end(); ++property)
		results.append(wrap_unknown(*property));
	return results;
}

static object get_property(iunknown_wrapper& Self, const string_t& Name)
{
	return wrap_unknown(k3d::property::get(Self.wrapped<k3d::iproperty_collection>(), Name));
}

static object create_property(iunknown_wrapper& Self, const string_t& Type, const string_t& Name, const string_t& Label, const string_t& Description)
{
	k3d::inode* const node = Self.wrapped_ptr<k3d::inode>();
	if(!node)
		throw std::runtime_error("missing node");

	k3d::iproperty* const result = k3d::property::create(*node, Type, Name, Label, Description);
	if(!result)
		throw std::invalid_argument("unknown user property type: " + Type);

	return wrap_unknown(result);
}

static object create_renderman_attribute(iunknown_wrapper& Self, const string_t& Type, const string_t& AttributeName, const string_t& Name, const string_t& Label, const string_t& Description)
{
	k3d::inode* const node = Self.wrapped_ptr<k3d::inode>();
	if(!node)
		throw std::runtime_error("missing node");

	k3d::iproperty* const result = k3d::property::ri::create_attribute(*node, Type, AttributeName, Name, Label, Description);
	if(!result)
		throw std::invalid_argument("unknown attribute type: " + Type);

	return wrap_unknown(result);
}

static object create_renderman_option(iunknown_wrapper& Self, const string_t& Type, const string_t& OptionName, const string_t& Name, const string_t& Label, const string_t& Description)
{
	k3d::inode* const node = Self.wrapped_ptr<k3d::inode>();
	if(!node)
		throw std::runtime_error("missing node");

	k3d::iproperty* const result = k3d::property::ri::create_option(*node, Type, OptionName, Name, Label, Description);
	if(!result)
		throw std::invalid_argument("unknown option type: " + Type);

	return wrap_unknown(result);
}

static bool has_property(iunknown_wrapper& Self, const string_t& Name)
{
	return k3d::property::get(Self.wrapped<k3d::iproperty_collection>(), Name) ? true : false;
}

void define_methods_iproperty_collection(iunknown& Interface, boost::python::object& Instance)
{
	if(!dynamic_cast<k3d::iproperty_collection*>(&Interface))
		return;

	utility::add_method(utility::make_function(&properties,
		"Returns the set of all properties held within this collection.\n\n"
		"@return: A list of L{iproperty} objects."), "properties", Instance);
	utility::add_method(utility::make_function(&get_property,
		"Returns a single property by name.\n\n"
		"@rtype: L{iproperty}\n"
		"@return: The property object if it exists, or None."), "get_property", Instance);
	utility::add_method(utility::make_function(&create_property,
		"Adds a custom user property to the collection.\n\n"
		"@rtype: L{iproperty}\n"
		"@return: The newly-created property.\n\n"), "create_property", Instance);
	utility::add_method(utility::make_function(&create_renderman_attribute,
		"Adds a custom RenderMan attribute property to the collection."), "create_renderman_attribute", Instance);
	utility::add_method(utility::make_function(&create_renderman_option,
		"Adds a custom RenderMan option property to the collection."), "create_renderman_option", Instance);
	utility::add_method(utility::make_function(&has_property,
		"True if the named property is registered"), "has_property", Instance);
}

} // namespace python

} // namespace k3d

