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

#include "classes.h"
#include "ipipeline.h"
#include "idocument.h"
#include "idocument_importer.h"
#include "idocument_exporter.h"
#include "imaterial.h"
#include "imesh_painter_gl.h"
#include "imesh_painter_ri.h"
#include "inode.h"
#include "inode_collection.h"
#include "ipersistent.h"
#include "ipersistent_lookup.h"
#include "iplugin_factory.h"
#include "iproperty.h"
#include "iproperty_collection.h"
#include "itransform_sink.h"
#include "itransform_source.h"
#include "plugins.h"
#include "property.h"
#include "types_ri.h"
#include "result.h"
#include "serialization.h"
#include "share.h"
#include "shared_pointer.h"
#include "string_cast.h"
#include "type_registry.h"
#include "xml.h"

using namespace k3d::xml;

#include <boost/lexical_cast.hpp>

namespace k3d
{

namespace detail
{

////////////////////////////////////////////////////////////////////////////////////
// save_dependencies

class save_dependencies
{
public:
	typedef std::map<iproperty*, inode*> map_t;

	save_dependencies(map_t& Map, element& Element, const ipersistent::save_context& Context) :
		m_map(Map),
		m_element(Element),
		m_context(Context)
	{
	}

	void operator()(const ipipeline::dependency_t& Dependency)
	{
		// Sanity checks ...
		iproperty* const from_property = Dependency.second;
		if(!from_property)
			return;
		inode* const from_node = m_map[from_property];
		return_if_fail(from_node);

		iproperty* const to_property = Dependency.first;
		return_if_fail(to_property);
		inode* const to_node = m_map[to_property];
		return_if_fail(to_node);

		m_element.append(
			element("dependency",
				attribute("from_node", m_context.lookup.lookup_id(from_node)),
				attribute("from_property", from_property->property_name()),
				attribute("to_node", m_context.lookup.lookup_id(to_node)),
				attribute("to_property", to_property->property_name())));
	}

private:
	map_t& m_map;
	element& m_element;
	const ipersistent::save_context& m_context;
};

/////////////////////////////////////////////////////////////////////////////////////////////////
// load_dependencies

class load_dependencies
{
public:
	load_dependencies(ipipeline::dependencies_t& Dependencies, const ipersistent::load_context& Context) :
		m_dependencies(Dependencies),
		m_context(Context)
	{
	}

	void operator()(const element& Dependency)
	{
		if("dependency" != Dependency.name)
			return;

		ipersistent_lookup::id_type from_node_id = attribute_value<ipersistent_lookup::id_type>(Dependency, "from_node", 0);
		if(!from_node_id)
			from_node_id = attribute_value<ipersistent_lookup::id_type>(Dependency, "from_object", 0);
		return_if_fail(from_node_id);

		inode* const from_node = dynamic_cast<inode*>(m_context.lookup.lookup_object(from_node_id));
		if(!from_node)
		{
			log() << error << "Missing dependency source node [" << from_node_id << "]" << std::endl;
			return;
		}

		const std::string from_property_name = attribute_text(Dependency, "from_property");
		return_if_fail(from_property_name.size());
		iproperty* const from_property = property::get(*from_node, from_property_name);
		if(!from_property)
		{
			log() << error << "Missing dependency source property [" << from_node->name() << "." << from_property_name << "]" << std::endl;
			return;
		}

		ipersistent_lookup::id_type to_node_id = attribute_value<ipersistent_lookup::id_type>(Dependency, "to_node", 0);
		if(!to_node_id)
			to_node_id = attribute_value<ipersistent_lookup::id_type>(Dependency, "to_object", 0);
		return_if_fail(to_node_id);

		inode* const to_node = dynamic_cast<inode*>(m_context.lookup.lookup_object(to_node_id));
		if(!to_node)
		{
			log() << error << "Missing dependency target node [" << to_node_id << "]" << std::endl;
			return;
		}

		const std::string to_property_name = attribute_text(Dependency, "to_property");
		return_if_fail(to_property_name.size());
		iproperty* const to_property = property::get(*to_node, to_property_name);
		if(!to_property)
		{
			log() << error << "Missing dependency target property [" << to_node->name() << "." << to_property_name << "]" << std::endl;
			return;
		}

		if(from_property->property_type() != to_property->property_type())
		{
			log() << error << "Property type mismatch between [" << from_node->name() << "." << from_property_name << "] and [" << to_node->name() << "." << to_property_name << "]" << std::endl;
			return;
		}

		m_dependencies[to_property] = from_property;
	}

private:
	ipipeline::dependencies_t& m_dependencies;
	const ipersistent::load_context& m_context;
};

void upgrade_objects_element(element& XMLDocument)
{
	// Change <objects> to <nodes> ...
	if(element* const xml_objects = find_element(XMLDocument, "objects"))
	{
		log() << warning << "Converting obsolete <objects> tag to <nodes> tag" << std::endl;
		xml_objects->name = "nodes";
	}
}

void upgrade_object_elements(element& XMLDocument)
{
	// Change <object> to <node> ...
	if(element* const xml_nodes = find_element(XMLDocument, "nodes"))
	{
		bool nag = true;

		for(element::elements_t::iterator xml_node = xml_nodes->children.begin(); xml_node != xml_nodes->children.end(); ++xml_node)
		{
			if(xml_node->name == "object")
			{
				if(nag)
				{
					log() << warning << "Converting obsolete <object> tags to <node> tags" << std::endl;
					nag = false;
				}

				xml_node->name = "node";
			}
		}
	}
}

void upgrade_class_properties(element& XMLDocument)
{
	// Change "class" to "factory"
	if(element* const xml_nodes = find_element(XMLDocument, "nodes"))
	{
		bool nag = true;

		for(element::elements_t::iterator xml_node = xml_nodes->children.begin(); xml_node != xml_nodes->children.end(); ++xml_node)
		{
			if(xml_node->name != "node")
				continue;

			if(attribute* const xml_class = find_attribute(*xml_node, "class"))
			{
				xml_class->name = "factory";

				if(nag)
				{
					log() << warning << "Converting obsolete \"class\" properties to \"factory\" properties" << std::endl;
					nag = false;
				}
			}
		}
	}
}

void upgrade_variables_elements(element& XMLDocument)
{
	// Change <variables> to <properties>
	if(element* const xml_nodes = find_element(XMLDocument, "nodes"))
	{
		bool nag_variables = true;

		for(element::elements_t::iterator xml_node = xml_nodes->children.begin(); xml_node != xml_nodes->children.end(); ++xml_node)
		{
			if(xml_node->name != "node")
				continue;

			element* const xml_variables = find_element(*xml_node, "variables");
			if(!xml_variables)
				continue;

			if(nag_variables)
			{
				nag_variables = false;
				log() << warning << "Converting obsolete <variables> tags to <properties> tags" << std::endl;
			}

			xml_variables->name = "properties";
		}
	}
}

void upgrade_variable_elements(element& XMLDocument)
{
	// Change <variable>, <object>, <shader>, and <argument> to <property>
	if(element* const xml_nodes = find_element(XMLDocument, "nodes"))
	{
		bool nag_object = true;
		bool nag_shader = true;
		bool nag_variable = true;

		for(element::elements_t::iterator xml_node = xml_nodes->children.begin(); xml_node != xml_nodes->children.end(); ++xml_node)
		{
			if(xml_node->name != "node")
				continue;

			element* const xml_properties = find_element(*xml_node, "properties");
			if(!xml_properties)
				continue;

			for(element::elements_t::iterator xml_property = xml_properties->children.begin(); xml_property != xml_properties->children.end(); ++xml_property)
			{
				if(xml_property->name == "object")
				{
					if(nag_object)
					{
						nag_object = false;
						log() << warning << "Converting obsolete <object> tags to <property> tags" << std::endl;
					}
					xml_property->name = "property";
				}
				else if(xml_property->name == "shader")
				{
					if(nag_shader)
					{
						nag_shader = false;
						log() << warning << "Converting obsolete <shader> tags to <property> tags" << std::endl;
					}
					xml_property->name = "property";
				}
				else if(xml_property->name == "variable")
				{
					if(nag_variable)
					{
						nag_variable = false;
						log() << warning << "Converting obsolete <variable> tags to <property> tags" << std::endl;
					}
					xml_property->name = "property";
				}
			}
		}
	}
}

void upgrade_property_values(element& XMLDocument)
{
	if(element* const xml_nodes = find_element(XMLDocument, "nodes"))
	{
		bool nag_attribute = true;

		for(element::elements_t::iterator xml_node = xml_nodes->children.begin(); xml_node != xml_nodes->children.end(); ++xml_node)
		{
			if(xml_node->name != "node")
				continue;

			element* const xml_properties = find_element(*xml_node, "properties");
			if(!xml_properties)
				continue;

			for(element::elements_t::iterator xml_property = xml_properties->children.begin(); xml_property != xml_properties->children.end(); ++xml_property)
			{
				if(xml_property->name != "property")
					continue;

				attribute* const xml_value = find_attribute(*xml_property, "value");
				if(!xml_value)
					continue;

				xml_property->text = xml_value->value;

				if(nag_attribute)
				{
					log() << warning << "Moving obsolete \"value\" attribute to element text" << std::endl;
					nag_attribute = false;
				}
			}
		}
	}
}

void upgrade_user_property_types(element& XMLDocument)
{
	element* const xml_nodes = find_element(XMLDocument, "nodes");
	if(!xml_nodes)
		return;

	bool nag_attribute = true;

	for(element::elements_t::iterator xml_node = xml_nodes->children.begin(); xml_node != xml_nodes->children.end(); ++xml_node)
	{
		if(xml_node->name != "node")
			continue;

		element* const xml_properties = find_element(*xml_node, "properties");
		if(!xml_properties)
			continue;

		for(element::elements_t::iterator xml_property = xml_properties->children.begin(); xml_property != xml_properties->children.end(); ++xml_property)
		{
			if(xml_property->name != "property")
				continue;

			attribute* const xml_user_property = find_attribute(*xml_property, "user_property");
			if(!xml_user_property)
				continue;

			attribute* const xml_type = find_attribute(*xml_property, "type");
			if(!xml_type)
				continue;

			if(xml_type->value == "double")
				xml_type->value = "k3d::double_t";
			else
				continue;

			if(nag_attribute)
			{
				log() << warning << "Upgrading obsolete user property type" << std::endl;
				nag_attribute = false;
			}
		}
	}
}

void upgrade_pipeline_element(element& XMLDocument)
{
	// Change <pipeline> to <dependencies> ...
	if(element* const xml_pipeline = find_element(XMLDocument, "pipeline"))
	{
		log() << warning << "Converting obsolete <pipeline> tag to <dependencies> tag" << std::endl;
		xml_pipeline->name = "dependencies";
	}
}

void upgrade_dependency_elements(element& XMLDocument)
{
	// Update <dependency> properties ...
	if(element* const xml_dependencies = find_element(XMLDocument, "dependencies"))
	{
		bool nag_from = true;
		bool nag_to = true;

		for(element::elements_t::iterator xml_dependency = xml_dependencies->children.begin(); xml_dependency != xml_dependencies->children.end(); ++xml_dependency)
		{
			if(xml_dependency->name != "dependency")
				continue;

			if(attribute* const from_object = find_attribute(*xml_dependency, "from_object"))
			{
				if(nag_from)
				{
					nag_from = false;
					log() << warning << "converting from_object attributes" << std::endl;
				}

				from_object->name = "from_node";
			}

			if(attribute* const to_object = find_attribute(*xml_dependency, "to_object"))
			{
				if(nag_to)
				{
					nag_to = false;
					log() << warning << "converting to_object attributes" << std::endl;
				}

				to_object->name = "to_node";
			}
		}
	}
}

/// Upgrades LSystemParser nodes so their "orientation" property is set correctly
void upgrade_l_system_parser_nodes(element& XMLDocument)
{
	element* const xml_nodes = find_element(XMLDocument, "nodes");
	if(!xml_nodes)
		return;

	for(element::elements_t::iterator xml_node = xml_nodes->children.begin(); xml_node != xml_nodes->children.end(); ++xml_node)
	{
		if(xml_node->name != "node")
			continue;

		const uuid node_factory_id = attribute_value<uuid>(*xml_node, "factory", uuid::null());
		if(node_factory_id != uuid(0xa637e99d, 0x707c4342, 0x8c6d4d15, 0x78c9054a))
			continue;

		element* const xml_properties = find_element(*xml_node, "properties");
		if(!xml_properties)
			continue;

		element* xml_type = 0;
		for(element::elements_t::iterator xml_property = xml_properties->children.begin(); xml_property != xml_properties->children.end(); ++xml_property)
		{
			if(xml_property->name != "property")
				continue;

			if(attribute_text(*xml_property, "name") == "orientation")
			{
				xml_type = &*xml_property;
				break;
			}
		}

		if(!xml_type)
		{
			log() << warning << "Upgrading LSystemParser node" << std::endl;
			// Add new property, defaults to old behaviour
			xml_properties->append(element("property", "+y", attribute("name", "orientation")));
			xml_properties->append(element("property", "true", attribute("name", "flip_normals")));
		}
	}
}

/// Upgrades PolyGrid nodes so their "orientation" property is set correctly
void upgrade_poly_grid_nodes(element& XMLDocument)
{
	element* const xml_nodes = find_element(XMLDocument, "nodes");
	if(!xml_nodes)
		return;

	for(element::elements_t::iterator xml_node = xml_nodes->children.begin(); xml_node != xml_nodes->children.end(); ++xml_node)
	{
		if(xml_node->name != "node")
			continue;

		const uuid node_factory_id = attribute_value<uuid>(*xml_node, "factory", uuid::null());
		if(node_factory_id != uuid(0xacb3b4f8, 0x5cd6471c, 0xaed72686, 0xc576987c))
			continue;

		element* const xml_properties = find_element(*xml_node, "properties");
		if(!xml_properties)
			continue;

		element* xml_type = 0;
		for(element::elements_t::iterator xml_property = xml_properties->children.begin(); xml_property != xml_properties->children.end(); ++xml_property)
		{
			if(xml_property->name != "property")
				continue;

			if(attribute_text(*xml_property, "name") == "orientation")
			{
				xml_type = &*xml_property;
				break;
			}
		}

		if(!xml_type)
		{
			log() << warning << "Upgrading PolyGrid node" << std::endl;
			// Add new property, defaults to old behaviour
			xml_properties->append(element("property", "-z", attribute("name", "orientation")));
		}
	}
}

/// Upgrades PolyTerrainFFT nodes so their "orientation" property is set correctly
void upgrade_poly_terrain_fft_nodes(element& XMLDocument)
{
	element* const xml_nodes = find_element(XMLDocument, "nodes");
	if(!xml_nodes)
		return;

	for(element::elements_t::iterator xml_node = xml_nodes->children.begin(); xml_node != xml_nodes->children.end(); ++xml_node)
	{
		if(xml_node->name != "node")
			continue;

		const uuid node_factory_id = attribute_value<uuid>(*xml_node, "factory", uuid::null());
		if(node_factory_id != uuid(0x7646f5a1, 0x3f3640d6, 0x8d4c70af, 0x91bcb418))
			continue;

		element* const xml_properties = find_element(*xml_node, "properties");
		if(!xml_properties)
			continue;

		element* xml_type = 0;
		for(element::elements_t::iterator xml_property = xml_properties->children.begin(); xml_property != xml_properties->children.end(); ++xml_property)
		{
			if(xml_property->name != "property")
				continue;

			if(attribute_text(*xml_property, "name") == "orientation")
			{
				xml_type = &*xml_property;
				break;
			}
		}

		if(!xml_type)
		{
			log() << warning << "Upgrading PolyTerrainFFT node" << std::endl;
			// Add new property, defaults to old behaviour
			xml_properties->append(element("property", "+y", attribute("name", "orientation")));
		}
	}
}

/// Upgrades PolyTerrainHfBm nodes so their "orientation" property is set correctly
void upgrade_poly_terrain_hfbm_nodes(element& XMLDocument)
{
	element* const xml_nodes = find_element(XMLDocument, "nodes");
	if(!xml_nodes)
		return;

	for(element::elements_t::iterator xml_node = xml_nodes->children.begin(); xml_node != xml_nodes->children.end(); ++xml_node)
	{
		if(xml_node->name != "node")
			continue;

		const uuid node_factory_id = attribute_value<uuid>(*xml_node, "factory", uuid::null());
		if(node_factory_id != uuid(0xff22f8f8, 0xa8b540f6, 0xb612a012, 0x8d4e9adb))
			continue;

		element* const xml_properties = find_element(*xml_node, "properties");
		if(!xml_properties)
			continue;

		element* xml_type = 0;
		for(element::elements_t::iterator xml_property = xml_properties->children.begin(); xml_property != xml_properties->children.end(); ++xml_property)
		{
			if(xml_property->name != "property")
				continue;

			if(attribute_text(*xml_property, "name") == "orientation")
			{
				xml_type = &*xml_property;
				break;
			}
		}

		if(!xml_type)
		{
			log() << warning << "Upgrading PolyTerrainHfBm node" << std::endl;
			// Add new property, defaults to old behaviour
			xml_properties->append(element("property", "+y", attribute("name", "orientation")));
		}
	}
}

/// Upgrades PolyText nodes so their "orientation" property is set correctly
void upgrade_poly_text_nodes(element& XMLDocument)
{
	element* const xml_nodes = find_element(XMLDocument, "nodes");
	if(!xml_nodes)
		return;

	for(element::elements_t::iterator xml_node = xml_nodes->children.begin(); xml_node != xml_nodes->children.end(); ++xml_node)
	{
		if(xml_node->name != "node")
			continue;

		const uuid node_factory_id = attribute_value<uuid>(*xml_node, "factory", uuid::null());
		if(node_factory_id != uuid(0xd0691ef7, 0x0d6c41c0, 0xa607bea2, 0x09d386f5))
			continue;

		element* const xml_properties = find_element(*xml_node, "properties");
		if(!xml_properties)
			continue;

		element* xml_type = 0;
		for(element::elements_t::iterator xml_property = xml_properties->children.begin(); xml_property != xml_properties->children.end(); ++xml_property)
		{
			if(xml_property->name != "property")
				continue;

			if(attribute_text(*xml_property, "name") == "orientation")
			{
				xml_type = &*xml_property;
				break;
			}
		}

		if(!xml_type)
		{
			log() << warning << "Upgrading PolyText node" << std::endl;
			// Add new property, defaults to old behaviour
			xml_properties->append(element("property", "-z", attribute("name", "orientation")));
		}
	}
}

/// Upgrades PolySphere nodes so their "type" property is set correctly
void upgrade_poly_sphere_nodes(element& XMLDocument)
{
	element* const xml_nodes = find_element(XMLDocument, "nodes");
	if(!xml_nodes)
		return;

	for(element::elements_t::iterator xml_node = xml_nodes->children.begin(); xml_node != xml_nodes->children.end(); ++xml_node)
	{
		if(xml_node->name != "node")
			continue;

		const uuid node_factory_id = attribute_value<uuid>(*xml_node, "factory", uuid::null());
		if(node_factory_id != uuid(0x919c3786, 0x619e4e84, 0xb4ad868f, 0x1e77e67c))
			continue;

		element* const xml_properties = find_element(*xml_node, "properties");
		if(!xml_properties)
			continue;

		element* xml_type = 0;
		for(element::elements_t::iterator xml_property = xml_properties->children.begin(); xml_property != xml_properties->children.end(); ++xml_property)
		{
			if(xml_property->name != "property")
				continue;

			if(attribute_text(*xml_property, "name") == "type")
			{
				xml_type = &*xml_property;
				break;
			}
		}

		if(!xml_type)
		{
			log() << warning << "Upgrading PolySphere node" << std::endl;
			// Add new type, defaults to old behaviour
			xml_properties->append(element("property", "sphereized_cylinder", attribute("name", "type")));
		}
	}
}

/// Returns the largest node id in use in the given document
const ipersistent_lookup::id_type max_node_id(element& XMLDocument)
{
	ipersistent_lookup::id_type result = 0;
	if(element* const xml_nodes = find_element(XMLDocument, "nodes"))
	{
		for(element::elements_t::iterator xml_node = xml_nodes->children.begin(); xml_node != xml_nodes->children.end(); ++xml_node)
		{
			if(xml_node->name != "node")
				continue;

			result = std::max(result, attribute_value<ipersistent_lookup::id_type>(*xml_node, "id", 0));
		}
	}

	return result;
}

void adjust_dependencies(element& XMLDocument, const ipersistent_lookup::id_type FromID, const std::string& FromProperty, const ipersistent_lookup::id_type ToID, const std::string& ToProperty)
{
	if(element* const xml_dependencies = find_element(XMLDocument, "dependencies"))
	{
		for(element::elements_t::iterator xml_dependency = xml_dependencies->children.begin(); xml_dependency != xml_dependencies->children.end(); ++xml_dependency)
		{
			if(xml_dependency->name != "dependency")
				continue;

			attribute* const to_node = find_attribute(*xml_dependency, "to_node");
			if(!to_node)
				continue;

			if(from_string<ipersistent_lookup::id_type>(to_node->value, 0) != ToID)
				continue;

			attribute* const to_property = find_attribute(*xml_dependency, "to_property");
			if(!to_property)
				continue;

			if(to_property->value != ToProperty)
				continue;

			to_node->value = string_cast(FromID);
			to_property->value = FromProperty;

			break;
		}
	}
}

/// Upgrades transformable nodes so that their position/orientation/scale properties are replaced with a Transform node
void upgrade_transformable_nodes(element& XMLDocument)
{
	element* const xml_nodes = find_element(XMLDocument, "nodes");
	if(!xml_nodes)
		return;

	ipersistent_lookup::id_type next_node_id = max_node_id(XMLDocument) + 1;

	element::elements_t new_dependencies;
	element::elements_t new_nodes;

	for(element::elements_t::iterator xml_node = xml_nodes->children.begin(); xml_node != xml_nodes->children.end(); ++xml_node)
	{
		if(xml_node->name != "node")
			continue;

		const uuid node_factory_id = attribute_value<uuid>(*xml_node, "factory", uuid::null());
		iplugin_factory* const node_factory = plugin(node_factory_id);
		if(!node_factory)
			continue;

		const ipersistent_lookup::id_type node_id = attribute_value<ipersistent_lookup::id_type>(*xml_node, "id", 0);
		if(!node_id)
			continue;

		if(!node_factory->implements(typeid(itransform_source)))
			continue;

		if(!node_factory->implements(typeid(itransform_sink)))
			continue;

		element* const xml_properties = find_element(*xml_node, "properties");
		if(!xml_properties)
			continue;

		element* xml_position = 0;
		element* xml_orientation = 0;
		element* xml_scale = 0;

		for(element::elements_t::iterator xml_property = xml_properties->children.begin(); xml_property != xml_properties->children.end(); ++xml_property)
		{
			if(xml_property->name != "property")
				continue;

			const std::string property_name = attribute_text(*xml_property, "name");
			if(property_name == "position")
				xml_position = &(*xml_property);
			else if(property_name == "orientation")
				xml_orientation = &(*xml_property);
			else if(property_name == "scale")
				xml_scale = &(*xml_property);
		}

		if(!(xml_position && xml_orientation && xml_scale))
			continue;

		if(xml_position->text == "0 0 0" && xml_orientation->text == "0 0 0 1" && xml_scale->text == "1 1 1")
			continue;

		const std::string node_name = attribute_text(*xml_node, "name");
		log() << warning << "Upgrading old transformable node " << node_name << std::endl;

		const point3 position = from_string<point3>(xml_position->text, point3(0, 0, 0));
		const angle_axis orientation = from_string<angle_axis>(xml_orientation->text, angle_axis(0, point3(0, 0, 1)));
		const point3 scale = from_string<point3>(xml_scale->text, point3(1, 1, 1));
		const matrix4 matrix = translation3D(position) * rotation3D(orientation) * scaling3D(scale);

		adjust_dependencies(XMLDocument, next_node_id, "input_matrix", node_id, "input_matrix");

		new_dependencies.push_back(
			element("dependency",
				attribute("from_node", next_node_id),
				attribute("from_property", "output_matrix"),
				attribute("to_node", node_id),
				attribute("to_property", "input_matrix")));

		new_nodes.push_back(
			element("node",
				attribute("name", "Transformation"),
				attribute("factory", classes::FrozenTransformation()),
				attribute("id", next_node_id),
				element("properties",
					element("property", string_cast(identity3D()),
						attribute("name", "input_matrix")),
					element("property", string_cast(matrix),
						attribute("name", "matrix")))));

		++next_node_id;
	}

	xml_nodes->children.insert(xml_nodes->children.end(), new_nodes.begin(), new_nodes.end());

	element& xml_dependencies = XMLDocument.safe_element("dependencies");
	xml_dependencies.children.insert(xml_dependencies.children.end(), new_dependencies.begin(), new_dependencies.end());
}

/// Add default painters if no painters are present
void upgrade_painters(element& XMLDocument)
{
	element* const xml_nodes = find_element(XMLDocument, "nodes");
	if(!xml_nodes)
		return;

	ipersistent_lookup::id_type next_node_id = max_node_id(XMLDocument) + 1;

	element::elements_t new_dependencies;
	element::elements_t new_nodes;
	
	bool has_gl_painter = false;
	bool has_ri_painter = false;
	// Check if the document contains painters
	for(element::elements_t::iterator xml_node = xml_nodes->children.begin(); xml_node != xml_nodes->children.end(); ++xml_node)
	{
		if(xml_node->name != "node")
			continue;

		const uuid node_factory_id = attribute_value<uuid>(*xml_node, "factory", uuid::null());
		iplugin_factory* const node_factory = plugin(node_factory_id);
		if(!node_factory)
			continue;

		const ipersistent_lookup::id_type node_id = attribute_value<ipersistent_lookup::id_type>(*xml_node, "id", 0);
		if(!node_id)
			continue;

		if(node_factory->implements(typeid(gl::imesh_painter)))
			has_gl_painter = true;

		if(node_factory->implements(typeid(ri::imesh_painter)))
			has_ri_painter = true;
			
		if (has_gl_painter && has_ri_painter)
			break;
	}
	
	// if ri or gl painters are not found, a default version is constructed:
	if (!has_gl_painter)
	{
		ipersistent_lookup::id_type gl_painter_id = next_node_id;
		new_nodes.push_back(
			element("node",
				attribute("name", "GL Default Painter"),
				attribute("factory", (*plugins("OpenGLMultiPainter").begin())->persistent_factory_id()),
				attribute("id", next_node_id),
				element("properties",
					element("property", string_cast(next_node_id + 1),
						attribute("name", "points"),
						attribute("label", "Points"),
						attribute("description", ""),
						attribute("type", "k3d::gl::imesh_painter*"),
						attribute("user_property", "vanilla")),
					element("property", string_cast(next_node_id + 2),
						attribute("name", "edges"),
						attribute("label", "Edges"),
						attribute("description", ""),
						attribute("type", "k3d::gl::imesh_painter*"),
						attribute("user_property", "vanilla")),
					element("property", string_cast(next_node_id + 3),
						attribute("name", "faces"),
						attribute("label", "Faces"),
						attribute("description", ""),
						attribute("type", "k3d::gl::imesh_painter*"),
						attribute("user_property", "vanilla")),
					element("property", string_cast(next_node_id + 4),
						attribute("name", "sds_points"),
						attribute("label", "SDS Points"),
						attribute("description", ""),
						attribute("type", "k3d::gl::imesh_painter*"),
						attribute("user_property", "vanilla")),
					element("property", string_cast(next_node_id + 5),
						attribute("name", "sds_edges"),
						attribute("label", "SDS Edges"),
						attribute("description", ""),
						attribute("type", "k3d::gl::imesh_painter*"),
						attribute("user_property", "vanilla")),
					element("property", string_cast(next_node_id + 6),
						attribute("name", "sds_faces"),
						attribute("label", "SDS Faces"),
						attribute("description", ""),
						attribute("type", "k3d::gl::imesh_painter*"),
						attribute("user_property", "vanilla")),
					element("property", string_cast(next_node_id + 7),
						attribute("name", "linear_curves"),
						attribute("label", "Linear Curves"),
						attribute("description", ""),
						attribute("type", "k3d::gl::imesh_painter*"),
						attribute("user_property", "vanilla")),
					element("property", string_cast(next_node_id + 8),
						attribute("name", "cubic_curves"),
						attribute("label", "Cubic Curves"),
						attribute("description", ""),
						attribute("type", "k3d::gl::imesh_painter*"),
						attribute("user_property", "vanilla")),
					element("property", string_cast(next_node_id + 9),
						attribute("name", "nurbs_curves"),
						attribute("label", "NURBS Curves"),
						attribute("description", ""),
						attribute("type", "k3d::gl::imesh_painter*"),
						attribute("user_property", "vanilla")),
					element("property", string_cast(next_node_id + 10),
						attribute("name", "bilinear_patches"),
						attribute("label", "Bilinear Patches"),
						attribute("description", ""),
						attribute("type", "k3d::gl::imesh_painter*"),
						attribute("user_property", "vanilla")),
					element("property", string_cast(next_node_id + 11),
						attribute("name", "bicubic_patches"),
						attribute("label", "Bicubic Patches"),
						attribute("description", ""),
						attribute("type", "k3d::gl::imesh_painter*"),
						attribute("user_property", "vanilla")),
					element("property", string_cast(next_node_id + 12),
						attribute("name", "nurbs_patches"),
						attribute("label", "NURBS Patches"),
						attribute("description", ""),
						attribute("type", "k3d::gl::imesh_painter*"),
						attribute("user_property", "vanilla")),
					element("property", string_cast(next_node_id + 13),
						attribute("name", "blobbies"),
						attribute("label", "Blobbies"),
						attribute("description", ""),
						attribute("type", "k3d::gl::imesh_painter*"),
						attribute("user_property", "vanilla")),
					element("property", string_cast(next_node_id + 14),
						attribute("name", "face_normals"),
						attribute("label", "Face Normals"),
						attribute("description", ""),
						attribute("type", "k3d::gl::imesh_painter*"),
						attribute("user_property", "vanilla")),
					element("property", string_cast(next_node_id + 15),
						attribute("name", "face_orientation"),
						attribute("label", "Face Orientation"),
						attribute("description", ""),
						attribute("type", "k3d::gl::imesh_painter*"),
						attribute("user_property", "vanilla")))));
		++next_node_id;
		new_nodes.push_back(
			element("node",
				attribute("name", "GL Point Painter"),
				attribute("factory", (*plugins("GLPointPainter").begin())->persistent_factory_id()),
				attribute("id", next_node_id)));
		++next_node_id;
		new_nodes.push_back(
			element("node",
				attribute("name", "GL Edge Painter"),
				attribute("factory", (*plugins("GLEdgePainter").begin())->persistent_factory_id()),
				attribute("id", next_node_id)));
		++next_node_id;
		new_nodes.push_back(
			element("node",
				attribute("name", "GL Face Painter"),
				attribute("factory", (*plugins("GLFacePainter").begin())->persistent_factory_id()),
				attribute("id", next_node_id)));
		++next_node_id;
		new_nodes.push_back(
			element("node",
				attribute("name", "SDS Point Painter"),
				attribute("factory", (*plugins("GLSDSPointPainter").begin())->persistent_factory_id()),
				attribute("id", next_node_id)));
		++next_node_id;
		new_nodes.push_back(
			element("node",
				attribute("name", "SDS Edge Painter"),
				attribute("factory", (*plugins("GLSDSEdgePainter").begin())->persistent_factory_id()),
				attribute("id", next_node_id)));
		++next_node_id;
		new_nodes.push_back(
			element("node",
				attribute("name", "SDS Face Painter"),
				attribute("factory", (*plugins("GLSDSFacePainter").begin())->persistent_factory_id()),
				attribute("id", next_node_id)));
		++next_node_id;
		new_nodes.push_back(
			element("node",
				attribute("name", "GL Linear Curve Painter"),
				attribute("factory", (*plugins("OpenGLLinearCurvePainter").begin())->persistent_factory_id()),
				attribute("id", next_node_id)));
		++next_node_id;
		new_nodes.push_back(
			element("node",
				attribute("name", "GL Cubic Curve Painter"),
				attribute("factory", (*plugins("OpenGLCubicCurvePainter").begin())->persistent_factory_id()),
				attribute("id", next_node_id)));
		++next_node_id;
		new_nodes.push_back(
			element("node",
				attribute("name", "GL NURBS Curve Painter"),
				attribute("factory", (*plugins("OpenGLNURBSCurvePainter").begin())->persistent_factory_id()),
				attribute("id", next_node_id)));
		++next_node_id;
		new_nodes.push_back(
			element("node",
				attribute("name", "GL Bilinear Patch Painter"),
				attribute("factory", (*plugins("OpenGLBilinearPatchPainter").begin())->persistent_factory_id()),
				attribute("id", next_node_id)));
		++next_node_id;
		new_nodes.push_back(
			element("node",
				attribute("name", "GL Bicubic Patch Painter"),
				attribute("factory", (*plugins("OpenGLBicubicPatchPainter").begin())->persistent_factory_id()),
				attribute("id", next_node_id)));
		++next_node_id;
		new_nodes.push_back(
			element("node",
				attribute("name", "GL NURBS Patch Painter"),
				attribute("factory", (*plugins("OpenGLNURBSPatchPainter").begin())->persistent_factory_id()),
				attribute("id", next_node_id)));
		++next_node_id;
		new_nodes.push_back(
			element("node",
				attribute("name", "GL Blobby Point Painter"),
				attribute("factory", (*plugins("OpenGLBlobbyPointPainter").begin())->persistent_factory_id()),
				attribute("id", next_node_id)));
		++next_node_id;
		new_nodes.push_back(
			element("node",
				attribute("name", "GL Face Normal Painter"),
				attribute("factory", (*plugins("OpenGLFaceNormalPainter").begin())->persistent_factory_id()),
				attribute("id", next_node_id)));
		++next_node_id;
		new_nodes.push_back(
			element("node",
				attribute("name", "GL Face Orientation Painter"),
				attribute("factory", (*plugins("OpenGLFaceOrientationPainter").begin())->persistent_factory_id()),
				attribute("id", next_node_id)));
				
		// Now add painter properties to the MeshInstance nodes
		for(element::elements_t::iterator xml_node = xml_nodes->children.begin(); xml_node != xml_nodes->children.end(); ++xml_node)
		{
			if(xml_node->name != "node")
				continue;
	
			const uuid node_factory_id = attribute_value<uuid>(*xml_node, "factory", uuid::null());
			if(node_factory_id != classes::MeshInstance())
				continue;
	
			element* const xml_properties = find_element(*xml_node, "properties");
			if(!xml_properties)
				continue;
			
			xml_properties->push_back(element("property", string_cast(gl_painter_id), attribute("name", "gl_painter")));
		}
	}
	if (!has_ri_painter)
	{
		++next_node_id;
		ipersistent_lookup::id_type ri_painter_id = next_node_id;
		new_nodes.push_back(
			element("node",
				attribute("name", "Renderman Default Painter"),
				attribute("factory", (*plugins("RenderManMultiPainter").begin())->persistent_factory_id()),
				attribute("id", next_node_id),
				element("properties",
					element("property", string_cast(next_node_id + 1),
						attribute("name", "point_groups"),
						attribute("label", "Point Groups"),
						attribute("description", ""),
						attribute("type", "k3d::ri::imesh_painter*"),
						attribute("user_property", "vanilla")),
					element("property", string_cast(next_node_id + 2),
						attribute("name", "polyhedra"),
						attribute("label", "Polyhedra"),
						attribute("description", ""),
						attribute("type", "k3d::ri::imesh_painter*"),
						attribute("user_property", "vanilla")),
					element("property", string_cast(next_node_id + 3),
						attribute("name", "subdivision_surfaces"),
						attribute("label", "Subdivision Surfaces"),
						attribute("description", ""),
						attribute("type", "k3d::ri::imesh_painter*"),
						attribute("user_property", "vanilla")),
					element("property", string_cast(next_node_id + 4),
						attribute("name", "linear_curves"),
						attribute("label", "Linear Curves"),
						attribute("description", ""),
						attribute("type", "k3d::ri::imesh_painter*"),
						attribute("user_property", "vanilla")),
					element("property", string_cast(next_node_id + 5),
						attribute("name", "cubic_curves"),
						attribute("label", "Cubic Curves"),
						attribute("description", ""),
						attribute("type", "k3d::ri::imesh_painter*"),
						attribute("user_property", "vanilla")),
					element("property", string_cast(next_node_id + 6),
						attribute("name", "bilinear_patches"),
						attribute("label", "Bilinear Patches"),
						attribute("description", ""),
						attribute("type", "k3d::ri::imesh_painter*"),
						attribute("user_property", "vanilla")),
					element("property", string_cast(next_node_id + 7),
						attribute("name", "bicubic_patches"),
						attribute("label", "Bicubic Patches"),
						attribute("description", ""),
						attribute("type", "k3d::ri::imesh_painter*"),
						attribute("user_property", "vanilla")),
					element("property", string_cast(next_node_id + 8),
						attribute("name", "nurbs_patches"),
						attribute("label", "NURBS Patches"),
						attribute("description", ""),
						attribute("type", "k3d::ri::imesh_painter*"),
						attribute("user_property", "vanilla")),
					element("property", string_cast(next_node_id + 9),
						attribute("name", "blobbies"),
						attribute("label", "Blobbies"),
						attribute("description", ""),
						attribute("type", "k3d::ri::imesh_painter*"),
						attribute("user_property", "vanilla")))));
		++next_node_id;
		new_nodes.push_back(
			element("node",
				attribute("name", "RenderMan Point Group Painter"),
				attribute("factory", (*plugins("RenderManPointGroupPainter").begin())->persistent_factory_id()),
				attribute("id", next_node_id)));
		++next_node_id;
		new_nodes.push_back(
			element("node",
				attribute("name", "RenderMan Polyhedron Painter"),
				attribute("factory", (*plugins("RenderManPolyhedronPainter").begin())->persistent_factory_id()),
				attribute("id", next_node_id)));
		++next_node_id;
		new_nodes.push_back(
			element("node",
				attribute("name", "RenderMan Subdivision Surface Painter"),
				attribute("factory", (*plugins("RenderManSubdivisionSurfacePainter").begin())->persistent_factory_id()),
				attribute("id", next_node_id)));
		++next_node_id;
		new_nodes.push_back(
			element("node",
				attribute("name", "RenderMan Linear Curve Painter"),
				attribute("factory", (*plugins("RenderManLinearCurvePainter").begin())->persistent_factory_id()),
				attribute("id", next_node_id)));
		++next_node_id;
		new_nodes.push_back(
			element("node",
				attribute("name", "RenderMan Cubic Curve Painter"),
				attribute("factory", (*plugins("RenderManCubicCurvePainter").begin())->persistent_factory_id()),
				attribute("id", next_node_id)));
		++next_node_id;
		new_nodes.push_back(
			element("node",
				attribute("name", "RenderMan Bilinear Patch Painter"),
				attribute("factory", (*plugins("RenderManBilinearPatchPainter").begin())->persistent_factory_id()),
				attribute("id", next_node_id)));
		++next_node_id;
		new_nodes.push_back(
			element("node",
				attribute("name", "RenderMan Bicubic Patch Painter"),
				attribute("factory", (*plugins("RenderManBicubicPatchPainter").begin())->persistent_factory_id()),
				attribute("id", next_node_id)));
		++next_node_id;
		new_nodes.push_back(
			element("node",
				attribute("name", "RenderMan NURBS Patch Painter"),
				attribute("factory", (*plugins("RenderManNURBSPatchPainter").begin())->persistent_factory_id()),
				attribute("id", next_node_id)));
		++next_node_id;
		new_nodes.push_back(
			element("node",
				attribute("name", "RenderMan Blobby Painter"),
				attribute("factory", (*plugins("RenderManBlobbyPainter").begin())->persistent_factory_id()),
				attribute("id", next_node_id)));
				
		// Now add painter properties to the MeshInstance nodes
		for(element::elements_t::iterator xml_node = xml_nodes->children.begin(); xml_node != xml_nodes->children.end(); ++xml_node)
		{
			if(xml_node->name != "node")
				continue;
	
			const uuid node_factory_id = attribute_value<uuid>(*xml_node, "factory", uuid::null());
			if(node_factory_id != classes::MeshInstance())
				continue;
	
			element* const xml_properties = find_element(*xml_node, "properties");
			if(!xml_properties)
				continue;
			
			xml_properties->push_back(element("property", string_cast(ri_painter_id), attribute("name", "ri_painter")));
		}
	}
	
	xml_nodes->children.insert(xml_nodes->children.end(), new_nodes.begin(), new_nodes.end());
}

/// Helper functor for searching for shaders by name
struct same_name
{
	same_name(const std::string& Name) :
		name(Name)
	{
	}

	template<typename T>
	bool operator()(const T& LHS)
	{
		return LHS.name == name;
	}

	const std::string name;
};

} // namespace detail

/////////////////////////////////////////////////////////////////////////////
// upgrade_document

void upgrade_document(element& XMLDocument)
{
	// Note ... the order that these are called-in matters!
	detail::upgrade_objects_element(XMLDocument);
	detail::upgrade_object_elements(XMLDocument);
	detail::upgrade_class_properties(XMLDocument);
	detail::upgrade_variables_elements(XMLDocument);
	detail::upgrade_variable_elements(XMLDocument);
	detail::upgrade_property_values(XMLDocument);
	detail::upgrade_user_property_types(XMLDocument);
	detail::upgrade_pipeline_element(XMLDocument);
	detail::upgrade_dependency_elements(XMLDocument);
	detail::upgrade_l_system_parser_nodes(XMLDocument);
	detail::upgrade_poly_grid_nodes(XMLDocument);
	detail::upgrade_poly_sphere_nodes(XMLDocument);
	detail::upgrade_poly_terrain_fft_nodes(XMLDocument);
	detail::upgrade_poly_terrain_hfbm_nodes(XMLDocument);
	detail::upgrade_poly_text_nodes(XMLDocument);
	detail::upgrade_transformable_nodes(XMLDocument);
	detail::upgrade_painters(XMLDocument);
}

/////////////////////////////////////////////////////////////////////////////
// save_pipeline

void save_pipeline(idocument& Document, element& XML, const ipersistent::save_context& Context)
{
	// Create a mapping of properties to objects ...
	detail::save_dependencies::map_t object_map;
	const inode_collection::nodes_t& objects = Document.nodes().collection();
	for(inode_collection::nodes_t::const_iterator object = objects.begin(); object != objects.end(); ++object)
	{
		iproperty_collection* const property_collection = dynamic_cast<iproperty_collection*>(*object);
		if(!property_collection)
			continue;

		const iproperty_collection::properties_t properties(property_collection->properties());
		for(iproperty_collection::properties_t::const_iterator property = properties.begin(); property != properties.end(); ++property)
			object_map[*property] = *object;
	}

	// Save all dependencies
	element& xml_dependencies = XML.append(element("dependencies"));
	std::for_each(Document.pipeline().dependencies().begin(), Document.pipeline().dependencies().end(), detail::save_dependencies(object_map, xml_dependencies, Context));
}

/////////////////////////////////////////////////////////////////////////////
// load_pipeline

void load_pipeline(idocument& Document, element& XML, const ipersistent::load_context& Context)
{
	// If we don't have any DAG information, we're done ...
	element* xml_dependencies = find_element(XML, "dependencies");

	if(!xml_dependencies)
		return;

	// Load data and update the DAG ...
	ipipeline::dependencies_t dependencies;
	std::for_each(xml_dependencies->children.begin(), xml_dependencies->children.end(), detail::load_dependencies(dependencies, Context));
	Document.pipeline().set_dependencies(dependencies);
}

bool import_file(idocument& Document, idocument_importer& FormatFilter, const filesystem::path& File)
{
	return FormatFilter.read_file(Document, File);
}

bool export_file(idocument& Document, idocument_exporter& FormatFilter, const filesystem::path& File)
{
	return FormatFilter.write_file(Document, File);
}

void save_node(ipersistent& Node, xml::element& XML, const ipersistent::save_context& Context)
{
	inode* const node = dynamic_cast<inode*>(&Node);
	return_if_fail(node);

	xml::element& xml_node = XML.append(element("node",
		attribute("name", node->name()),
		attribute("factory", node->factory().persistent_factory_id()),
		attribute("id", Context.lookup.lookup_id(node))));
	Node.save(xml_node, Context);
}

} // namespace k3d

