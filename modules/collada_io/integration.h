#ifndef MODULES_COLLADA_IO_INTEGRATION_H
#define MODULES_COLLADA_IO_INTEGRATION_H

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
#include <stack>
#include <dae.h>
#include <dom/domMaterial.h>
#include <dom/domGeometry.h>
#include <dom/domNode.h>
#include <dom/domCOLLADA.h>
#include <k3dsdk/mesh_source.h>
#include "int_elements.h"

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
		list<intCamera*> cameras;
		list<intLight*> lights;
		list<Node*> childNodes;
	
		// This is defined later to work around a circular dependency on the lookup function
		Node(domNode& node, const k3d::matrix4& mat);
		k3d::matrix4 getTransformation(domNode& node);
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

	enum
	{IMPORTER,READER};	

	class daeParser
	{
	public:
		daeParser(domCOLLADA& root, k3d::mesh& Mesh);
		daeParser(domCOLLADA& root, k3d::idocument& Document);
		//k3d::mesh get_mesh(){return Mesh;}
		//k3d::idocument &get_document(){return Document;}
	private:
		//k3d::mesh Mesh;
		//k3d::idocument Document;
	};
	//void convertModel(domCOLLADA&, k3d::mesh& );
}}}

#endif // !MODULES_COLLADA_IO_INTEGRATION_H

