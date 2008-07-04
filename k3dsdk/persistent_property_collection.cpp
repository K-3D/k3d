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

#include "imetadata.h"
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
#include <set>

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

	// Save property values ...
	xml::element& xml_properties = Element.append(xml::element("properties"));
	for(uint_t i = 0; i != m_names.size(); ++i)
		(*m_properties[i]).save(xml_properties, Context);
	
	// Save property metadata ...
	/** \todo Next time we break backwards compatibility on documents, merge properties and their metadata */
	for(uint_t i = 0; i != m_names.size(); ++i)
	{
		if(imetadata* const metadata = dynamic_cast<imetadata*>(m_properties[i]))
		{
			xml::element& xml_metadata = xml_properties.append(xml::element("metadata", xml::attribute("name", m_names[i])));

			const imetadata::metadata_t pairs = metadata->get_metadata();
			for(imetadata::metadata_t::const_iterator pair = pairs.begin(); pair != pairs.end(); ++pair)
				xml_metadata.append(xml::element("pair", xml::attribute("name", pair->first), xml::attribute("value", pair->second)));
		}
	}
}

void persistent_property_collection::load(xml::element& Element, const ipersistent::load_context& Context)
{
	xml::element* const xml_properties = xml::find_element(Element, "properties");
	if(!xml_properties)
		return;

	// Keep-track of whether this object is a document node (required for user-properties) ...
	k3d::inode* const node = dynamic_cast<k3d::inode*>(this);

	// Keep-track of xml property names as we go, for error-detection ...
	std::set<string_t> xml_property_names;

	// Load <property> tags ...
	for(xml::element::elements_t::iterator xml_property = xml_properties->children.begin(); xml_property != xml_properties->children.end(); ++xml_property)
	{
		if(xml_property->name != "property")
			continue;

		const string_t xml_property_name = xml::attribute_text(*xml_property, "name");
		if(xml_property_name.empty())
		{
			log() << error << "property with missing name attribute will not be loaded" << std::endl;
			continue;
		}

		if(xml_property_names.count(xml_property_name))
		{
			log() << error << "property with duplicate name [" << xml_property_name << "] will not be loaded" << std::endl;
			continue;
		}

		xml_property_names.insert(xml_property_name);

		bool xml_property_used = false;

		// Try matching the XML property to a builtin property ...
		if(!xml_property_used)
		{
			for(uint_t i = 0; i != m_names.size(); ++i)
			{
				if(xml_property_name == m_names[i])
				{
					m_properties[i]->load(*xml_property, Context);
					xml_property_used = true;
					break;
				}
			}
		}

		// See if the XML property is a user-property to be instantiated ...
		if(!xml_property_used && node)
		{
			const string_t xml_user_property = xml::attribute_text(*xml_property, "user_property");
			if(xml_user_property == "generic" || xml_user_property == "vanilla" /* for backwards compatibility */)
			{
				xml_property_used = true;

				const string_t type = xml::attribute_text(*xml_property, "type");
				const string_t name = xml::attribute_text(*xml_property, "name");
				const string_t label = xml::attribute_text(*xml_property, "label");
				const string_t description = xml::attribute_text(*xml_property, "description");

				k3d::iproperty* const new_property = property::create(*node, type, name, label, description);

				if(new_property)
				{
					if(k3d::ipersistent* const persistent = dynamic_cast<k3d::ipersistent*>(new_property))
						persistent->load(*xml_property, Context);
				}
				else
				{
					log() << error << k3d_file_reference << ": error creating property [" << name << "] with type [" << type << "]" << std::endl;
				}
			}
			else if(xml_user_property == "renderman")
			{
				xml_property_used = true;

				const string_t type = xml::attribute_text(*xml_property, "type");
				const string_t parameter_type = xml::attribute_text(*xml_property, "parameter_type");
				const string_t parameter_name = xml::attribute_text(*xml_property, "parameter_name");
				const string_t name = xml::attribute_text(*xml_property, "name");
				const string_t label = xml::attribute_text(*xml_property, "label");
				const string_t description = xml::attribute_text(*xml_property, "description");

				k3d::iproperty* new_property = 0;
				if(parameter_type == "attribute")
					new_property = property::ri::create_attribute(*node, type, parameter_name, name, label, description);
				else if(parameter_type == "option")
					new_property = property::ri::create_option(*node, type, parameter_name, name, label, description);
			
				if(new_property)
				{
					if(k3d::ipersistent* const persistent = dynamic_cast<k3d::ipersistent*>(new_property))
						persistent->load(*xml_property, Context);
				}
				else
				{
					log() << error << k3d_file_reference << ": error creating property [" << name << "] with type [" << type << "]" << std::endl;
				}
			}
			else if(xml_user_property.size())
			{
				log() << error << "unknown user property type: " << xml_user_property << std::endl;
			}
		}
	}

	// Load <metadata> tags ...
	for(xml::element::elements_t::iterator xml_metadata = xml_properties->children.begin(); xml_metadata != xml_properties->children.end(); ++xml_metadata)
	{
		if(xml_metadata->name != "metadata")
			continue;

		const string_t xml_metadata_name = xml::attribute_text(*xml_metadata, "name");
		if(xml_metadata_name.empty())
		{
			log() << error << "property metadata with missing name attribute will not be loaded" << std::endl;
			continue;
		}

		if(!xml_property_names.count(xml_metadata_name))
		{
			log() << error << "property metadata with unknown name [" << xml_metadata_name << "] will not be loaded" << std::endl;
			continue;
		}

		// Try matching the XML metadata to a property ...
		for(uint_t i = 0; i != m_names.size(); ++i)
		{
			if(xml_metadata_name != m_names[i])
				continue;

			imetadata::metadata_t pairs;
			for(xml::element::elements_t::const_iterator xml_pair = xml_metadata->children.begin(); xml_pair != xml_metadata->children.end(); ++xml_pair)
			{
				if(xml_pair->name != "pair")
					continue;

				pairs.insert(std::make_pair(xml::attribute_text(*xml_pair, "name"), xml::attribute_text(*xml_pair, "value")));
			}

			if(imetadata* const metadata = dynamic_cast<imetadata*>(m_properties[i]))
				metadata->set_metadata(pairs);

			break;
		}
	}
}

} // namespace k3d

