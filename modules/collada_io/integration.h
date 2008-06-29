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

#ifndef __integration_h__
#define __integration_h__

#include <list>
#include <vector>
#include <iostream>
#include <stack>
#include <dae.h>
#include <dom/domMaterial.h>
#include <dom/domGeometry.h>
#include <dom/domNode.h>
#include <dom/domCOLLADA.h>
#include <k3dsdk/mesh_source.h>
#include "intGeometry.h"

using namespace std;
namespace module
{

namespace collada
{

namespace io
{
	// Our node structure, which we create by converting a domNode object
	class Node {
	public:
		list<intGeometry*> meshes;
		list<Node*> childNodes;
	
		// This is defined later to work around a circular dependency on the lookup function
		Node(domNode& node, const k3d::matrix4& mat);
		k3d::matrix4 getTransformation(domNode& node, const k3d::matrix4& mat);
	};
	
	// This function checks to see if a user data object has already been attached to
	// the DOM object. If so, that object is casted from void* to the appropriate type
	// and returned, otherwise the object is created and attached to the DOM object
	// via the setUserData method.
	template<typename MyType, typename DomType>
	MyType& lookup(DomType& domObject, const k3d::matrix4& mat) {
		if (!domObject.getUserData())
			domObject.setUserData(new MyType(domObject,mat));
		return *(MyType*)(domObject.getUserData());
	
	}
	
	// This function traverses all the DOM objects of a particular type and frees
	// destroys the associated user data object.
	template<typename MyType, typename DomType>
	void freeConversionObjects(DAE& dae) {
		vector<daeElement*> elts = dae.getDatabase()->typeLookup(DomType::ID());
		for (size_t i = 0; i < elts.size(); i++)
			delete (MyType*)elts[i]->getUserData();
	
	}
	
	class daeParser
	{
	public:
		daeParser(domCOLLADA& root);
		k3d::mesh get_mesh(){return Mesh;}
	private:
		k3d::mesh Mesh;
		///Matrix stack to handle coordinate system transformations
		std::stack<k3d::matrix4> mstack;
	};
	//void convertModel(domCOLLADA&, k3d::mesh& );
}}}
#endif
