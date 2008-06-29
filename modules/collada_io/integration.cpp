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
#include <k3dsdk/algebra.h>
#include "integration.h"
#include "intGeometry.h"
using namespace std;
namespace module
{

namespace collada
{

namespace io
{

Node::Node(domNode& node, const k3d::matrix4& mat)
{
	// Extract and compute current transformation matrix
	k3d::matrix4 mcurrent = getTransformation(node,mat);
	

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
}

k3d::matrix4 Node::getTransformation(domNode& node, const k3d::matrix4& mat)
{
	k3d::matrix4 result = mat;

	// Look for Translations
	domTranslate_Array translate_array = node.getTranslate_array();
	for(int i=0; i<translate_array.getCount(); i++)
	{
		domTranslate* translate = translate_array[i];
		domFloat3 trans = translate->getValue();
		k3d::matrix4 tmp_matrix = k3d::identity3D();
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
		k3d::matrix4 tmp_matrix = k3d::identity3D();
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
		k3d::matrix4 tmp_matrix = k3d::identity3D();
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
daeParser::daeParser(domCOLLADA& root)
{
	// Push an identity matrix to the stack as bottom
	k3d::matrix4 mstack = k3d::identity3D();

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
		lookup<Node, domNode>(*nodes[i],k3d::identity3D());
		
	
	for (size_t i = 0; i < nodes.getCount(); i++)
		if(!lookup<Node, domNode>(*nodes[i],k3d::identity3D()).meshes.empty()){	
			Mesh = lookup<Node, domNode>(*nodes[i],k3d::identity3D()).meshes.back()->getMesh();		
			return;
		}
	
}

}}}
