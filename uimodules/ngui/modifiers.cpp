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

#include <k3dsdk/create_plugins.h>
#include <k3dsdk/i18n.h>
#include <k3dsdk/idag.h>
#include <k3dsdk/idocument.h>
#include <k3dsdk/inode.h>
#include <k3dsdk/imesh_selection_sink.h>
#include <k3dsdk/imesh_sink.h>
#include <k3dsdk/imesh_source.h>
#include <k3dsdk/iproperty.h>
#include <k3dsdk/itransform_sink.h>
#include <k3dsdk/itransform_source.h>
#include <k3dsdk/legacy_mesh.h>
#include <k3dsdk/mesh_selection.h>
#include <k3dsdk/nodes.h>
#include <k3dsdk/plugins.h>
#include <k3dsdk/property.h>
#include <k3dsdk/result.h>
#include <k3dsdk/state_change_set.h>
#include <k3dsdk/string_cast.h>

namespace libk3dngui
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

/// Returns a collection of mesh modifier plugin factories, sorted by name
const factories_t& mesh_modifiers()
{
	static factories_t modifiers;
	if(modifiers.empty())
	{
		const k3d::factories_t data_source_modifiers = k3d::plugins<k3d::imesh_source>();
		const k3d::factories_t data_sink_modifiers = k3d::plugins<k3d::imesh_sink>();
		std::set_intersection(data_source_modifiers.begin(), data_source_modifiers.end(), data_sink_modifiers.begin(), data_sink_modifiers.end(), std::inserter(modifiers, modifiers.end()));
		std::sort(modifiers.begin(), modifiers.end(), detail::sort_by_name());
	}

	return modifiers;
}

/// Returns a collection of transform modifier plugin factories, sorted by name
const factories_t& transform_modifiers()
{
	static factories_t modifiers;
	if(modifiers.empty())
	{
		const k3d::factories_t data_source_modifiers = k3d::plugins<k3d::itransform_source>();
		const k3d::factories_t data_sink_modifiers = k3d::plugins<k3d::itransform_sink>();
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

	k3d::iproperty* const upstream_output = Document.dag().dependency(downstream_input);

	k3d::inode* modifier = 0;

	// This block is recorded for undo purposes ...
	{
		k3d::record_state_change_set changeset(Document, k3d::string_cast(boost::format(_("Add Modifier %1%")) % Modifier->name()), __PRETTY_FUNCTION__);

		// Create our modifier object ...
		modifier = k3d::create_plugin<k3d::inode>(*Modifier, Document, k3d::unique_name(Document.nodes(), Modifier->name()));
		return_val_if_fail(modifier, 0);

		// Get its input and output properties ...
		k3d::itransform_sink* const modifier_sink = dynamic_cast<k3d::itransform_sink*>(modifier);
		return_val_if_fail(modifier_sink, 0);
		k3d::itransform_source* const modifier_source = dynamic_cast<k3d::itransform_source*>(modifier);
		return_val_if_fail(modifier_source, 0);

		// Insert the modifier into the DAG ...
		k3d::idag::dependencies_t dependencies;
		if(upstream_output)
			dependencies.insert(std::make_pair(&modifier_sink->transform_sink_input(), upstream_output));
		dependencies.insert(std::make_pair(&downstream_input, &modifier_source->transform_source_output()));
		Document.dag().set_dependencies(dependencies);
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

	k3d::iproperty* const upstream_output = document.dag().dependency(downstream_input);
	return_val_if_fail(upstream_output, 0);

	k3d::inode* modifier = 0;

	// This block is recorded for undo purposes ...
	{
		k3d::record_state_change_set changeset(document, k3d::string_cast(boost::format(_("Add Modifier %1%")) % Modifier->name()), __PRETTY_FUNCTION__);

		// Create our modifier object ...
		modifier = k3d::create_plugin<k3d::inode>(*Modifier, document, k3d::unique_name(document.nodes(), Modifier->name()));
		return_val_if_fail(modifier, 0);

		// Get its input and output properties ...
		k3d::imesh_sink* const modifier_sink = dynamic_cast<k3d::imesh_sink*>(modifier);
		return_val_if_fail(modifier_sink, 0);
		k3d::imesh_source* const modifier_source = dynamic_cast<k3d::imesh_source*>(modifier);
		return_val_if_fail(modifier_source, 0);

		// Insert the modifier into the pipeline ...
		k3d::idag::dependencies_t dependencies;
		dependencies.insert(std::make_pair(&modifier_sink->mesh_sink_input(), upstream_output));
		dependencies.insert(std::make_pair(&downstream_input, &modifier_source->mesh_source_output()));
		document.dag().set_dependencies(dependencies);

		// If the modifier is a mesh selection sink, set its selection state ...
		k3d::imesh_selection_sink* const modifier_mesh_selection_sink = dynamic_cast<k3d::imesh_selection_sink*>(modifier);
		k3d::imesh_selection_sink* const downstream_mesh_selection_sink = dynamic_cast<k3d::imesh_selection_sink*>(&Node);
		if(modifier_mesh_selection_sink && downstream_mesh_selection_sink)
		{
			if(SELECT_NODES == DocumentState.selection_mode().value())
			{
				k3d::set_value(
					modifier_mesh_selection_sink->mesh_selection_sink_input(),
					k3d::mesh_selection::select_all());
			}
			else
			{
				k3d::set_value(
					modifier_mesh_selection_sink->mesh_selection_sink_input(),
					downstream_mesh_selection_sink->mesh_selection_sink_input().property_value());
			}
	
			k3d::set_value(
				downstream_mesh_selection_sink->mesh_selection_sink_input(),
				k3d::mesh_selection::select_null());
		}
	}

	return modifier;
}

const transform_modifier create_transform_modifier(k3d::idocument& Document, const k3d::uuid& ModifierType, const std::string& ModifierName)
{
	k3d::inode* const object = k3d::create_plugin<k3d::inode>(ModifierType, Document, ModifierName);
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
	k3d::iproperty* const upstream_output = Object.document().dag().dependency(downstream_input);

	const transform_modifier modifier = create_transform_modifier(Object.document(), ModifierType, ModifierName);
	return_val_if_fail(!modifier.empty(), 0);

	k3d::idag::dependencies_t dependencies;
	dependencies.insert(std::make_pair(&modifier.sink->transform_sink_input(), upstream_output));
	dependencies.insert(std::make_pair(&downstream_input, &modifier.source->transform_source_output()));
	Object.document().dag().set_dependencies(dependencies);

	return modifier.node;
}

k3d::inode* upstream_transform_modifier(k3d::inode& Object)
{
	k3d::itransform_sink* const downstream_sink = dynamic_cast<k3d::itransform_sink*>(&Object);
	return_val_if_fail(downstream_sink, 0);

	k3d::iproperty& downstream_input = downstream_sink->transform_sink_input();
	k3d::iproperty* const upstream_output = Object.document().dag().dependency(downstream_input);

	// Check upstream object
	if(upstream_output)
		return upstream_output->property_node();

	return 0;
}

const mesh_modifier create_mesh_modifier(k3d::idocument& Document, const k3d::uuid& ModifierType, const std::string& ModifierName)
{
	k3d::inode* const object = k3d::create_plugin<k3d::inode>(ModifierType, Document, ModifierName);
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
	k3d::iproperty* const upstream_output = Object.document().dag().dependency(downstream_input);

	const mesh_modifier modifier = create_mesh_modifier(Object.document(), ModifierType, ModifierName);
	return_val_if_fail(!modifier.empty(), 0);

	k3d::idag::dependencies_t dependencies;
	dependencies.insert(std::make_pair(&modifier.sink->mesh_sink_input(), upstream_output));
	dependencies.insert(std::make_pair(&downstream_input, &modifier.source->mesh_source_output()));
	Object.document().dag().set_dependencies(dependencies, modifier.source->hint());

	return modifier.node;
}

k3d::inode* upstream_mesh_modifier(k3d::inode& Object)
{
	k3d::imesh_sink* const downstream_sink = dynamic_cast<k3d::imesh_sink*>(&Object);
	return_val_if_fail(downstream_sink, 0);

	k3d::iproperty& downstream_input = downstream_sink->mesh_sink_input();
	k3d::iproperty* const upstream_output = Object.document().dag().dependency(downstream_input);

	// Check upstream object
	if(upstream_output)
		return upstream_output->property_node();

	return 0;
}

} // namespace libk3dngui

