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
	\author Tim Shead (tshead@k-3d.com)
*/

#include "inode.h"
#include "iproperty_collection.h"
#include "iuser_property.h"
#include "persistent_property_collection.h"
#include "properties.h"
#include "result.h"
#include "tokens.h"
#include "type_registry.h"
#include "utility.h"
#include "xml.h"

#include <iostream>
#include <map>

namespace k3d
{

/////////////////////////////////////////////////////////////////////////////
// persistent_property_collection

persistent_property_collection::persistent_property_collection()
{
}

persistent_property_collection::~persistent_property_collection()
{
}

void persistent_property_collection::enable_serialization(const string_t& Name, ipersistent& Property)
{
	if(std::count(m_names.begin(), m_names.end(), Name))
	{
		log() << error << "cannot enable serialization for property with duplicate name [" << Name << "]" << std::endl;
		return;
	}

	if(std::count(m_properties.begin(), m_properties.end(), &Property))
	{
		log() << error << "serialization is already enabled for property [" << Name << "]" << std::endl;
		return;
	}

	m_names.push_back(Name);
	m_properties.push_back(&Property);
}

void persistent_property_collection::disable_serialization(ipersistent& Property)
{
	for(uint_t i = 0; i != m_names.size(); ++i)
	{
		if(m_properties[i] == &Property)
		{
			m_names.erase(m_names.begin() + i);
			m_properties.erase(m_properties.begin() + i);
			return;
		}
	}
}

const std::vector<std::pair<string_t, ipersistent*> > persistent_property_collection::persistent_objects()
{
	std::vector<std::pair<string_t, ipersistent*> > results;

	for(uint_t i = 0; i != m_names.size(); ++i)
		results.push_back(std::make_pair(m_names[i], m_properties[i]));

	return results;
}

void persistent_property_collection::save(xml::element& Element, const ipersistent::save_context& Context)
{
	// If we don't have anything to save, we're done ...
	if(m_properties.empty())
		return;

	// Create an XML element to contain our proxies if it doesn't already exist ...
	xml::element& xml_properties = Element.safe_element("properties");

	// Save 'em ...
	for(properties_t::iterator property = m_properties.begin(); property != m_properties.end(); ++property)
		(**property).save(xml_properties, Context);
}

void persistent_property_collection::load(xml::element& Element, const ipersistent::load_context& Context)
{
	// Create an order-preserving mapping from names to XML elements ...
	std::vector<string_t> xml_names;
	std::vector<xml::element*> xml_property_storage;

	xml::element* const xml_properties = xml::find_element(Element, "properties");
	if(!xml_properties)
		return;

	for(xml::element::elements_t::iterator xml_property = xml_properties->children.begin(); xml_property != xml_properties->children.end(); ++xml_property)
	{
		if(xml_property->name != "property")
			continue;

		const string_t name = xml::attribute_text(*xml_property, "name");
		if(name.empty())
			continue;

		if(std::count(xml_names.begin(), xml_names.end(), name))
		{
			log() << warning << "Duplicate property [" << name << "] will be ignored" << std::endl;
			continue;
		}

		xml_names.push_back(name);
		xml_property_storage.push_back(&(*xml_property));
	}

	// Match builtin properties to each XML element ...
	for(uint_t i = 0; i != m_names.size(); ++i)
	{
		for(uint_t j = 0; j != xml_names.size(); ++j)
		{
			if(xml_names[j] == m_names[i])
			{
				m_properties[i]->load(*xml_property_storage[j], Context);
				xml_names.erase(xml_names.begin() + j);
				xml_property_storage.erase(xml_property_storage.begin() + j);
				break;
			}
		}
	}

	// If we don't have anything else to look at, we're done ...
	if(xml_property_storage.empty())
		return;

	// Otherwise, everything left-over should be a user-property ...
	if(k3d::inode* const node = dynamic_cast<k3d::inode*>(this))
	{
		for(uint_t j = 0; j != xml_property_storage.size(); ++j)
		{
			xml::element& xml_property = *xml_property_storage[j];

			const string_t user_property = xml::attribute_text(xml_property, "user_property");
			if(user_property == "generic" || user_property == "vanilla" /* for backwards compatibility */)
			{
				const string_t type = xml::attribute_text(xml_property, "type");
				const string_t name = xml::attribute_text(xml_property, "name");
				const string_t label = xml::attribute_text(xml_property, "label");
				const string_t description = xml::attribute_text(xml_property, "description");

				k3d::iproperty* const new_property = property::create(*node, type, name, label, description);
				if(!new_property)
				{
					log() << error << k3d_file_reference << ": error creating property [" << name << "] with type [" << type << "]" << std::endl;
					continue;
				}

				if(k3d::ipersistent* const persistent = dynamic_cast<k3d::ipersistent*>(new_property))
					persistent->load(xml_property, Context);
			}
			else if(user_property == "renderman")
			{
				const string_t type = xml::attribute_text(xml_property, "type");
				const string_t parameter_type = xml::attribute_text(xml_property, "parameter_type");
				const string_t parameter_name = xml::attribute_text(xml_property, "parameter_name");
				const string_t name = xml::attribute_text(xml_property, "name");
				const string_t label = xml::attribute_text(xml_property, "label");
				const string_t description = xml::attribute_text(xml_property, "description");

				k3d::iproperty* new_property = 0;
				if(parameter_type == "attribute")
					new_property = property::ri::create_attribute(*node, type, parameter_name, name, label, description);
				else if(parameter_type == "option")
					new_property = property::ri::create_option(*node, type, parameter_name, name, label, description);
					
				if(!new_property)
				{
					log() << error << k3d_file_reference << ": error creating property [" << name << "] with type [" << type << "]" << std::endl;
					continue;
				}

				if(k3d::ipersistent* const persistent = dynamic_cast<k3d::ipersistent*>(new_property))
					persistent->load(xml_property, Context);
			}
		}
	}
}

} // namespace k3d

