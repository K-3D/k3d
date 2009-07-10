// K-3D
// Copyright (c) 1995-2006, Timothy M. Shead
//
// Contact: tshead@k-3d.com
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public
// License as published by the Free Software Foundation; either
// version 2 of the License, or (at your argument) any later version.
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
		\brief Implements useful GUI agnostic functions
		\author Tim Shead (tshead@k-3d.com)
		\author Romain Behar (romainbehar@yahoo.com)
*/

#include "detail.h"
#include "document_state.h"
#include "modifiers.h"
#include "transform.h"

#include <k3d-i18n-config.h>
#include <k3dsdk/classes.h>
#include <k3dsdk/plugins.h>
#include <k3dsdk/dependencies.h>
#include <k3dsdk/ipipeline.h>
#include <k3dsdk/imesh_selection_sink.h>
#include <k3dsdk/imesh_sink.h>
#include <k3dsdk/imesh_source.h>
#include <k3dsdk/imesh_storage.h>
#include <k3dsdk/iparentable.h>
#include <k3dsdk/iproperty_collection.h>
#include <k3dsdk/itransform_sink.h>
#include <k3dsdk/itransform_source.h>
#include <k3dsdk/legacy_mesh.h>
#include <k3dsdk/ngui/selection_state.h>
#include <k3dsdk/nodes.h>
#include <k3dsdk/properties.h>
#include <k3dsdk/result.h>
#include <k3dsdk/state_change_set.h>
#include <k3dsdk/string_cast.h>
#include <k3dsdk/transform.h>

namespace k3d
{

namespace ngui
{

namespace detail
{

/// Duplicates mesh source into a FrozenMesh and instantiate the new object
k3d::inode* duplicate_mesh(k3d::idocument& Document, k3d::inode& Node)
{
	// Get the upstream property
	k3d::iproperty_collection* property_collection = dynamic_cast<k3d::iproperty_collection*>(&Node);
	return_val_if_fail(property_collection, 0);

	k3d::imesh_sink* const downstream_sink = dynamic_cast<k3d::imesh_sink*>(&Node);
	return_val_if_fail(downstream_sink, 0);

	k3d::iproperty& downstream_input = downstream_sink->mesh_sink_input();

	k3d::iproperty* const upstream_output = Document.pipeline().dependency(downstream_input);
	return_val_if_fail(upstream_output, 0);

	k3d::imesh_source* const upstream_mesh_source = dynamic_cast<k3d::imesh_source*>(upstream_output->property_node());
	return_val_if_fail(upstream_mesh_source, 0);

	// Create a new FrozenMesh and a copy of upstream mesh instance ...
	k3d::inode* frozen_mesh = k3d::plugin::create<k3d::inode>(k3d::classes::FrozenMesh(), Document);
	return_val_if_fail(frozen_mesh, 0);

	const std::string frozen_mesh_name = k3d::unique_name(Document.nodes(), upstream_output->property_node()->name());
	frozen_mesh->set_name(frozen_mesh_name);

	k3d::inode* mesh_instance = k3d::plugin::create<k3d::inode>(k3d::classes::MeshInstance(), Document);
	return_val_if_fail(mesh_instance, 0);

	mesh_instance->set_name(k3d::unique_name(Document.nodes(), frozen_mesh_name + " Instance"));

	// Set DAG dependencies
	k3d::imesh_sink* const mesh_instance_sink = dynamic_cast<k3d::imesh_sink*>(mesh_instance);
	return_val_if_fail(mesh_instance_sink, 0);

	k3d::imesh_source* const frozen_mesh_source = dynamic_cast<k3d::imesh_source*>(frozen_mesh);
	return_val_if_fail(frozen_mesh_source, 0);

	k3d::ipipeline::dependencies_t dependencies;
	dependencies.insert(std::make_pair(&mesh_instance_sink->mesh_sink_input(), &frozen_mesh_source->mesh_source_output()));
	Document.pipeline().set_dependencies(dependencies);

	// Duplicate transformation, if any
	freeze_transformation(Node, *mesh_instance, Document);

	// Copy property values
	const k3d::iproperty_collection::properties_t properties = property_collection->properties();
	for(k3d::iproperty_collection::properties_t::const_iterator property = properties.begin(); property != properties.end(); ++property)
	{
		const std::string name = (*property)->property_name();
		if(name != "output_matrix"	// Property not writable
			&& name != "name"
			&& name != "input_mesh"
			&& name != "output_mesh")
		{
			k3d::property::set_internal_value(*mesh_instance, name, (*property)->property_internal_value());
		}
	}

	// Copy upstream mesh to our new FrozenMesh ...
	if(k3d::mesh* const upstream_mesh = boost::any_cast<k3d::mesh*>(k3d::property::pipeline_value(upstream_mesh_source->mesh_source_output())))
	{
		if(k3d::imesh_storage* const frozen_mesh_storage = dynamic_cast<k3d::imesh_storage*>(frozen_mesh))
		{
			k3d::mesh* const new_mesh = new k3d::mesh();
			k3d::mesh::deep_copy(*upstream_mesh, *new_mesh);
			frozen_mesh_storage->reset_mesh(new_mesh);
		}
	}

	return mesh_instance;
}

k3d::inode* duplicate_node(k3d::idocument& Document, k3d::inode& Node)
{
	// Get node's properties
	k3d::iproperty_collection* property_collection = dynamic_cast<k3d::iproperty_collection*>(&Node);
	return_val_if_fail(property_collection, 0);

	// Clone the node
	k3d::inode* clone = k3d::plugin::create<k3d::inode>(Node.factory(), Document);
	return_val_if_fail(clone, 0);

	clone->set_name(k3d::unique_name(Document.nodes(), Node.name()));

	// Duplicate transformation, if any
	freeze_transformation(Node, *clone, Document);

	// Copy property values
	const k3d::iproperty_collection::properties_t properties = property_collection->properties();
	for(k3d::iproperty_collection::properties_t::const_iterator property = properties.begin(); property != properties.end(); ++property)
	{
		const std::string name = (*property)->property_name();
		if(name != "output_matrix"	// Property not writable
			&& name != "name"
			&& name != "input_mesh"
			&& name != "output_mesh"
			&& !(Node.factory().factory_id() == k3d::classes::Camera() && name == "navigation_target")) // Skip Camera's navigation target property
		{
			k3d::property::set_internal_value(*clone, name, (*property)->property_internal_value());
		}
	}

	return clone;
}

} // namespace detail

/// Duplicates first node's transformation into a FrozenTransformationa and connects it to second node
void freeze_transformation(k3d::inode& FromNode, k3d::inode& ToNode, k3d::idocument& Document)
{
	// Check for "input_matrix" property
	k3d::iproperty* const transformation_property = k3d::property::get<k3d::matrix4>(FromNode, "input_matrix");
	if(!transformation_property)
		return;

	// Check whether it's connected
	if(!Document.pipeline().dependency(*transformation_property))
		return;

	// Duplicate input matrix into a new FrozenTransformation
	k3d::inode* frozen_transformation = k3d::plugin::create<k3d::inode>(k3d::classes::FrozenTransformation(), Document);
	return_if_fail(frozen_transformation);

	frozen_transformation->set_name(k3d::unique_name(Document.nodes(), ToNode.name() + " Transformation"));

	// Connect new FrozenTransformation and ToNode
	k3d::itransform_sink* const transformation_sink = dynamic_cast<k3d::itransform_sink*>(&ToNode);
	return_if_fail(transformation_sink);
	k3d::itransform_source* const transformation_source = dynamic_cast<k3d::itransform_source*>(frozen_transformation);
	return_if_fail(transformation_source);

	k3d::ipipeline::dependencies_t dependencies;
	dependencies.insert(std::make_pair(&transformation_sink->transform_sink_input(), &transformation_source->transform_source_output()));
	Document.pipeline().set_dependencies(dependencies);

	// Copy transformation value
	const k3d::matrix4 transformation = k3d::node_to_world_matrix(FromNode);
	k3d::property::set_internal_value(*frozen_transformation, "matrix", transformation);
}

void instantiate_selected_nodes(document_state& DocumentState)
{
	// Save selection
	const k3d::nodes_t nodes = selection::state(DocumentState.document()).selected_nodes();
	if(!nodes.size())
		return;

	// Record action
	std::string action = "Instantiate Nodes";
	if(nodes.size() == 1)
		action = k3d::string_cast(boost::format(_("Instantiate %1%")) % (*nodes.begin())->name());
	k3d::record_state_change_set changeset(DocumentState.document(), action, K3D_CHANGE_SET_CONTEXT);

	// Deselect all
	selection::state(DocumentState.document()).deselect_all();

	// Instantiate nodes and select new ones
	k3d::nodes_t new_nodes;
	for(k3d::nodes_t::const_iterator selected_node = nodes.begin(); selected_node != nodes.end(); ++selected_node)
	{
		if(k3d::inode* new_node = DocumentState.instantiate_mesh(*selected_node))
		{
			selection::state(DocumentState.document()).select(*new_node);
			new_nodes.push_back(new_node);
		}
	}

	// Show the new instance properties if only one was processed
	if(new_nodes.size() == 1)
		DocumentState.view_node_properties_signal().emit(*new_nodes.begin());
}

void duplicate_selected_nodes(document_state& DocumentState)
{
	// Save selection
	const k3d::nodes_t nodes = selection::state(DocumentState.document()).selected_nodes();
	if(!nodes.size())
		return;

	// Record action
	std::string action = "Duplicate Nodes";
	if(nodes.size() == 1)
		action = k3d::string_cast(boost::format(_("Duplicate %1%")) % (*nodes.begin())->name());
	k3d::record_state_change_set changeset(DocumentState.document(), action, K3D_CHANGE_SET_CONTEXT);

	// Deselect all
	selection::state(DocumentState.document()).deselect_all();

	// Instantiate nodes and select new ones
	k3d::nodes_t new_nodes;
	for(k3d::nodes_t::const_iterator selected_node = nodes.begin(); selected_node != nodes.end(); ++selected_node)
	{
		k3d::inode* new_node = 0;
		if(k3d::classes::MeshInstance() == (*selected_node)->factory().factory_id())
			new_node = detail::duplicate_mesh(DocumentState.document(), **selected_node);
		else
			new_node = detail::duplicate_node(DocumentState.document(), **selected_node);

		if(new_node)
		{
			selection::state(DocumentState.document()).select(*new_node);
			new_nodes.push_back(new_node);
		}
	}

	// Show duplicated node properties if only one was processed
	if(new_nodes.size() == 1)
		DocumentState.view_node_properties_signal().emit(*new_nodes.begin());
}

} // namespace ngui

} // namespace k3d

