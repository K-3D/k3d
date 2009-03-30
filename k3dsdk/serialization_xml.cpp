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

#include "array.h"
#include "classes.h"
#include "idocument.h"
#include "imaterial.h"
#include "imesh_painter_gl.h"
#include "imesh_painter_ri.h"
#include "imetadata.h"
#include "inode.h"
#include "inode_collection.h"
#include "inode_selection.h"
#include "ipersistent_lookup.h"
#include "ipipeline.h"
#include "iplugin_factory.h"
#include "iproperty.h"
#include "iproperty_collection.h"
#include "itransform_sink.h"
#include "itransform_source.h"
#include "legacy_mesh.h"
#include "mesh.h"
#include "mesh_selection.h"
#include "named_array_types.h"
#include "nurbs_curve.h"
#include "plugins.h"
#include "properties.h"
#include "result.h"
#include "serialization_xml.h"
#include "share.h"
#include "string_cast.h"
#include "type_registry.h"
#include "types_ri.h"
#include "xml.h"

#include <boost/lexical_cast.hpp>
#include <boost/mpl/for_each.hpp>
#include <boost/scoped_ptr.hpp>

namespace k3d
{

namespace xml
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

		const string_t from_property_name = attribute_text(Dependency, "from_property");
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

		const string_t to_property_name = attribute_text(Dependency, "to_property");
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

void adjust_dependencies(element& XMLDocument, const ipersistent_lookup::id_type FromID, const string_t& FromProperty, const ipersistent_lookup::id_type ToID, const string_t& ToProperty)
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
		iplugin_factory* const node_factory = plugin::factory::lookup(node_factory_id);
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

			const string_t property_name = attribute_text(*xml_property, "name");
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

		const string_t node_name = attribute_text(*xml_node, "name");
		log() << warning << "Upgrading old transformable node " << node_name << std::endl;

		const vector3 position = from_string<vector3>(xml_position->text, vector3(0, 0, 0));
		const angle_axis orientation = from_string<angle_axis>(xml_orientation->text, angle_axis(0, point3(0, 0, 1)));
		const point3 scale = from_string<point3>(xml_scale->text, point3(1, 1, 1));
		const matrix4 matrix = translate3(position) * rotate3(orientation) * scale3(scale[0], scale[1], scale[2]);

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
					element("property", string_cast(identity3()),
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
		iplugin_factory* const node_factory = plugin::factory::lookup(node_factory_id);
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
		return_if_fail(plugin::factory::lookup("OpenGLMultiPainter"));
		return_if_fail(plugin::factory::lookup("VirtualOpenGLPointPainter"));
		return_if_fail(plugin::factory::lookup("VirtualOpenGLEdgePainter"));
		return_if_fail(plugin::factory::lookup("VirtualOpenGLFacePainter"));
		return_if_fail(plugin::factory::lookup("VirtualOpenGLSDSPointPainter"));
		return_if_fail(plugin::factory::lookup("VirtualOpenGLSDSEdgePainter"));
		return_if_fail(plugin::factory::lookup("VirtualOpenGLSDSFacePainter"));
		return_if_fail(plugin::factory::lookup("OpenGLLinearCurvePainter"));
		return_if_fail(plugin::factory::lookup("OpenGLCubicCurvePainter"));
		return_if_fail(plugin::factory::lookup("OpenGLNURBSCurvePainter"));
		return_if_fail(plugin::factory::lookup("OpenGLBilinearPatchPainter"));
		return_if_fail(plugin::factory::lookup("OpenGLBicubicPatchPainter"));
		return_if_fail(plugin::factory::lookup("OpenGLNURBSPatchPainter"));
		return_if_fail(plugin::factory::lookup("OpenGLBlobbyPointPainter"));
		return_if_fail(plugin::factory::lookup("OpenGLFaceNormalPainter"));
		return_if_fail(plugin::factory::lookup("OpenGLFaceOrientationPainter"));

		ipersistent_lookup::id_type gl_painter_id = next_node_id;
		new_nodes.push_back(
			element("node",
				attribute("name", "GL Default Painter"),
				attribute("factory", plugin::factory::lookup("OpenGLMultiPainter")->factory_id()),
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
				attribute("factory", plugin::factory::lookup("OpenGLPointPainter")->factory_id()),
				attribute("id", next_node_id)));
		++next_node_id;
		new_nodes.push_back(
			element("node",
				attribute("name", "GL Edge Painter"),
				attribute("factory", plugin::factory::lookup("VirtualOpenGLEdgePainter")->factory_id()),
				attribute("id", next_node_id)));
		++next_node_id;
		new_nodes.push_back(
			element("node",
				attribute("name", "GL Face Painter"),
				attribute("factory", plugin::factory::lookup("VirtualOpenGLFacePainter")->factory_id()),
				attribute("id", next_node_id)));
		++next_node_id;
		new_nodes.push_back(
			element("node",
				attribute("name", "SDS Point Painter"),
				attribute("factory", plugin::factory::lookup("VirtualOpenGLSDSPointPainter")->factory_id()),
				attribute("id", next_node_id)));
		++next_node_id;
		new_nodes.push_back(
			element("node",
				attribute("name", "SDS Edge Painter"),
				attribute("factory", plugin::factory::lookup("VirtualOpenGLSDSEdgePainter")->factory_id()),
				attribute("id", next_node_id)));
		++next_node_id;
		new_nodes.push_back(
			element("node",
				attribute("name", "SDS Face Painter"),
				attribute("factory", plugin::factory::lookup("VirtualOpenGLSDSFacePainter")->factory_id()),
				attribute("id", next_node_id)));
		++next_node_id;
		new_nodes.push_back(
			element("node",
				attribute("name", "GL Linear Curve Painter"),
				attribute("factory", plugin::factory::lookup("OpenGLLinearCurvePainter")->factory_id()),
				attribute("id", next_node_id)));
		++next_node_id;
		new_nodes.push_back(
			element("node",
				attribute("name", "GL Cubic Curve Painter"),
				attribute("factory", plugin::factory::lookup("OpenGLCubicCurvePainter")->factory_id()),
				attribute("id", next_node_id)));
		++next_node_id;
		new_nodes.push_back(
			element("node",
				attribute("name", "GL NURBS Curve Painter"),
				attribute("factory", plugin::factory::lookup("OpenGLNURBSCurvePainter")->factory_id()),
				attribute("id", next_node_id)));
		++next_node_id;
		new_nodes.push_back(
			element("node",
				attribute("name", "GL Bilinear Patch Painter"),
				attribute("factory", plugin::factory::lookup("OpenGLBilinearPatchPainter")->factory_id()),
				attribute("id", next_node_id)));
		++next_node_id;
		new_nodes.push_back(
			element("node",
				attribute("name", "GL Bicubic Patch Painter"),
				attribute("factory", plugin::factory::lookup("OpenGLBicubicPatchPainter")->factory_id()),
				attribute("id", next_node_id)));
		++next_node_id;
		new_nodes.push_back(
			element("node",
				attribute("name", "GL NURBS Patch Painter"),
				attribute("factory", plugin::factory::lookup("OpenGLNURBSPatchPainter")->factory_id()),
				attribute("id", next_node_id)));
		++next_node_id;
		new_nodes.push_back(
			element("node",
				attribute("name", "GL Blobby Point Painter"),
				attribute("factory", plugin::factory::lookup("OpenGLBlobbyPointPainter")->factory_id()),
				attribute("id", next_node_id)));
		++next_node_id;
		new_nodes.push_back(
			element("node",
				attribute("name", "GL Face Normal Painter"),
				attribute("factory", plugin::factory::lookup("OpenGLFaceNormalPainter")->factory_id()),
				attribute("id", next_node_id)));
		++next_node_id;
		new_nodes.push_back(
			element("node",
				attribute("name", "GL Face Orientation Painter"),
				attribute("factory", plugin::factory::lookup("OpenGLFaceOrientationPainter")->factory_id()),
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
		return_if_fail(plugin::factory::lookup("RenderManMultiPainter"));
		return_if_fail(plugin::factory::lookup("RenderManPointGroupPainter"));
		return_if_fail(plugin::factory::lookup("RenderManPolyhedronPainter"));
		return_if_fail(plugin::factory::lookup("RenderManSubdivisionSurfacePainter"));
		return_if_fail(plugin::factory::lookup("RenderManLinearCurvePainter"));
		return_if_fail(plugin::factory::lookup("RenderManCubicCurvePainter"));
		return_if_fail(plugin::factory::lookup("RenderManBilinearPatchPainter"));
		return_if_fail(plugin::factory::lookup("RenderManBicubicPatchPainter"));
		return_if_fail(plugin::factory::lookup("RenderManNURBSPatchPainter"));
		return_if_fail(plugin::factory::lookup("RenderManBlobbyPainter"));

		++next_node_id;
		ipersistent_lookup::id_type ri_painter_id = next_node_id;
		new_nodes.push_back(
			element("node",
				attribute("name", "Renderman Default Painter"),
				attribute("factory", plugin::factory::lookup("RenderManMultiPainter")->factory_id()),
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
				attribute("factory", plugin::factory::lookup("RenderManPointGroupPainter")->factory_id()),
				attribute("id", next_node_id)));
		++next_node_id;
		new_nodes.push_back(
			element("node",
				attribute("name", "RenderMan Polyhedron Painter"),
				attribute("factory", plugin::factory::lookup("RenderManPolyhedronPainter")->factory_id()),
				attribute("id", next_node_id)));
		++next_node_id;
		new_nodes.push_back(
			element("node",
				attribute("name", "RenderMan Subdivision Surface Painter"),
				attribute("factory", plugin::factory::lookup("RenderManSubdivisionSurfacePainter")->factory_id()),
				attribute("id", next_node_id)));
		++next_node_id;
		new_nodes.push_back(
			element("node",
				attribute("name", "RenderMan Linear Curve Painter"),
				attribute("factory", plugin::factory::lookup("RenderManLinearCurvePainter")->factory_id()),
				attribute("id", next_node_id)));
		++next_node_id;
		new_nodes.push_back(
			element("node",
				attribute("name", "RenderMan Cubic Curve Painter"),
				attribute("factory", plugin::factory::lookup("RenderManCubicCurvePainter")->factory_id()),
				attribute("id", next_node_id)));
		++next_node_id;
		new_nodes.push_back(
			element("node",
				attribute("name", "RenderMan Bilinear Patch Painter"),
				attribute("factory", plugin::factory::lookup("RenderManBilinearPatchPainter")->factory_id()),
				attribute("id", next_node_id)));
		++next_node_id;
		new_nodes.push_back(
			element("node",
				attribute("name", "RenderMan Bicubic Patch Painter"),
				attribute("factory", plugin::factory::lookup("RenderManBicubicPatchPainter")->factory_id()),
				attribute("id", next_node_id)));
		++next_node_id;
		new_nodes.push_back(
			element("node",
				attribute("name", "RenderMan NURBS Patch Painter"),
				attribute("factory", plugin::factory::lookup("RenderManNURBSPatchPainter")->factory_id()),
				attribute("id", next_node_id)));
		++next_node_id;
		new_nodes.push_back(
			element("node",
				attribute("name", "RenderMan Blobby Painter"),
				attribute("factory", plugin::factory::lookup("RenderManBlobbyPainter")->factory_id()),
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

void upgrade_node_selection(element& XMLDocument)
{
	element* const xml_nodes = find_element(XMLDocument, "nodes");
	if(!xml_nodes)
		return;

	ipersistent_lookup::id_type node_selection_id = max_node_id(XMLDocument) + 1;
	
	// Check if the document contains an inode_selection node
	for(element::elements_t::iterator xml_node = xml_nodes->children.begin(); xml_node != xml_nodes->children.end(); ++xml_node)
	{
		if(xml_node->name != "node")
			continue;

		const uuid node_factory_id = attribute_value<uuid>(*xml_node, "factory", uuid::null());
		iplugin_factory* const node_factory = plugin::factory::lookup(node_factory_id);
		if(!node_factory)
			continue;

		const ipersistent_lookup::id_type node_id = attribute_value<ipersistent_lookup::id_type>(*xml_node, "id", 0);
		if(!node_id)
			continue;

		if(node_factory->implements(typeid(inode_selection)))
			return;
	}
	
	element node_selection("node",
			attribute("name", "Node Selection"),
			attribute("factory", plugin::factory::lookup("NodeSelection")->factory_id()),
			attribute("id", node_selection_id),
			element("metadata",
					element("pair", "node_selection", attribute("name", "ngui:unique_node"))));
	
	xml_nodes->children.push_back(node_selection);
	
	// Now add the node_selection node to the GL Engine node(s)
	for(element::elements_t::iterator xml_node = xml_nodes->children.begin(); xml_node != xml_nodes->children.end(); ++xml_node)
	{
		if(xml_node->name != "node")
			continue;

		const uuid node_factory_id = attribute_value<uuid>(*xml_node, "factory", uuid::null());
		if(node_factory_id != plugin::factory::lookup("OpenGLEngine")->factory_id())
			continue;

		element* const xml_properties = find_element(*xml_node, "properties");
		if(!xml_properties)
			continue;
		
		xml_properties->push_back(element("property", string_cast(node_selection_id), attribute("name", "node_selection")));
	}
}

/// Inserts a SelectCompanion modifier before old Delete nodes
void upgrade_delete_components_nodes(element& XMLDocument)
{
	element* const xml_nodes = find_element(XMLDocument, "nodes");
	if(!xml_nodes)
		return;
	
	element* const xml_dependencies = find_element(XMLDocument, "dependencies");
	if(!xml_dependencies)
		return;
	
	ipersistent_lookup::id_type next_id = max_node_id(XMLDocument) + 1;
	k3d::uint_t sequence = 1;
	
	element::elements_t new_nodes;
	for(element::elements_t::iterator xml_node = xml_nodes->children.begin(); xml_node != xml_nodes->children.end(); ++xml_node)
	{
		if(xml_node->name != "node")
			continue;

		const uuid node_factory_id = attribute_value<uuid>(*xml_node, "factory", uuid::null());
		if(node_factory_id != uuid(0xc98c37d5, 0xa50c43c7, 0xb88c3687, 0x73cd3c4d)) // Old Delete ID
			continue;

		const ipersistent_lookup::id_type node_id = attribute_value<ipersistent_lookup::id_type>(*xml_node, "id", 0);
		if(!node_id)
			continue;
		
		// Upgrade the factory attribute
		set_attribute(*xml_node, attribute("factory", string_cast(plugin::factory::lookup("Delete")->factory_id())));
		
		element select_companion("node",
				attribute("name", "Select Companion " + string_cast(sequence)),
				attribute("factory", plugin::factory::lookup("SelectCompanion")->factory_id()),
				attribute("id", next_id),
				element("properties",
						element("property", "true", attribute("name", "keep_original_selection"))));
		new_nodes.push_back(select_companion);
		
		element::elements_t new_dependencies;
		for(element::elements_t::iterator xml_dependency = xml_dependencies->children.begin(); xml_dependency != xml_dependencies->children.end(); ++xml_dependency)
		{
			if(attribute_value<ipersistent_lookup::id_type>(*xml_dependency, "to_node", 0) == node_id
					&& attribute_value<string_t>(*xml_dependency, "to_property", "") == "input_mesh")
			{
				ipersistent_lookup::id_type from_node = attribute_value<ipersistent_lookup::id_type>(*xml_dependency, "from_node", 0);
				element new_dependency("dependency",
						attribute("from_node", string_cast(from_node)),
						attribute("from_property", "output_mesh"),
						attribute("to_node", string_cast(next_id)),
						attribute("to_property", "input_mesh"));
				new_dependencies.push_back(new_dependency);
				set_attribute(*xml_dependency, attribute("from_node", string_cast(next_id)));
			}
		}
		xml_dependencies->children.insert(xml_dependencies->children.end(), new_dependencies.begin(), new_dependencies.end());
		
		++sequence;
		++next_id;
	}
	xml_nodes->children.insert(xml_nodes->children.end(), new_nodes.begin(), new_nodes.end());
}

/// Helper functor for searching for shaders by name
struct same_name
{
	same_name(const string_t& Name) :
		name(Name)
	{
	}

	template<typename T>
	bool operator()(const T& LHS)
	{
		return LHS.name == name;
	}

	const string_t name;
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
	detail::upgrade_node_selection(XMLDocument);
	detail::upgrade_delete_components_nodes(XMLDocument);
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

//////////////////////////////////////////////////////////////////////////////
// save

void save(inode& Node, element& XML, const ipersistent::save_context& Context)
{
	// Save the basic node information ...
	element& xml_node = XML.append(element("node",
		attribute("name", Node.name()),
		attribute("factory", Node.factory().factory_id()),
		attribute("id", Context.lookup.lookup_id(&Node))));

	// Save node metadata (if any) ...
	if(imetadata* const metadata = dynamic_cast<imetadata*>(&Node))
	{
		const imetadata::metadata_t pairs = metadata->get_metadata();
		if(pairs.size())
		{
			xml::element& xml_metadata = xml_node.append(xml::element("metadata"));

			for(imetadata::metadata_t::const_iterator pair = pairs.begin(); pair != pairs.end(); ++pair)
				xml_metadata.append(xml::element("pair", xml::attribute("name", pair->first), pair->second));
		}
	}

	// Allow the node to save the rest of its internal state ...
	if(ipersistent* const persistent = dynamic_cast<ipersistent*>(&Node))
		persistent->save(xml_node, Context);
}

//////////////////////////////////////////////////////////////////////////////
// load

void load(inode& Node, element& XML, const ipersistent::load_context& Context)
{
	// Load (optional) node metadata ...
	if(xml::element* const xml_metadata = xml::find_element(XML, "metadata"))
	{
		imetadata::metadata_t pairs;
		for(xml::element::elements_t::const_iterator xml_pair = xml_metadata->children.begin(); xml_pair != xml_metadata->children.end(); ++xml_pair)
		{
			if(xml_pair->name != "pair")
				continue;

			pairs.insert(std::make_pair(xml::attribute_text(*xml_pair, "name"), xml_pair->text));
		}

		if(imetadata* const metadata = dynamic_cast<imetadata*>(&Node))
			metadata->set_metadata(pairs);
	}

	// Allow the node to load the rest of its internal state ...
	if(ipersistent* const persistent = dynamic_cast<ipersistent*>(&Node))
		persistent->load(XML, Context);
}

namespace detail
{

/////////////////////////////////////////////////////////////////////////////
// save_array_metadata

void save_array_metadata(element& Storage, const array& Array, const ipersistent::save_context& Context)
{
	const array::metadata_t metadata = Array.get_metadata();
	if(metadata.empty())
		return;

	element& xml_metadata = Storage.append(element("metadata"));

	for(array::metadata_t::const_iterator pair = metadata.begin(); pair != metadata.end(); ++pair)
		xml_metadata.append(element("pair", attribute("name", pair->first), pair->second));
}

/////////////////////////////////////////////////////////////////////////////
// save_array

template<typename array_type>
void save_array(element& Container, element Storage, const array_type& Array, const ipersistent::save_context& Context)
{
	typename array_type::const_iterator item = Array.begin();
	const typename array_type::const_iterator end = Array.end();

	std::ostringstream buffer;

	if(item != end)
		buffer << *item++;
	for(; item != end; ++item)
		buffer << " " << *item;

	Storage.text = buffer.str();
	save_array_metadata(Storage, Array, Context);

	Container.append(Storage);
}

/////////////////////////////////////////////////////////////////////////////
// save_array

/// Specialization of save_array to ensure that 8-bit integers don't get serialized as characters
void save_array(element& Container, element Storage, const typed_array<int8_t>& Array, const ipersistent::save_context& Context)
{
	typedef typed_array<int8_t> array_type;

	array_type::const_iterator item = Array.begin();
	const array_type::const_iterator end = Array.end();

	std::ostringstream buffer;

	if(item != end)
		buffer << static_cast<int16_t>(*item++);
	for(; item != end; ++item)
		buffer << " " << static_cast<int16_t>(*item);

	Storage.text = buffer.str();
	save_array_metadata(Storage, Array, Context);

	Container.append(Storage);
}

/////////////////////////////////////////////////////////////////////////////
// save_array

/// Specialization of save_array to ensure that 8-bit unsigned integers don't get serialized as characters
void save_array(element& Container, element Storage, const typed_array<uint8_t>& Array, const ipersistent::save_context& Context)
{
	typedef typed_array<uint8_t> array_type;

	array_type::const_iterator item = Array.begin();
	const array_type::const_iterator end = Array.end();

	std::ostringstream buffer;

	if(item != end)
		buffer << static_cast<uint16_t>(*item++);
	for(; item != end; ++item)
		buffer << " " << static_cast<uint16_t>(*item);

	Storage.text = buffer.str();
	save_array_metadata(Storage, Array, Context);

	Container.append(Storage);
}

/////////////////////////////////////////////////////////////////////////////
// save_array

/// Specialization of save_array to handle whitespace in strings
void save_array(element& Container, element Storage, const typed_array<string_t>& Array, const ipersistent::save_context& Context)
{
	typedef typed_array<string_t> array_type;

	const array_type::const_iterator end = Array.end();
	for(array_type::const_iterator item = Array.begin(); item != end; ++item)
		Storage.append(element("value", *item));

	save_array_metadata(Storage, Array, Context);

	Container.append(Storage);
}

/////////////////////////////////////////////////////////////////////////////
// save_array

/// Specialization of save_array to ensure we don't lose precision when writing arrays of double
void save_array(element& Container, element Storage, const typed_array<double_t>& Array, const ipersistent::save_context& Context)
{
	typedef typed_array<double_t> array_type;

	array_type::const_iterator item = Array.begin();
	const array_type::const_iterator end = Array.end();

	std::ostringstream buffer;
	buffer << std::setprecision(17);

	if(item != end)
		buffer << *item++;
	for(; item != end; ++item)
		buffer << " " << *item;

	Storage.text = buffer.str();
	save_array_metadata(Storage, Array, Context);

	Container.append(Storage);
}

/////////////////////////////////////////////////////////////////////////////
// save_array

void save_array(element& Container, element Storage, const typed_array<imaterial*>& Array, const ipersistent::save_context& Context)
{
	typedef typed_array<imaterial*> array_type;

	array_type::const_iterator item = Array.begin();
	const array_type::const_iterator end = Array.end();

	std::ostringstream buffer;

	if(item != end)
		buffer << Context.lookup.lookup_id(*item++);
	for(; item != end; ++item)
		buffer << " " << Context.lookup.lookup_id(*item);

	Storage.text = buffer.str();
	save_array_metadata(Storage, Array, Context);

	Container.append(Storage);
}

/////////////////////////////////////////////////////////////////////////////
// save_array

template<typename array_type>
void save_array(element& Container, element Storage, const pipeline_data<array_type>& Array, const ipersistent::save_context& Context)
{
	if(!Array)
		return;

	save_array(Container, Storage, *Array, Context);
}

/////////////////////////////////////////////////////////////////////////////
// save_typed_array

class save_typed_array
{
public:
	save_typed_array(element& Container, const string_t& Name, const array& AbstractArray, const ipersistent::save_context& Context, bool_t& Saved) :
		container(Container),
		name(Name),
		abstract_array(AbstractArray),
		context(Context),
		saved(Saved)
	{
		if(const uint_t_array* const concrete_array = dynamic_cast<const uint_t_array*>(&abstract_array))
		{
			saved = true;
			save_array(container, element("array", attribute("name", name), attribute("type", "k3d::uint_t")), *concrete_array, context);
		}
	}

	template<typename T>
	void operator()(T) const
	{
		if(saved)
			return;

		if(const typed_array<T>* const concrete_array = dynamic_cast<const typed_array<T>*>(&abstract_array))
		{
			saved = true;
			save_array(container, element("array", attribute("name", name), attribute("type", type_string<T>())), *concrete_array, context);
		}
	}

private:
	element& container;
	const string_t& name;
	const array& abstract_array;
	const ipersistent::save_context& context;
	k3d::bool_t& saved;
};

/////////////////////////////////////////////////////////////////////////////
// save_arrays

void save_arrays(element& Container, element Storage, const mesh::named_arrays_t& Arrays, const ipersistent::save_context& Context)
{
	element& container = Container.append(Storage);
	for(mesh::named_arrays_t::const_iterator array_iterator = Arrays.begin(); array_iterator != Arrays.end(); ++array_iterator)
	{
		const string_t name = array_iterator->first;
		const array* const abstract_array = array_iterator->second.get();

		if(name.empty())
		{
			log() << error << "will not serialize unnamed array" << std::endl;
			continue;
		}

		if(!abstract_array)
		{
			log() << error << "will not serialize null array [" << name << "]" << std::endl;
			continue;
		}

		bool_t saved = false;
		boost::mpl::for_each<named_array_types>(save_typed_array(container, name, *abstract_array, Context, saved));
		if(!saved)
			log() << error << k3d_file_reference << ": array [" << name << "] with unknown type [" << demangle(typeid(*abstract_array)) << "] will not be serialized" << std::endl;
	}
}

void save_arrays(element& Container, element Storage, const mesh::attribute_arrays_t& Arrays, const ipersistent::save_context& Context)
{
	element& container = Container.append(Storage);
	for(mesh::attribute_arrays_t::const_iterator array_iterator = Arrays.begin(); array_iterator != Arrays.end(); ++array_iterator)
	{
		const string_t name = array_iterator->first;
		const array* const abstract_array = array_iterator->second.get();

		if(name.empty())
		{
			log() << error << "will not serialize unnamed array" << std::endl;
			continue;
		}

		if(!abstract_array)
		{
			log() << error << "will not serialize null array [" << name << "]" << std::endl;
			continue;
		}

		bool_t saved = false;
		boost::mpl::for_each<named_array_types>(save_typed_array(container, name, *abstract_array, Context, saved));
		if(!saved)
			log() << error << k3d_file_reference << ": array [" << name << "] with unknown type [" << demangle(typeid(*abstract_array)) << "] will not be serialized" << std::endl;
	}
}

/////////////////////////////////////////////////////////////////////////////
// load_array_metadata

void load_array_metadata(const element& Storage, array& Array, const ipersistent::load_context& Context)
{
	if(const xml::element* const xml_metadata = xml::find_element(Storage, "metadata"))
	{
		for(xml::element::elements_t::const_iterator xml_pair = xml_metadata->children.begin(); xml_pair != xml_metadata->children.end(); ++xml_pair)
		{
			if(xml_pair->name != "pair")
				continue;

			Array.set_metadata_value(xml::attribute_text(*xml_pair, "name"), xml_pair->text);
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
// load_array

template<typename array_type>
void load_array(const element& Storage, array_type& Array, const ipersistent::load_context& Context)
{
	typename array_type::value_type value;

	std::istringstream buffer(Storage.text);
	while(true)
	{
		buffer >> value;

		if(!buffer)
			break;

		Array.push_back(value);
	}

	load_array_metadata(Storage, Array, Context);
}

/////////////////////////////////////////////////////////////////////////////
// load_array

void load_array(const element& Storage, typed_array<int8_t>& Array, const ipersistent::load_context& Context)
{
	int16_t value;

	std::istringstream buffer(Storage.text);
	while(true)
	{
		buffer >> value;

		if(!buffer)
			break;

		Array.push_back(static_cast<int8_t>(value));
	}

	load_array_metadata(Storage, Array, Context);
}

/////////////////////////////////////////////////////////////////////////////
// load_array

void load_array(const element& Storage, typed_array<uint8_t>& Array, const ipersistent::load_context& Context)
{
	uint16_t value;

	std::istringstream buffer(Storage.text);
	while(true)
	{
		buffer >> value;

		if(!buffer)
			break;

		Array.push_back(static_cast<uint8_t>(value));
	}

	load_array_metadata(Storage, Array, Context);
}

/////////////////////////////////////////////////////////////////////////////
// load_array

void load_array(const element& Storage, typed_array<string_t>& Array, const ipersistent::load_context& Context)
{
	for(element::elements_t::const_iterator xml_value = Storage.children.begin(); xml_value != Storage.children.end(); ++xml_value)
	{
		if(xml_value->name != "value")
			continue;

		Array.push_back(xml_value->text);
	}

	load_array_metadata(Storage, Array, Context);
}

/////////////////////////////////////////////////////////////////////////////
// load_array

void load_array(const element& Storage, typed_array<imaterial*>& Array, const ipersistent::load_context& Context)
{
	std::istringstream buffer(Storage.text);
	while(true)
	{
		ipersistent_lookup::id_type id = 0;
		buffer >> id;

		if(!buffer)
			break;

		Array.push_back(dynamic_cast<imaterial*>(Context.lookup.lookup_object(id)));
	}

	load_array_metadata(Storage, Array, Context);
}

/////////////////////////////////////////////////////////////////////////////
// load_array

void load_array(const element& Storage, typed_array<inode*>& Array, const ipersistent::load_context& Context)
{
	std::istringstream buffer(Storage.text);
	while(true)
	{
		ipersistent_lookup::id_type id = 0;
		buffer >> id;

		if(!buffer)
			break;

		Array.push_back(dynamic_cast<inode*>(Context.lookup.lookup_object(id)));
	}

	load_array_metadata(Storage, Array, Context);
}

/////////////////////////////////////////////////////////////////////////////
// load_array

template<typename array_type>
void load_array(const element& Container, const string_t& Storage, pipeline_data<array_type>& Array, const ipersistent::load_context& Context)
{
	const element* const storage = find_element(Container, Storage);
	if(!storage)
		return;

	array_type* const array = Array ? &Array.writable() : &Array.create();
	load_array(*storage, *array, Context);
}

template<typename array_type>
void load_array(const element& Container, const string_t& Storage, array_type& Array, const ipersistent::load_context& Context)
{
	const element* const storage = find_element(Container, Storage);
	if(!storage)
		return;

	load_array(*storage, Array, Context);
}

/////////////////////////////////////////////////////////////////////////////
// load_typed_array

template<typename arrays_t>
class load_typed_array
{
public:
	load_typed_array(const element& Storage, const string_t& Name, const string_t& Type, arrays_t& Arrays, const ipersistent::load_context& Context, k3d::bool_t& Loaded) :
		storage(Storage),
		name(Name),
		type(Type),
		arrays(Arrays),
		context(Context),
		loaded(Loaded)
	{
		if(type == "k3d::uint_t")
		{
			loaded = true;
			uint_t_array* const array = new uint_t_array();
			load_array(storage, *array, context);
			arrays.insert(std::make_pair(name, array));
		}
	}

	template<typename T>
	void operator()(T) const
	{
		if(loaded)
			return;

		if(type_string<T>() == type)
		{
			loaded = true;
			typed_array<T>* const array = new typed_array<T>();
			load_array(storage, *array, context);
			arrays.insert(std::make_pair(name, array));
		}
	}

private:
	const element& storage;
	const string_t& name;
	const string_t& type;
	arrays_t& arrays;
	const ipersistent::load_context& context;
	k3d::bool_t& loaded;
};

/////////////////////////////////////////////////////////////////////////////
// load_arrays

template<typename arrays_t>
void load_arrays(const element& Container, arrays_t& Arrays, const ipersistent::load_context& Context)
{
	for(size_t i = 0; i != Container.children.size(); ++i)
	{
		const element& storage = Container.children[i];

		if(storage.name != "array")
			continue;

		const string_t name = attribute_text(storage, "name");
		if(name.empty())
		{
			log() << error << "unnamed array will not be loaded" << std::endl;
			continue;
		}

		if(Arrays.count(name))
		{
			log() << error << "duplicate array [" << name << "] will not be loaded" << std::endl;
			continue;
		}

		const string_t type = attribute_text(storage, "type");
		if(type.empty())
		{
			log() << error << "untyped array [" << name << "] will not be loaded" << std::endl;
			continue;
		}

		bool loaded = false;
		boost::mpl::for_each<named_array_types>(load_typed_array<arrays_t>(storage, name, type, Arrays, Context, loaded));
		if(!loaded)
			log() << error << "array [" << name << "] with unknown type [" << type << "] will not be loaded" << std::endl;
	}
}

/////////////////////////////////////////////////////////////////////////////
// load_arrays

void load_arrays(const element& Container, const string_t& Storage, mesh::named_arrays_t& Arrays, const ipersistent::load_context& Context)
{
	const element* const container = find_element(Container, Storage);
	if(!container)
		return;

	load_arrays<mesh::named_arrays_t>(*container, Arrays, Context);
}

/////////////////////////////////////////////////////////////////////////////
// load_arrays

void load_arrays(const element& Container, const string_t& Storage, mesh::attribute_arrays_t& Arrays, const ipersistent::load_context& Context)
{
	const element* const container = find_element(Container, Storage);
	if(!container)
		return;

	load_arrays<mesh::attribute_arrays_t>(*container, Arrays, Context);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
// save_selection

void save_selection(element& Element, const mesh_selection::records_t& Records, const string_t& ElementName)
{
	if(Records.empty())
		return;

	element& xml_records = Element.append(element(ElementName));

	for(mesh_selection::records_t::const_iterator record = Records.begin(); record != Records.end(); ++record)
	{
		xml_records.append(
			element("selection",
				attribute("begin", record->begin),
				attribute("end", record->end),
				attribute("weight", record->weight)));
	}
}

void load_selection(const element& Element, mesh_selection::records_t& Records)
{
	for(element::elements_t::const_iterator xml_selection = Element.children.begin(); xml_selection != Element.children.end(); ++xml_selection)
	{
		if(xml_selection->name != "selection")
			continue;

		const size_t begin = attribute_value<size_t>(*xml_selection, "begin", 0);
		const size_t end = attribute_value<size_t>(*xml_selection, "end", 0);
		const double weight = attribute_value<double>(*xml_selection, "weight", 0.0);

		Records.push_back(mesh_selection::record(begin, end, weight));
	}
}

} // namespace detail

/////////////////////////////////////////////////////////////////////////////
// save

void save(const mesh& Mesh, element& Container, const ipersistent::save_context& Context)
{
	// Save points ...
	detail::save_array(Container, element("points"), Mesh.points, Context);
	detail::save_array(Container, element("point_selection"), Mesh.point_selection, Context);
	detail::save_arrays(Container, element("vertex_data"), Mesh.vertex_data, Context);

	if(Mesh.primitives.size())
	{
		element& xml_primitives = Container.append(element("primitives"));
		for(mesh::primitives_t::const_iterator primitive = Mesh.primitives.begin(); primitive != Mesh.primitives.end(); ++primitive)
		{
			if(!primitive->get())
				continue;

			element& xml_primitive = xml_primitives.append(element("primitive", attribute("type", (*primitive)->type)));

			if((*primitive)->structure.size())
			{
				detail::save_arrays(xml_primitive, element("structure"), (*primitive)->structure, Context);
			}
			
			if((*primitive)->attributes.size())
			{
				element& xml_attributes = xml_primitive.append(element("attributes"));
				for(mesh::named_attribute_arrays_t::const_iterator attribute = (*primitive)->attributes.begin(); attribute != (*primitive)->attributes.end(); ++attribute)
				{
					detail::save_arrays(xml_attributes, element("arrays", xml::attribute("type", attribute->first)), attribute->second, Context);
				}
			}
		}
	}

	if(Mesh.polyhedra)
	{
		element& container = Container.append(element("polyhedra"));
		detail::save_array(container, element("first_faces"), Mesh.polyhedra->first_faces, Context);
		detail::save_array(container, element("face_counts"), Mesh.polyhedra->face_counts, Context);
		detail::save_array(container, element("types"), Mesh.polyhedra->types, Context);
		detail::save_arrays(container, element("constant_data"), Mesh.polyhedra->constant_data, Context);
		detail::save_array(container, element("face_first_loops"), Mesh.polyhedra->face_first_loops, Context);
		detail::save_array(container, element("face_loop_counts"), Mesh.polyhedra->face_loop_counts, Context);
		detail::save_array(container, element("face_selection"), Mesh.polyhedra->face_selection, Context);
		detail::save_array(container, element("face_materials"), Mesh.polyhedra->face_materials, Context);
		detail::save_arrays(container, element("uniform_data"), Mesh.polyhedra->uniform_data, Context);
		detail::save_array(container, element("loop_first_edges"), Mesh.polyhedra->loop_first_edges, Context);
		detail::save_array(container, element("edge_points"), Mesh.polyhedra->edge_points, Context);
		detail::save_array(container, element("clockwise_edges"), Mesh.polyhedra->clockwise_edges, Context);
		detail::save_array(container, element("edge_selection"), Mesh.polyhedra->edge_selection, Context);
		detail::save_arrays(container, element("face_varying_data"), Mesh.polyhedra->face_varying_data, Context);
	}
}

/////////////////////////////////////////////////////////////////////////////
// load

void load(mesh& Mesh, element& Container, const ipersistent::load_context& Context)
{
	detail::load_array(Container, "points", Mesh.points, Context);
	detail::load_array(Container, "point_selection", Mesh.point_selection, Context);
	detail::load_arrays(Container, "vertex_data", Mesh.vertex_data, Context);

	if(const element* const xml_primitives = find_element(Container, "primitives"))
	{
		for(element::elements_t::const_iterator xml_primitive = xml_primitives->children.begin(); xml_primitive != xml_primitives->children.end(); ++xml_primitive)
		{
			if(xml_primitive->name != "primitive")
				continue;

			mesh::primitive& primitive = Mesh.primitives.create(attribute_text(*xml_primitive, "type"));

			if(const element* const xml_structure = find_element(*xml_primitive, "structure"))
			{
				detail::load_arrays(*xml_structure, primitive.structure, Context);
			}
			else if(const element* const xml_topology = find_element(*xml_primitive, "topology"))
			{
				detail::load_arrays(*xml_topology, primitive.structure, Context);
			}

			if(const element* const xml_attributes = find_element(*xml_primitive, "attributes"))
			{
				for(element::elements_t::const_iterator xml_arrays = xml_attributes->children.begin(); xml_arrays != xml_attributes->children.end(); ++xml_arrays)
				{
					if(xml_arrays->name != "arrays")
						continue;

					mesh::attribute_arrays_t arrays;
					detail::load_arrays(*xml_arrays, arrays, Context);

					primitive.attributes.insert(std::make_pair(attribute_text(*xml_arrays, "type"), arrays));
				}
			}
		}
	}

	if(element* const container = find_element(Container, "point_groups"))
	{
		assert_not_implemented();
/*
		mesh::point_groups_t* const point_groups = &Mesh.point_groups.create();
		detail::load_array(*container, "first_points", point_groups->first_points, Context);
		detail::load_array(*container, "point_counts", point_groups->point_counts, Context);
		detail::load_array(*container, "materials", point_groups->materials, Context);
		detail::load_arrays(*container, "constant_data", point_groups->constant_data, Context);
		detail::load_array(*container, "points", point_groups->points, Context);
		detail::load_arrays(*container, "varying_data", point_groups->varying_data, Context);
*/
	}

	if(element* const container = find_element(Container, "linear_curve_groups"))
	{
		assert_not_implemented();
/*
		mesh::linear_curve_groups_t* const linear_curve_groups = &Mesh.linear_curve_groups.create();
		detail::load_array(*container, "first_curves", linear_curve_groups->first_curves, Context);
		detail::load_array(*container, "curve_counts", linear_curve_groups->curve_counts, Context);
		detail::load_array(*container, "periodic_curves", linear_curve_groups->periodic_curves, Context);
		detail::load_array(*container, "materials", linear_curve_groups->materials, Context);
		detail::load_arrays(*container, "constant_data", linear_curve_groups->constant_data, Context);
		detail::load_array(*container, "curve_first_points", linear_curve_groups->curve_first_points, Context);
		detail::load_array(*container, "curve_point_counts", linear_curve_groups->curve_point_counts, Context);
		detail::load_array(*container, "curve_selection", linear_curve_groups->curve_selection, Context);
		detail::load_arrays(*container, "uniform_data", linear_curve_groups->uniform_data, Context);
		detail::load_array(*container, "curve_points", linear_curve_groups->curve_points, Context);
*/
	}

	if(element* const container = find_element(Container, "cubic_curve_groups"))
	{
		assert_not_implemented();
/*
		mesh::cubic_curve_groups_t* const cubic_curve_groups = &Mesh.cubic_curve_groups.create();
		detail::load_array(*container, "first_curves", cubic_curve_groups->first_curves, Context);
		detail::load_array(*container, "curve_counts", cubic_curve_groups->curve_counts, Context);
		detail::load_array(*container, "periodic_curves", cubic_curve_groups->periodic_curves, Context);
		detail::load_array(*container, "materials", cubic_curve_groups->materials, Context);
		detail::load_arrays(*container, "constant_data", cubic_curve_groups->constant_data, Context);
		detail::load_array(*container, "curve_first_points", cubic_curve_groups->curve_first_points, Context);
		detail::load_array(*container, "curve_point_counts", cubic_curve_groups->curve_point_counts, Context);
		detail::load_array(*container, "curve_selection", cubic_curve_groups->curve_selection, Context);
		detail::load_arrays(*container, "uniform_data", cubic_curve_groups->uniform_data, Context);
		detail::load_array(*container, "curve_points", cubic_curve_groups->curve_points, Context);
*/
	}

	if(element* const container = find_element(Container, "nurbs_curve_groups"))
	{
		boost::scoped_ptr<nurbs_curve::primitive> nurbs_curves(nurbs_curve::create(Mesh));

		detail::load_array(*container, "materials", nurbs_curves->material, Context);
		detail::load_arrays(*container, "constant_data", nurbs_curves->constant_data, Context);
		detail::load_array(*container, "curve_first_points", nurbs_curves->curve_first_points, Context);
		detail::load_array(*container, "curve_point_counts", nurbs_curves->curve_point_counts, Context);
		detail::load_array(*container, "curve_orders", nurbs_curves->curve_orders, Context);
		detail::load_array(*container, "curve_first_knots", nurbs_curves->curve_first_knots, Context);
		detail::load_array(*container, "curve_selection", nurbs_curves->curve_selections, Context);
		detail::load_arrays(*container, "uniform_data", nurbs_curves->uniform_data, Context);
		detail::load_array(*container, "curve_points", nurbs_curves->curve_points, Context);
		detail::load_array(*container, "curve_point_weights", nurbs_curves->curve_point_weights, Context);
		detail::load_array(*container, "curve_knots", nurbs_curves->curve_knots, Context);

	}

	if(element* const container = find_element(Container, "bilinear_patches"))
	{
		assert_not_implemented();

/*
		mesh::bilinear_patches_t* const bilinear_patches = &Mesh.bilinear_patches.create();
		detail::load_array(*container, "patch_selection", bilinear_patches->patch_selection, Context);
		detail::load_array(*container, "patch_materials", bilinear_patches->patch_materials, Context);
		detail::load_arrays(*container, "constant_data", bilinear_patches->constant_data, Context);
		detail::load_arrays(*container, "uniform_data", bilinear_patches->uniform_data, Context);
		detail::load_array(*container, "patch_points", bilinear_patches->patch_points, Context);
		detail::load_arrays(*container, "varying_data", bilinear_patches->varying_data, Context);
*/
	}

	if(element* const container = find_element(Container, "bicubic_patches"))
	{
		assert_not_implemented();

/*
		mesh::bicubic_patches_t* const bicubic_patches = &Mesh.bicubic_patches.create();
		detail::load_array(*container, "patch_selection", bicubic_patches->patch_selection, Context);
		detail::load_array(*container, "patch_materials", bicubic_patches->patch_materials, Context);
		detail::load_arrays(*container, "constant_data", bicubic_patches->constant_data, Context);
		detail::load_arrays(*container, "uniform_data", bicubic_patches->uniform_data, Context);
		detail::load_array(*container, "patch_points", bicubic_patches->patch_points, Context);
		detail::load_arrays(*container, "varying_data", bicubic_patches->varying_data, Context);
*/
	}

	if(element* const container = find_element(Container, "nurbs_patches"))
	{
		assert_not_implemented();
/*
		mesh::nurbs_patches_t* const nurbs_patches = &Mesh.nurbs_patches.create();
		detail::load_array(*container, "patch_first_points", nurbs_patches->patch_first_points, Context);
		detail::load_array(*container, "patch_u_point_counts", nurbs_patches->patch_u_point_counts, Context);
		detail::load_array(*container, "patch_v_point_counts", nurbs_patches->patch_v_point_counts, Context);
		detail::load_array(*container, "patch_u_orders", nurbs_patches->patch_u_orders, Context);
		detail::load_array(*container, "patch_v_orders", nurbs_patches->patch_v_orders, Context);
		detail::load_array(*container, "patch_u_first_knots", nurbs_patches->patch_u_first_knots, Context);
		detail::load_array(*container, "patch_v_first_knots", nurbs_patches->patch_v_first_knots, Context);
		detail::load_array(*container, "patch_selection", nurbs_patches->patch_selection, Context);
		detail::load_array(*container, "patch_materials", nurbs_patches->patch_materials, Context);
		detail::load_arrays(*container, "constant_data", nurbs_patches->constant_data, Context);
		detail::load_arrays(*container, "uniform_data", nurbs_patches->uniform_data, Context);
		detail::load_array(*container, "patch_points", nurbs_patches->patch_points, Context);
		detail::load_array(*container, "patch_point_weights", nurbs_patches->patch_point_weights, Context);
		detail::load_array(*container, "patch_u_knots", nurbs_patches->patch_u_knots, Context);
		detail::load_array(*container, "patch_v_knots", nurbs_patches->patch_v_knots, Context);
		detail::load_arrays(*container, "varying_data", nurbs_patches->varying_data, Context);
		detail::load_array(*container, "patch_trim_curve_loop_counts", nurbs_patches->patch_trim_curve_loop_counts, Context);
		detail::load_array(*container, "patch_first_trim_curve_loops", nurbs_patches->patch_first_trim_curve_loops, Context);
		detail::load_array(*container, "trim_points", nurbs_patches->trim_points, Context);
		detail::load_array(*container, "trim_point_selection", nurbs_patches->trim_point_selection, Context);
		detail::load_array(*container, "first_trim_curves", nurbs_patches->first_trim_curves, Context);
		detail::load_array(*container, "trim_curve_counts", nurbs_patches->trim_curve_counts, Context);
		detail::load_array(*container, "trim_curve_loop_selection", nurbs_patches->trim_curve_loop_selection, Context);
		detail::load_array(*container, "trim_curve_first_points", nurbs_patches->trim_curve_first_points, Context);
		detail::load_array(*container, "trim_curve_point_counts", nurbs_patches->trim_curve_point_counts, Context);
		detail::load_array(*container, "trim_curve_orders", nurbs_patches->trim_curve_orders, Context);
		detail::load_array(*container, "trim_curve_first_knots", nurbs_patches->trim_curve_first_knots, Context);
		detail::load_array(*container, "trim_curve_selection", nurbs_patches->trim_curve_selection, Context);
		detail::load_array(*container, "trim_curve_points", nurbs_patches->trim_curve_points, Context);
		detail::load_array(*container, "trim_curve_point_weights", nurbs_patches->trim_curve_point_weights, Context);
		detail::load_array(*container, "trim_curve_knots", nurbs_patches->trim_curve_knots, Context);
*/
	}

	if(element* const container = find_element(Container, "polyhedra"))
	{
		mesh::polyhedra_t* const polyhedra = &Mesh.polyhedra.create();
		detail::load_array(*container, "first_faces", polyhedra->first_faces, Context);
		detail::load_array(*container, "face_counts", polyhedra->face_counts, Context);
		detail::load_array(*container, "types", polyhedra->types, Context);
		detail::load_arrays(*container, "constant_data", polyhedra->constant_data, Context);
		detail::load_array(*container, "face_first_loops", polyhedra->face_first_loops, Context);
		detail::load_array(*container, "face_loop_counts", polyhedra->face_loop_counts, Context);
		detail::load_array(*container, "face_selection", polyhedra->face_selection, Context);
		detail::load_array(*container, "face_materials", polyhedra->face_materials, Context);
		detail::load_arrays(*container, "uniform_data", polyhedra->uniform_data, Context);
		detail::load_array(*container, "loop_first_edges", polyhedra->loop_first_edges, Context);
		detail::load_array(*container, "edge_points", polyhedra->edge_points, Context);
		detail::load_array(*container, "clockwise_edges", polyhedra->clockwise_edges, Context);
		detail::load_array(*container, "edge_selection", polyhedra->edge_selection, Context);
		detail::load_arrays(*container, "face_varying_data", polyhedra->face_varying_data, Context);
	}

/*
	if(element* const container = find_element(Container, "blobbies"))
	{
		mesh::blobbies_t* const blobbies = &Mesh.blobbies.create();
		detail::load_array(*container, "first_primitives", blobbies->first_primitives, Context);
		detail::load_array(*container, "primitive_counts", blobbies->primitive_counts, Context);
		detail::load_array(*container, "first_operators", blobbies->first_operators, Context);
		detail::load_array(*container, "operator_counts", blobbies->operator_counts, Context);
		detail::load_array(*container, "materials", blobbies->materials, Context);
		detail::load_arrays(*container, "constant_data", blobbies->constant_data, Context);
		detail::load_arrays(*container, "uniform_data", blobbies->uniform_data, Context);
		detail::load_array(*container, "primitives", blobbies->primitives, Context);
		detail::load_array(*container, "primitive_first_floats", blobbies->primitive_first_floats, Context);
		detail::load_array(*container, "primitive_float_counts", blobbies->primitive_float_counts, Context);
		detail::load_arrays(*container, "varying_data", blobbies->varying_data, Context);
		detail::load_arrays(*container, "vertex_data", blobbies->vertex_data, Context);
		detail::load_array(*container, "operators", blobbies->operators, Context);
		detail::load_array(*container, "operator_first_operands", blobbies->operator_first_operands, Context);
		detail::load_array(*container, "operator_operand_counts", blobbies->operator_operand_counts, Context);
		detail::load_array(*container, "floats", blobbies->floats, Context);
		detail::load_array(*container, "operands", blobbies->operands, Context);
	}
*/
}

////////////////////////////////////////////////////
// load_legacy_mesh and associated detail namespace

namespace detail
{

template<typename data_t>
bool load_parameter(const string_t& XMLType, const string_t& Name, const string_t& Type, const string_t& Value, legacy::parameters_t& Parameters)
{
	if(XMLType != Type)
		return false;

	Parameters[Name] = from_string<data_t>(Value, data_t());
	return true;
}
	
void load_parameters(const element& Element, legacy::parameters_t& Parameters)
{
	for(element::elements_t::const_iterator xml_parameter = Element.children.begin(); xml_parameter != Element.children.end(); ++xml_parameter)
	{
		if(xml_parameter->name != "parameter")
			continue;

		const string_t name = attribute_text(*xml_parameter, "name");
		if(name.empty())
		{
			log() << error << k3d_file_reference << " unnamed parameter will not be loaded" << std::endl;
			continue;
		}

		const string_t type = attribute_text(*xml_parameter, "type");
		if(type.empty())
		{
			log() << error << k3d_file_reference << " parameter [" << name << "] with unknown type will not be loaded" << std::endl;
			continue;
		}

		const string_t value = attribute_text(*xml_parameter, "value");

		if(load_parameter<ri::integer>("integer", name, type, value, Parameters)) continue;
		if(load_parameter<ri::real>("real", name, type, value, Parameters)) continue;
		if(load_parameter<ri::string>("string", name, type, value, Parameters)) continue;
		if(load_parameter<ri::point>("point", name, type, value, Parameters)) continue;
		if(load_parameter<ri::vector>("vector", name, type, value, Parameters)) continue;
//		if(load_parameter<ri::normal>("normal", name, type, value, Parameters)) continue;
		if(load_parameter<ri::color>("color", name, type, value, Parameters)) continue;
		if(load_parameter<ri::hpoint>("hpoint", name, type, value, Parameters)) continue;
		if(load_parameter<bool>("bool", name, type, value, Parameters)) continue;
//		if(load_parameter<ri::matrix>("matrix", name, type, value, Parameters)) continue;

		log() << error << k3d_file_reference << " parameter [" << name << "] with unknown type [" << type << "] will not be loaded" << std::endl;
	}
}

void load_parameters(const element& Element, const string_t& StorageClass, legacy::parameters_t& Parameters)
{
	for(element::elements_t::const_iterator xml_parameters = Element.children.begin(); xml_parameters != Element.children.end(); ++xml_parameters)
	{
		if(xml_parameters->name != "parameters")
			continue;

		string_t storage_class = attribute_text(*xml_parameters, "storageclass");
		if(storage_class.empty())
			storage_class = attribute_text(*xml_parameters, "type");
		if(storage_class != StorageClass)
			continue;

		load_parameters(*xml_parameters, Parameters);
	}
}

void load_parameters(const element& Element, const ri::storage_class_t StorageClass, legacy::parameters_t& Parameters)
{
	load_parameters(Element, string_cast(StorageClass), Parameters);
}

void load_varying_parameters(const element& Element, boost::array<legacy::parameters_t, 4>& Parameters)
{
	for(element::elements_t::const_iterator xml_parameters = Element.children.begin(); xml_parameters != Element.children.end(); ++xml_parameters)
	{
		if(xml_parameters->name != "parameters")
			continue;

		string_t storage_class = attribute_text(*xml_parameters, "storageclass");
		if(storage_class.empty())
			storage_class = attribute_text(*xml_parameters, "type");
		string_t keyword("varying");
		if(storage_class != string_cast(keyword))
			continue;

		if(xml_parameters->children.size() != 4)
		{
			log() << error << k3d_file_reference << " varying parameters with incorrect child count will not be loaded" << std::endl;
			continue;
		}

		load_parameters(xml_parameters->children[0], Parameters[0]);
		load_parameters(xml_parameters->children[1], Parameters[1]);
		load_parameters(xml_parameters->children[2], Parameters[2]);
		load_parameters(xml_parameters->children[3], Parameters[3]);
	}
}

template<typename ContainerT>
void load_varying_parameters(const element& Element, ContainerT& Parameters, const unsigned long VaryingCount)
{
	for(element::elements_t::const_iterator xml_parameters = Element.children.begin(); xml_parameters != Element.children.end(); ++xml_parameters)
	{
		if(xml_parameters->name != "parameters")
			continue;

		string_t storage_class = attribute_text(*xml_parameters, "storageclass");
		if(storage_class.empty())
			storage_class = attribute_text(*xml_parameters, "type");
		string_t keyword("varying");
		if(storage_class != string_cast(keyword))
			continue;

		if(xml_parameters->children.size() != VaryingCount)
		{
			log() << error << k3d_file_reference << " varying parameters require " << VaryingCount << " values, found " << xml_parameters->children.size() << ", will not be loaded" << std::endl;
			continue;
		}

		for(element::elements_t::const_iterator xml_parameters2 = xml_parameters->children.begin(); xml_parameters2 != xml_parameters->children.end(); ++xml_parameters2)
		{
			Parameters.push_back(legacy::parameters_t());
			load_parameters(*xml_parameters2, Parameters.back());
		}
	}
}

void load_tags(const element& Element, legacy::parameters_t& Parameters)
{
	load_parameters(Element, "tag", Parameters);
}

} // namespace detail

void load(legacy::mesh& Mesh, element& XML, const ipersistent::load_context& Context)
{
	// Load points ...
	if(element* const xml_points = find_element(XML, "points"))
	{
		for(element::elements_t::const_iterator xml_point = xml_points->children.begin(); xml_point != xml_points->children.end(); ++xml_point)
		{
			if(xml_point->name != "point")
				continue;

			Mesh.points.push_back(new legacy::point(attribute_value<point3>(*xml_point, "position", point3(0, 0, 0))));
			detail::load_parameters(*xml_point, ri::VERTEX, Mesh.points.back()->vertex_data);
			detail::load_tags(*xml_point, Mesh.points.back()->tags);
		}
	}

	// Load point groups ...
	if(element* const xml_point_groups = find_element(XML, "pointgroups"))
	{
		assert_not_implemented();
/*
		for(element::elements_t::iterator xml_group = xml_point_groups->children.begin(); xml_group != xml_point_groups->children.end(); ++xml_group)
		{
			if(xml_group->name != "group")
				continue;

			legacy::point_group* const group = new legacy::point_group();
			group->material = dynamic_cast<imaterial*>(Context.lookup.lookup_object(attribute_value<ipersistent_lookup::id_type>(*xml_group, "material", 0)));
			Mesh.point_groups.push_back(group);

			element* const xml_points = find_element(*xml_group, "points");
			if(xml_points)
			{
				std::istringstream points_buffer(xml_points->text);
				for(std::istream_iterator<unsigned long> point(points_buffer); point != std::istream_iterator<unsigned long>(); ++point)
				{
					const unsigned long point_index = *point - 1;
					return_if_fail(point_index < Mesh.points.size());
					group->points.push_back(Mesh.points[point_index]);
				}
			}

			detail::load_parameters(*xml_group, ri::CONSTANT, group->constant_data);
		}
*/
	}

	// Load polyhedra ...
	if(element* const xml_polyhedra = find_element(XML, "polyhedra"))
	{
		for(element::elements_t::iterator xml_polyhedron = xml_polyhedra->children.begin(); xml_polyhedron != xml_polyhedra->children.end(); ++xml_polyhedron)
		{
			if(xml_polyhedron->name != "polyhedron")
				continue;

			k3d::legacy::polyhedron* const polyhedron = new k3d::legacy::polyhedron();

			polyhedron->type = attribute_value(*xml_polyhedron, "type", legacy::polyhedron::POLYGONS);
			Mesh.polyhedra.push_back(polyhedron);

			detail::load_tags(*xml_polyhedron, polyhedron->tags);

			// Load edges ...
			typedef std::vector<legacy::split_edge*> edges_t;
			edges_t edges;
			if(element* const xml_edges = find_element(*xml_polyhedron, "edges"))
			{
				//const unsigned long edge_count = std::count_if(xml_edges->children.begin(), xml_edges->children.end(), same_name("edge"));
				// Above statement often crashes, count eges the old fashioned way
				unsigned long edge_count = 0;
				for(element::elements_t::const_iterator xe = xml_edges->children.begin(); xe != xml_edges->children.end(); ++xe)
				{
					if((*xe).name == "edge")
						++edge_count;
				}

				edges.resize(edge_count);
				for(edges_t::iterator edge = edges.begin(); edge != edges.end(); ++edge)
					*edge = new legacy::split_edge(0, 0, 0);

				edges_t::iterator edge = edges.begin();
				for(element::elements_t::iterator xml_edge = xml_edges->children.begin(); xml_edge != xml_edges->children.end(); ++xml_edge)
				{
					if(xml_edge->name != "edge")
						continue;

					unsigned long vertex_index = attribute_value<unsigned long>(*xml_edge, "vertex", 0);
					return_if_fail(vertex_index <= Mesh.points.size());

					unsigned long face_clockwise_index = attribute_value<unsigned long>(*xml_edge, "faceclockwise", 0);
					return_if_fail(face_clockwise_index <= edges.size());

					unsigned long companion_index = attribute_value<unsigned long>(*xml_edge, "companion", 0);
					return_if_fail(companion_index <= edges.size());

					if(vertex_index)
						(*edge)->vertex = Mesh.points[vertex_index-1];

					if(face_clockwise_index)
						(*edge)->face_clockwise = edges[face_clockwise_index-1];

					if(companion_index)
						(*edge)->companion = edges[companion_index-1];

					detail::load_parameters(*xml_edge, ri::FACEVARYING, (*edge)->facevarying_data);
					detail::load_tags(*xml_edge, (*edge)->tags);

					++edge;
				}
			}

			// Load faces ...
			if(element* const xml_faces = find_element(*xml_polyhedron, "faces"))
			{
				for(element::elements_t::iterator xml_face = xml_faces->children.begin(); xml_face != xml_faces->children.end(); ++xml_face)
				{
					if(xml_face->name != "face")
						continue;

					unsigned long first_edge_index = attribute_value<unsigned long>(*xml_face, "firstedge", 0);
					return_if_fail(first_edge_index);
					return_if_fail(first_edge_index <= edges.size());

					k3d::legacy::face* const face = new k3d::legacy::face(
						first_edge_index ? edges[first_edge_index-1] : 0,
						dynamic_cast<imaterial*>(Context.lookup.lookup_object(attribute_value<ipersistent_lookup::id_type>(*xml_face, "material", 0))));
					polyhedron->faces.push_back(face);

					detail::load_parameters(*xml_face, ri::UNIFORM, face->uniform_data);
					detail::load_tags(*xml_face, face->tags);

					if(element* const xml_holes = find_element(*xml_face, "holes"))
					{
						for(element::elements_t::iterator xml_hole = xml_holes->children.begin(); xml_hole != xml_holes->children.end(); ++xml_hole)
						{
							if(xml_hole->name != "hole")
								continue;

							unsigned long first_edge_index = attribute_value<unsigned long>(*xml_hole, "firstedge", 0);
							return_if_fail(first_edge_index);
							return_if_fail(first_edge_index <= edges.size());

							face->holes.push_back(first_edge_index ? edges[first_edge_index-1] : 0);
						}
					}
				}
			}
		}
	}

	// Load linear curve groups ...
	if(element* const xml_linear_curve_groups = find_element(XML, "linearcurvegroups"))
	{
		for(element::elements_t::iterator xml_group = xml_linear_curve_groups->children.begin(); xml_group != xml_linear_curve_groups->children.end(); ++xml_group)
		{
			if(xml_group->name != "group")
				continue;

			legacy::linear_curve_group* const group = new legacy::linear_curve_group();
			group->wrap = attribute_value<bool>(*xml_group, "wrap", false);
			group->material = dynamic_cast<imaterial*>(Context.lookup.lookup_object(attribute_value<ipersistent_lookup::id_type>(*xml_group, "material", 0)));
			detail::load_parameters(*xml_group, ri::CONSTANT, group->constant_data);
			Mesh.linear_curve_groups.push_back(group);

			element* const xml_curves = find_element(*xml_group, "curves");
			if(!xml_curves)
				continue;

			for(element::elements_t::iterator xml_curve = xml_curves->children.begin(); xml_curve != xml_curves->children.end(); ++xml_curve)
			{
				if(xml_curve->name != "curve")
					continue;

				legacy::linear_curve* const curve = new legacy::linear_curve();
				detail::load_parameters(*xml_curve, ri::UNIFORM, curve->uniform_data);
				group->curves.push_back(curve);

				element* const xml_control_points = find_element(*xml_curve, "controlpoints");
				if(!xml_control_points)
					continue;

				std::istringstream points_buffer(xml_control_points->text);
				for(std::istream_iterator<unsigned long> control_point(points_buffer); control_point != std::istream_iterator<unsigned long>(); ++control_point)
				{
					const unsigned long control_point_index = *control_point - 1;
					return_if_fail(control_point_index < Mesh.points.size());
					curve->control_points.push_back(Mesh.points[control_point_index]);
				}
				detail::load_varying_parameters(*xml_curve, curve->varying_data, varying_count(*curve, group->wrap));
			}
		}
	}

	// Load cubic curve groups ...
	if(element* const xml_cubic_curve_groups = find_element(XML, "cubiccurvegroups"))
	{
		for(element::elements_t::iterator xml_group = xml_cubic_curve_groups->children.begin(); xml_group != xml_cubic_curve_groups->children.end(); ++xml_group)
		{
			if(xml_group->name != "group")
				continue;

			legacy::cubic_curve_group* const group = new legacy::cubic_curve_group();
			group->wrap = attribute_value<bool>(*xml_group, "wrap", false);
			group->material = dynamic_cast<imaterial*>(Context.lookup.lookup_object(attribute_value<ipersistent_lookup::id_type>(*xml_group, "material", 0)));
			detail::load_parameters(*xml_group, ri::CONSTANT, group->constant_data);
			Mesh.cubic_curve_groups.push_back(group);

			element* const xml_curves = find_element(*xml_group, "curves");
			if(!xml_curves)
				continue;

			for(element::elements_t::iterator xml_curve = xml_curves->children.begin(); xml_curve != xml_curves->children.end(); ++xml_curve)
			{
				if(xml_curve->name != "curve")
					continue;

				legacy::cubic_curve* const curve = new legacy::cubic_curve();
				detail::load_parameters(*xml_curve, ri::UNIFORM, curve->uniform_data);
				group->curves.push_back(curve);

				element* const xml_control_points = find_element(*xml_curve, "controlpoints");
				if(!xml_control_points)
					continue;

				std::istringstream points_buffer(xml_control_points->text);
				for(std::istream_iterator<unsigned long> control_point(points_buffer); control_point != std::istream_iterator<unsigned long>(); ++control_point)
				{
					const unsigned long control_point_index = *control_point - 1;
					return_if_fail(control_point_index < Mesh.points.size());
					curve->control_points.push_back(Mesh.points[control_point_index]);
				}

				detail::load_varying_parameters(*xml_curve, curve->varying_data, varying_count(*curve, group->wrap));
			}
		}
	}

	// Load NURBS curve groups ...
	if(element* const xml_nucurve_groups = find_element(XML, "nucurvegroups"))
	{
		for(element::elements_t::iterator xml_group = xml_nucurve_groups->children.begin(); xml_group != xml_nucurve_groups->children.end(); ++xml_group)
		{
			if(xml_group->name != "group")
				continue;

			legacy::nucurve_group* const group = new legacy::nucurve_group();
			group->material = dynamic_cast<imaterial*>(Context.lookup.lookup_object(attribute_value<ipersistent_lookup::id_type>(*xml_group, "material", 0)));
			Mesh.nucurve_groups.push_back(group);

			element* const xml_curves = find_element(*xml_group, "curves");
			if(!xml_curves)
				continue;

			for(element::elements_t::iterator xml_curve = xml_curves->children.begin(); xml_curve != xml_curves->children.end(); ++xml_curve)
			{
				if(xml_curve->name != "curve")
					continue;

				element* const xml_knot_vector = find_element(*xml_curve, "knotvector");
				element* const xml_control_points = find_element(*xml_curve, "controlpoints");
				element* const xml_weights = find_element(*xml_curve, "weights");

				if(xml_knot_vector && xml_control_points && xml_weights)
				{
					legacy::nucurve* const curve = new legacy::nucurve();
					curve->order = attribute_value<unsigned long>(*xml_curve, "order", 0);
					group->curves.push_back(curve);

					std::istringstream knots_buffer(xml_knot_vector->text);
					std::copy(std::istream_iterator<double>(knots_buffer), std::istream_iterator<double>(), std::back_inserter(curve->knots));

					std::istringstream points_buffer(xml_control_points->text);
					std::istringstream weights_buffer(xml_weights->text);

					std::istream_iterator<unsigned long> control_point(points_buffer);
					std::istream_iterator<double> weight(weights_buffer);
					for(; control_point != std::istream_iterator<unsigned long>() && weight != std::istream_iterator<double>(); ++control_point, ++weight)
					{
						const unsigned long control_point_index = *control_point - 1;
						return_if_fail(control_point_index < Mesh.points.size());
						curve->control_points.push_back(legacy::nucurve::control_point(Mesh.points[control_point_index], *weight));
					}
				}
			}
		}
	}

	// Load bilinear patches ...
	if(element* const xml_bilinear_patches = find_element(XML, "bilinearpatches"))
	{
		for(element::elements_t::iterator xml_patch = xml_bilinear_patches->children.begin(); xml_patch != xml_bilinear_patches->children.end(); ++xml_patch)
		{
			if(xml_patch->name != "patch")
				continue;

			legacy::bilinear_patch* const patch = new legacy::bilinear_patch();
			patch->material = dynamic_cast<imaterial*>(Context.lookup.lookup_object(attribute_value<ipersistent_lookup::id_type>(*xml_patch, "material", 0)));
			Mesh.bilinear_patches.push_back(patch);

			element* const xml_control_points = find_element(*xml_patch, "controlpoints");
			if(!xml_control_points)
				continue;

			unsigned long storage_index = 0;
			std::istringstream points_buffer(xml_control_points->text);
			for(std::istream_iterator<unsigned long> control_point(points_buffer); control_point != std::istream_iterator<unsigned long>(); ++control_point)
			{
				return_if_fail(storage_index < 4);

				const unsigned long control_point_index = *control_point - 1;
				return_if_fail(control_point_index < Mesh.points.size());
				patch->control_points[storage_index++] = Mesh.points[control_point_index];
			}

			detail::load_parameters(*xml_patch, ri::UNIFORM, patch->uniform_data);
			detail::load_varying_parameters(*xml_patch, patch->varying_data);
		}
	}

	// Load bicubic patches ...
	if(element* const xml_bicubic_patches = find_element(XML, "bicubicpatches"))
	{
		for(element::elements_t::iterator xml_patch = xml_bicubic_patches->children.begin(); xml_patch != xml_bicubic_patches->children.end(); ++xml_patch)
		{
			if(xml_patch->name != "patch")
				continue;

			legacy::bicubic_patch* const patch = new legacy::bicubic_patch();
			patch->material = dynamic_cast<imaterial*>(Context.lookup.lookup_object(attribute_value<ipersistent_lookup::id_type>(*xml_patch, "material", 0)));
			Mesh.bicubic_patches.push_back(patch);

			element* const xml_control_points = find_element(*xml_patch, "controlpoints");
			if(!xml_control_points)
				continue;

			unsigned long storage_index = 0;
			std::istringstream points_buffer(xml_control_points->text);
			for(std::istream_iterator<unsigned long> control_point(points_buffer); control_point != std::istream_iterator<unsigned long>(); ++control_point)
			{
				return_if_fail(storage_index < 16);

				const unsigned long control_point_index = *control_point - 1;
				return_if_fail(control_point_index < Mesh.points.size());
				patch->control_points[storage_index++] = Mesh.points[control_point_index];
			}

			detail::load_parameters(*xml_patch, ri::UNIFORM, patch->uniform_data);
			detail::load_varying_parameters(*xml_patch, patch->varying_data);
		}
	}

	// Load NURBS patches ...
	if(element* const xml_nupatches = find_element(XML, "nupatches"))
	{
		for(element::elements_t::iterator xml_patch = xml_nupatches->children.begin(); xml_patch != xml_nupatches->children.end(); ++xml_patch)
		{
			if(xml_patch->name != "patch")
				continue;

			element* const xml_u_knot_vector = find_element(*xml_patch, "uknotvector");
			element* const xml_v_knot_vector = find_element(*xml_patch, "vknotvector");
			element* const xml_control_points = find_element(*xml_patch, "controlpoints");
			element* const xml_weights = find_element(*xml_patch, "weights");

			if(xml_u_knot_vector && xml_v_knot_vector && xml_control_points && xml_weights)
			{
				legacy::nupatch* const patch = new legacy::nupatch();
				patch->u_order = attribute_value<unsigned long>(*xml_patch, "uorder", 0);
				patch->v_order = attribute_value<unsigned long>(*xml_patch, "vorder", 0);
				patch->material = dynamic_cast<imaterial*>(Context.lookup.lookup_object(attribute_value<ipersistent_lookup::id_type>(*xml_patch, "material", 0)));
				Mesh.nupatches.push_back(patch);

				std::istringstream u_knots_buffer(xml_u_knot_vector->text);
				std::copy(std::istream_iterator<double>(u_knots_buffer), std::istream_iterator<double>(), std::back_inserter(patch->u_knots));

				std::istringstream v_knots_buffer(xml_v_knot_vector->text);
				std::copy(std::istream_iterator<double>(v_knots_buffer), std::istream_iterator<double>(), std::back_inserter(patch->v_knots));

				std::istringstream points_buffer(xml_control_points->text);
				std::istringstream weights_buffer(xml_weights->text);

				std::istream_iterator<unsigned long> control_point(points_buffer);
				std::istream_iterator<double> weight(weights_buffer);
				for(; control_point != std::istream_iterator<unsigned long>() && weight != std::istream_iterator<double>(); ++control_point, ++weight)
				{
					const unsigned long control_point_index = *control_point - 1;
					return_if_fail(control_point_index < Mesh.points.size());
					patch->control_points.push_back(legacy::nupatch::control_point(Mesh.points[control_point_index], *weight));
				}
			}
		}
	}
}

// end of legacy stuff
///////////////////////////////////////////////////////////////////////////////////

void save(const mesh_selection& Selection, element& XML, const ipersistent::save_context& Context)
{
	detail::save_selection(XML, Selection.points, "points");
	detail::save_selection(XML, Selection.edges, "edges");
	detail::save_selection(XML, Selection.faces, "faces");

	// Handle generic primitives
	assert_not_implemented();
}

void load(mesh_selection& Selection, element& XML, const ipersistent::load_context& Context)
{
	for(element::elements_t::const_iterator xml_selection = XML.children.begin(); xml_selection != XML.children.end(); ++xml_selection)
	{
		if(xml_selection->name == "points")
			detail::load_selection(*xml_selection, Selection.points);
		if(xml_selection->name == "edges")
			detail::load_selection(*xml_selection, Selection.edges);
		if(xml_selection->name == "faces")
			detail::load_selection(*xml_selection, Selection.faces);
	}

	// Handle generic primitives
	assert_not_implemented();
}

} // namespace xml

} // namespace k3d

