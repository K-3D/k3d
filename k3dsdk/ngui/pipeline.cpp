// K-3D
// Copyright (c) 1995-2010, Timothy M. Shead
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
	\author Tim Shead (tshead@k-3d.com)
	\author Romain Behar (romainbehar@yahoo.com)
*/

#include <k3d-i18n-config.h>
#include <k3dsdk/classes.h>
#include <k3dsdk/dependencies.h>
#include <k3dsdk/imaterial_sink.h>
#include <k3dsdk/imesh_selection_sink.h>
#include <k3dsdk/imesh_sink.h>
#include <k3dsdk/imesh_source.h>
#include <k3dsdk/imesh_storage.h>
#include <k3dsdk/imulti_mesh_sink.h>
#include <k3dsdk/inode_collection_sink.h>
#include <k3dsdk/iparentable.h>
#include <k3dsdk/ipipeline.h>
#include <k3dsdk/iproperty_collection.h>
#include <k3dsdk/ireset_properties.h>
#include <k3dsdk/itime_sink.h>
#include <k3dsdk/imatrix_sink.h>
#include <k3dsdk/imatrix_source.h>
#include <k3dsdk/ngui/document_state.h>
#include <k3dsdk/ngui/modifiers.h>
#include <k3dsdk/ngui/panel_mediator.h>
#include <k3dsdk/ngui/pipeline.h>
#include <k3dsdk/ngui/selection.h>
#include <k3dsdk/ngui/transform.h>
#include <k3dsdk/node.h>
#include <k3dsdk/nodes.h>
#include <k3dsdk/plugins.h>
#include <k3dsdk/properties.h>
#include <k3dsdk/result.h>
#include <k3dsdk/state_change_set.h>
#include <k3dsdk/string_cast.h>
#include <k3dsdk/time_source.h>
#include <k3dsdk/transform.h>
#include <k3dsdk/utility_gl.h>

namespace k3d
{

namespace ngui
{

namespace pipeline
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

inode* default_gl_painter(idocument& Document)
{
	const std::vector<k3d::inode*> nodes = k3d::node::lookup(Document, "GL Default Painter");
	return (1 == nodes.size()) ? nodes[0] : 0;
}

inode* default_ri_painter(idocument& Document)
{
	const std::vector<k3d::inode*> nodes = k3d::node::lookup(Document, "RenderMan Default Painter");
	return (1 == nodes.size()) ? nodes[0] : 0;
}
	
inode* create_node(idocument& Document, iplugin_factory& Factory)
{
	// Switch to node selection mode
	if(selection::NODE != selection::state(Document).current_mode())
		selection::state(Document).set_current_mode(selection::NODE);

	// Create the requested node ...
	k3d::record_state_change_set changeset(Document, k3d::string_cast(boost::format(_("Create %1%")) % Factory.name()), K3D_CHANGE_SET_CONTEXT);
	const std::string node_name = k3d::unique_name(Document.nodes(), Factory.name());
	k3d::inode* const node = k3d::plugin::create<k3d::inode>(Factory, Document, node_name);
	return_val_if_fail(node, 0);

	// Keep track of the node to be selected ...
	k3d::inode* to_be_selected = node;

	// Keep track of every new node created ...
	std::vector<k3d::inode*> new_nodes;
	new_nodes.push_back(node);

	// Prepare to make connections to other nodes ...
	k3d::ipipeline::dependencies_t dependencies;

	// If the new node is a mesh source (but not a MeshInstance!), create a MeshInstance node and attach it so it's immediately visible ...
	k3d::imesh_source* const mesh_source = dynamic_cast<k3d::imesh_source*>(node);
	if(mesh_source && k3d::classes::MeshInstance() != Factory.factory_id())
	{
		// Create a mesh instance ...
		k3d::inode* const mesh_instance = pipeline::instantiate_mesh(Document, *node);
		return_val_if_fail(mesh_instance, 0);
		new_nodes.push_back(mesh_instance);

		// In this case, we want to select the mesh instance instead of the mesh source ...
		to_be_selected = mesh_instance;
	}

	// If the new node is a material sink, assign a default material ...
	if(k3d::imaterial_sink* const material_sink = dynamic_cast<k3d::imaterial_sink*>(node))
	{
		const std::vector<k3d::imaterial*> materials = k3d::node::lookup<k3d::imaterial>(Document);
		if(materials.size())
			k3d::property::set_internal_value(material_sink->material_sink_input(), dynamic_cast<k3d::inode*>(*materials.rbegin()));
	}

	// If the new node is a time sink, connect it to the document time source ...
	if(k3d::itime_sink* const time_sink = dynamic_cast<k3d::itime_sink*>(node))
		dependencies.insert(std::make_pair(&time_sink->time_sink_input(), k3d::get_time(Document)));

	if(!dependencies.empty())
		Document.pipeline().set_dependencies(dependencies);

	// If the new node is a camera, orient it horizontally
	if(k3d::classes::Camera() == Factory.factory_id())
		k3d::set_matrix(*node, k3d::rotate3(k3d::radians(90.0), k3d::vector3(1, 0, 0)));
	
	// If the new node is a multiple mesh sink, add two mesh inputs
	if(Factory.implements(typeid(k3d::imulti_mesh_sink)))
	{
		k3d::property::create<k3d::mesh*>(*node, "input_mesh1", "Input Mesh 1", "", static_cast<k3d::mesh*>(0));
		k3d::property::create<k3d::mesh*>(*node, "input_mesh2", "Input Mesh 2", "", static_cast<k3d::mesh*>(0));
	}

	// If the new node is a render-engine, default to making every node in the document visible ...
	if(k3d::inode_collection_sink* const node_collection_sink = dynamic_cast<k3d::inode_collection_sink*>(node))
	{
		const k3d::inode_collection_sink::properties_t properties = node_collection_sink->node_collection_properties();
		for(k3d::inode_collection_sink::properties_t::const_iterator property = properties.begin(); property != properties.end(); ++property)
			k3d::property::set_internal_value(**property, Document.nodes().collection());
	}

	// By default, make the new node visible in any node collection sinks that already exist ...
	k3d::node::show(Document, new_nodes);

	// Give nodes a chance to initialize their property values based on their inputs, if any ...
	if(k3d::ireset_properties* const reset_properties = dynamic_cast<k3d::ireset_properties*>(node))
		reset_properties->reset_properties();

	// Replace the current selection
	selection::state(Document).deselect_all();
	selection::state(Document).select(*to_be_selected);

	// Make the newly-created node properties visible ...
	panel::mediator(Document).set_focus(*node);

	k3d::gl::redraw_all(Document, k3d::gl::irender_viewport::ASYNCHRONOUS);

	return node;
}

/// Duplicates first node's transformation into a FrozenMatrixa and connects it to second node
void freeze_transformation(k3d::inode& FromNode, k3d::inode& ToNode, k3d::idocument& Document)
{
	// Check for "input_matrix" property
	k3d::iproperty* const transformation_property = k3d::property::get<k3d::matrix4>(FromNode, "input_matrix");
	if(!transformation_property)
		return;

	// Check whether it's connected
	if(!Document.pipeline().dependency(*transformation_property))
		return;

	// Duplicate input matrix into a new FrozenMatrix
	k3d::inode* frozen_transformation = k3d::plugin::create<k3d::inode>(k3d::classes::FrozenMatrix(), Document);
	return_if_fail(frozen_transformation);

	frozen_transformation->set_name(k3d::unique_name(Document.nodes(), ToNode.name() + " Transformation"));

	// Connect new FrozenMatrix and ToNode
	k3d::imatrix_sink* const transformation_sink = dynamic_cast<k3d::imatrix_sink*>(&ToNode);
	return_if_fail(transformation_sink);
	k3d::imatrix_source* const transformation_source = dynamic_cast<k3d::imatrix_source*>(frozen_transformation);
	return_if_fail(transformation_source);

	k3d::ipipeline::dependencies_t dependencies;
	dependencies.insert(std::make_pair(&transformation_sink->matrix_sink_input(), &transformation_source->matrix_source_output()));
	Document.pipeline().set_dependencies(dependencies);

	// Copy transformation value
	const k3d::matrix4 transformation = k3d::node_to_world_matrix(FromNode);
	k3d::property::set_internal_value(*frozen_transformation, "matrix", transformation);
}

inode* instantiate_mesh(idocument& Document, inode& Node)
{
	inode* mesh_instance = 0;
	iproperty* mesh_source_property = 0;
	if(classes::MeshInstance() == Node.factory().factory_id()) // Instantiate the input in case of a mesh instance
	{
		imesh_sink* sink = dynamic_cast<imesh_sink*>(&Node);
		mesh_source_property = Document.pipeline().dependency(sink->mesh_sink_input());
		if(mesh_source_property)
			mesh_instance = k3d::plugin::create<k3d::inode>(k3d::classes::MeshInstance(), Document, k3d::unique_name(Document.nodes(), Node.name() + " Instance"));
	}
	else
	{
		k3d::ipipeline& pipeline = Document.pipeline();
		const k3d::ipipeline::dependencies_t& dependencies = pipeline.dependencies();
		k3d::imesh_source* mesh_source = dynamic_cast<k3d::imesh_source*>(&Node);
		if(!mesh_source)
			return 0;
		mesh_source_property = &(mesh_source->mesh_source_output());
		if(mesh_source_property)
			mesh_instance = k3d::plugin::create<k3d::inode>(k3d::classes::MeshInstance(), Document, k3d::unique_name(Document.nodes(), Node.name() + " Instance"));
			
		k3d::iproperty* instance_mesh_source_property = &dynamic_cast<k3d::imesh_source*>(mesh_instance)->mesh_source_output();
		k3d::iproperty* instance_matrix_source_property = &dynamic_cast<k3d::imatrix_source*>(mesh_instance)->matrix_source_output();
		
		k3d::imatrix_source* matrix_source = dynamic_cast<k3d::imatrix_source*>(&Node);
		k3d::iproperty* matrix_source_property = 0;
		if(matrix_source)
			matrix_source_property = &(matrix_source->matrix_source_output());
		
		// Connect the MeshInstance outputs to the inputs of the downstream node, if any
		k3d::ipipeline::dependencies_t new_dependencies;
		for(k3d::ipipeline::dependencies_t::const_iterator dependency = dependencies.begin(); dependency != dependencies.end(); ++dependency)
		{
			if(dependency->second == mesh_source_property)
			{
				dependency->first->property_set_dependency(0);
				new_dependencies.insert(std::make_pair(dependency->first, instance_mesh_source_property));
			}
			if(matrix_source_property && dependency->second == matrix_source_property)
			{
				dependency->first->property_set_dependency(0);
				new_dependencies.insert(std::make_pair(dependency->first, instance_matrix_source_property));
			}
		}
		pipeline.set_dependencies(new_dependencies);
		
	}
	 
	if(!mesh_source_property)
		return 0;
	
	// Assign a default painter ...
	k3d::property::set_internal_value(*mesh_instance, "gl_painter", default_gl_painter(Document));
	k3d::property::set_internal_value(*mesh_instance, "ri_painter", default_ri_painter(Document));

	// Connect the mesh instance to the source ...
	k3d::ipipeline::dependencies_t dependencies;
	k3d::imesh_sink* const mesh_sink = dynamic_cast<k3d::imesh_sink*>(mesh_instance);
	if(mesh_sink)
		dependencies.insert(std::make_pair(&mesh_sink->mesh_sink_input(), mesh_source_property));
	Document.pipeline().set_dependencies(dependencies);
	
	freeze_transformation(Node, *mesh_instance, Document);
	
	return mesh_instance;
}

void instantiate_selected_nodes(idocument& Document)
{
	// Save selection
	const k3d::nodes_t nodes = selection::state(Document).selected_nodes();
	if(!nodes.size())
		return;

	// Record action
	std::string action = "Instantiate Nodes";
	if(nodes.size() == 1)
		action = k3d::string_cast(boost::format(_("Instantiate %1%")) % (*nodes.begin())->name());
	k3d::record_state_change_set changeset(Document, action, K3D_CHANGE_SET_CONTEXT);

	// Deselect all
	selection::state(Document).deselect_all();

	// Instantiate nodes and select new ones
	k3d::nodes_t new_nodes;
	for(k3d::nodes_t::const_iterator selected_node = nodes.begin(); selected_node != nodes.end(); ++selected_node)
	{
		if(k3d::inode* new_node = instantiate_mesh(Document, **selected_node))
		{
			selection::state(Document).select(*new_node);
			new_nodes.push_back(new_node);
		}
	}

	k3d::node::show(Document, new_nodes);

	// Show the new instance properties if only one was processed
	if(new_nodes.size() == 1)
		panel::mediator(Document).set_focus(**new_nodes.begin());
}

void duplicate_selected_nodes(idocument& Document)
{
	// Save selection
	const k3d::nodes_t nodes = selection::state(Document).selected_nodes();
	if(!nodes.size())
		return;

	// Record action
	std::string action = "Duplicate Nodes";
	if(nodes.size() == 1)
		action = k3d::string_cast(boost::format(_("Duplicate %1%")) % (*nodes.begin())->name());
	k3d::record_state_change_set changeset(Document, action, K3D_CHANGE_SET_CONTEXT);

	// Deselect all
	selection::state(Document).deselect_all();

	// Instantiate nodes and select new ones
	k3d::nodes_t new_nodes;
	for(k3d::nodes_t::const_iterator selected_node = nodes.begin(); selected_node != nodes.end(); ++selected_node)
	{
		k3d::inode* new_node = 0;
		if(k3d::classes::MeshInstance() == (*selected_node)->factory().factory_id())
			new_node = detail::duplicate_mesh(Document, **selected_node);
		else
			new_node = detail::duplicate_node(Document, **selected_node);

		if(new_node)
		{
			selection::state(Document).select(*new_node);
			new_nodes.push_back(new_node);
		}
	}

	k3d::node::show(Document, new_nodes);

	// Show duplicated node properties if only one was processed
	if(new_nodes.size() == 1)
		panel::mediator(Document).set_focus(**new_nodes.begin());
}

} // namespace pipeline

} // namespace ngui

} // namespace k3d

