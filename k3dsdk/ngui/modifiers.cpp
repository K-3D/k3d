// K-3D
// Copyright (c) 1995-2009, Timothy M. Shead
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

#include <k3d-i18n-config.h>
#include <k3dsdk/classes.h>
#include <k3dsdk/geometry.h>
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
#include <k3dsdk/ngui/document_state.h>
#include <k3dsdk/ngui/modifiers.h>
#include <k3dsdk/ngui/selection.h>
#include <k3dsdk/nodes.h>
#include <k3dsdk/plugins.h>
#include <k3dsdk/plugins.h>
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
	bool operator()(iplugin_factory* LHS, iplugin_factory* RHS)
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
		const plugin::factory::collection_t data_source_modifiers = plugin::factory::lookup<imesh_source>();
		const plugin::factory::collection_t data_sink_modifiers = plugin::factory::lookup<imesh_sink>();
		const plugin::factory::collection_t multi_sink_modifiers = plugin::factory::lookup<imulti_mesh_sink>();
		const plugin::factory::collection_t scripted_modifiers = plugin::factory::lookup("k3d:plugin-type", "MeshModifierScript");
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
		const plugin::factory::collection_t data_source_modifiers = plugin::factory::lookup<itransform_source>();
		const plugin::factory::collection_t data_sink_modifiers = plugin::factory::lookup<itransform_sink>();
		std::set_intersection(data_source_modifiers.begin(), data_source_modifiers.end(), data_sink_modifiers.begin(), data_sink_modifiers.end(), std::inserter(modifiers, modifiers.end()));
		std::sort(modifiers.begin(), modifiers.end(), detail::sort_by_name());
	}

	return modifiers;
}

/// Modify transformation
inode* modify_transformation(idocument& Document, inode& Object, iplugin_factory* Modifier)
{
	return_val_if_fail(Modifier, 0);

	// Get the upstream and downstream properties ...
	itransform_sink* const downstream_sink = dynamic_cast<itransform_sink*>(&Object);
	return_val_if_fail(downstream_sink, 0);

	iproperty& downstream_input = downstream_sink->transform_sink_input();

	iproperty* const upstream_output = Document.pipeline().dependency(downstream_input);

	inode* modifier = 0;

	// This block is recorded for undo purposes ...
	{
		record_state_change_set changeset(Document, string_cast(boost::format(_("Add Modifier %1%")) % Modifier->name()), K3D_CHANGE_SET_CONTEXT);

		// Create our modifier object ...
		modifier = plugin::create<inode>(*Modifier, Document, unique_name(Document.nodes(), Modifier->name()));
		return_val_if_fail(modifier, 0);

		// Get its input and output properties ...
		itransform_sink* const modifier_sink = dynamic_cast<itransform_sink*>(modifier);
		return_val_if_fail(modifier_sink, 0);
		itransform_source* const modifier_source = dynamic_cast<itransform_source*>(modifier);
		return_val_if_fail(modifier_source, 0);

		// Insert the modifier into the DAG ...
		ipipeline::dependencies_t dependencies;
		if(upstream_output)
			dependencies.insert(std::make_pair(&modifier_sink->transform_sink_input(), upstream_output));
		dependencies.insert(std::make_pair(&downstream_input, &modifier_source->transform_source_output()));
		Document.pipeline().set_dependencies(dependencies);
	}

	return modifier;
}

/// Modify mesh
inode* modify_mesh(document_state& DocumentState, inode& Node, iplugin_factory* Modifier)
{
	return_val_if_fail(Modifier, 0);

	idocument& document = DocumentState.document();
	
	// Get the upstream and downstream properties ...
	imesh_sink* const downstream_sink = dynamic_cast<imesh_sink*>(&Node);
	return_val_if_fail(downstream_sink, 0);

	iproperty& downstream_input = downstream_sink->mesh_sink_input();

	iproperty* const upstream_output = document.pipeline().dependency(downstream_input);
	return_val_if_fail(upstream_output, 0);

	inode* modifier = 0;

	// This block is recorded for undo purposes ...
	{
		record_state_change_set changeset(document, string_cast(boost::format(_("Add Modifier %1%")) % Modifier->name()), K3D_CHANGE_SET_CONTEXT);

		// Create our modifier object ...
		modifier = plugin::create<inode>(*Modifier, document, unique_name(document.nodes(), Modifier->name()));
		return_val_if_fail(modifier, 0);

		// Get its input and output properties ...
		imesh_sink* const modifier_sink = dynamic_cast<imesh_sink*>(modifier);
		return_val_if_fail(modifier_sink, 0);
		imesh_source* const modifier_source = dynamic_cast<imesh_source*>(modifier);
		return_val_if_fail(modifier_source, 0);

		// Insert the modifier into the pipeline ...
		ipipeline::dependencies_t dependencies;
		dependencies.insert(std::make_pair(&modifier_sink->mesh_sink_input(), upstream_output));
		dependencies.insert(std::make_pair(&downstream_input, &modifier_source->mesh_source_output()));
		document.pipeline().set_dependencies(dependencies);

		// If the modifier is a mesh selection sink, set its selection state ...
		imesh_selection_sink* const modifier_mesh_selection_sink = dynamic_cast<imesh_selection_sink*>(modifier);
		imesh_selection_sink* const downstream_mesh_selection_sink = dynamic_cast<imesh_selection_sink*>(&Node);
		if(modifier_mesh_selection_sink && downstream_mesh_selection_sink)
		{
			if(selection::NODES == selection::state(DocumentState.document()).current_mode())
			{
				k3d::selection::set select_all;
				geometry::reset_selection(select_all, 1.0);

				property::set_internal_value(
					modifier_mesh_selection_sink->mesh_selection_sink_input(),
					select_all);
			}
			else
			{
				property::set_internal_value(
					modifier_mesh_selection_sink->mesh_selection_sink_input(),
					downstream_mesh_selection_sink->mesh_selection_sink_input().property_internal_value());
			}
	
			property::set_internal_value(
				downstream_mesh_selection_sink->mesh_selection_sink_input(),
				k3d::selection::set());
		}
	}
	
	// Give nodes a chance to initialize their property values based on their inputs, if any ...
	if(ireset_properties* const reset_properties = dynamic_cast<ireset_properties*>(modifier))
		reset_properties->reset_properties();

	return modifier;
}

void modify_selected_meshes(document_state& DocumentState, iplugin_factory* Modifier)
{
	return_if_fail(Modifier);
	idocument& document = DocumentState.document();
	if (Modifier->implements(typeid(imulti_mesh_sink)))
	{ // Mesh modifier taking multiple inputs
		uint_t count = 0;
		ipipeline::dependencies_t dependencies;
		const nodes_t selected_nodes = selection::state(DocumentState.document()).selected_nodes();
		// Create the node
		inode* multi_sink = DocumentState.create_node(Modifier);
		record_state_change_set changeset(document, string_cast(boost::format(_("Add Modifier %1%")) % Modifier->name()), K3D_CHANGE_SET_CONTEXT);
		nodes_t nodes_to_delete;
		for(nodes_t::const_iterator node = selected_nodes.begin(); node != selected_nodes.end(); ++node)
		{
			imesh_sink* const mesh_sink = dynamic_cast<imesh_sink*>(*node);
			if(!mesh_sink)
				continue;
			itransform_sink* const transform_sink = dynamic_cast<itransform_sink*>(*node);

			iproperty* source_mesh = document.pipeline().dependency(mesh_sink->mesh_sink_input());
			if (!source_mesh)
				continue;
			
			if (transform_sink) // Insert a transform node
			{
				iproperty* const source_transformation = document.pipeline().dependency(transform_sink->transform_sink_input());
				if (source_transformation)
				{
					inode* transform_points = plugin::create<inode>("TransformPoints", document, unique_name(document.nodes(), "TransformPoints"));
					return_if_fail(transform_points);
					itransform_sink* transform_points_transform_sink = dynamic_cast<itransform_sink*>(transform_points);
					return_if_fail(transform_points_transform_sink);
					imesh_sink* transform_points_mesh_sink = dynamic_cast<imesh_sink*>(transform_points);
					return_if_fail(transform_points_mesh_sink);
					dependencies.insert(std::make_pair(&transform_points_transform_sink->transform_sink_input(), source_transformation));
					dependencies.insert(std::make_pair(&transform_points_mesh_sink->mesh_sink_input(), source_mesh));
					imesh_source* transform_points_mesh_source = dynamic_cast<imesh_source*>(transform_points);
					return_if_fail(transform_points_mesh_source);
					source_mesh = &transform_points_mesh_source->mesh_source_output();
					imesh_selection_sink* selection_sink = dynamic_cast<imesh_selection_sink*>(transform_points);
					return_if_fail(selection_sink);
assert_not_implemented();
/*
					property::set_internal_value(selection_sink->mesh_selection_sink_input(), mesh_selection::select_all());
*/
				}
			}
			++count;
			// Create a new user property
			std::stringstream name, label;
			name << "input_mesh" << count;
			label << "Input Mesh " << count;
			iproperty* sink = property::get(*multi_sink, name.str());
			if (!sink)
				sink = property::create<mesh*>(*multi_sink, name.str(), label.str(), "", static_cast<mesh*>(0));
			// Store the connection
			dependencies.insert(std::make_pair(sink, source_mesh));
			// Delete the input node
			nodes_to_delete.push_back(*node);
		}
		document.pipeline().set_dependencies(dependencies);
		delete_nodes(document, nodes_to_delete);
		// Give nodes a chance to initialize their property values based on their inputs, if any ...
		if(ireset_properties* const reset_properties = dynamic_cast<ireset_properties*>(multi_sink))
			reset_properties->reset_properties();
		DocumentState.view_node_properties_signal().emit(multi_sink);
	}
	else
	{ // Normal mesh modifier
		nodes_t new_modifiers;
	
		const nodes_t selected_nodes = selection::state(DocumentState.document()).selected_nodes();
		for(nodes_t::const_iterator node = selected_nodes.begin(); node != selected_nodes.end(); ++node)
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
			iproperty_collection* first_property_collection = dynamic_cast<iproperty_collection*>(new_modifiers.front());
			if (first_property_collection)
			{
				// Get the in-and output property names, to exclude them from the connections
				imesh_sink* const modifier_sink = dynamic_cast<imesh_sink*>(new_modifiers.front());
				return_if_fail(modifier_sink);
				imesh_source* const modifier_source = dynamic_cast<imesh_source*>(new_modifiers.front());
				return_if_fail(modifier_source);
				const std::string sink_name = modifier_sink->mesh_sink_input().property_name();
				const std::string source_name = modifier_source->mesh_source_output().property_name();
				
				ipipeline::dependencies_t dependencies;
				const iproperty_collection::properties_t& first_properties = first_property_collection->properties();
				nodes_t::iterator modifier = new_modifiers.begin();
				++modifier;
				for (modifier; modifier != new_modifiers.end(); ++modifier)
				{
					iproperty_collection* property_collection = dynamic_cast<iproperty_collection*>(*modifier);
					return_if_fail(property_collection);
					const iproperty_collection::properties_t& properties = property_collection->properties();
					iproperty_collection::properties_t::const_iterator property = properties.begin();
					for (iproperty_collection::properties_t::const_iterator first_property = first_properties.begin(); first_property != first_properties.end(); ++first_property)
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

const transform_modifier create_transform_modifier(idocument& Document, const uuid& ModifierType, const std::string& ModifierName)
{
	inode* const object = plugin::create<inode>(ModifierType, Document, ModifierName);
	return_val_if_fail(object, transform_modifier());
	itransform_sink* const sink = dynamic_cast<itransform_sink*>(object);
	return_val_if_fail(sink, transform_modifier());
	itransform_source* const source = dynamic_cast<itransform_source*>(object);
	return_val_if_fail(source, transform_modifier());

	return transform_modifier(*object, *sink, *source);
}

inode* insert_transform_modifier(inode& Object, const uuid& ModifierType, const std::string& ModifierName)
{
	itransform_sink* const downstream_sink = dynamic_cast<itransform_sink*>(&Object);
	return_val_if_fail(downstream_sink, 0);

	iproperty& downstream_input = downstream_sink->transform_sink_input();
	iproperty* const upstream_output = Object.document().pipeline().dependency(downstream_input);

	const transform_modifier modifier = create_transform_modifier(Object.document(), ModifierType, ModifierName);
	return_val_if_fail(!modifier.empty(), 0);

	ipipeline::dependencies_t dependencies;
	dependencies.insert(std::make_pair(&modifier.sink->transform_sink_input(), upstream_output));
	dependencies.insert(std::make_pair(&downstream_input, &modifier.source->transform_source_output()));
	Object.document().pipeline().set_dependencies(dependencies);

	return modifier.node;
}

inode* upstream_transform_modifier(inode& Object)
{
	itransform_sink* const downstream_sink = dynamic_cast<itransform_sink*>(&Object);
	return_val_if_fail(downstream_sink, 0);

	iproperty& downstream_input = downstream_sink->transform_sink_input();
	iproperty* const upstream_output = Object.document().pipeline().dependency(downstream_input);

	// Check upstream object
	if(upstream_output)
		return upstream_output->property_node();

	return 0;
}

const mesh_modifier create_mesh_modifier(idocument& Document, const uuid& ModifierType, const std::string& ModifierName)
{
	inode* const object = plugin::create<inode>(ModifierType, Document, ModifierName);
	return_val_if_fail(object, mesh_modifier());
	imesh_sink* const sink = dynamic_cast<imesh_sink*>(object);
	return_val_if_fail(sink, mesh_modifier());
	imesh_source* const source = dynamic_cast<imesh_source*>(object);
	return_val_if_fail(source, mesh_modifier());

	return mesh_modifier(*object, *sink, *source);
}

inode* insert_mesh_modifier(inode& Object, const uuid& ModifierType, const std::string& ModifierName)
{
	imesh_sink* const downstream_sink = dynamic_cast<imesh_sink*>(&Object);
	return_val_if_fail(downstream_sink, 0);

	iproperty& downstream_input = downstream_sink->mesh_sink_input();
	iproperty* const upstream_output = Object.document().pipeline().dependency(downstream_input);

	const mesh_modifier modifier = create_mesh_modifier(Object.document(), ModifierType, ModifierName);
	return_val_if_fail(!modifier.empty(), 0);

	ipipeline::dependencies_t dependencies;
	dependencies.insert(std::make_pair(&modifier.sink->mesh_sink_input(), upstream_output));
	dependencies.insert(std::make_pair(&downstream_input, &modifier.source->mesh_source_output()));
	Object.document().pipeline().set_dependencies(dependencies, modifier.source->hint());

	return modifier.node;
}

inode* upstream_mesh_modifier(inode& Object)
{
	imesh_sink* const downstream_sink = dynamic_cast<imesh_sink*>(&Object);
	return_val_if_fail(downstream_sink, 0);

	iproperty& downstream_input = downstream_sink->mesh_sink_input();
	iproperty* const upstream_output = Object.document().pipeline().dependency(downstream_input);

	// Check upstream object
	if(upstream_output)
		return upstream_output->property_node();

	return 0;
}

} // namespace ngui

} // namespace k3d

