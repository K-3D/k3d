#ifndef K3DSDK_NGUI_SPIN_BUTTON_H
#define K3DSDK_NGUI_SPIN_BUTTON_H

// K-3D
// Copyright (c) 1995-2008, Timothy M. Shead
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
		\brief Declares the spin_button control, which provides a standard MVC UI for scalar values
		\author Tim Shead (tshead@k-3d.com)
		\author Dan Erikson <derikson@montana.com>
*/

#include "ui_component.h"

#include <k3dsdk/measurement.h>
#include <k3dsdk/types.h>

#include <gtkmm/table.h>

namespace k3d { class iproperty; }
namespace k3d { class istate_recorder; }

namespace k3d
{

namespace ngui
{

namespace spin_button
{

/////////////////////////////////////////////////////////////////////////////
// imodel

/// Abstract data model for a spin_button::control
class imodel
{
public:
	virtual ~imodel() {}

	/// Returns a human-readable label for the underlying data
	virtual const Glib::ustring label() = 0;
	/// Returns true iff the underlying data source is writable
	virtual k3d::bool_t writable() = 0;
	/// Called to return the underlying data value
	virtual k3d::double_t value() = 0;
	/// Called to set a new data value
	virtual void set_value(const k3d::double_t Value) = 0;
	/// Connects a slot to a signal that will be emitted if the underlying data changes
	virtual sigc::connection connect_changed_signal(const sigc::slot<void>& Slot) = 0;
	virtual k3d::double_t step_increment() = 0;
	virtual const std::type_info& units() = 0;

protected:
	imodel() {}

private:
	imodel(const imodel&);
	imodel& operator=(const imodel&);
};

/// Factory method for creating an imodel object given a suitably-typed property
imodel* const model(k3d::iproperty& Property);

/// Provides an implementation of spin_button::imodel that can "wrap" any data source that supports the internal_value(), set_value(), and changed_signal() concepts
template<typename value_t, typename data_t>
class generic_model_t :
	public imodel
{
public:
	generic_model_t(data_t& Data, const Glib::ustring& Label, const value_t StepIncrement, const std::type_info& Units) :
		m_data(Data),
		m_label(Label),
		m_step_increment(StepIncrement),
		m_units(Units)
	{
	}

	const Glib::ustring label()
	{
		return m_label;
	}

	k3d::bool_t writable()
	{
		return true;
	}

	k3d::double_t value()
	{
		return m_data.internal_value();
	}

	void set_value(const k3d::double_t Value)
	{
		m_data.set_value(static_cast<value_t>(Value));
	}

	sigc::connection connect_changed_signal(const sigc::slot<void>& Slot)
	{
		return m_data.changed_signal().connect(sigc::hide(Slot));
	}

	k3d::double_t step_increment()
	{
		return m_step_increment;
	}

	const std::type_info& units()
	{
		return m_units;
	}

private:
	data_t& m_data;
	const Glib::ustring m_label;
	const value_t m_step_increment;
	const std::type_info& m_units;
};

/// Convenience factory function for creating generic_model_t objects
template<typename value_t, typename data_t>
imodel* model(data_t& Data, const Glib::ustring& Label = "", const value_t StepIncrement = 1, const std::type_info& Units = typeid(k3d::measurement::scalar))
{
	return new generic_model_t<value_t, data_t>(Data, Label, StepIncrement, Units);
}

/////////////////////////////////////////////////////////////////////////////
// control

/// Provides a UI for manipulating scalar quantities (i.e. the view and the controller from model-view-controller)
class control :
	public Gtk::Table,
	public ui_component
{
	typedef Gtk::Table base;

public:
	control(k3d::icommand_node& Parent, const k3d::string_t& Name, imodel* const Model, k3d::istate_recorder* const StateRecorder);
	~control();

	/// Sets the step increment between values when the user clicks on the up or down arrows
	void set_step_increment(const double StepIncrement);
	/// Sets the physical unit-of-measure that should be used to display values
	void set_units(const std::type_info& Units);

private:
	/// Creates up or down button
	void setup_arrow_button(Gtk::Button* Button, const Gtk::ArrowType ArrowType, const bool Up);

	/// Called whenever the underlying data changes
	void on_data_changed();
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

	const k3d::string_t change_message(const double Value);

	class implementation;
	implementation* const m_implementation;
};

} // namespace spin_button

} // namespace ngui

} // namespace k3d

#endif // !K3DSDK_NGUI_SPIN_BUTTON_H

