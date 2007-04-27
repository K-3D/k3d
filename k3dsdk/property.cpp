// K-3D
// Copyright (c) 1995-2005, Timothy M. Shead
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
		\brief Implements helper functions for working with object properties
		\author Tim Shead (tshead@k-3d.com)
*/

#include "data.h"
#include "idag.h"
#include "idocument.h"
#include "inode.h"

#include "property.h"
#include "types.h"

namespace k3d
{

iproperty* get_property(iunknown& Object, const std::string& Name)
{
	// If the object doesn't have properties, we're done ...
	iproperty_collection* const property_collection = dynamic_cast<iproperty_collection*>(&Object);
	if(!property_collection)
		return 0;

	// Get the object's properties ...
	const iproperty_collection::properties_t properties(property_collection->properties());

	// Look for a property with a matching name ...
	for(iproperty_collection::properties_t::const_iterator property_iterator = properties.begin(); property_iterator != properties.end(); ++property_iterator)
	{
		// if the name matches, we're done ...
		if((*property_iterator)->property_name() == Name)
			return *property_iterator;
	}

	// No property by that name!
	return 0;
}

boost::any get_value(idag& DAG, iproperty& Property)
{
	return property_lookup(&Property, DAG)->property_value();
}

boost::any get_value(iunknown& Object, const std::string& Name)
{
	if(iproperty* const property = get_property(Object, Name))
		return property_lookup(property, dynamic_cast<inode*>(&Object)->document().dag())->property_value();

	return boost::any();
}

boost::any get_internal_value(iunknown& Object, const std::string& Name)
{
	// Look for the property by name ...
	iproperty* const property = get_property(Object, Name);
	if(!property)
		return boost::any();

	return property->property_value();
}

bool set_value(iproperty& Property, const boost::any& Value)
{
	// If the property type doesn't match, we're done ...
	if(Value.type() != Property.property_type())
	{
		log() << error << "Value type [" << demangle(Value.type()) << "] doesn't match property [" << Property.property_name() << "] type [" << demangle(Property.property_type()) << "]" << std::endl;
		return false;
	}

	// If the property is read-only, we're done ...
	iwritable_property* const writable_property = dynamic_cast<iwritable_property*>(&Property);
	if(!writable_property)
	{
		log() << error << "Property [" << Property.property_name() << "] is not writable!" << std::endl;
		return false;
	}

	return writable_property->property_set_value(Value);
}

bool quiet_set_value(iproperty& Property, const boost::any& Value)
{
	// If the property type doesn't match, we're done ...
	if(Value.type() != Property.property_type())
		return false;

	// If the property is read-only, we're done ...
	iwritable_property* const writable_property = dynamic_cast<iwritable_property*>(&Property);
	if(!writable_property)
		return false;

	return writable_property->property_set_value(Value);
}

bool set_value(iunknown& Object, const std::string Name, const boost::any& Value)
{
	// If the object isn't a property collection, we're done ...
	iproperty_collection* const property_collection = dynamic_cast<iproperty_collection*>(&Object);
	if(!property_collection)
	{
		log() << error << k3d_file_reference << " : object has no property collection!" << std::endl;
		return false;
	}

	// Look for a property with a matching name ...
	const iproperty_collection::properties_t properties(property_collection->properties());
	for(iproperty_collection::properties_t::const_iterator property = properties.begin(); property != properties.end(); ++property)
	{
		// Name doesn't match, so keep going ...
		if((*property)->property_name() != Name)
			continue;

		// Success ...
		return set_value(**property, Value);
	}

	// Couldn't find a property by that name!
	log() << error << k3d_file_reference << " : could not find property [" << Name << "]" << std::endl;
	return false;
}

bool quiet_set_value(iunknown& Object, const std::string Name, const boost::any& Value)
{
	// If the object isn't a property collection, we're done ...
	iproperty_collection* const property_collection = dynamic_cast<iproperty_collection*>(&Object);
	if(!property_collection)
		return false;

	// Look for a property with a matching name ...
	const iproperty_collection::properties_t properties(property_collection->properties());
	for(iproperty_collection::properties_t::const_iterator property = properties.begin(); property != properties.end(); ++property)
	{
		// Name doesn't match, so keep going ...
		if((*property)->property_name() != Name)
			continue;

		// Success ...
		return quiet_set_value(**property, Value);
	}

	// Couldn't find a property by that name!
	return false;
}

} // namespace k3d


