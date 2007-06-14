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
		\author Romain Behar (romainbehar@yahoo.com)
*/

#include <gdkmm/cursor.h>
#include <gtkmm/widget.h>

#include "command_arguments.h"
#include "document_state.h"
#include "icons.h"
#include "interactive.h"
#include "keyboard.h"
#include "rubber_band.h"
#include "selection.h"
#include "selection_input_model.h"
#include "tutorial_message.h"
#include "utility.h"
#include "viewport.h"

#include <k3dsdk/color.h>
#include <k3dsdk/geometry.h>
#include <k3dsdk/high_res_timer.h>
#include <k3dsdk/i18n.h>
#include <k3dsdk/icamera.h>
#include <k3dsdk/itransform_source.h>
#include <k3dsdk/legacy_mesh.h>
#include <k3dsdk/property.h>
#include <k3dsdk/property_collection.h>
#include <k3dsdk/system.h>

namespace libk3dngui
{

/////////////////////////////////////////////////////////////////////////////
// selection_input_model::implementation

struct selection_input_model::implementation :
	public k3d::property_collection
{
	implementation(document_state& DocumentState) :
		m_document_state(DocumentState),
		m_rubber_band(k3d::color(0.8, 0, 0)),
		m_start_selection(k3d::selection::record::empty_record()),
		m_motion_type(MOTION_NONE),
		m_extended_mode(true),
		m_extended_component_mode(true),
		m_paint_mode(true),
		m_double_click_mode(true)
	{
	}

	k3d::idocument& document()
	{
		return m_document_state.document();
	}

	void on_button_down(viewport::control& Viewport, const GdkEventButton& Event)
	{
		m_start_selection = Viewport.pick_object(k3d::point2(Event.x, Event.y));

k3d::log() << debug << m_start_selection << std::endl;

		m_motion_type = MOTION_NONE;
	}

	void on_button_click(viewport::control& Viewport, const GdkEventButton& Event)
	{
		const k3d::key_modifiers modifiers = convert(Event.state);

		enum
		{
			NONE,
			SELECT,
			DESELECT,
			REPLACE
		} operation = NONE;

		if(modifiers.shift())
		{
			operation = SELECT;
		}
		else if(modifiers.control())
		{
			operation = DESELECT;
		}
		else
		{
			operation = REPLACE;

			// If something's under the mouse cursor
			if(k3d::selection::get_node(m_start_selection))
			{
				bool extended_mode;
				switch(m_document_state.selection_mode().value())
				{
					case SELECT_NODES:
						// Extended selection
						extended_mode = m_extended_mode;
					break;

					default:
						// Extended component selection
						extended_mode = m_extended_component_mode;
				}

				if(extended_mode)
					operation = m_document_state.is_selected(m_start_selection) ? DESELECT : SELECT;
			}
		}

		switch(operation)
		{
			case NONE:
				break;

			case SELECT:
			{
				command_arguments arguments;
				arguments.append_viewport_coordinates("mouse", Viewport, Event);
				arguments.append("selection", m_start_selection);
				m_command_signal.emit("pick_select", arguments);

				k3d::record_state_change_set change_set(m_document_state.document(), _("Select"), K3D_CHANGE_SET_CONTEXT);
				m_document_state.select(m_start_selection);
				break;
			}

			case DESELECT:
			{
				command_arguments arguments;
				arguments.append_viewport_coordinates("mouse", Viewport, Event);
				arguments.append("selection", m_start_selection);
				m_command_signal.emit("pick_deselect", arguments);

				k3d::record_state_change_set change_set(m_document_state.document(), _("Deselect"), K3D_CHANGE_SET_CONTEXT);
				m_document_state.deselect(m_start_selection);
				break;
			}

			case REPLACE:
			{
				command_arguments arguments;
				arguments.append_viewport_coordinates("mouse", Viewport, Event);
				arguments.append("selection", m_start_selection);
				m_command_signal.emit("pick_replace", arguments);

				k3d::record_state_change_set change_set(m_document_state.document(), _("Replace"), K3D_CHANGE_SET_CONTEXT);
				m_document_state.deselect_all();
				m_document_state.select(m_start_selection);
				break;
			}
		}
	}

	void on_button_double_click(viewport::control& Viewport, const GdkEventButton& Event)
	{
		if(!m_double_click_mode)
			return;

		switch(m_document_state.selection_mode().value())
		{
			case SELECT_NODES:
			{
				command_arguments arguments;
				arguments.append_viewport_coordinates("mouse", Viewport, Event);
				m_command_signal.emit("deselect_all", arguments);

				k3d::record_state_change_set change_set(m_document_state.document(), _("Deselect All"), K3D_CHANGE_SET_CONTEXT);
				m_document_state.deselect_all();
				break;
			}

			default:
			{
				command_arguments arguments;
				arguments.append_viewport_coordinates("mouse", Viewport, Event);
				m_command_signal.emit("node_selection", arguments);

				k3d::record_state_change_set change_set(m_document_state.document(), _("Node Selection"), K3D_CHANGE_SET_CONTEXT);
				m_document_state.set_selection_mode(SELECT_NODES);
				break;
			}
		}
	}

	void on_button_start_drag(viewport::control& Viewport, const GdkEventMotion& Event)
	{
		m_timer.restart();

		const k3d::key_modifiers modifiers = convert(Event.state);

		if(modifiers.shift())
		{
			//m_motion_type = (m_paint_mode && !m_start_selection.empty()) ? MOTION_PAINT_SELECT : MOTION_RUBBER_BAND_SELECT;
			m_motion_type = MOTION_RUBBER_BAND_SELECT;
		}
		else if(modifiers.control())
		{
			//m_motion_type = (m_paint_mode && !m_start_selection.empty()) ? MOTION_PAINT_DESELECT : MOTION_RUBBER_BAND_DESELECT;
			m_motion_type = MOTION_RUBBER_BAND_DESELECT;
		}
		else
		{
			if(m_start_selection.empty())
			{
				m_motion_type = MOTION_RUBBER_BAND_REPLACE;
			}
			else
			{
				if(m_paint_mode)
				{
					m_motion_type = m_document_state.is_selected(m_start_selection) ? MOTION_PAINT_DESELECT : MOTION_PAINT_SELECT;
				}
				else
				{
					m_motion_type = m_document_state.is_selected(m_start_selection) ? MOTION_RUBBER_BAND_DESELECT : MOTION_RUBBER_BAND_SELECT;
				}
			}
		}

		switch(m_motion_type)
		{
			case MOTION_NONE:
				break;

			case MOTION_PAINT_SELECT:
			{
				command_arguments arguments;
				arguments.append_viewport_coordinates("mouse", Viewport, Event);
				arguments.append("selection", m_start_selection);
				m_command_signal.emit("start_paint_select", arguments);

				k3d::start_state_change_set(m_document_state.document(), K3D_CHANGE_SET_CONTEXT);
				m_document_state.select(m_start_selection);

				break;
			}

			case MOTION_PAINT_DESELECT:
			{
				command_arguments arguments;
				arguments.append_viewport_coordinates("mouse", Viewport, Event);
				arguments.append("selection", m_start_selection);
				m_command_signal.emit("start_paint_deselect", arguments);

				k3d::start_state_change_set(m_document_state.document(), K3D_CHANGE_SET_CONTEXT);
				m_document_state.deselect(m_start_selection);

				break;
			}

			case MOTION_RUBBER_BAND_REPLACE:
			{
				m_rubber_band.box = k3d::rectangle(Event.x, Event.x, Event.y, Event.y);
				m_rubber_band.draw(Viewport);

				command_arguments arguments;
				arguments.append_viewport_coordinates("mouse", Viewport, Event);
				arguments.append("box", m_rubber_band.box);
				m_command_signal.emit("start_rubber_band_replace", arguments);

				break;
			}

			case MOTION_RUBBER_BAND_SELECT:
			{
				m_rubber_band.box = k3d::rectangle(Event.x, Event.x, Event.y, Event.y);
				m_rubber_band.draw(Viewport);

				command_arguments arguments;
				arguments.append_viewport_coordinates("mouse", Viewport, Event);
				arguments.append_viewport_coordinates("box", Viewport, m_rubber_band.box);
				m_command_signal.emit("start_rubber_band_select", arguments);

				break;
			}

			case MOTION_RUBBER_BAND_DESELECT:
			{
				m_rubber_band.box = k3d::rectangle(Event.x, Event.x, Event.y, Event.y);
				m_rubber_band.draw(Viewport);

				command_arguments arguments;
				arguments.append_viewport_coordinates("mouse", Viewport, Event);
				arguments.append("box", m_rubber_band.box);
				m_command_signal.emit("start_rubber_band_deselect", arguments);

				break;
			}

		}
	}

	void on_button_drag(viewport::control& Viewport, const GdkEventMotion& Event)
	{
		const k3d::point2 coordinates(Event.x, Event.y);

		switch(m_motion_type)
		{
			case MOTION_NONE:
				break;

			case MOTION_PAINT_SELECT:
			{
				const k3d::selection::record selection = Viewport.pick_object(coordinates);
				if(!m_document_state.is_selected(selection))
				{
					command_arguments arguments;
					arguments.append_viewport_coordinates("mouse", Viewport, Event);
					arguments.append("timestamp", m_timer.elapsed());
					arguments.append("selection", selection);
					m_command_signal.emit("paint_select", arguments);

					m_document_state.select(selection);
				}
				else
				{
					command_arguments arguments;
					arguments.append_viewport_coordinates("mouse", Viewport, Event);
					arguments.append("timestamp", m_timer.elapsed());
					m_command_signal.emit("paint_motion", arguments);
				}
				break;
			}

			case MOTION_PAINT_DESELECT:
			{
				const k3d::selection::record selection = Viewport.pick_object(coordinates);
				if(m_document_state.is_selected(selection))
				{
					command_arguments arguments;
					arguments.append_viewport_coordinates("mouse", Viewport, Event);
					arguments.append("timestamp", m_timer.elapsed());
					arguments.append("selection", selection);
					m_command_signal.emit("paint_deselect", arguments);

					m_document_state.deselect(selection);
				}
				else
				{
					command_arguments arguments;
					arguments.append_viewport_coordinates("mouse", Viewport, Event);
					arguments.append("timestamp", m_timer.elapsed());
					m_command_signal.emit("paint_motion", arguments);
				}
				break;
			}

			case MOTION_RUBBER_BAND_REPLACE:
			case MOTION_RUBBER_BAND_SELECT:
			case MOTION_RUBBER_BAND_DESELECT:
			{
				m_rubber_band.draw(Viewport);
				m_rubber_band.box.right = Event.x;
				m_rubber_band.box.bottom = Event.y;
				m_rubber_band.draw(Viewport);

				command_arguments arguments;
				arguments.append_viewport_coordinates("mouse", Viewport, Event);
				arguments.append("timestamp", m_timer.elapsed());
				arguments.append_viewport_coordinates("box", Viewport, m_rubber_band.box);
				m_command_signal.emit("rubber_band_motion", arguments);
				break;
			}
		}
	}

	void on_button_end_drag(viewport::control& Viewport, const GdkEventButton& Event)
	{
		switch(m_motion_type)
		{
			case MOTION_NONE:
				break;

			case MOTION_PAINT_SELECT:
			{
				command_arguments arguments;
				arguments.append_viewport_coordinates("mouse", Viewport, Event);
				m_command_signal.emit("end_paint_select", arguments);
				k3d::finish_state_change_set(m_document_state.document(), _("Paint Select"), K3D_CHANGE_SET_CONTEXT);
				break;
			}

			case MOTION_PAINT_DESELECT:
			{
				command_arguments arguments;
				arguments.append_viewport_coordinates("mouse", Viewport, Event);
				m_command_signal.emit("end_paint_deselect", arguments);
				k3d::finish_state_change_set(m_document_state.document(), _("Paint Deselect"), K3D_CHANGE_SET_CONTEXT);
				break;
			}

			case MOTION_RUBBER_BAND_REPLACE:
			{
				const k3d::selection::records selection = Viewport.get_selectable_objects(m_rubber_band.box);

				command_arguments arguments;
				arguments.append_viewport_coordinates("mouse", Viewport, Event);
				arguments.append("selection", selection);
				m_command_signal.emit("end_rubber_band_replace", arguments);

				m_rubber_band.draw(Viewport);

				k3d::record_state_change_set change_set(m_document_state.document(), _("Rubber Band Replace"), K3D_CHANGE_SET_CONTEXT);
				m_document_state.deselect_all();
				m_document_state.select(selection);
				break;
			}

			case MOTION_RUBBER_BAND_SELECT:
			{
				const k3d::selection::records selection = Viewport.get_selectable_objects(m_rubber_band.box);

				command_arguments arguments;
				arguments.append_viewport_coordinates("mouse", Viewport, Event);
				arguments.append("selection", selection);
				m_command_signal.emit("end_rubber_band_select", arguments);

				m_rubber_band.draw(Viewport);

				k3d::record_state_change_set change_set(m_document_state.document(), _("Rubber Band Select"), K3D_CHANGE_SET_CONTEXT);
				m_document_state.select(selection);
				break;
			}

			case MOTION_RUBBER_BAND_DESELECT:
			{
				const k3d::selection::records selection = Viewport.get_selectable_objects(m_rubber_band.box);

				command_arguments arguments;
				arguments.append_viewport_coordinates("mouse", Viewport, Event);
				arguments.append("selection", selection);
				m_command_signal.emit("end_rubber_band_deselect", arguments);

				m_rubber_band.draw(Viewport);

				k3d::record_state_change_set change_set(m_document_state.document(), _("Rubber Band Deselect"), K3D_CHANGE_SET_CONTEXT);
				m_document_state.deselect(selection);
				break;
			}
		}
		m_motion_type = MOTION_NONE;
	}

	const k3d::icommand_node::result execute_command(const std::string& Command, const std::string& Arguments)
	{
		try
		{
			if(Command == "pick_select")
			{
				command_arguments arguments(Arguments);
				viewport::control& viewport = arguments.get_viewport();
				const k3d::point2 mouse = arguments.get_viewport_point2("mouse");
				const k3d::selection::record selection = arguments.get_selection_record(m_document_state.document(), "selection");

				interactive::move_pointer(viewport, mouse);

				k3d::record_state_change_set change_set(m_document_state.document(), _("Select"), K3D_CHANGE_SET_CONTEXT);
				m_document_state.select(selection);

				return k3d::icommand_node::RESULT_CONTINUE;
			}

			if(Command == "pick_deselect")
			{
				command_arguments arguments(Arguments);
				viewport::control& viewport = arguments.get_viewport();
				const k3d::point2 mouse = arguments.get_viewport_point2("mouse");
				const k3d::selection::record selection = arguments.get_selection_record(m_document_state.document(), "selection");

				interactive::move_pointer(viewport, mouse);

				k3d::record_state_change_set change_set(m_document_state.document(), _("Deselect"), K3D_CHANGE_SET_CONTEXT);
				m_document_state.deselect(selection);

				return k3d::icommand_node::RESULT_CONTINUE;
			}

			if(Command == "pick_replace")
			{
				command_arguments arguments(Arguments);
				viewport::control& viewport = arguments.get_viewport();
				const k3d::point2 mouse = arguments.get_viewport_point2("mouse");
				const k3d::selection::record selection = arguments.get_selection_record(m_document_state.document(), "selection");

				interactive::move_pointer(viewport, mouse);

				k3d::record_state_change_set change_set(m_document_state.document(), _("Replace Selection"), K3D_CHANGE_SET_CONTEXT);
				m_document_state.deselect_all();
				m_document_state.select(selection);

				return k3d::icommand_node::RESULT_CONTINUE;
			}

			if(Command == "deselect_all")
			{
				command_arguments arguments(Arguments);
				viewport::control& viewport = arguments.get_viewport();
				const k3d::point2 mouse = arguments.get_viewport_point2("mouse");

				interactive::move_pointer(viewport, mouse);

				k3d::record_state_change_set change_set(m_document_state.document(), _("Deselect All"), K3D_CHANGE_SET_CONTEXT);
				m_document_state.deselect_all();

				return k3d::icommand_node::RESULT_CONTINUE;
			}

			if(Command == "node_selection")
			{
				command_arguments arguments(Arguments);
				viewport::control& viewport = arguments.get_viewport();
				const k3d::point2 mouse = arguments.get_viewport_point2("mouse");

				interactive::move_pointer(viewport, mouse);

				k3d::record_state_change_set change_set(m_document_state.document(), _("Node Selection"), K3D_CHANGE_SET_CONTEXT);
				m_document_state.set_selection_mode(SELECT_NODES);

				return k3d::icommand_node::RESULT_CONTINUE;
			}

			if(Command == "start_paint_select")
			{
				command_arguments arguments(Arguments);
				viewport::control& viewport = arguments.get_viewport();
				const k3d::point2 mouse = arguments.get_viewport_point2("mouse");
				const k3d::selection::record selection = arguments.get_selection_record(m_document_state.document(), "selection");

				interactive::move_pointer(viewport, mouse);

				k3d::start_state_change_set(m_document_state.document(), K3D_CHANGE_SET_CONTEXT);
				m_document_state.select(selection);

				m_timer.restart();

				return k3d::icommand_node::RESULT_CONTINUE;
			}

			if(Command == "start_paint_deselect")
			{
				command_arguments arguments(Arguments);
				viewport::control& viewport = arguments.get_viewport();
				const k3d::point2 mouse = arguments.get_viewport_point2("mouse");
				const k3d::selection::record selection = arguments.get_selection_record(m_document_state.document(), "selection");

				interactive::move_pointer(viewport, mouse);

				k3d::start_state_change_set(m_document_state.document(), K3D_CHANGE_SET_CONTEXT);
				m_document_state.deselect(selection);

				m_timer.restart();

				return k3d::icommand_node::RESULT_CONTINUE;
			}

			if(Command == "paint_motion")
			{
				command_arguments arguments(Arguments);
				viewport::control& viewport = arguments.get_viewport();
				const k3d::point2 mouse = arguments.get_viewport_point2("mouse");
				const double timestamp = arguments.get_double("timestamp");

				interactive::warp_pointer(viewport, mouse, timestamp, m_timer);

				return k3d::icommand_node::RESULT_CONTINUE;
			}

			if(Command == "paint_select")
			{
				command_arguments arguments(Arguments);
				viewport::control& viewport = arguments.get_viewport();
				const k3d::point2 mouse = arguments.get_viewport_point2("mouse");
				const double timestamp = arguments.get_double("timestamp");
				const k3d::selection::record selection = arguments.get_selection_record(m_document_state.document(), "selection");

				interactive::warp_pointer(viewport, mouse, timestamp, m_timer);

				m_document_state.select(selection);

				return k3d::icommand_node::RESULT_CONTINUE;
			}

			if(Command == "paint_deselect")
			{
				command_arguments arguments(Arguments);
				viewport::control& viewport = arguments.get_viewport();
				const k3d::point2 mouse = arguments.get_viewport_point2("mouse");
				const double timestamp = arguments.get_double("timestamp");
				const k3d::selection::record selection = arguments.get_selection_record(m_document_state.document(), "selection");

				interactive::warp_pointer(viewport, mouse, timestamp, m_timer);

				m_document_state.deselect(selection);

				return k3d::icommand_node::RESULT_CONTINUE;
			}

			if(Command == "end_paint_select")
			{
				command_arguments arguments(Arguments);
				viewport::control& viewport = arguments.get_viewport();
				const k3d::point2 mouse = arguments.get_viewport_point2("mouse");

				interactive::warp_pointer(viewport, mouse);

				k3d::finish_state_change_set(m_document_state.document(), _("Paint Select"), K3D_CHANGE_SET_CONTEXT);

				return k3d::icommand_node::RESULT_CONTINUE;
			}

			if(Command == "end_paint_deselect")
			{
				command_arguments arguments(Arguments);
				viewport::control& viewport = arguments.get_viewport();
				const k3d::point2 mouse = arguments.get_viewport_point2("mouse");

				interactive::warp_pointer(viewport, mouse);

				k3d::finish_state_change_set(m_document_state.document(), _("Paint Deselect"), K3D_CHANGE_SET_CONTEXT);

				return k3d::icommand_node::RESULT_CONTINUE;
			}

			if(Command == "start_rubber_band_replace")
			{
				command_arguments arguments(Arguments);
				viewport::control& viewport = arguments.get_viewport();
				const k3d::point2 mouse = arguments.get_viewport_point2("mouse");
				const k3d::rectangle box = arguments.get_viewport_rectangle("box");

				interactive::move_pointer(viewport, mouse);
				m_rubber_band.box = box;
				m_rubber_band.draw(viewport);

				m_timer.restart();

				return k3d::icommand_node::RESULT_CONTINUE;
			}

			if(Command == "start_rubber_band_select")
			{
				command_arguments arguments(Arguments);
				viewport::control& viewport = arguments.get_viewport();
				const k3d::point2 mouse = arguments.get_viewport_point2("mouse");
				const k3d::rectangle box = arguments.get_viewport_rectangle("box");

				interactive::move_pointer(viewport, mouse);
				m_rubber_band.box = box;
				m_rubber_band.draw(viewport);

				m_timer.restart();

				return k3d::icommand_node::RESULT_CONTINUE;
			}

			if(Command == "start_rubber_band_deselect")
			{
				command_arguments arguments(Arguments);
				viewport::control& viewport = arguments.get_viewport();
				const k3d::point2 mouse = arguments.get_viewport_point2("mouse");
				const k3d::rectangle box = arguments.get_viewport_rectangle("box");

				interactive::move_pointer(viewport, mouse);
				m_rubber_band.box = box;
				m_rubber_band.draw(viewport);

				m_timer.restart();

				return k3d::icommand_node::RESULT_CONTINUE;
			}

			if(Command == "rubber_band_motion")
			{
				command_arguments arguments(Arguments);
				viewport::control& viewport = arguments.get_viewport();
				const k3d::point2 mouse = arguments.get_viewport_point2("mouse");
				const double timestamp = arguments.get_double("timestamp");
				const k3d::rectangle box = arguments.get_viewport_rectangle("box");

				interactive::warp_pointer(viewport, mouse, timestamp, m_timer);
				m_rubber_band.draw(viewport);
				m_rubber_band.box = box;
				m_rubber_band.draw(viewport);

				return k3d::icommand_node::RESULT_CONTINUE;
			}

			if(Command == "end_rubber_band_replace")
			{
				command_arguments arguments(Arguments);
				viewport::control& viewport = arguments.get_viewport();
				const k3d::point2 mouse = arguments.get_viewport_point2("mouse");
				const k3d::selection::records selection = arguments.get_selection_records(m_document_state.document(), "selection");

				interactive::warp_pointer(viewport, mouse);
				m_rubber_band.draw(viewport);

				k3d::record_state_change_set change_set(m_document_state.document(), _("Rubber Band Select"), K3D_CHANGE_SET_CONTEXT);
				m_document_state.deselect_all();
				m_document_state.select(selection);

				return k3d::icommand_node::RESULT_CONTINUE;
			}

			if(Command == "end_rubber_band_select")
			{
				command_arguments arguments(Arguments);
				viewport::control& viewport = arguments.get_viewport();
				const k3d::point2 mouse = arguments.get_viewport_point2("mouse");
				const k3d::selection::records selection = arguments.get_selection_records(m_document_state.document(), "selection");

				interactive::warp_pointer(viewport, mouse);
				m_rubber_band.draw(viewport);

				k3d::record_state_change_set change_set(m_document_state.document(), _("Rubber Band Select"), K3D_CHANGE_SET_CONTEXT);
				m_document_state.select(selection);

				return k3d::icommand_node::RESULT_CONTINUE;
			}

			if(Command == "end_rubber_band_deselect")
			{
				command_arguments arguments(Arguments);
				viewport::control& viewport = arguments.get_viewport();
				const k3d::point2 mouse = arguments.get_viewport_point2("mouse");
				const k3d::selection::records selection = arguments.get_selection_records(m_document_state.document(), "selection");

				interactive::warp_pointer(viewport, mouse);
				m_rubber_band.draw(viewport);

				k3d::record_state_change_set change_set(m_document_state.document(), _("Rubber Band Deselect"), K3D_CHANGE_SET_CONTEXT);
				m_document_state.deselect(selection);

				return k3d::icommand_node::RESULT_CONTINUE;
			}
		}
		catch(std::exception& e)
		{
			k3d::log() << k3d_file_reference << ": caught exception: " << e.what() << std::endl;
			return k3d::icommand_node::RESULT_ERROR;
		}

		return k3d::icommand_node::RESULT_UNKNOWN_COMMAND;
	}

	/// Stores a reference to the owning document
	document_state& m_document_state;
	/// Used to draw rubber-band selections
	rubber_band m_rubber_band;

	k3d::selection::record m_start_selection;
	enum
	{
		MOTION_NONE,
		MOTION_PAINT_SELECT,
		MOTION_PAINT_DESELECT,
		MOTION_RUBBER_BAND_REPLACE,
		MOTION_RUBBER_BAND_SELECT,
		MOTION_RUBBER_BAND_DESELECT,
	} m_motion_type;

	sigc::signal<void, const std::string&, const std::string&> m_command_signal;

	bool m_extended_mode;
	bool m_extended_component_mode;
	bool m_paint_mode;
	bool m_double_click_mode;

	k3d::timer m_timer;
};

/////////////////////////////////////////////////////////////////////////////
// selection_input_model

selection_input_model::selection_input_model(document_state& DocumentState) :
	m_implementation(new implementation(DocumentState))
{
}

selection_input_model::~selection_input_model()
{
	delete m_implementation;
}

void selection_input_model::set_extended_mode(const bool Enabled)
{
	m_implementation->m_extended_mode = Enabled;
}

void selection_input_model::set_extended_component_mode(const bool Enabled)
{
	m_implementation->m_extended_component_mode = Enabled;
}

void selection_input_model::set_paint_mode(const bool Enabled)
{
	m_implementation->m_paint_mode = Enabled;
}

void selection_input_model::set_double_click_mode(const bool Enabled)
{
	m_implementation->m_double_click_mode = Enabled;
}

void selection_input_model::on_button_down(viewport::control& Viewport, const GdkEventButton& Event)
{
	m_implementation->on_button_down(Viewport, Event);
}

void selection_input_model::on_button_click(viewport::control& Viewport, const GdkEventButton& Event)
{
	m_implementation->on_button_click(Viewport, Event);
}

void selection_input_model::on_button_double_click(viewport::control& Viewport, const GdkEventButton& Event)
{
	m_implementation->on_button_double_click(Viewport, Event);
}

void selection_input_model::on_button_start_drag(viewport::control& Viewport, const GdkEventMotion& Event)
{
	m_implementation->on_button_start_drag(Viewport, Event);
}

void selection_input_model::on_button_drag(viewport::control& Viewport, const GdkEventMotion& Event)
{
	m_implementation->on_button_drag(Viewport, Event);
}

void selection_input_model::on_button_end_drag(viewport::control& Viewport, const GdkEventButton& Event)
{
	m_implementation->on_button_end_drag(Viewport, Event);
}

sigc::connection selection_input_model::connect_command_signal(const sigc::slot<void, const std::string&, const std::string&>&Slot)
{
	return m_implementation->m_command_signal.connect(Slot);
}

const k3d::icommand_node::result selection_input_model::execute_command(const std::string& Command, const std::string& Arguments)
{
	return m_implementation->execute_command(Command, Arguments);
}

} // namespace libk3dngui

