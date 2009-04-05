#ifndef K3DSDK_NGUI_ROTATE_TOOL_H
#define K3DSDK_NGUI_ROTATE_TOOL_H

// K-3D
// Copyright (c) 1995-2006, Timothy M. Shead
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

#include "keyboard.h"
#include "viewport.h"
#include "transform_tool.h"

#include <k3dsdk/property_collection.h>

namespace k3d
{

namespace ngui
{

/////////////////////////////////////////////////////////////////////////////
// rotate_tool

/// Tool implementation that provides object/mesh rotation behavior
class rotate_tool :
	public transform_tool
{
	typedef transform_tool transform_tool_base;

public:
	rotate_tool(document_state& DocumentState, const std::string& Name);
	~rotate_tool();

	const k3d::string_t tool_type() { return "rotate_tool"; }

	const k3d::icommand_node::result execute_command(const std::string& Command, const std::string& Arguments);
	
protected:
	k3d::point3 world_position();

private:
	virtual void on_activate();
	virtual void on_deactivate();
	virtual void on_document_selection_changed();
	virtual void on_redraw(viewport::control& Viewport);
	virtual void on_select(viewport::control& Viewport);

	k3d::iproperty_collection* get_property_collection();
	viewport_input_model& get_input_model();

	// input_model implementation
	void record_command(viewport::control& Viewport, const GdkEventButton& Event, const bool Move);
	void record_transform(viewport::control& Viewport, const GdkEventMotion& Event, const k3d::angle_axis& Rotation);

	void on_lbutton_down(viewport::control& Viewport, const GdkEventButton& Event)
	{
		const k3d::point2 coordinates(Event.x, Event.y);
		const k3d::key_modifiers modifiers = convert(Event.state);

		lbutton_down(Viewport, coordinates, modifiers);

		// Record command for tutorials
		record_command(Viewport, Event, true);
	}

	void on_lbutton_click(viewport::control& Viewport, const GdkEventButton& Event)
	{
		const k3d::point2 coordinates(Event.x, Event.y);

		lbutton_click(Viewport, coordinates);

		// Record command for tutorials
		record_command(Viewport, Event, false);
	}

	void on_lbutton_start_drag(viewport::control& Viewport, const GdkEventMotion& Event)
	{
		const k3d::point2 coordinates(Event.x, Event.y);

		lbutton_start_drag(Viewport, coordinates);

		// Record command for tutorials
		record_transform(Viewport, Event, k3d::angle_axis(0, 1, 0, 0));
	}

	void on_lbutton_drag(viewport::control& Viewport, const GdkEventMotion& Event)
	{
		k3d::point2 coordinates(Event.x, Event.y);

		// Off-screen wrap
		off_screen_warp(Viewport, coordinates);

		const k3d::angle_axis rotation = lbutton_drag(Viewport, coordinates);

		// Record command for tutorials
		record_transform(Viewport, Event, rotation);
	}

	void on_lbutton_end_drag(viewport::control& Viewport, const GdkEventButton& Event)
	{
		const k3d::point2 coordinates(Event.x, Event.y);

		lbutton_end_drag(Viewport, coordinates);

		// Record command for tutorials
		record_command(Viewport, Event, false);
	}

	void on_mbutton_click(viewport::control& Viewport, const GdkEventButton& Event)
	{
		const k3d::point2 coordinates(Event.x, Event.y);
		const k3d::key_modifiers modifiers = convert(Event.state);

		mbutton_click(Viewport, coordinates, modifiers);

		// Record command for tutorials
		record_command(Viewport, Event, true);
	}

	void on_rbutton_click(viewport::control& Viewport, const GdkEventButton& Event)
	{
		const k3d::point2 coordinates(Event.x, Event.y);

		rbutton_click(Viewport, coordinates);

		// Record command for tutorials
		record_command(Viewport, Event, true);
	}

	void on_mouse_move(viewport::control& Viewport, const GdkEventMotion& Event)
	{
		if(MOTION_CLICK_DRAG != m_current_motion)
			return;

		k3d::point2 coordinates(Event.x, Event.y);

		// Off-screen wrap
		off_screen_warp(Viewport, coordinates);

		const k3d::angle_axis rotation = mouse_move_action(Viewport, coordinates);

		// Record command for tutorials
		record_transform(Viewport, Event, rotation);
	}

	// Manipulator functions
	virtual std::string manipulator_name(const k3d::selection::id ID)
	{
		return m_manipulators->constraint_name(ID);
	}

	virtual std::string get_manipulator(const manipulators_t& Manipulators)
	{
		if(!Manipulators.size())
			return std::string("");

		return *Manipulators.begin();
	}

	virtual void set_manipulator(const std::string ManipulatorName)
	{
		m_document_state.set_cursor_signal().emit(m_manipulators->set_constraint(ManipulatorName));
	}

	virtual std::string get_constraint_name()
	{
		return m_manipulators->get_constraint_name();
	}

	virtual void begin_mouse_move(const k3d::point2& Coordinates)
	{
		start_rotation();
		reset();

		m_manipulators->begin_mouse_move(Coordinates, world_position());
	}

	virtual void update_constraint(viewport::control& Viewport, const k3d::point2& Coordinates)
	{
		m_manipulators->update_constraint(Viewport, Coordinates, world_position(), world_orientation());

		begin_mouse_move(Coordinates);
	}

	virtual void reset()
	{
		m_mutex = true;
		m_rotation.set_value(k3d::angle_axis(0, 1, 0, 0));
		m_mutex = false;
	}

	// LMB drag actions
	k3d::angle_axis mouse_move_to_3d(viewport::control& Viewport, const k3d::point2& Coordinates);
	k3d::angle_axis lbutton_drag(viewport::control& Viewport, const k3d::point2& Coordinates);
	k3d::angle_axis mouse_move_action(viewport::control& Viewport, const k3d::point2& Coordinates);
	void rotate_selection(const k3d::angle_axis& Rotation);
	void on_rotate(k3d::iunknown*);

	/// Stores manipulators
	detail::imanipulators<k3d::angle_axis>* m_manipulators;

	// Rotation value
	bool m_mutex;
	k3d_data(k3d::angle_axis, immutable_name, explicit_change_signal, with_undo, local_storage, no_constraint, writable_property, no_serialization) m_rotation;
	// Rotation center
	k3d_data(k3d::point3, immutable_name, explicit_change_signal, with_undo, local_storage, no_constraint, writable_property, no_serialization) m_center;
	k3d_data(bool, immutable_name, explicit_change_signal, with_undo, local_storage, no_constraint, writable_property, no_serialization) m_auto_center;
};

} // namespace ngui

} // namespace k3d

#endif // !K3DSDK_NGUI_ROTATE_TOOL_H

