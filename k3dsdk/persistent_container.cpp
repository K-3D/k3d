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
#include "persistent.h"
#include "properties.h"
#include "result.h"
#include "tokens.h"
#include "type_registry.h"
#include "utility.h"

#include <iostream>
#include <map>

namespace k3d
{

/////////////////////////////////////////////////////////////////////////////
// persistent_container

persistent_container::persistent_container()
{
}

persistent_container::~persistent_container()
{
}

void persistent_container::enable_serialization(const string_t& Name, ipersistent& Child)
{
	if(m_children.count(Name))
	{
		k3d::log() << error << "attempt to enable serialization for duplicate object [" << Name << "]" << std::endl;
		return;
	}

	m_children.insert(std::make_pair(Name, &Child));
}

void persistent_container::enable_serialization(const named_children_t& Children)
{
	for(named_children_t::const_iterator child = Children.begin(); child != Children.end(); ++child)
		enable_serialization(child->first, *child->second);
}

void persistent_container::disable_serialization(ipersistent& Child)
{
	for(named_children_t::iterator child = m_children.begin(); child != m_children.end(); ++child)
	{
		if(child->second == &Child)
		{
			m_children.erase(child);
			return;
		}
	}
}

void persistent_container::disable_serialization(const children_t& Children)
{
	for(children_t::const_iterator child = Children.begin(); child != Children.end(); ++child)
		disable_serialization(**child);
}

const ipersistent_container::named_children_t& persistent_container::persistent_children()
{
	return m_children;
}

/*
template<typename PropertyT, typename ValueT>
void load_user_property(xml::element& Element, const ipersistent::load_context& Context, iproperty_collection& PropertyCollection, ipersistent_container& PersistentContainer, inode& Object, const ValueT& Value)
{
	const string_t name = xml::attribute_text(Element, "name");
	const string_t label = xml::attribute_text(Element, "label");
	const string_t description = xml::attribute_text(Element, "description");

	PropertyT* const property = user::create_property<PropertyT, ValueT>(name, label, description, Object.document(), PropertyCollection, PersistentContainer, &Object, Value);
	property->load(Element, Context);
}

template<typename PropertyT, typename ValueT>
void load_renderman_property(xml::element& Element, const ipersistent::load_context& Context, iproperty_collection& PropertyCollection, ipersistent_container& PersistentContainer, inode& Object, const ValueT& Value)
{
	const string_t name = xml::attribute_text(Element, "name");
	const string_t label = xml::attribute_text(Element, "label");
	const string_t description = xml::attribute_text(Element, "description");
	const irenderman_property::parameter_type_t parameter_type = xml::attribute_value<irenderman_property::parameter_type_t>(Element, "parameter_type", irenderman_property::ATTRIBUTE);
	const string_t parameter_name = xml::attribute_text(Element, "parameter_name");

	PropertyT* const property = ri::create_property<PropertyT, ValueT>(parameter_type, parameter_name, name, label, description, Object.document(), PropertyCollection, PersistentContainer, &Object, Value);
	property->load(Element, Context);
}
*/

void persistent_container::save(xml::element& Element, const ipersistent::save_context& Context)
{
	// If we don't have anything to save, we're done ...
	if(!m_children.size())
		return;

	// Create an XML element to contain our proxies if it doesn't already exist ...
	xml::element& properties = Element.safe_element("properties");

	// Save 'em ...
	for(named_children_t::iterator child = m_children.begin(); child != m_children.end(); ++child)
		child->second->save(properties, Context);
}

void persistent_container::load(xml::element& Element, const ipersistent::load_context& Context)
{
	// Create a mapping of elements to choose from ...
	typedef std::map<string_t, xml::element*> element_map_t;
	element_map_t element_map;

	// Insert <property> tags ... look for a container called <properties> first ...
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

		if(element_map.count(name))
		{
			log() << warning << "Duplicate property [" << name << "] will be ignored" << std::endl;
			continue;
		}

		element_map[name] = &(*xml_property);
	}

	// Find a proxy that matches the element ...
	for(named_children_t::iterator child = m_children.begin(); child != m_children.end(); ++child)
	{
		const element_map_t::iterator child_element = element_map.find(child->first);
		if(child_element == element_map.end())
			continue;

		// Load it up!
		child->second->load(*child_element->second, Context);
		element_map.erase(child_element);
	}

	// If we don't have anything else to look at, we're done ...
	if(element_map.empty())
		return;

	if(k3d::inode* const node = dynamic_cast<k3d::inode*>(this))
	{
		for(element_map_t::iterator element = element_map.begin(); element != element_map.end(); ++element)
		{
			const string_t user_property = xml::attribute_text(*element->second, "user_property");
			if(user_property == "generic" || user_property == "vanilla" /* for backwards compatibility */)
			{
				xml::element& xml_property = *element->second;

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
				xml::element& xml_property = *element->second;

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

////////////////////////////////////////////////////////////////////////////////////////////////////
// user_properties

const ipersistent_container::children_t user_properties(ipersistent_container& PersistentContainer)
{
	ipersistent_container::children_t results;

	const ipersistent_container::named_children_t& persistent_children = PersistentContainer.persistent_children();
	for(ipersistent_container::named_children_t::const_iterator child = persistent_children.begin(); child != persistent_children.end(); ++child)
	{
		if(dynamic_cast<iuser_property*>(child->second))
			results.push_back(child->second);
	}

	return results;
}

} // namespace k3d

