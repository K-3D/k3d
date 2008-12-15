// K-3D
// Copyright (c) 1995-2007, Timothy M. Shead
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
	\author Carlos Andres Dominguez Caballero (carlosadc@gmail.com)
	\author Barbiero Mattia
*/

#include <list>
#include <vector>
#include <iostream>
#include <dae.h>
#include <dom/domMaterial.h>
#include <dom/domGeometry.h>
#include <dom/domNode.h>
#include <dom/domCOLLADA.h>
#include <k3dsdk/algebra.h>
#include <k3dsdk/plugins.h>
#include <k3dsdk/idocument_plugin_factory.h>

#include <k3dsdk/imesh_sink.h>
#include <k3dsdk/imesh_source.h>
#include <k3dsdk/imesh_storage.h>
#include <k3dsdk/ipipeline.h>
#include <k3dsdk/properties.h>
#include <k3dsdk/inode_collection_sink.h>
#include <k3dsdk/classes.h>

#include <k3dsdk/icamera.h>

#include "integration.h"
#include "int_elements.h"
using namespace std;
namespace module
{

namespace collada
{

namespace io
{

/// Creates a new FrozenMesh using the given name, and returns a pointer to the MeshInstance of the new node
k3d::inode* create_frozen_mesh(k3d::idocument& Document, const std::string& Name, k3d::mesh* Mesh)
{
	std::string unique_name = k3d::unique_name(Document.nodes(), Name);
	// The frozen mesh
	k3d::inode* const mesh_node = k3d::plugin::create<k3d::inode>(k3d::classes::FrozenMesh(), Document, unique_name);
	// The mesh instance
	k3d::inode* const mesh_instance = k3d::plugin::create<k3d::inode>(k3d::classes::MeshInstance(), Document, unique_name + " Instance");
	// Set painters
	const k3d::nodes_t gl_nodes = k3d::find_nodes(Document.nodes(), "GL Default Painter");
	k3d::inode* gl_painter = (1 == gl_nodes.size()) ? *gl_nodes.begin() : 0;
	const k3d::nodes_t ri_nodes = k3d::find_nodes(Document.nodes(), "RenderMan Default Painter");
	k3d::inode* ri_painter = (1 == ri_nodes.size()) ? *ri_nodes.begin() : 0;
	k3d::property::set_internal_value(*mesh_instance, "gl_painter", gl_painter);
	k3d::property::set_internal_value(*mesh_instance, "ri_painter", ri_painter);
	// Connect the MeshInstance
	k3d::ipipeline::dependencies_t dependencies;
	k3d::imesh_sink* const mesh_sink = dynamic_cast<k3d::imesh_sink*>(mesh_instance);
	k3d::imesh_source* const mesh_source = dynamic_cast<k3d::imesh_source*>(mesh_node);
	if(mesh_sink && mesh_node)
		dependencies.insert(std::make_pair(&mesh_sink->mesh_sink_input(), &mesh_source->mesh_source_output()));
	Document.pipeline().set_dependencies(dependencies);
	k3d::imesh_storage* mesh_storage = dynamic_cast<k3d::imesh_storage*>(mesh_node);
	return_val_if_fail(mesh_storage, 0);
	mesh_storage->reset_mesh(Mesh);
	
	// Make the mesh_instance visible (shamelessly stolen from document_state.cpp)
	const k3d::inode_collection::nodes_t::const_iterator doc_node_end = Document.nodes().collection().end();
	for(k3d::inode_collection::nodes_t::const_iterator doc_node = Document.nodes().collection().begin(); doc_node != doc_node_end; ++doc_node)
	{
		if(k3d::inode_collection_sink* const node_collection_sink = dynamic_cast<k3d::inode_collection_sink*>(*doc_node))
		{
			const k3d::inode_collection_sink::properties_t properties = node_collection_sink->node_collection_properties();
			for(k3d::inode_collection_sink::properties_t::const_iterator property = properties.begin(); property != properties.end(); ++property)
			{
				if(k3d::inode_collection_property* const node_collection_property = dynamic_cast<k3d::inode_collection_property*>(*property))
				{
					k3d::inode_collection_property::nodes_t nodes = k3d::property::internal_value<k3d::inode_collection_property::nodes_t>(**property);
					if(node_collection_property->property_allow(*mesh_instance))
						nodes.push_back(mesh_instance);
					k3d::property::set_internal_value(**property, nodes);
				}
			}
		}
	}
	
	return mesh_instance;
}

/// Creates a new Light using the given name, and returns a pointer to the Node of the new node
k3d::inode* create_light(k3d::idocument& Document, const std::string& Name)
{
	std::string unique_name = k3d::unique_name(Document.nodes(), Name);
	k3d::inode* const light_node = k3d::plugin::create<k3d::inode>(k3d::uuid(1,0,0,29), Document, unique_name);
	return light_node;
}

/// Creates a new Camera using the given name, and returns a pointer to the Node of the new node
k3d::inode* create_camera(k3d::idocument& Document, const std::string& Name)
{
	std::string unique_name = k3d::unique_name(Document.nodes(), Name);
	k3d::inode* const camera_node = k3d::plugin::create<k3d::inode>(k3d::classes::Camera(), Document, unique_name);
	return camera_node;
}

Node::Node(domNode& node, const k3d::matrix4& mat)
{
	// Extract and compute transformation matrix
	k3d::matrix4 mcurrent = getTransformation(node);
	

	// Recursively convert all child nodes. First iterate over the <node> elements.
	for (size_t i = 0; i < node.getNode_array().getCount(); i++)
		childNodes.push_back(&lookup<Node, domNode>(*node.getNode_array()[i],mcurrent));

	// Then iterate over the <instance_node> elements.
	for (size_t i = 0; i < node.getInstance_node_array().getCount(); i++) 
	{
		domNode* child = daeSafeCast<domNode>(node.getInstance_node_array()[i]->getUrl().getElement());
		if(!child)
		{
			k3d::log() << "Invalid <instance_node> element\n";
			return;
		}
		childNodes.push_back(&lookup<Node, domNode>(*child,mcurrent));
	}

	// Iterate over all the <instance_geometry> elements
	for (size_t i = 0; i < node.getInstance_geometry_array().getCount(); i++) 
	{
		domInstance_geometry* instanceGeom = node.getInstance_geometry_array()[i];
		domGeometry* geom = daeSafeCast<domGeometry>(instanceGeom->getUrl().getElement());
		if(!geom)
		{
			k3d::log() << "Invalid <instance_geometry> element\n";
			return;
		}
		// Lookup the material that we should apply to the <geometry>. In a real app
		// we'd need to worry about having multiple <instance_material>s, but in this
		// test let's just convert the first <instance_material> we find.
		//domInstance_material* instanceMtl = daeSafeCast<domInstance_material>(
			//instanceGeom->getDescendant("instance_material"));
		//Check(instanceMtl);
		//domMaterial* mtl = daeSafeCast<domMaterial>(instanceMtl->getTarget().getElement());
		//Check(mtl);
		//Material& convertedMtl = lookup<Material, domMaterial>(*mtl);

		// Now convert the geometry, add the result to our list of meshes, and assign
		// the mesh a material.
		meshes.push_back(&lookup<intGeometry, domGeometry>(*geom,mcurrent));
		//meshes.back()->mtl = &convertedMtl;
	}

	// Iterate over all the <instance_light> elements
	for(size_t i=0; i < node.getInstance_light_array().getCount(); i++)
	{
		domInstance_light* instanceLight = node.getInstance_light_array()[i];
		domLight* light = daeSafeCast<domLight>(instanceLight->getUrl().getElement());
		if(!light)
		{
			k3d::log() << "Invalid <instance_light> element\n";
			return;
		}
		k3d::log() << debug << "A light" << std::endl;
		lights.push_back(&lookup<intLight, domLight>(*light, mcurrent));
	}

	// Iterate over all the <instance_camera> elements
	for(size_t i=0; i < node.getInstance_camera_array().getCount(); i++)
	{
		domInstance_camera* instanceCamera = node.getInstance_camera_array()[i];
		domCamera* camera = daeSafeCast<domCamera>(instanceCamera->getUrl().getElement());
		if(!camera)
		{
			k3d::log() << "Invalid <instance_camera> element\n";
			return;
		}
		k3d::log() << debug << "A camera" << std::endl;
		cameras.push_back(&lookup<intCamera, domCamera>(*camera, mcurrent));
	}
}

k3d::matrix4 Node::getTransformation(domNode& node)
{
	k3d::matrix4 result = k3d::identity3();

	// Look for Translations
	domTranslate_Array translate_array = node.getTranslate_array();
	for(int i=0; i<translate_array.getCount(); i++)
	{
		domTranslate* translate = translate_array[i];
		domFloat3 trans = translate->getValue();
		k3d::matrix4 tmp_matrix = k3d::identity3();
		tmp_matrix[0][3] = trans[0];
		tmp_matrix[1][3] = trans[1];
		tmp_matrix[2][3] = trans[2];
		result = result * tmp_matrix;
	}

	// Look for Scaling
	domScale_Array scale_array = node.getScale_array();
	for(int i=0; i<scale_array.getCount(); i++)
	{
		domScale* scale = scale_array[i];
		domFloat3 sc = scale->getValue();
		k3d::matrix4 tmp_matrix = k3d::identity3();
		tmp_matrix[0][0] = sc[0];
		tmp_matrix[1][1] = sc[1];
		tmp_matrix[2][2] = sc[2];
		result = result * tmp_matrix;
	}

	// Look for Rotations
	domRotate_Array rotate_array = node.getRotate_array();
	for(int i=0; i<rotate_array.getCount(); i++)
	{
		domRotate* rotate = rotate_array[i];
		domFloat4 rot = rotate->getValue();
		k3d::matrix4 tmp_matrix = k3d::identity3();
		float c = cos(k3d::radians(rot[3])), s = sin(k3d::radians(rot[3])), C = 1-c;
		float x = rot[0], y = rot[1], z = rot[2];
		float xs = x*s, ys = y*s, zs = z*s;
		float xC = x*C, yC = y*C, zC = z*C;
		float xyC=x*y*C,yzC=y*z*C,zxC=z*x*C;
		tmp_matrix[0][0] = x*xC+c;
		tmp_matrix[0][1] = xyC-zs;
		tmp_matrix[0][2] = zxC+ys;
		tmp_matrix[1][0] = xyC+zs;
		tmp_matrix[1][1] = y*yC+c;
		tmp_matrix[1][2] = yzC-xs;
		tmp_matrix[2][0] = zxC-ys;
		tmp_matrix[2][1] = yzC+xs;
		tmp_matrix[2][2] = z*zC+c;
		result = result * tmp_matrix;
	}

	return result;
}


//void convertModel(domCOLLADA& root, k3d::mesh& Mesh) 
daeParser::daeParser(domCOLLADA& root, k3d::mesh& Mesh)
{
	// We need to convert the model from the DOM's representation to our internal representation.
	// First find a <visual_scene> to load. In a real app we would look for and load all
	// the <visual_scene>s in a document, but for this app we just convert the first
	// <visual_scene> we find.

	domVisual_scene* visualScene = daeSafeCast<domVisual_scene>(root.getDescendant("visual_scene"));
	if(!visualScene)
	{
		k3d::log() << "Invalid <visual_scene> element\n";
		return;
	}
	// Now covert all the <node>s in the <visual_scene>. This is a recursive process,
	// so any child nodes will also be converted.
	domNode_Array& nodes = visualScene->getNode_array();
	for (size_t i = 0; i < nodes.getCount(); i++)
		lookup<Node, domNode>(*nodes[i],k3d::identity3());
	//k3d::log() << "ASDFASDFASDf" << std::endl;
	
	//for (size_t i = 0; i < nodes.getCount(); i++)
	//	if(!lookup<Node, domNode>(*nodes[i],k3d::identity3()).meshes.empty()){	
	//		Mesh = lookup<Node, domNode>(*nodes[i],k3d::identity3()).meshes.back()->getMesh();
	//		return;
	//	}
	
}

daeParser::daeParser(domCOLLADA& root, k3d::idocument& Document)
{

	// We need to convert the model from the DOM's representation to our internal representation.
	// First find a <visual_scene> to load. In a real app we would look for and load all
	// the <visual_scene>s in a document, but for this app we just convert the first
	// <visual_scene> we find.
	domVisual_scene* visualScene = daeSafeCast<domVisual_scene>(root.getDescendant("visual_scene"));
	if(!visualScene)
	{
	     k3d::log() << "Invalid <visual_scene> element\n";
	     return;
	}

	// Now covert all the <node>s in the <visual_scene>. This is a recursive process,
	// so any child nodes will also be converted.
	domNode_Array& scene_nodes = visualScene->getNode_array();
	for (size_t i = 0; i < scene_nodes.getCount(); i++)
		lookup<Node, domNode>(*scene_nodes[i],k3d::identity3());


	////////////////////////////////////////////////////
	////////////////////////////////////////////////////

	//k3d::inode_collection::nodes_t nodes;
	//std::vector<k3d::inode*> persistent_nodes;

	for(size_t i =0; i < scene_nodes.getCount(); i++)
	{
		std::string name;// = "Light";
		k3d::uuid factory_id;// = k3d::uuid(1,0,0,29);

		if(!lookup<Node, domNode>(*scene_nodes[i],k3d::identity3()).lights.empty())
		{
			k3d::inode *light_node;
			light_node = create_light(Document,"COLLADA Light");
		}
		else
		if(!lookup<Node, domNode>(*scene_nodes[i],k3d::identity3()).cameras.empty())
		{
			k3d::inode *camera_node;
			camera_node = create_camera(Document,"COLLADA Camera");
		}
		else
		if(!lookup<Node, domNode>(*scene_nodes[i],k3d::identity3()).meshes.empty())
		{
			k3d::mesh *mesh = new k3d::mesh();
			create_frozen_mesh(Document,"COLLADA Mesh", mesh);
			*mesh = lookup<Node, domNode>(*scene_nodes[i],k3d::identity3()).meshes.back()->getMesh();
			continue;
		}
		else
			factory_id == k3d::uuid::null();
/*
		if(factory_id == k3d::uuid::null())
		{
			k3d::log() << error << "node [" << name << "] with unspecified factory ID will not be loaded" << std::endl;
			continue;
		}

		const k3d::ipersistent_lookup::id_type node_id = 2;
		if(node_id == 0)
		{
			k3d::log() << error << "node [" << name << "] with unspecified ID will not be loaded" << std::endl;
			continue;
		}
	
		k3d::iplugin_factory* const plugin_factory = k3d::plugin::factory::lookup(factory_id);
		if(!plugin_factory)
		{
			k3d::log() << error << "node [" << name << "] with unknown factory ID [" << factory_id << "] will not be loaded" << std::endl;
			continue;
		}
	
		k3d::idocument_plugin_factory* const document_plugin_factory = dynamic_cast<k3d::idocument_plugin_factory*>(plugin_factory);
		if(!document_plugin_factory)
		{
			k3d::log() << error << "Non-document plugin [" << name << "] will not be loaded" << std::endl;
			continue;
		}
	
		k3d::inode* const node = document_plugin_factory->create_plugin(*plugin_factory, Document);
		if(!node)
		{
			k3d::log() << error << "Error creating node [" << name << "] instance" << std::endl;
			continue;
		}
	
		k3d::ipersistent* const persistent = dynamic_cast<k3d::ipersistent*>(node);
		if(!persistent)
		{
			k3d::log() << error << "node [" << name << "] does not support persistence" << std::endl;
	
			delete node;
			continue;
		}
	
		k3d::undoable_new(node, Document);
	
		nodes.push_back(node);
		persistent_nodes.push_back(node);
*/
	}

	//Document.nodes().add_nodes(nodes);
	////////////////////////////////////////////////////
	////////////////////////////////////////////////////
}

}}}
