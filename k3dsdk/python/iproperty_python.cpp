// K-3D
// Copyright (c) 1995-2006, Timothy M. Shead
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
#include "iproperty_python.h"
#include "node_python.h"

#include <k3dsdk/idocument.h>
#include <k3dsdk/ienumeration_property.h>
#include <k3dsdk/inode.h>
#include <k3dsdk/iproperty.h>
#include <k3dsdk/imeasurement_property.h>
#include <k3dsdk/iwritable_property.h>
#include <k3dsdk/measurement.h>
#include <k3dsdk/properties.h>
#include <k3dsdk/type_registry.h>

#include <boost/python.hpp>
using namespace boost::python;

namespace k3d
{

namespace python
{

typedef interface_wrapper<k3d::iproperty> iproperty_wrapper;

const string_t name(iproperty_wrapper& Self)
{
	return Self.wrapped().property_name();
}

const string_t label(iproperty_wrapper& Self)
{
	return Self.wrapped().property_label();
}

const string_t description(iproperty_wrapper& Self)
{
	return Self.wrapped().property_description();
}

const string_t type(iproperty_wrapper& Self)
{
	return k3d::type_string(Self.wrapped().property_type());
}

object internal_value(iproperty_wrapper& Self)
{
	return any_to_python(k3d::property::internal_value(Self.wrapped()));
}

object pipeline_value(iproperty_wrapper& Self)
{
	return any_to_python(k3d::property::pipeline_value(Self.wrapped()));
}

object node(iproperty_wrapper& Self)
{
	return any_to_python(Self.wrapped().property_node());
}

const bool is_writable(iproperty_wrapper& Self)
{
	return dynamic_cast<k3d::iwritable_property*>(Self.wrapped_ptr()) ? true : false;
}

void set_value(iproperty_wrapper& Self, const boost::python::object& Value)
{
	if(k3d::iwritable_property* const writable = dynamic_cast<k3d::iwritable_property*>(Self.wrapped_ptr()))
	{
		writable->property_set_value(python_to_any(Value, Self.wrapped_ptr()->property_type()));
		return;
	}

	throw std::runtime_error("property " + Self.wrapped().property_name() + " is a read-only property");
}

const bool is_enumeration(iproperty_wrapper& Self)
{
	return dynamic_cast<k3d::ienumeration_property*>(Self.wrapped_ptr()) ? true : false;
}

list enumeration_values(iproperty_wrapper& Self)
{
	list results;
	if(k3d::ienumeration_property* const enumeration = dynamic_cast<k3d::ienumeration_property*>(Self.wrapped_ptr()))
	{
		const k3d::ienumeration_property::enumeration_values_t values = enumeration->enumeration_values();
		for(k3d::ienumeration_property::enumeration_values_t::const_iterator value = values.begin(); value != values.end(); ++value)
			results.append(value->value);
	}

	return results;
}

const string_t units(iproperty_wrapper& Self)
{
	if(k3d::imeasurement_property* const measurement = dynamic_cast<k3d::imeasurement_property*>(Self.wrapped_ptr()))
	{
		const std::type_info& units = measurement->property_units();

		if(units == typeid(void))
			return "scalar";

		if(units == typeid(k3d::measurement::angle))
			return "angle";

		if(units == typeid(k3d::measurement::area))
			return "area";

		if(units == typeid(k3d::measurement::distance))
			return "distance";

		if(units == typeid(k3d::measurement::force))
			return "force";

		if(units == typeid(k3d::measurement::mass))
			return "mass";

		if(units == typeid(k3d::measurement::pressure))
			return "pressure";

		if(units == typeid(k3d::measurement::time))
			return "time";

		if(units == typeid(k3d::measurement::volume))
			return "volume";

		return "unknown";
	}

	return "";
}

void define_class_iproperty()
{
	class_<iproperty_wrapper>("iproperty",
		"Encapsulates a K-3D property. In K-3D, a document contains nodes, and nodes contain properties, "
		"which are the external representations of a node's internal state.", no_init)
		.def("name", &name,
			"Unique identifier, used for serialization and scripting.")
		.def("label", &label,
			"Localized, human-readable text that labels the property in the user interface.")
		.def("description", &description,
			"Localized, human-readable description of the property.")
		.def("type", &type,
			"Returns the type of data the property stores as a string.")
		.def("internal_value", &internal_value,
			"Returns the value stored by the property.")
		.def("pipeline_value", &pipeline_value,
			"Returns the property's 'pipeline' value, which will differ from its internal value if it's connected to another property by the Visualization Pipeline.")
		.def("node", &node,
			"Returns the node (if any) that owns the property, or None.")
		.def("is_writable", &is_writable,
			"Returns true if the property's internal value can be modified.")
		.def("set_value", &set_value,
			"Sets the property's internal value.")
		.def("is_enumeration", &is_enumeration,
			"Returns true if the property datatype is an enumeration.")
		.def("enumeration_values", &enumeration_values,
			"Returns a list containing the set of allowable property values, if the property is an enumeration.")
		.def("units", &units,
			"Returns a string describing the real-world unit-of-measure stored by the property, if any.");
}

} // namespace python

} // namespace k3d

