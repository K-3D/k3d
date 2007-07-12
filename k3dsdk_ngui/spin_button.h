#ifndef NGUI_SPIN_BUTTON_H
#define NGUI_SPIN_BUTTON_H

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
		\brief Declares the libk3dngui::spin_button control, which provides a standard MVC UI for scalar values
		\author Tim Shead (tshead@k-3d.com)
		\author Dan Erikson <derikson@montana.com>
*/

#include "ui_component.h"

#include <k3dsdk/high_res_timer.h>
#include <k3dsdk/vectors.h>

#include <gtkmm/button.h>
#include <gtkmm/table.h>

namespace Gtk { class Button; }
namespace k3d { class iproperty; }
namespace k3d { class istate_recorder; }

namespace libk3dngui
{

class hotkey_entry;

namespace spin_button
{

/////////////////////////////////////////////////////////////////////////////
// idata_proxy

/// Abstract interface for an object that proxies a data source for a spin-button control (i.e. the "model" in model-view-controller)
class idata_proxy
{
public:
	virtual ~idata_proxy() {}

	/// Returns true iff the underlying data source is writable
	virtual bool writable() = 0;
	/// Called to return the underlying data value
	virtual double value() = 0;
	/// Called to set a new data value
	virtual void set_value(const double Value) = 0;
	/// Signal emitted if the underlying data changes
	typedef sigc::signal<void, k3d::iunknown*> changed_signal_t;
	/// Signal emitted if the underlying data changes
	virtual changed_signal_t& changed_signal() = 0;

	/// Stores an optional state recorder for recording undo/redo data
	k3d::istate_recorder* const state_recorder;
	/// Stores an optional message for labelling undo/redo state changes
	const Glib::ustring change_message;

protected:
	idata_proxy(k3d::istate_recorder* const StateRecorder, const Glib::ustring& ChangeMessage) :
		state_recorder(StateRecorder),
		change_message(ChangeMessage)
	{
	}

private:
	idata_proxy(const idata_proxy& RHS);
	idata_proxy& operator=(const idata_proxy& RHS);
};

/////////////////////////////////////////////////////////////////////////////
// control

/// Provides a UI for manipulating scalar quantities (i.e. the view and the controller from model-view-controller)
class control :
	public Gtk::Table,
	public ui_component
{
	typedef Gtk::Table base;

public:
	control(k3d::icommand_node& Parent, const std::string& Name, std::auto_ptr<idata_proxy> Data);

	/// Sets the step increment between values when the user clicks on the up or down arrows
	void set_step_increment(const double StepIncrement);
	/// Sets the physical unit-of-measure that should be used to display values
	void set_units(const std::type_info& Units);

	const k3d::icommand_node::result execute_command(const std::string& Command, const std::string& Arguments);

private:
	/// Creates up or down button
	void setup_arrow_button(Gtk::Button* Button, const Gtk::ArrowType ArrowType, const bool Up);

	/// Called whenever the underlying data changes
	void on_data_changed(k3d::iunknown*);
	/// Formats a value and displays it in the entry
	void display_value(const double Value);

	/// Called when the entry control loses the keyboard focus
	bool on_entry_focus_out_event(GdkEventFocus* Event);
	/// Called when the entry control is activated
	void on_entry_activated();
	/// Sets the proxied value from the entry widget
	void on_manual_value();

	/// Called when the user pressed the "drag" button
	void on_drag_pressed(const bool Up);
	/// Called when the user drags the mouse over the "drag" button
	bool on_drag_motion_notify_event(GdkEventMotion*);
	/// Called when a key is pressed during dragging
	bool on_drag_key_press_event(GdkEventKey* Event);
	/// Called when a key is released during dragging
	bool on_drag_key_release_event(GdkEventKey* Event);
	/// Called by the idle timeout during dragging
	bool on_drag_timeout();
	/// Called when the user releases the "drag" button
	void on_drag_released();

	/// Called to increment the current value
	void increment();
	/// Called to decrement the current value
	void decrement();

	/// Entry control for display and manual input
	hotkey_entry* const m_entry;
	/// Button controls for interactive input
	Gtk::Button* const m_up_button;
	Gtk::Button* const m_down_button;
	/// Stores a reference to the underlying data object
	std::auto_ptr<idata_proxy> m_data;

	/// Stores whether the 'up' button was pressed
	bool m_up_button_pressed;
	/// Stores the increment used to modify the button's value using the spin controls
	double m_step_increment;
	/// Stores the type of type of real-world-units to use for formatting & parsing
	const std::type_info* m_units;

	/// Stores the most recent mouse position in screen coordinates during dragging
	k3d::point2 m_last_mouse;
	/// Stores the increment used to modify data during dragging
	double m_drag_increment;
	/// Set to true during dragging
	bool m_dragging;
	/// Stores the drag timeout connection
	sigc::connection m_drag_timeout;
	/// Stores whether this is the first drag timeout
	bool m_drag_first_timeout;
	/// Prevents keyboard auto-repeat from sending drag-sensitivity through the roof
	bool m_tap_started;
	k3d::timer m_timer;
};

/////////////////////////////////////////////////////////////////////////////
// data_proxy

/// Provides an implementation of k3d::spin_button::idata_proxy that supports any data source that supports the value(), set_value(), and changed_signal() concepts
template<typename data_t>
class data_proxy :
	public idata_proxy
{
public:
	data_proxy(data_t& Data, k3d::istate_recorder* const StateRecorder, const Glib::ustring& ChangeMessage) :
		idata_proxy(StateRecorder, ChangeMessage),
		m_data(Data)
	{
	}

	bool writable()
	{
		return true;
	}

	double value()
	{
		return m_data.value();
	}

	void set_value(const double Value)
	{
		m_data.set_value(static_cast<typename data_t::value_t>(Value));
	}

	changed_signal_t& changed_signal()
	{
		return m_data.changed_signal();
	}

private:
	data_proxy(const data_proxy& RHS);
	data_proxy& operator=(const data_proxy& RHS);

	data_t& m_data;
};

/////////////////////////////////////////////////////////////////////////////
// proxy

/// Convenience factory function for creating k3d::spin_button::idata_proxy objects
template<typename data_t>
std::auto_ptr<idata_proxy> proxy(data_t& Data, k3d::istate_recorder* const StateRecorder = 0, const Glib::ustring& ChangeMessage = "")
{
	return std::auto_ptr<idata_proxy>(new data_proxy<data_t>(Data, StateRecorder, ChangeMessage));
}

/// Convenience factory function for creating k3d::spin_button::idata_proxy objects, specialized for k3d::iproperty
std::auto_ptr<idata_proxy> proxy(k3d::iproperty& Data, k3d::istate_recorder* const StateRecorder = 0, const Glib::ustring& ChangeMessage = "");

} // namespace spin_button

} // namespace libk3dngui

#endif // NGUI_SPIN_BUTTON_H

