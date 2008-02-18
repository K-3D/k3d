#ifndef K3DSDK_PROPERTIES_H
#define K3DSDK_PROPERTIES_H

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
	\brief defines helper functions for working with properties
	\author Tim Shead (tshead@k-3d.com)
*/

#include "iproperty.h"

namespace k3d
{

class inode;
class iunknown;

namespace property
{

/// Returns a matching property by name, or NULL
iproperty* get(iunknown& Object, const string_t& Name);

/// Returns a matching property by name and type, or NULL
template<typename value_t>
iproperty* get(iunknown& Object, const string_t& Name)
{
	iproperty* const property = get(Object, Name);
	return property && (property->property_type() == typeid(value_t)) ? property : 0;
}

/// Returns the "internal" value of a property - the value of the property itself, regardless of any pipeline dependencies
const boost::any internal_value(iunknown& Object, const string_t& Name);
/// Returns the "internal" value of a property - the value of the property itself, regardless of any pipeline dependencies
template<typename value_t>
const value_t internal_value(iunknown& Object, const string_t& Name)
{
	return boost::any_cast<value_t>(internal_value(Object, Name));
}

/// Returns the "internal" value of a property - the value of the property itself, regardless of any pipeline dependencies
const boost::any internal_value(iproperty& Property);
/// Returns the "internal" value of a property - the value of the property itself, regardless of any pipeline dependencies
template<typename value_t>
const value_t internal_value(iproperty& Property)
{
	return boost::any_cast<value_t>(internal_value(Property));
}

/// Returns the "pipeline" value of a property - the value of the property, possibly overridden by pipeline dependencies
const boost::any pipeline_value(iunknown& Object, const string_t& Name);
/// Returns the "pipeline" value of a property - the value of the property, possibly overridden by pipeline dependencies
template<typename value_t>
const value_t pipeline_value(iunknown& Object, const string_t& Name)
{
	return boost::any_cast<value_t>(pipeline_value(Object, Name));
}
/// Returns the "pipeline" value of a property - the value of the property, possibly overridden by pipeline dependencies
const boost::any pipeline_value(iproperty& Property);
/// Returns the "pipeline" value of a property - the value of the property, possibly overridden by pipeline dependencies
template<typename value_t>
const value_t pipeline_value(iproperty& Property)
{
	return boost::any_cast<value_t>(pipeline_value(Property));
}

/** \brief Sets the value of a named property
    \return true, iff the value was set successfully, false otherwise (couldn't find a property with a matching name, it wasn't writable, or the type didn't match)
*/
bool set_internal_value(iunknown& Object, const string_t Name, const boost::any& Value);

/** \brief Sets the value of a property
	\return true, iff the value was set successfully, false otherwise (the property wasn't writable or the type didn't match)
*/
bool set_internal_value(iproperty& Property, const boost::any& Value);

/// Creates a new user property with the given type and initial value.
iproperty* create(inode& Owner, const std::type_info& Type, const string_t& Name, const string_t& Label, const string_t& Description, const boost::any& Value = boost::any());
/// Creates a new user property with the given type and initial value.
iproperty* create(inode& Owner, const string_t& Type, const string_t& Name, const string_t& Label, const string_t& Description, const boost::any& Value = boost::any());
/// Creates a new user property with the given type and initial value.
template<typename T>
iproperty* create(inode& Owner, const string_t& Name, const string_t& Label, const string_t& Description, const boost::any& Value = boost::any())
{
	return create(Owner, typeid(T), Name, Label, Description, Value);
}

namespace ri
{

/// Creates a new RenderMan attribute with the given type, name, and value
iproperty* create_attribute(inode& Owner, const std::type_info& Type, const string_t& AttributeName, const string_t& Name, const string_t& Label, const string_t& Description, const boost::any& Value = boost::any());
/// Creates a new RenderMan attribute with the given type, name, and value
iproperty* create_attribute(inode& Owner, const string_t& Type, const string_t& AttributeName, const string_t& Name, const string_t& Label, const string_t& Description, const boost::any& Value = boost::any());
/// Creates a new RenderMan attribute with the given type, name, and value
template<typename T>
iproperty* create_attribute(inode& Owner, const string_t& AttributeName, const string_t& Name, const string_t& Label, const string_t& Description, const boost::any& Value = boost::any())
{
	return create_attribute(Owner, typeid(T), AttributeName, Name, Label, Description, Value);
}

/// Creates a new RenderMan option with the given type, name, and value
iproperty* create_option(inode& Owner, const std::type_info& Type, const string_t& OptionName, const string_t& Name, const string_t& Label, const string_t& Description, const boost::any& Value = boost::any());
/// Creates a new RenderMan option with the given type, name, and value
iproperty* create_option(inode& Owner, const string_t& Type, const string_t& OptionName, const string_t& Name, const string_t& Label, const string_t& Description, const boost::any& Value = boost::any());
/// Creates a new RenderMan option with the given type, name, and value
template<typename T>
iproperty* create_option(inode& Owner, const string_t& OptionName, const string_t& Name, const string_t& Label, const string_t& Description, const boost::any& Value = boost::any())
{
	return create_option(Owner, typeid(T), OptionName, Name, Label, Description, Value);
}

} // namespace ri

} // namespace property

} // namespace k3d

#endif // !K3DSDK_PROPERTIES_H

