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

#include <gdkmm/cursor.h>
#include <gtkmm/widget.h>

#include "basic_input_model.h"
#include "command_arguments.h"
#include "document_state.h"
#include "modifiers.h"
#include "icons.h"
#include "interactive.h"
#include "navigation_input_model.h"
#include "parent_tool.h"
#include "transform.h"
#include "tutorial_message.h"
#include "viewport.h"

#include <k3dsdk/classes.h>
#include <k3dsdk/i18n.h>
#include <k3dsdk/inode_collection.h>
#include <k3dsdk/iparentable.h>
#include <k3dsdk/iselectable.h>
#include <k3dsdk/itransform_sink.h>
#include <k3dsdk/itransform_source.h>
#include <k3dsdk/iuser_interface.h>
#include <k3dsdk/log.h>
#include <k3dsdk/property.h>
#include <k3dsdk/state_change_set.h>
#include <k3dsdk/transform.h>

namespace libk3dngui
{

/////////////////////////////////////////////////////////////////////////////
// parent_tool::implementation

struct parent_tool::implementation
{
	implementation(document_state& DocumentState) :
		m_document_state(DocumentState),
		m_set_parent(false),
		m_navigation_model(DocumentState)
	{
		m_input_model.connect_lbutton_click(sigc::mem_fun(*this, &implementation::on_lbutton_click));
		m_input_model.connect_rbutton_click(sigc::mem_fun(*this, &implementation::on_rbutton_click));
		
		m_input_model.connect_mbutton_click(sigc::mem_fun(m_navigation_model, &navigation_input_model::on_button1_click));
		m_input_model.connect_mbutton_start_drag(sigc::mem_fun(m_navigation_model, &navigation_input_model::on_button1_start_drag));
		m_input_model.connect_mbutton_drag(sigc::mem_fun(m_navigation_model, &navigation_input_model::on_button1_drag));
		m_input_model.connect_mbutton_end_drag(sigc::mem_fun(m_navigation_model, &navigation_input_model::on_button1_end_drag));
		m_input_model.connect_rbutton_start_drag(sigc::mem_fun(m_navigation_model, &navigation_input_model::on_button2_start_drag));
		m_input_model.connect_rbutton_drag(sigc::mem_fun(m_navigation_model, &navigation_input_model::on_button2_drag));
		m_input_model.connect_rbutton_end_drag(sigc::mem_fun(m_navigation_model, &navigation_input_model::on_button2_end_drag));
		m_input_model.connect_scroll(sigc::mem_fun(m_navigation_model, &navigation_input_model::on_scroll));
	}

	void on_lbutton_click(viewport::control& Viewport, const GdkEventButton& Event)
	{
		if(m_set_parent)
			on_set_parent(Viewport, Event);
		else
			on_pick(Viewport, Event);
	}

	void on_rbutton_click(viewport::control& Viewport, const GdkEventButton& Event)
	{
		command_arguments arguments;
		arguments.append_viewport_coordinates("mouse", Viewport, Event);
		m_command_signal.emit("selection_tool", arguments);

		m_set_parent = false;

		m_document_state.set_active_tool(m_document_state.selection_tool());
	}

	void set_parent(viewport::control& Viewport, k3d::inode& Parent)
	{
		m_set_parent = false;
		m_document_state.clear_cursor_signal().emit();

		k3d::itransform_source* const transform_source = dynamic_cast<k3d::itransform_source*>(&Parent);
		return_if_fail(transform_source);

		const k3d::matrix4 parent_compensation = k3d::inverse(k3d::node_to_world_matrix(Parent));

		k3d::idag::dependencies_t dependencies;

		k3d::record_state_change_set changeset(m_document_state.document(), _("Set Parent"), __PRETTY_FUNCTION__);

		// Note - we enumerate over a *copy* of the set of document nodes, since we're adding nodes as we go ...
		const k3d::nodes_t nodes = m_document_state.document().nodes().collection();
		for(k3d::nodes_t::const_iterator node = nodes.begin(); node != nodes.end(); ++node)
		{
			// Ensure we don't try to parent an object to itself ...
			if(&Parent == *node)
				continue;

			if(!k3d::selection::is_selected(*node))
				continue;

			const transform_history_t history = parent_to_node_history(**node);
			if(!history.empty())
			{
				if(k3d::itransform_sink* const transform_sink = dynamic_cast<k3d::itransform_sink*>(history.front()))
				{
					const transform_modifier modifier = create_transform_modifier(m_document_state.document(), k3d::classes::FrozenTransformation(), "Parent Compensation");
					if(modifier)
					{
						k3d::set_value(*modifier.node, "matrix", parent_compensation);
						dependencies.insert(std::make_pair(&transform_sink->transform_sink_input(), &modifier.source->transform_source_output()));
						dependencies.insert(std::make_pair(&modifier.sink->transform_sink_input(), &transform_source->transform_source_output()));
					}
				}
			}

			if(k3d::iparentable* const parentable = dynamic_cast<k3d::iparentable*>(*node))
			{
				if(k3d::iwritable_property* const writable_parent = dynamic_cast<k3d::iwritable_property*>(&parentable->parent()))
					writable_parent->property_set_value(&Parent);
			}
		}

		m_document_state.deselect_all();
		k3d::selection::select(&Parent);
		m_document_state.document().dag().set_dependencies(dependencies);
	}
	
	void on_set_parent(viewport::control& Viewport, const GdkEventButton& Event)
	{
		const k3d::selection::record selection = Viewport.pick_node(k3d::point2(Event.x, Event.y));
		if(selection.empty())
			return;

		k3d::inode* const node = k3d::selection::get_node(selection);
		if(!node)
			return;

		set_parent(Viewport, *node);

		command_arguments arguments;
		arguments.append_viewport_coordinates("mouse", Viewport, Event);
		arguments.append("selection", selection);
		m_command_signal.emit("set_parent", arguments);
	}

	void on_pick(viewport::control& Viewport, const GdkEventButton& Event)
	{
		const k3d::selection::record selection = Viewport.pick_node(k3d::point2(Event.x, Event.y));
		if(selection.empty())
			return;

		k3d::inode* const node = k3d::selection::get_node(selection);
		return_if_fail(node);

		if(k3d::selection::is_selected(node))
		{
			command_arguments arguments;
			arguments.append_viewport_coordinates("mouse", Viewport, Event);
			m_command_signal.emit("child_selection_complete", arguments);

			m_set_parent = true;
			m_document_state.set_cursor_signal().emit(load_icon("parent_cursor", Gtk::ICON_SIZE_BUTTON));
		}
		else
		{
			command_arguments arguments;
			arguments.append_viewport_coordinates("mouse", Viewport, Event);
			arguments.append("selection", selection);
			m_command_signal.emit("select_node", arguments);

			m_document_state.select(selection);
		}
	}

	const k3d::icommand_node::result execute_command(const std::string& Command, const std::string& Arguments)
	{
		try
		{
			if(Command == "selection_tool")
			{
				command_arguments arguments(Arguments);
				viewport::control& viewport = arguments.get_viewport();
				const k3d::point2 mouse = arguments.get_viewport_point2("mouse");

				interactive::move_pointer(viewport, mouse);

				m_set_parent = false;

				m_document_state.clear_cursor_signal().emit();
				m_document_state.set_active_tool(m_document_state.selection_tool());

				return RESULT_CONTINUE;
			}

			if(Command == "select_node")
			{
				command_arguments arguments(Arguments);
				viewport::control& viewport = arguments.get_viewport();
				const k3d::point2 mouse = arguments.get_viewport_point2("mouse");
				const k3d::selection::record selection = arguments.get_selection_record(m_document_state.document(), "selection");

				interactive::move_pointer(viewport, mouse);

				m_document_state.select(selection);

				return RESULT_CONTINUE;
			}

			if(Command == "child_selection_complete")
			{
				command_arguments arguments(Arguments);
				viewport::control& viewport = arguments.get_viewport();
				const k3d::point2 mouse = arguments.get_viewport_point2("mouse");

				interactive::move_pointer(viewport, mouse);

				m_set_parent = true;

				m_document_state.set_cursor_signal().emit(load_icon("parent_cursor", Gtk::ICON_SIZE_BUTTON));

				return RESULT_CONTINUE;
			}

			if(Command == "set_parent")
			{
				command_arguments arguments(Arguments);
				viewport::control& viewport = arguments.get_viewport();
				const k3d::point2 mouse = arguments.get_viewport_point2("mouse");
				const k3d::selection::record selection = arguments.get_selection_record(m_document_state.document(), "selection");
			
				k3d::inode* const node = k3d::selection::get_node(selection);
				return_val_if_fail(node, RESULT_ERROR);

				interactive::move_pointer(viewport, mouse);
				
				set_parent(viewport, *node);

				return RESULT_CONTINUE;
			}
		}
		catch(std::exception& e)
		{
			k3d::log() << error << k3d_file_reference << ": caught exception: " << e.what() << std::endl;
			return RESULT_ERROR;
		}

		return RESULT_UNKNOWN_COMMAND;
	}

	/// Stores a reference to the owning document
	document_state& m_document_state;
	/// Set to true iff the next pick should choose a new parent for all selected objects
	bool m_set_parent;
	/// Provides interactive navigation behavior
	navigation_input_model m_navigation_model;
	/// Dispatches incoming user input events
	basic_input_model m_input_model;
	/// Emitted to record a command-node command
	sigc::signal<void, const std::string&, const std::string&> m_command_signal;
};

/////////////////////////////////////////////////////////////////////////////
// parent_tool

parent_tool::parent_tool(document_state& DocumentState, const std::string& Name) :
	base(DocumentState, Name),
	m_implementation(new implementation(DocumentState))
{
	m_implementation->m_navigation_model.connect_command_signal(sigc::mem_fun(*this, &parent_tool::record_command));
	m_implementation->m_command_signal.connect(sigc::mem_fun(*this, &parent_tool::record_command));
}

parent_tool::~parent_tool()
{
	delete m_implementation;
}

const k3d::icommand_node::result parent_tool::execute_command(const std::string& Command, const std::string& Arguments)
{
	return m_implementation->execute_command(Command, Arguments);
}

void parent_tool::on_deactivate()
{
	m_implementation->m_document_state.clear_cursor_signal().emit();
}

iuser_input_model& parent_tool::get_input_model()
{
	return m_implementation->m_input_model;
}

} // namespace libk3dngui

