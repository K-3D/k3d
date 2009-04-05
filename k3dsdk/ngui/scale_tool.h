#ifndef K3DSDK_NGUI_SCALE_TOOL_H
#define K3DSDK_NGUI_SCALE_TOOL_H

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
		\author Romain Behar (romainbehar@yahoo.com)
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
// scale_tool

/// Tool implementation that provides standard node/mesh scaling
class scale_tool :
	public transform_tool
{
	typedef transform_tool transform_tool_base;

public:
	scale_tool(document_state& DocumentState, const std::string& Name);
	~scale_tool();

	const k3d::string_t tool_type() { return "scale_tool"; }

	const k3d::icommand_node::result execute_command(const std::string& Command, const std::string& Arguments);
	
protected:
	k3d::point3 world_position();

private:
	void on_activate();
	void on_deactivate();
	void on_document_selection_changed();
	void on_redraw(viewport::control& Viewport);
	void on_select(viewport::control& Viewport);

	k3d::iproperty_collection* get_property_collection();
	viewport_input_model& get_input_model();

	// input_model implementation
	void record_command(viewport::control& Viewport, const GdkEventButton& Event, const bool Move);
	void record_transform(viewport::control& Viewport, const GdkEventMotion& Event, const k3d::point3& Scaling);

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
		record_transform(Viewport, Event, k3d::point3(0, 0, 0));
	}

	void on_lbutton_drag(viewport::control& Viewport, const GdkEventMotion& Event)
	{
		k3d::point2 coordinates(Event.x, Event.y);

		// Off-screen wrap
		off_screen_warp(Viewport, coordinates);

		const k3d::point3 scaling = lbutton_drag(Viewport, coordinates);

		// Record command for tutorials
		record_transform(Viewport, Event, scaling);
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

		const k3d::point3 scaling = mouse_move_action(Viewport, coordinates);

		// Record command for tutorials
		record_transform(Viewport, Event, scaling);
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

		// Give priority to the middle cube
		if(std::find(Manipulators.begin(), Manipulators.end(), std::string("xyz_constraint"))
			!= Manipulators.end())
			return std::string("xyz_constraint");

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
		start_scaling();
		m_original_scaling = get_scaling();

		m_manipulators->begin_mouse_move(Coordinates, world_position());
	}

	virtual void update_constraint(viewport::control& Viewport, const k3d::point2& Coordinates)
	{
		m_manipulators->update_constraint(Viewport, Coordinates, world_position(), world_orientation());

		begin_mouse_move(Coordinates);
	}

	virtual void reset()
	{
	}

	// LMB drag/move actions
	k3d::point3 mouse_move_to_3d(viewport::control& Viewport, const k3d::point2& Coordinates);
	k3d::point3 lbutton_drag(viewport::control& Viewport, const k3d::point2& Coordinates);
	k3d::point3 mouse_move_action(viewport::control& Viewport, const k3d::point2& Coordinates);
	void scale_selection(const k3d::point3& Scaling);
	k3d::point3 get_scaling();
	void on_scale(k3d::iunknown*);
	k3d::point3 get_center();

	/// Stores manipulators
	detail::imanipulators<k3d::point3>* m_manipulators;

	// Scaling value
	k3d::point3 m_original_scaling;
	k3d_data(k3d::point3, immutable_name, explicit_change_signal, with_undo, local_storage, no_constraint, writable_property, no_serialization) m_scaling;
	// Scaling center
	k3d_data(k3d::point3, immutable_name, explicit_change_signal, with_undo, local_storage, no_constraint, writable_property, no_serialization) m_center;
	k3d_data(bool, immutable_name, explicit_change_signal, with_undo, local_storage, no_constraint, writable_property, no_serialization) m_auto_center;
};

} // namespace ngui

} // namespace k3d

#endif // !K3DSDK_NGUI_SCALE_TOOL_H

