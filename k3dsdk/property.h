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
	\brief defines helper functions for working with properties
	\author Tim Shead (tshead@k-3d.com)
*/

#include "iproperty.h"

namespace k3d
{

class ipipeline;
class iunknown;

namespace property
{

/// Returns a matching property by name, or NULL
iproperty* get(iunknown& Object, const std::string& Name);

/// Returns a matching property by name and type, or NULL
template<typename value_t>
iproperty* get(iunknown& Object, const std::string& Name)
{
	iproperty* const property = get(Object, Name);
	return property && (property->property_type() == typeid(value_t)) ? property : 0;
}

/// Returns the "internal" value of a property - the value of the property itself, regardless of any pipeline dependencies
const boost::any internal_value(iunknown& Object, const std::string& Name);

/// Returns the "pipeline" value of a property - the value of the property, possibly overridden by pipeline dependencies
const boost::any pipeline_value(iunknown& Object, const std::string& Name);

/// Returns the "pipeline" value of a property - the value of the property, possibly overridden by pipeline dependencies
const boost::any pipeline_value(ipipeline& Pipeline, iproperty& Property);

/** \brief Sets the value of a named property
    \return true, iff the value was set successfully, false otherwise (couldn't find a property with a matching name, it wasn't writable, or the type didn't match)
*/
bool set_internal_value(iunknown& Object, const std::string Name, const boost::any& Value);

/** \brief Sets the value of a property
	\return true, iff the value was set successfully, false otherwise (the property wasn't writable or the type didn't match)
*/
bool set_internal_value(iproperty& Property, const boost::any& Value);

} // namespace property

} // namespace k3d

#endif // !K3DSDK_PROPERTY_H

