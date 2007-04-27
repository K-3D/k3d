// K-3D
// Copyright (c) 2004-2006, Romain Behar
//
// Contact: romainbehar@yahoo.com
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
		\brief Implements the geometry helpers
		\author Romain Behar (romainbehar@yahoo.com)
*/

#include <k3dsdk/classes.h>
#include <k3dsdk/create_plugins.h>
#include <k3dsdk/idag.h>
#include <k3dsdk/idrawable_gl.h>
#include <k3dsdk/imesh_sink.h>
#include <k3dsdk/imesh_source.h>
#include <k3dsdk/imesh_storage.h>
#include <k3dsdk/inode.h>
#include <k3dsdk/nodes.h>
#include <k3dsdk/property.h>
#include <k3dsdk/result.h>

#include "helpers.h"

namespace libk3dgeometry
{

namespace detail
{

/// Creates a new mesh, puts it in a FrozenMesh() and connects it to a MeshInstance
k3d::legacy::mesh* create_mesh(k3d::idocument& Document, const std::string Name, k3d::inode*& ReturnFrozenMesh, k3d::inode*& ReturnMeshInstance)
{
	k3d::legacy::mesh* const new_mesh = new k3d::legacy::mesh();
	return_val_if_fail(new_mesh, 0);

	// Create document object ...
	k3d::inode* const frozen_mesh = k3d::create_plugin<k3d::inode>(k3d::classes::FrozenMesh(), Document);
	return_val_if_fail(frozen_mesh, 0);
	ReturnFrozenMesh = frozen_mesh;

	k3d::imesh_storage* const frozen_mesh_storage = dynamic_cast<k3d::imesh_storage*>(frozen_mesh);
	return_val_if_fail(frozen_mesh_storage, false);

	frozen_mesh_storage->reset_mesh(new_mesh);
	frozen_mesh->set_name(k3d::unique_name(Document.nodes(), Name));

	// Create mesh object instance ...
	k3d::inode* instance = k3d::create_plugin<k3d::inode>(k3d::classes::MeshInstance(), Document);
	return_val_if_fail(instance, 0);
	ReturnMeshInstance = instance;

	instance->set_name(k3d::unique_name(Document.nodes(), Name + " instance"));

	// Set dependencies ...
	k3d::imesh_sink* const instance_sink = dynamic_cast<k3d::imesh_sink*>(instance);
	return_val_if_fail(instance_sink, false);
	k3d::imesh_source* const frozen_mesh_source = dynamic_cast<k3d::imesh_source*>(frozen_mesh);
	return_val_if_fail(frozen_mesh_source, 0);

	k3d::idag::dependencies_t dependencies;
	dependencies[&instance_sink->mesh_sink_input()] = &frozen_mesh_source->mesh_source_output();
	Document.dag().set_dependencies(dependencies);

	return new_mesh;
}

void get_mesh_instances(k3d::idocument& Document, mesh_instances_t& MeshInstances)
{
	const k3d::inode_collection::nodes_t& nodes = Document.nodes().collection();
	for(k3d::inode_collection::nodes_t::const_iterator node = nodes.begin(); node != nodes.end(); ++node)
	{
		if(!dynamic_cast<k3d::gl::idrawable*>(*node))
			continue;

		k3d::imesh_source* const mesh_source = dynamic_cast<k3d::imesh_source*>(*node);
		if(!mesh_source)
			continue;

		// Check for transformed output mesh
		k3d::dev::mesh* mesh = 0;
		if(k3d::iproperty* property = k3d::get_property(**node, "transformed_output_mesh"))
		{
			mesh = boost::any_cast<k3d::dev::mesh*>(property->property_value());
		}
		else
		{
			// Fall back to mesh source
			k3d::iproperty& property = mesh_source->mesh_source_output();
			mesh = boost::any_cast<k3d::dev::mesh*>(property.property_value());
		}

		if(!mesh)
			continue;

		MeshInstances.insert(std::make_pair(mesh, *node));
	}
}

bool triangle_test(const k3d::legacy::mesh& Mesh)
{
	k3d::legacy::mesh::polyhedra_t::const_iterator polyhedron;
	for(polyhedron = Mesh.polyhedra.begin(); polyhedron != Mesh.polyhedra.end(); polyhedron++)
	{
		k3d::legacy::polyhedron::faces_t::const_iterator face;
		for(face = (*polyhedron)->faces.begin(); face != (*polyhedron)->faces.end(); face++)
		{
			k3d::legacy::split_edge* first = (*face)->first_edge;
			// Skip empty faces
			if(!first)
				continue;

			k3d::legacy::split_edge* current_edge = first->face_clockwise;
			// Skip one-edged faces
			if(!current_edge)
				continue;

			unsigned long edge_number = 1;

			while(current_edge != first)
			{
				edge_number++;
				current_edge = current_edge->face_clockwise;
			}

			if(edge_number != 3)
				return false;
		}
	}

	return true;
}

} // namespace detail

} // namespace libk3dgeometry

