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
		\brief Implements the k3d::persistent_container class, which automatically serializes / deserializes a group of data objects
		\author Tim Shead (tshead@k-3d.com)
*/

#include "aqsis_properties.h"
#include "iproperty_collection.h"
#include "persistent.h"
#include "properties_ri.h"
#include "result.h"
#include "tokens.h"
#include "type_registry.h"
#include "user_properties.h"
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

void persistent_container::enable_serialization(const std::string& Name, ipersistent& Child)
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

template<typename PropertyT, typename ValueT>
void load_user_property(xml::element& Element, const ipersistent::load_context& Context, iproperty_collection& PropertyCollection, ipersistent_container& PersistentContainer, inode& Object, const ValueT& Value)
{
	const std::string name = xml::attribute_text(Element, "name");
	const std::string label = xml::attribute_text(Element, "label");
	const std::string description = xml::attribute_text(Element, "description");

	PropertyT* const property = user::create_property<PropertyT, ValueT>(name, label, description, Object.document(), PropertyCollection, PersistentContainer, &Object, Value);
	property->load(Element, Context);
}

template<typename PropertyT, typename ValueT>
void load_renderman_property(xml::element& Element, const ipersistent::load_context& Context, iproperty_collection& PropertyCollection, ipersistent_container& PersistentContainer, inode& Object, const ValueT& Value)
{
	const std::string name = xml::attribute_text(Element, "name");
	const std::string label = xml::attribute_text(Element, "label");
	const std::string description = xml::attribute_text(Element, "description");
	const irenderman_property::parameter_type_t parameter_type = xml::attribute_value<irenderman_property::parameter_type_t>(Element, "parameter_type", irenderman_property::ATTRIBUTE);
	const std::string parameter_name = xml::attribute_text(Element, "parameter_name");

	PropertyT* const property = ri::create_property<PropertyT, ValueT>(parameter_type, parameter_name, name, label, description, Object.document(), PropertyCollection, PersistentContainer, &Object, Value);
	property->load(Element, Context);
}

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
	typedef std::map<std::string, xml::element*> element_map_t;
	element_map_t element_map;

	// Insert <property> tags ... look for a container called <properties> first ...
	xml::element* const xml_properties = xml::find_element(Element, "properties");
	if(!xml_properties)
		return;

	for(xml::element::elements_t::iterator xml_property = xml_properties->children.begin(); xml_property != xml_properties->children.end(); ++xml_property)
	{
		if(xml_property->name != "property")
			continue;

		const std::string name = xml::attribute_text(*xml_property, "name");
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

	k3d::iproperty_collection* const property_collection = dynamic_cast<k3d::iproperty_collection*>(this);
	k3d::ipersistent_container* const persistent_container = dynamic_cast<k3d::ipersistent_container*>(this);
	k3d::inode* const object = dynamic_cast<k3d::inode*>(this);

	if(property_collection && persistent_container && object)
	{
		for(element_map_t::iterator element = element_map.begin(); element != element_map.end(); ++element)
		{
			const std::string user_property = xml::attribute_text(*element->second, "user_property");
			const std::string type = xml::attribute_text(*element->second, "type");

			if(user_property == "vanilla")
			{
				if(type == type_string<k3d::bool_t>())
					load_user_property<user::k3d_bool_t_property>(*element->second, Context, *property_collection, *persistent_container, *object, false);
				else if(type == type_string<k3d::int32_t>())
					load_user_property<user::k3d_int32_t_property>(*element->second, Context, *property_collection, *persistent_container, *object, 0);
				else if(type == type_string<k3d::double_t>())
					load_user_property<user::k3d_double_t_property>(*element->second, Context, *property_collection, *persistent_container, *object, 0);
				else if(type == type_string<k3d::string_t>())
					load_user_property<user::k3d_string_t_property>(*element->second, Context, *property_collection, *persistent_container, *object, std::string());
				else if(type == type_string<k3d::ri::itexture*>())
					load_user_property<user::k3d_ri_itexture_property>(*element->second, Context, *property_collection, *persistent_container, *object, static_cast<k3d::ri::itexture*>(0));
				else if(type == type_string<k3d::point3>() || type == "k3d::ri::point")
					load_user_property<user::k3d_point3_property>(*element->second, Context, *property_collection, *persistent_container, *object, point3(0, 0, 0));
				else if(type == type_string<k3d::vector3>() || type == "k3d::ri::vector")
					load_user_property<user::k3d_vector3_property>(*element->second, Context, *property_collection, *persistent_container, *object, vector3(0, 0, 0));
				else if(type == type_string<k3d::normal3>() || type == "k3d::ri::normal")
					load_user_property<user::k3d_normal3_property>(*element->second, Context, *property_collection, *persistent_container, *object, normal3(0, 0, 0));
				else if(type == type_string<k3d::point4>() || type == "k3d::ri::hpoint")
					load_user_property<user::k3d_point4_property>(*element->second, Context, *property_collection, *persistent_container, *object, point4(0, 0, 0, 0));
				else if(type == type_string<k3d::matrix4>())
					load_user_property<user::k3d_matrix4_property>(*element->second, Context, *property_collection, *persistent_container, *object, identity3D());
				else if(type == type_string<k3d::color>() || type == "k3d::ri::color")
					load_user_property<user::k3d_color_property>(*element->second, Context, *property_collection, *persistent_container, *object, color(1, 1, 1));
				else if(type == type_string<k3d::legacy::mesh*>())
					load_user_property<user::legacy_mesh_property>(*element->second, Context, *property_collection, *persistent_container, *object, static_cast<k3d::legacy::mesh*>(0));
				else if(type == type_string<k3d::mesh*>())
					load_user_property<user::k3d_mesh_property>(*element->second, Context, *property_collection, *persistent_container, *object, static_cast<k3d::mesh*>(0));
				else if(type == type_string<k3d::gl::imesh_painter*>())
					load_user_property<user::k3d_gl_imesh_painter_property>(*element->second, Context, *property_collection, *persistent_container, *object, static_cast<k3d::gl::imesh_painter*>(0));
				else if(type == type_string<k3d::ri::imesh_painter*>())
					load_user_property<user::k3d_ri_imesh_painter_property>(*element->second, Context, *property_collection, *persistent_container, *object, static_cast<k3d::ri::imesh_painter*>(0));
				else if(type == type_string<k3d::imaterial*>())
					load_user_property<user::k3d_imaterial_property>(*element->second, Context, *property_collection, *persistent_container, *object, static_cast<k3d::imaterial*>(0));
				else
				{
					log() << warning << k3d_file_reference << ": ignoring unknown user property type [" << type << "]" << std::endl;
					continue;
				}
			}
			else if(user_property == "renderman")
			{
				if(type == type_string<ri::integer>())
					load_renderman_property<ri::integer_property>(*element->second, Context, *property_collection, *persistent_container, *object, 0);
				else if(type == type_string<ri::real>())
					load_renderman_property<ri::real_property>(*element->second, Context, *property_collection, *persistent_container, *object, 0.0);
				else if(type == type_string<ri::string>())
					load_renderman_property<ri::string_property>(*element->second, Context, *property_collection, *persistent_container, *object, std::string());
				else if(type == type_string<ri::point>())
					load_renderman_property<ri::point_property>(*element->second, Context, *property_collection, *persistent_container, *object, ri::point(0, 0, 0));
				else if(type == type_string<ri::vector>())
					load_renderman_property<ri::vector_property>(*element->second, Context, *property_collection, *persistent_container, *object, ri::vector(0, 0, 0));
				else if(type == type_string<ri::normal>())
					load_renderman_property<ri::normal_property>(*element->second, Context, *property_collection, *persistent_container, *object, ri::normal(0, 0, 0));
				else if(type == type_string<ri::hpoint>())
					load_renderman_property<ri::hpoint_property>(*element->second, Context, *property_collection, *persistent_container, *object, ri::hpoint(0, 0, 0, 0));
				else if(type == type_string<ri::matrix>())
					load_renderman_property<ri::matrix_property>(*element->second, Context, *property_collection, *persistent_container, *object, identity3D());
				else if(type == type_string<ri::color>())
					load_renderman_property<ri::color_property>(*element->second, Context, *property_collection, *persistent_container, *object, ri::color(0, 0, 0));
				else
				{
					log() << warning << k3d_file_reference << ": ignoring unknown renderman property type [" << type << "]" << std::endl;
					continue;
				}
			}
			else if(user_property == "aqsis")
			{
				if(type == "displacement_layer_connection")
					load_user_property<aqsis::displacement_layer_connection_property>(*element->second, Context, *property_collection, *persistent_container, *object, static_cast<k3d::inode*>(0));
				else if(type == "surface_layer_connection")
					load_user_property<aqsis::surface_layer_connection_property>(*element->second, Context, *property_collection, *persistent_container, *object, static_cast<k3d::inode*>(0));
				else
				{
					log() << warning << k3d_file_reference <<  ": ignoring unknown aqsis property type [" << type << "]" << std::endl;
					continue;
				}
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

