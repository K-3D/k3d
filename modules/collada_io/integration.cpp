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
#include "integration.h"
#include "intGeometry.h"
using namespace std;
namespace module
{

namespace collada
{

namespace io
{

Node::Node(domNode& node) {
	// Recursively convert all child nodes. First iterate over the <node> elements.
		
	for (size_t i = 0; i < node.getNode_array().getCount(); i++)
		childNodes.push_back(&lookup<Node, domNode>(*node.getNode_array()[i]));

	// Then iterate over the <instance_node> elements.
	for (size_t i = 0; i < node.getInstance_node_array().getCount(); i++) {
		domNode* child = daeSafeCast<domNode>(
			node.getInstance_node_array()[i]->getUrl().getElement());
			if(!child){
			     k3d::log() << "Invalid <instance_node> element\n";
		             return;
			}
			childNodes.push_back(&lookup<Node, domNode>(*child));
	}

	// Iterate over all the <instance_geometry> elements
	for (size_t i = 0; i < node.getInstance_geometry_array().getCount(); i++) {
		domInstance_geometry* instanceGeom = node.getInstance_geometry_array()[i];
		domGeometry* geom = daeSafeCast<domGeometry>(instanceGeom->getUrl().getElement());
		if(!geom){
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
		meshes.push_back(&lookup<intGeometry, domGeometry>(*geom));
		//meshes.back()->mtl = &convertedMtl;
	}
	
}


void convertModel(domCOLLADA& root, k3d::mesh& Mesh) {
	// We need to convert the model from the DOM's representation to our internal representation.
	// First find a <visual_scene> to load. In a real app we would look for and load all
	// the <visual_scene>s in a document, but for this app we just convert the first
	// <visual_scene> we find.
	domVisual_scene* visualScene = daeSafeCast<domVisual_scene>(root.getDescendant("visual_scene"));
	if(!visualScene){
	     k3d::log() << "Invalid <visual_scene> element\n";
	     return;
	}
	// Now covert all the <node>s in the <visual_scene>. This is a recursive process,
	// so any child nodes will also be converted.
	domNode_Array& nodes = visualScene->getNode_array();
	for (size_t i = 0; i < nodes.getCount(); i++)
		lookup<Node, domNode>(*nodes[i]);
		
	
	for (size_t i = 0; i < nodes.getCount(); i++)
		if(!lookup<Node, domNode>(*nodes[i]).meshes.empty()){	
			Mesh = lookup<Node, domNode>(*nodes[i]).meshes.back()->getMesh();		
			return;
		}
	
}

}}}
