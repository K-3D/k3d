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
#include "iproperty_python.h"
#include "node_python.h"

#include <k3dsdk/idocument.h>
#include <k3dsdk/ienumeration_property.h>
#include <k3dsdk/inode.h>
#include <k3dsdk/iproperty.h>
#include <k3dsdk/imeasurement_property.h>
#include <k3dsdk/iwritable_property.h>
#include <k3dsdk/measurement.h>
#include <k3dsdk/property.h>
#include <k3dsdk/types.h>

#include <boost/python.hpp>
using namespace boost::python;

namespace k3d
{

namespace python
{

iproperty::iproperty() :
	base()
{
}

iproperty::iproperty(k3d::iproperty* Property) :
	base(Property)
{
}

const std::string iproperty::name()
{
	return wrapped().property_name();
}

const std::string iproperty::label()
{
	return wrapped().property_label();
}

const std::string iproperty::description()
{
	return wrapped().property_description();
}

const std::string iproperty::type()
{
	return k3d::type_string(wrapped().property_type());
}

object iproperty::internal_value()
{
	return any_to_python(wrapped().property_value());
}

object iproperty::value()
{
	if(k3d::inode* const node = wrapped().property_node())
		return any_to_python(k3d::get_value(node->document().dag(), wrapped()));

	return internal_value();
}

object iproperty::node()
{
	return any_to_python(wrapped().property_node());
}

const bool iproperty::is_writable()
{
	return dynamic_cast<k3d::iwritable_property*>(wrapped_ptr()) ? true : false;
}

const bool iproperty::is_enumeration()
{
	return dynamic_cast<k3d::ienumeration_property*>(wrapped_ptr()) ? true : false;
}

list iproperty::enumeration_values()
{
	list results;
	if(k3d::ienumeration_property* const enumeration = dynamic_cast<k3d::ienumeration_property*>(wrapped_ptr()))
	{
		const k3d::ienumeration_property::enumeration_values_t values = enumeration->enumeration_values();
		for(k3d::ienumeration_property::enumeration_values_t::const_iterator value = values.begin(); value != values.end(); ++value)
			results.append(value->value);
	}

	return results;
}

const std::string iproperty::units()
{
	if(k3d::imeasurement_property* const measurement = dynamic_cast<k3d::imeasurement_property*>(wrapped_ptr()))
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

void export_iproperty()
{
	class_<iproperty>("iproperty",
		"Encapsulates a K-3D property. In K-3D, a document contains nodes, and nodes contain properties, which are the external representations of a node's internal state.")
		.def("name", &iproperty::name,
			"Unique identifier, used for serialization and scripting.")
		.def("label", &iproperty::label,
			"Localized, human-readable text that labels the property in the user interface.")
		.def("description", &iproperty::description,
			"Localized, human-readable description of the property.")
		.def("type", &iproperty::type,
			"Returns the type of data the property stores as a string.")
		.def("internal_value", &iproperty::internal_value,
			"Returns the current value stored in the property.")
		.def("value", &iproperty::value,
			"Returns the property value, which may be different from its internal_value, if the property has been connected to another using the Visualization Pipeline.")
		.def("node", &iproperty::node,
			"Returns the node (if any) that owns the property, or None.")
		.def("is_writable", &iproperty::is_writable,
			"Returns true if the property's internal value can be modified.")
		.def("is_enumeration", &iproperty::is_enumeration,
			"Returns true if the property datatype is an enumeration.")
		.def("enumeration_values", &iproperty::enumeration_values,
			"Returns a list containing the set of allowable property values, if the property is an enumeration.")
		.def("units", &iproperty::units,
			"Returns a string describing the real-world unit-of-measure stored by the property, if any.");
}

} // namespace python

} // namespace k3d

