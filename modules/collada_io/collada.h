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
*/

#ifndef COLLADA_H
#define COLLADA_H

#include <k3dsdk/mesh.h>
#include <k3dsdk/idocument.h>
#include <k3dsdk/imesh_storage.h>
#include <k3dsdk/properties.h>
#include <dom/domGeometry.h>
#include "intElements.h"

namespace module
{
namespace collada
{
namespace io
{

	k3d::mesh to_k3d_mesh(domGeometry &geom)
	{
		intGeometry result(geom,k3d::identity3D());
		return result.getMesh();
	}

	class collada_obj
	{
	public:
		collada_obj(k3d::idocument &Document, domGeometry &geom)
		{
			id = geom.getAttribute("id");
			name = k3d::unique_name(Document.nodes(), geom.getAttribute("name"));
			type = "frozen_mesh";

			k3d::mesh *mesh = new k3d::mesh();
			//mesh = to_k3d_mesh(geom);
			k3d::inode *frozen_mesh = k3d::plugin::create<k3d::inode>(*k3d::plugin::factory::lookup("FrozenMesh"), Document, name);
			k3d::imesh_storage* mesh_storage = dynamic_cast<k3d::imesh_storage*>(frozen_mesh);
			mesh_storage->reset_mesh(mesh);
			*mesh  = to_k3d_mesh(geom);

			k3d::imesh_source* const mesh_source = dynamic_cast<k3d::imesh_source*>(frozen_mesh);
			mesh_source_output = &mesh_source->mesh_source_output();
		}
		std::string get_name(){return name;}
		std::string get_type(){return type;}
		std::string get_id(){return id;}
		k3d::iproperty *get_mesh_source_output(){return mesh_source_output;}
	private:
		std::string name;
		std::string id;
		std::string type;
		k3d::iproperty *mesh_source_output;
	};

}
}
}

#endif

