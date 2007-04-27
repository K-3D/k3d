#ifndef K3DSDK_PROPERTY_H
#define K3DSDK_PROPERTY_H

// K-3D
// Copyright (c) 1995-2004, Timothy M. Shead
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
		\brief defines helper functions for working with object properties
		\author Tim Shead (tshead@k-3d.com)
*/

#include "iproperty.h"
#include "iproperty_collection.h"
#include "iwritable_property.h"

namespace k3d
{

// Forward declarations
class idag;
class iproperty_collection;
class iunknown;

/// Returns a property by name - could return NULL if a property with the given name isn't found (don't forget to check the property type!)
iproperty* get_property(iunknown& Object, const std::string& Name);
/// Returns a property with the given name and type (could return NULL, if no match is found)
template<typename value_t>
iproperty* get_typed_property(iunknown& Object, const std::string& Name)
{
	iproperty* const property = get_property(Object, Name);
	if(property && (property->property_type() == typeid(value_t)))
		return property;

	return 0;
}

/// Returns the value of a property, transparently handling dependencies as needed
boost::any get_value(idag& DAG, iproperty& Property);
/// Returns the value of a property by name, transparently handling dependencies on other properties as-needed
boost::any get_value(iunknown& Object, const std::string& Name);
/// Returns the internal value of a property, regardless of any dependencies it may have
boost::any get_internal_value(iunknown& Object, const std::string& Name);

/** \brief Sets the value of a property
	\return true, iff the value was set successfully, false otherwise (the property wasn't writable or the type didn't match)
*/
bool set_value(iproperty& Property, const boost::any& Value);
/** \brief Quietly sets the value of a property - doesn't log errors if the property couldn't be set
	\return true, iff the value was set successfully, false otherwise (the property wasn't writable or the type didn't match)
*/
bool quiet_set_value(iproperty& Property, const boost::any& Value);

/** \brief Sets the value of a named property
    \return true, iff the value was set successfully, false otherwise (couldn't find a property with a matching name, it wasn't writable, or the type didn't match)
*/
bool set_value(iunknown& Object, const std::string Name, const boost::any& Value);
/** \brief Quietly sets the value of a named property - doesn't log errors if the property couldn't be set
    \return true, iff the value was set successfully, false otherwise (couldn't find a property with a matching name, it wasn't writable, or the type didn't match)
*/
bool quiet_set_value(iunknown& Object, const std::string Name, const boost::any& Value);

} // namespace k3d

#endif // !K3DSDK_PROPERTY_H

