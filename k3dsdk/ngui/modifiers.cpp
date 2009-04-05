// K-3D
// Copyright (c) 1995-2005, Timothy M. Shead
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
		\author Tim Shead (tshead@k-3d.com)
*/

#include "document_state.h"
#include "modifiers.h"

#include <k3d-i18n-config.h>
#include <k3dsdk/classes.h>
#include <k3dsdk/plugins.h>
#include <k3dsdk/plugins.h>
#include <k3dsdk/idocument.h>
#include <k3dsdk/imesh_selection_sink.h>
#include <k3dsdk/imesh_sink.h>
#include <k3dsdk/imesh_source.h>
#include <k3dsdk/imulti_mesh_sink.h>
#include <k3dsdk/inode.h>
#include <k3dsdk/ipipeline.h>
#include <k3dsdk/iproperty.h>
#include <k3dsdk/ireset_properties.h>
#include <k3dsdk/itransform_sink.h>
#include <k3dsdk/itransform_source.h>
#include <k3dsdk/legacy_mesh.h>
#include <k3dsdk/mesh_selection.h>
#include <k3dsdk/nodes.h>
#include <k3dsdk/properties.h>
#include <k3dsdk/result.h>
#include <k3dsdk/state_change_set.h>
#include <k3dsdk/string_cast.h>

#include <sstream>

namespace k3d
{

namespace ngui
{

namespace detail
{

struct sort_by_name
{
	bool operator()(k3d::iplugin_factory* LHS, k3d::iplugin_factory* RHS)
	{
		return LHS->name() < RHS->name();
	}
};

} // namespace detail

const factories_t& mesh_modifiers()
{
	static factories_t modifiers;
	if(modifiers.empty())
	{
		const k3d::plugin::factory::collection_t data_source_modifiers = k3d::plugin::factory::lookup<k3d::imesh_source>();
		const k3d::plugin::factory::collection_t data_sink_modifiers = k3d::plugin::factory::lookup<k3d::imesh_sink>();
		const k3d::plugin::factory::collection_t multi_sink_modifiers = k3d::plugin::factory::lookup<k3d::imulti_mesh_sink>();
		const k3d::plugin::factory::collection_t scripted_modifiers = k3d::plugin::factory::lookup("k3d:plugin-type", "MeshModifierScript");
		std::set_intersection(data_source_modifiers.begin(), data_source_modifiers.end(), data_sink_modifiers.begin(), data_sink_modifiers.end(), std::inserter(modifiers, modifiers.end()));
		modifiers.insert(modifiers.end(), multi_sink_modifiers.begin(), multi_sink_modifiers.end());
		modifiers.insert(modifiers.end(), scripted_modifiers.begin(), scripted_modifiers.end());
		std::sort(modifiers.begin(), modifiers.end(), detail::sort_by_name());
	}

	return modifiers;
}

const factories_t& transform_modifiers()
{
	static factories_t modifiers;
	if(modifiers.empty())
	{
		const k3d::plugin::factory::collection_t data_source_modifiers = k3d::plugin::factory::lookup<k3d::itransform_source>();
		const k3d::plugin::factory::collection_t data_sink_modifiers = k3d::plugin::factory::lookup<k3d::itransform_sink>();
		std::set_intersection(data_source_modifiers.begin(), data_source_modifiers.end(), data_sink_modifiers.begin(), data_sink_modifiers.end(), std::inserter(modifiers, modifiers.end()));
		std::sort(modifiers.begin(), modifiers.end(), detail::sort_by_name());
	}

	return modifiers;
}

/// Modify transformation
k3d::inode* modify_transformation(k3d::idocument& Document, k3d::inode& Object, k3d::iplugin_factory* Modifier)
{
	return_val_if_fail(Modifier, 0);

	// Get the upstream and downstream properties ...
	k3d::itransform_sink* const downstream_sink = dynamic_cast<k3d::itransform_sink*>(&Object);
	return_val_if_fail(downstream_sink, 0);

	k3d::iproperty& downstream_input = downstream_sink->transform_sink_input();

	k3d::iproperty* const upstream_output = Document.pipeline().dependency(downstream_input);

	k3d::inode* modifier = 0;

	// This block is recorded for undo purposes ...
	{
		k3d::record_state_change_set changeset(Document, k3d::string_cast(boost::format(_("Add Modifier %1%")) % Modifier->name()), K3D_CHANGE_SET_CONTEXT);

		// Create our modifier object ...
		modifier = k3d::plugin::create<k3d::inode>(*Modifier, Document, k3d::unique_name(Document.nodes(), Modifier->name()));
		return_val_if_fail(modifier, 0);

		// Get its input and output properties ...
		k3d::itransform_sink* const modifier_sink = dynamic_cast<k3d::itransform_sink*>(modifier);
		return_val_if_fail(modifier_sink, 0);
		k3d::itransform_source* const modifier_source = dynamic_cast<k3d::itransform_source*>(modifier);
		return_val_if_fail(modifier_source, 0);

		// Insert the modifier into the DAG ...
		k3d::ipipeline::dependencies_t dependencies;
		if(upstream_output)
			dependencies.insert(std::make_pair(&modifier_sink->transform_sink_input(), upstream_output));
		dependencies.insert(std::make_pair(&downstream_input, &modifier_source->transform_source_output()));
		Document.pipeline().set_dependencies(dependencies);
	}

	return modifier;
}

/// Modify mesh
k3d::inode* modify_mesh(document_state& DocumentState, k3d::inode& Node, k3d::iplugin_factory* Modifier)
{
	return_val_if_fail(Modifier, 0);

	k3d::idocument& document = DocumentState.document();
	
	// Get the upstream and downstream properties ...
	k3d::imesh_sink* const downstream_sink = dynamic_cast<k3d::imesh_sink*>(&Node);
	return_val_if_fail(downstream_sink, 0);

	k3d::iproperty& downstream_input = downstream_sink->mesh_sink_input();

	k3d::iproperty* const upstream_output = document.pipeline().dependency(downstream_input);
	return_val_if_fail(upstream_output, 0);

	k3d::inode* modifier = 0;

	// This block is recorded for undo purposes ...
	{
		k3d::record_state_change_set changeset(document, k3d::string_cast(boost::format(_("Add Modifier %1%")) % Modifier->name()), K3D_CHANGE_SET_CONTEXT);

		// Create our modifier object ...
		modifier = k3d::plugin::create<k3d::inode>(*Modifier, document, k3d::unique_name(document.nodes(), Modifier->name()));
		return_val_if_fail(modifier, 0);

		// Get its input and output properties ...
		k3d::imesh_sink* const modifier_sink = dynamic_cast<k3d::imesh_sink*>(modifier);
		return_val_if_fail(modifier_sink, 0);
		k3d::imesh_source* const modifier_source = dynamic_cast<k3d::imesh_source*>(modifier);
		return_val_if_fail(modifier_source, 0);

		// Insert the modifier into the pipeline ...
		k3d::ipipeline::dependencies_t dependencies;
		dependencies.insert(std::make_pair(&modifier_sink->mesh_sink_input(), upstream_output));
		dependencies.insert(std::make_pair(&downstream_input, &modifier_source->mesh_source_output()));
		document.pipeline().set_dependencies(dependencies);

		// If the modifier is a mesh selection sink, set its selection state ...
		k3d::imesh_selection_sink* const modifier_mesh_selection_sink = dynamic_cast<k3d::imesh_selection_sink*>(modifier);
		k3d::imesh_selection_sink* const downstream_mesh_selection_sink = dynamic_cast<k3d::imesh_selection_sink*>(&Node);
		if(modifier_mesh_selection_sink && downstream_mesh_selection_sink)
		{
			if(SELECT_NODES == DocumentState.selection_mode().internal_value())
			{
				k3d::property::set_internal_value(
					modifier_mesh_selection_sink->mesh_selection_sink_input(),
					k3d::mesh_selection::select_all());
			}
			else
			{
				k3d::property::set_internal_value(
					modifier_mesh_selection_sink->mesh_selection_sink_input(),
					downstream_mesh_selection_sink->mesh_selection_sink_input().property_internal_value());
			}
	
			k3d::property::set_internal_value(
				downstream_mesh_selection_sink->mesh_selection_sink_input(),
				k3d::mesh_selection::select_null());
		}
	}
	
	// Give nodes a chance to initialize their property values based on their inputs, if any ...
	if(k3d::ireset_properties* const reset_properties = dynamic_cast<k3d::ireset_properties*>(modifier))
		reset_properties->reset_properties();

	return modifier;
}

void modify_selected_meshes(document_state& DocumentState, k3d::iplugin_factory* Modifier)
{
	return_if_fail(Modifier);
	k3d::idocument& document = DocumentState.document();
	if (Modifier->implements(typeid(k3d::imulti_mesh_sink)))
	{ // Mesh modifier taking multiple inputs
		k3d::uint_t count = 0;
		k3d::ipipeline::dependencies_t dependencies;
		const k3d::nodes_t selected_nodes = DocumentState.selected_nodes();
		// Create the node
		k3d::inode* multi_sink = DocumentState.create_node(Modifier);
		k3d::record_state_change_set changeset(document, k3d::string_cast(boost::format(_("Add Modifier %1%")) % Modifier->name()), K3D_CHANGE_SET_CONTEXT);
		k3d::nodes_t nodes_to_delete;
		for(k3d::nodes_t::const_iterator node = selected_nodes.begin(); node != selected_nodes.end(); ++node)
		{
			k3d::imesh_sink* const mesh_sink = dynamic_cast<k3d::imesh_sink*>(*node);
			if(!mesh_sink)
				continue;
			k3d::itransform_sink* const transform_sink = dynamic_cast<k3d::itransform_sink*>(*node);

			k3d::iproperty* source_mesh = document.pipeline().dependency(mesh_sink->mesh_sink_input());
			if (!source_mesh)
				continue;
			
			if (transform_sink) // Insert a transform node
			{
				k3d::iproperty* const source_transformation = document.pipeline().dependency(transform_sink->transform_sink_input());
				if (source_transformation)
				{
					k3d::inode* transform_points = k3d::plugin::create<k3d::inode>("TransformPoints", document, k3d::unique_name(document.nodes(), "TransformPoints"));
					return_if_fail(transform_points);
					k3d::itransform_sink* transform_points_transform_sink = dynamic_cast<k3d::itransform_sink*>(transform_points);
					return_if_fail(transform_points_transform_sink);
					k3d::imesh_sink* transform_points_mesh_sink = dynamic_cast<k3d::imesh_sink*>(transform_points);
					return_if_fail(transform_points_mesh_sink);
					dependencies.insert(std::make_pair(&transform_points_transform_sink->transform_sink_input(), source_transformation));
					dependencies.insert(std::make_pair(&transform_points_mesh_sink->mesh_sink_input(), source_mesh));
					k3d::imesh_source* transform_points_mesh_source = dynamic_cast<k3d::imesh_source*>(transform_points);
					return_if_fail(transform_points_mesh_source);
					source_mesh = &transform_points_mesh_source->mesh_source_output();
					k3d::imesh_selection_sink* selection_sink = dynamic_cast<k3d::imesh_selection_sink*>(transform_points);
					return_if_fail(selection_sink);
					k3d::property::set_internal_value(selection_sink->mesh_selection_sink_input(), k3d::mesh_selection::select_all());
				}
			}
			++count;
			// Create a new user property
			std::stringstream name, label;
			name << "input_mesh" << count;
			label << "Input Mesh " << count;
			k3d::iproperty* sink = k3d::property::get(*multi_sink, name.str());
			if (!sink)
				sink = k3d::property::create<k3d::mesh*>(*multi_sink, name.str(), label.str(), "", static_cast<k3d::mesh*>(0));
			// Store the connection
			dependencies.insert(std::make_pair(sink, source_mesh));
			// Delete the input node
			nodes_to_delete.push_back(*node);
		}
		document.pipeline().set_dependencies(dependencies);
		k3d::delete_nodes(document, nodes_to_delete);
		// Give nodes a chance to initialize their property values based on their inputs, if any ...
		if(k3d::ireset_properties* const reset_properties = dynamic_cast<k3d::ireset_properties*>(multi_sink))
			reset_properties->reset_properties();
		DocumentState.view_node_properties_signal().emit(multi_sink);
	}
	else
	{ // Normal mesh modifier
		k3d::nodes_t new_modifiers;
	
		const k3d::nodes_t selected_nodes = DocumentState.selected_nodes();
		for(k3d::nodes_t::const_iterator node = selected_nodes.begin(); node != selected_nodes.end(); ++node)
		{
			new_modifiers.push_back(modify_mesh(DocumentState, **node, Modifier));
			assert_warning(new_modifiers.back());
		}
	
		// Show the new modifier properties if only one was processed
		if(selected_nodes.size() == 1)
		{
			DocumentState.view_node_properties_signal().emit(new_modifiers.front());
		}
		else // otherwise connect all parameter properties to the first node and show that one
		{
			k3d::iproperty_collection* first_property_collection = dynamic_cast<k3d::iproperty_collection*>(new_modifiers.front());
			if (first_property_collection)
			{
				// Get the in-and output property names, to exclude them from the connections
				k3d::imesh_sink* const modifier_sink = dynamic_cast<k3d::imesh_sink*>(new_modifiers.front());
				return_if_fail(modifier_sink);
				k3d::imesh_source* const modifier_source = dynamic_cast<k3d::imesh_source*>(new_modifiers.front());
				return_if_fail(modifier_source);
				const std::string sink_name = modifier_sink->mesh_sink_input().property_name();
				const std::string source_name = modifier_source->mesh_source_output().property_name();
				
				k3d::ipipeline::dependencies_t dependencies;
				const k3d::iproperty_collection::properties_t& first_properties = first_property_collection->properties();
				k3d::nodes_t::iterator modifier = new_modifiers.begin();
				++modifier;
				for (modifier; modifier != new_modifiers.end(); ++modifier)
				{
					k3d::iproperty_collection* property_collection = dynamic_cast<k3d::iproperty_collection*>(*modifier);
					return_if_fail(property_collection);
					const k3d::iproperty_collection::properties_t& properties = property_collection->properties();
					k3d::iproperty_collection::properties_t::const_iterator property = properties.begin();
					for (k3d::iproperty_collection::properties_t::const_iterator first_property = first_properties.begin(); first_property != first_properties.end(); ++first_property)
					{
						return_if_fail(property != properties.end());
						return_if_fail((*property)->property_name() == (*first_property)->property_name());
						if ((*property)->property_name() == sink_name || (*property)->property_name() == source_name || (*property)->property_name() == "name")
						{
							++property;
							continue;
						}
						dependencies.insert(std::make_pair(*property, *first_property));
						++property;
					}
				}
				document.pipeline().set_dependencies(dependencies);
				DocumentState.view_node_properties_signal().emit(new_modifiers.front());
			}
		}
	}
}

const transform_modifier create_transform_modifier(k3d::idocument& Document, const k3d::uuid& ModifierType, const std::string& ModifierName)
{
	k3d::inode* const object = k3d::plugin::create<k3d::inode>(ModifierType, Document, ModifierName);
	return_val_if_fail(object, transform_modifier());
	k3d::itransform_sink* const sink = dynamic_cast<k3d::itransform_sink*>(object);
	return_val_if_fail(sink, transform_modifier());
	k3d::itransform_source* const source = dynamic_cast<k3d::itransform_source*>(object);
	return_val_if_fail(source, transform_modifier());

	return transform_modifier(*object, *sink, *source);
}

k3d::inode* insert_transform_modifier(k3d::inode& Object, const k3d::uuid& ModifierType, const std::string& ModifierName)
{
	k3d::itransform_sink* const downstream_sink = dynamic_cast<k3d::itransform_sink*>(&Object);
	return_val_if_fail(downstream_sink, 0);

	k3d::iproperty& downstream_input = downstream_sink->transform_sink_input();
	k3d::iproperty* const upstream_output = Object.document().pipeline().dependency(downstream_input);

	const transform_modifier modifier = create_transform_modifier(Object.document(), ModifierType, ModifierName);
	return_val_if_fail(!modifier.empty(), 0);

	k3d::ipipeline::dependencies_t dependencies;
	dependencies.insert(std::make_pair(&modifier.sink->transform_sink_input(), upstream_output));
	dependencies.insert(std::make_pair(&downstream_input, &modifier.source->transform_source_output()));
	Object.document().pipeline().set_dependencies(dependencies);

	return modifier.node;
}

k3d::inode* upstream_transform_modifier(k3d::inode& Object)
{
	k3d::itransform_sink* const downstream_sink = dynamic_cast<k3d::itransform_sink*>(&Object);
	return_val_if_fail(downstream_sink, 0);

	k3d::iproperty& downstream_input = downstream_sink->transform_sink_input();
	k3d::iproperty* const upstream_output = Object.document().pipeline().dependency(downstream_input);

	// Check upstream object
	if(upstream_output)
		return upstream_output->property_node();

	return 0;
}

const mesh_modifier create_mesh_modifier(k3d::idocument& Document, const k3d::uuid& ModifierType, const std::string& ModifierName)
{
	k3d::inode* const object = k3d::plugin::create<k3d::inode>(ModifierType, Document, ModifierName);
	return_val_if_fail(object, mesh_modifier());
	k3d::imesh_sink* const sink = dynamic_cast<k3d::imesh_sink*>(object);
	return_val_if_fail(sink, mesh_modifier());
	k3d::imesh_source* const source = dynamic_cast<k3d::imesh_source*>(object);
	return_val_if_fail(source, mesh_modifier());

	return mesh_modifier(*object, *sink, *source);
}

k3d::inode* insert_mesh_modifier(k3d::inode& Object, const k3d::uuid& ModifierType, const std::string& ModifierName)
{
	k3d::imesh_sink* const downstream_sink = dynamic_cast<k3d::imesh_sink*>(&Object);
	return_val_if_fail(downstream_sink, 0);

	k3d::iproperty& downstream_input = downstream_sink->mesh_sink_input();
	k3d::iproperty* const upstream_output = Object.document().pipeline().dependency(downstream_input);

	const mesh_modifier modifier = create_mesh_modifier(Object.document(), ModifierType, ModifierName);
	return_val_if_fail(!modifier.empty(), 0);

	k3d::ipipeline::dependencies_t dependencies;
	dependencies.insert(std::make_pair(&modifier.sink->mesh_sink_input(), upstream_output));
	dependencies.insert(std::make_pair(&downstream_input, &modifier.source->mesh_source_output()));
	Object.document().pipeline().set_dependencies(dependencies, modifier.source->hint());

	return modifier.node;
}

k3d::inode* upstream_mesh_modifier(k3d::inode& Object)
{
	k3d::imesh_sink* const downstream_sink = dynamic_cast<k3d::imesh_sink*>(&Object);
	return_val_if_fail(downstream_sink, 0);

	k3d::iproperty& downstream_input = downstream_sink->mesh_sink_input();
	k3d::iproperty* const upstream_output = Object.document().pipeline().dependency(downstream_input);

	// Check upstream object
	if(upstream_output)
		return upstream_output->property_node();

	return 0;
}

} // namespace ngui

} // namespace k3d

