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
	\author Tim Shead <tshead@k-3d.com>
	\author Dan Erikson <derikson@montana.com>
	\author Romain Behar <romainbehar@yahoo.com>
*/

#include <gtkmm/arrow.h>
#include <gtkmm/button.h>
#include <gtkmm/entry.h>
#include <gtkmm/tooltips.h>
#include <gtkmm/window.h>

#include "command_arguments.h"
#include "hotkey_entry.h"
#include "interactive.h"
#include "spin_button.h"
#include "tutorial_message.h"

#include <k3d-i18n-config.h>
#include <k3dsdk/basic_math.h>
#include <k3dsdk/command_tree.h>
#include <k3dsdk/high_res_timer.h>
#include <k3dsdk/imeasurement_property.h>
#include <k3dsdk/inode.h>
#include <k3dsdk/iproperty.h>
#include <k3dsdk/istate_recorder.h>
#include <k3dsdk/iwritable_property.h>
#include <k3dsdk/log.h>
#include <k3dsdk/measurement.h>
#include <k3dsdk/result.h>
#include <k3dsdk/state_change_set.h>
#include <k3dsdk/string_cast.h>

#include <boost/scoped_ptr.hpp>

#include <iomanip>
#include <sstream>

using namespace k3d::measurement;

namespace k3d
{

namespace ngui
{

namespace spin_button
{

/////////////////////////////////////////////////////////////////////////////
// property_model

/// Implementation of spin_button::imodel for use with k3d::iproperty objects
class property_model :
	public imodel
{
public:
	property_model(k3d::iproperty& Data) :
		m_readable_data(Data),
		m_writable_data(dynamic_cast<k3d::iwritable_property*>(&Data))
	{
	}

	const Glib::ustring label()
	{
		Glib::ustring result = m_readable_data.property_label();

		if(m_readable_data.property_node())
			result = m_readable_data.property_node()->name() + " " + result;

		return result;
	}

	k3d::bool_t writable()
	{
		return m_writable_data ? true : false;
	}

	k3d::double_t value()
	{
		const std::type_info& type = m_readable_data.property_type();

		if(type == typeid(k3d::int32_t))
			return boost::any_cast<k3d::int32_t>(m_readable_data.property_internal_value());
		else if(type == typeid(k3d::uint32_t))
			return boost::any_cast<k3d::uint32_t>(m_readable_data.property_internal_value());
		else if(type == typeid(k3d::float_t))
			return boost::any_cast<k3d::float_t>(m_readable_data.property_internal_value());
		else if(type == typeid(k3d::double_t))
			return boost::any_cast<k3d::double_t>(m_readable_data.property_internal_value());
		else
			k3d::log() << error << k3d_file_reference << ": unknown property type: " << type.name() << std::endl;

		return 0;
	}

	void set_value(const double Value)
	{
		return_if_fail(m_writable_data);

		const std::type_info& type = m_readable_data.property_type();

		if(type == typeid(k3d::int32_t))
			m_writable_data->property_set_value(static_cast<k3d::int32_t>(k3d::round(Value)));
		else if(type == typeid(k3d::uint32_t))
			m_writable_data->property_set_value(static_cast<k3d::uint32_t>(k3d::round(Value)));
		else if(type == typeid(k3d::float_t))
			m_writable_data->property_set_value(static_cast<k3d::float_t>(Value));
		else if(type == typeid(k3d::double_t))
			m_writable_data->property_set_value(static_cast<k3d::double_t>(Value));
		else
			k3d::log() << error << k3d_file_reference << ": unknown property type: " << type.name() << std::endl;
	}

	sigc::connection connect_changed_signal(const sigc::slot<void>& Slot)
	{
		return m_readable_data.property_changed_signal().connect(sigc::hide(Slot));
	}

	k3d::double_t step_increment()
	{
		if(k3d::imeasurement_property* const measurement_property = dynamic_cast<k3d::imeasurement_property*>(&m_readable_data))
			return measurement_property->property_step_increment();

		return 1.0;
	}

	const std::type_info& units()
	{
		if(k3d::imeasurement_property* const measurement_property = dynamic_cast<k3d::imeasurement_property*>(&m_readable_data))
			return measurement_property->property_units();

		return typeid(k3d::measurement::scalar);
	}

private:
	property_model(const property_model& RHS);
	property_model& operator=(const property_model& RHS);
	~property_model() {}

	k3d::iproperty& m_readable_data;
	k3d::iwritable_property* const m_writable_data;
};

/////////////////////////////////////////////////////////////////////////////
// model

imodel* const model(k3d::iproperty& Property)
{
	return new property_model(Property);
}

/////////////////////////////////////////////////////////////////////////////
// control::implementation

class control::implementation
{
public:
	implementation(imodel* const Model, k3d::istate_recorder* const StateRecorder) :
		m_model(Model),
		m_state_recorder(StateRecorder),
		m_step_increment(Model->step_increment()),
		m_units(&Model->units()),
		m_entry(new hotkey_entry()),
		m_up_button(new Gtk::Button()),
		m_down_button(new Gtk::Button()),
		m_dragging(false),
		m_up_button_pressed(false),
		m_drag_increment(0),
		m_drag_first_timeout(false),
		m_tap_started(false)
	{
		assert(m_model.get());
	}

	/// Stores a reference to the underlying data object
	const boost::scoped_ptr<imodel> m_model;
	/// Stores a reference to the (optional) object for recording undo/redo data
	k3d::istate_recorder* const m_state_recorder;

	/// Stores the increment used to modify the button's value using the spin controls
	double m_step_increment;
	/// Stores the type of type of real-world-units to use for formatting & parsing
	const std::type_info* m_units;

	/// Entry control for display and manual input
	hotkey_entry* const m_entry;
	/// Button control for incremental input
	Gtk::Button* const m_up_button;
	/// Button control for incremental input
	Gtk::Button* const m_down_button;

	/// Set to true during dragging
	bool m_dragging;
	/// Stores whether the 'up' button was pressed
	bool m_up_button_pressed;
	/// Stores the increment used to modify data during dragging
	double m_drag_increment;
	
	/// Stores the most recent mouse position in screen coordinates during dragging
	k3d::point2 m_last_mouse;
	/// Stores the drag timeout connection
	sigc::connection m_drag_timeout;
	/// Stores whether this is the first drag timeout
	bool m_drag_first_timeout;
	/// Prevents keyboard auto-repeat from sending drag-sensitivity through the roof
	bool m_tap_started;
	k3d::timer m_timer;
};

/////////////////////////////////////////////////////////////////////////////
// control

control::control(k3d::icommand_node& Parent, const k3d::string_t& Name, imodel* const Model, k3d::istate_recorder* const StateRecorder) :
	base(2, 8, true),
	m_implementation(new implementation(Model, StateRecorder))
{
	k3d::command_tree().add(*this, Name, &Parent);

	set_name("k3d-spin-button");

	m_implementation->m_entry->set_name("entry");
	m_implementation->m_entry->set_width_chars(8);
	m_implementation->m_entry->signal_focus_out_event().connect(sigc::mem_fun(*this, &control::on_entry_focus_out_event));
	m_implementation->m_entry->signal_activate().connect(sigc::mem_fun(*this, &control::on_entry_activated));
	attach(*manage(m_implementation->m_entry), 0, 6, 0, 2);

	if(m_implementation->m_model->writable())
	{
		// Setup up and down buttons
		setup_arrow_button(m_implementation->m_up_button, Gtk::ARROW_UP, true);
		setup_arrow_button(m_implementation->m_down_button, Gtk::ARROW_DOWN, false);

		// Setup VBox containing arrows
		attach(*manage(m_implementation->m_up_button), 6, 7, 0, 1);
		attach(*manage(m_implementation->m_down_button), 6, 7, 1, 2);

		tooltips().set_tip(*m_implementation->m_entry, _("Enter a new value.  Real-world units and simple math expressions are allowed."));
		tooltips().set_tip(*m_implementation->m_up_button, _("LMB-Drag to modify, LMB-Click to step, Tap Shift and Control while dragging to change sensitivity."));
		tooltips().set_tip(*m_implementation->m_down_button, _("LMB-Drag to modify, LMB-Click to step, Tap Shift and Control while dragging to change sensitivity."));

		// Make sure buttons can't get the focus (makes tabbing difficult)
		m_implementation->m_up_button->unset_flags(Gtk::CAN_FOCUS);
		m_implementation->m_down_button->unset_flags(Gtk::CAN_FOCUS);
	}
	else
	{
		m_implementation->m_entry->set_editable(false);
	}

	// Synchronize the view with the data source ...
	on_data_changed();

	// We want to be notified if the data source changes ...
	m_implementation->m_model->connect_changed_signal(sigc::mem_fun(*this, &control::on_data_changed));
}

control::~control()
{
	delete m_implementation;
}

void control::set_step_increment(const double StepIncrement)
{
	m_implementation->m_step_increment = StepIncrement;
	on_data_changed();
}

void control::set_units(const std::type_info& Units)
{
	m_implementation->m_units = &Units;
	on_data_changed();
}

const k3d::icommand_node::result control::execute_command(const k3d::string_t& Command, const k3d::string_t& Arguments)
{
	try
	{
		if(Command == "set_value")
		{
			interactive::set_text(*m_implementation->m_entry, Arguments);
			return RESULT_CONTINUE;
		}
		else if(Command == "increment_value")
		{
			const double current_value = m_implementation->m_model->value();
			const double new_value = k3d::from_string<double>(Arguments, 0.0);
			const double old_step_increment = m_implementation->m_step_increment;
			m_implementation->m_step_increment = new_value - current_value;
			interactive::activate(*m_implementation->m_up_button);
			m_implementation->m_step_increment = old_step_increment;

			return RESULT_CONTINUE;
		}
		else if(Command == "decrement_value")
		{
			const double current_value = m_implementation->m_model->value();
			const double new_value = k3d::from_string<double>(Arguments, 0.0);
			const double old_step_increment = m_implementation->m_step_increment;
			m_implementation->m_step_increment = current_value - new_value;
			interactive::activate(*m_implementation->m_down_button);
			m_implementation->m_step_increment = old_step_increment;

			return RESULT_CONTINUE;
		}
		else if(Command == "start_drag")
		{
			if(Arguments == "down")
			{
				interactive::move_pointer(*m_implementation->m_down_button);
			}
			else
			{
				interactive::move_pointer(*m_implementation->m_up_button);
			}

			if(m_implementation->m_state_recorder)
				m_implementation->m_state_recorder->start_recording(k3d::create_state_change_set(K3D_CHANGE_SET_CONTEXT), K3D_CHANGE_SET_CONTEXT);

			m_implementation->m_timer.restart();

			return RESULT_CONTINUE;
		}
		else if(Command == "drag_motion")
		{
			command_arguments arguments(Arguments);
			const double timestamp = arguments.get_double("timestamp");
			const k3d::vector2 mouse_delta = arguments.get_vector2("mouse_delta");
			const double value = arguments.get_double("value");

			interactive::warp_pointer(interactive::get_pointer() + mouse_delta, timestamp, m_implementation->m_timer);
			m_implementation->m_model->set_value(value);

			return RESULT_CONTINUE;
		}
		else if(Command == "increase_sensitivity")
		{
			return RESULT_CONTINUE;
		}
		else if(Command == "decrease_sensitivity")
		{
			return RESULT_CONTINUE;
		}
		else if(Command == "end_drag")
		{
			if(m_implementation->m_state_recorder)
				m_implementation->m_state_recorder->commit_change_set(m_implementation->m_state_recorder->stop_recording(K3D_CHANGE_SET_CONTEXT), change_message(m_implementation->m_model->value()), K3D_CHANGE_SET_CONTEXT);

			return RESULT_CONTINUE;
		}
	}
	catch(std::exception& e)
	{
		k3d::log() << error << "exception: " << e.what() << std::endl;
		return RESULT_ERROR;
	}

	return ui_component::execute_command(Command, Arguments);
}

void control::setup_arrow_button(Gtk::Button* Button, const Gtk::ArrowType ArrowType, const bool Up)
{
	Gtk::Arrow* const arrow = Gtk::manage(new Gtk::Arrow(ArrowType, Gtk::SHADOW_NONE));
	arrow->set_size_request(0, 0);

	Button->set_size_request(0, 0);
	Button->add(*arrow);

	Button->signal_pressed().connect(sigc::bind(sigc::mem_fun(*this, &control::on_drag_pressed), Up));
	Button->signal_released().connect(sigc::mem_fun(*this, &control::on_drag_released));

	Button->add_events(Gdk::BUTTON_MOTION_MASK | Gdk::KEY_PRESS_MASK | Gdk::KEY_RELEASE_MASK);
	Button->signal_motion_notify_event().connect(sigc::mem_fun(*this, &control::on_drag_motion_notify_event));
	Button->signal_key_press_event().connect(sigc::mem_fun(*this, &control::on_drag_key_press_event));
	Button->signal_key_release_event().connect(sigc::mem_fun(*this, &control::on_drag_key_release_event));
}

void control::on_data_changed()
{
	display_value(m_implementation->m_model->value());
}

void control::display_value(const double Value)
{
	std::ostringstream buffer;

	if(*m_implementation->m_units == typeid(k3d::measurement::angle)) // Display angles in degrees, even though the internal unit is radians
		buffer << k3d::string_cast(boost::format("%g") % k3d::measurement::convert(k3d::measurement::quantity<k3d::measurement::angle>(Value, k3d::measurement::angle_units("rad")), k3d::measurement::angle_units("deg")).value()) << " deg";
	else if(*m_implementation->m_units == typeid(k3d::measurement::area))
		buffer << k3d::string_cast(boost::format("%g") % Value) << " m^2";
	else if(*m_implementation->m_units == typeid(k3d::measurement::distance))
		buffer << k3d::string_cast(boost::format("%g") % Value) << " m";
	else if(*m_implementation->m_units == typeid(k3d::measurement::force))
		buffer << k3d::string_cast(boost::format("%g") % Value) << " N";
	else if(*m_implementation->m_units == typeid(k3d::measurement::mass))
		buffer << k3d::string_cast(boost::format("%g") % Value) << " Kg";
	else if(*m_implementation->m_units == typeid(k3d::measurement::pressure))
		buffer << k3d::string_cast(boost::format("%g") % Value) << " Pa";
	else if(*m_implementation->m_units == typeid(k3d::measurement::time))
		buffer << k3d::string_cast(boost::format("%g") % Value) << " s";
	else if(*m_implementation->m_units == typeid(k3d::measurement::volume))
		buffer << k3d::string_cast(boost::format("%g") % Value) << " m^3";
	else if(*m_implementation->m_units == typeid(scalar))
		buffer << k3d::string_cast(boost::format("%g") % Value);
	else
		k3d::log() << error << "Unknown physical unit - " << m_implementation->m_units->name() << std::endl;

	m_implementation->m_entry->set_text(buffer.str());
}

bool control::on_entry_focus_out_event(GdkEventFocus* Event)
{
	on_manual_value();
	return false;
}

void control::on_entry_activated()
{
	on_manual_value();

	/** \todo What the heck is this? */
	// Set focus to the arrows such as hotkeys work again
	m_implementation->m_down_button->set_flags(Gtk::CAN_FOCUS);
	m_implementation->m_down_button->grab_focus();
	m_implementation->m_down_button->unset_flags(Gtk::CAN_FOCUS);
}

void control::on_manual_value()
{
	m_implementation->m_entry->select_region(0, 0);
	const k3d::string_t new_text = m_implementation->m_entry->get_text();

	// Default our results to the current value, in case it doesn't parse ...
	const double original_value = m_implementation->m_model->value();
	double new_value = original_value;

	// Parse the input expression into value converting it to SI units automatically (it can do mathematical expressions, too, for fun)
	if(!k3d::measurement::parse(new_text, new_value, m_implementation->m_units))
	{
		k3d::log() << error << "Couldn't parse expression: " << new_text << " restoring original value" << std::endl;
		display_value(original_value);
		return;
	}

	// If the value has changed, record it ...
	if(new_value != original_value)
	{
		// Record the command for posterity (tutorials) ...
		record_command("set_value", new_text);

		// Turn this into an undo/redo -able event ...
		if(m_implementation->m_state_recorder)
			m_implementation->m_state_recorder->start_recording(k3d::create_state_change_set(K3D_CHANGE_SET_CONTEXT), K3D_CHANGE_SET_CONTEXT);

		m_implementation->m_model->set_value(new_value);

		// Turn this into an undo/redo -able event ...
		if(m_implementation->m_state_recorder)
			m_implementation->m_state_recorder->commit_change_set(m_implementation->m_state_recorder->stop_recording(K3D_CHANGE_SET_CONTEXT), change_message(m_implementation->m_model->value()), K3D_CHANGE_SET_CONTEXT);
	}
	else
	{
		display_value(new_value);
	}
}

void control::on_drag_pressed(const bool Up)
{
	// Save which button was pressed
	m_implementation->m_up_button_pressed = Up;

	m_implementation->m_up_button->set_flags(Gtk::CAN_FOCUS);
	m_implementation->m_down_button->set_flags(Gtk::CAN_FOCUS);
	m_implementation->m_up_button->grab_focus();
	m_implementation->m_down_button->grab_focus();

	// Get the current mouse coordinates ...
	m_implementation->m_last_mouse = interactive::get_pointer();

	// Calculate the increment we should use while dragging ...
	m_implementation->m_drag_increment = std::abs(m_implementation->m_step_increment) * 0.2;
	if(!m_implementation->m_drag_increment)
		m_implementation->m_drag_increment = 0.002;

	// Connect idle timeout handler, called every 200ms
	m_implementation->m_drag_timeout = Glib::signal_timeout().connect(sigc::mem_fun(*this, &control::on_drag_timeout), 200);
	m_implementation->m_drag_first_timeout = true;

	// Turn this into an undo/redo -able event ...
	if(m_implementation->m_state_recorder)
		m_implementation->m_state_recorder->start_recording(k3d::create_state_change_set(K3D_CHANGE_SET_CONTEXT), K3D_CHANGE_SET_CONTEXT);
}

bool control::on_drag_motion_notify_event(GdkEventMotion* Event)
{
	// Get new mouse coordinates
	const k3d::point2 mouse = interactive::get_pointer();

	// Don't switch to drag mode until the mouse really moved
	if(!m_implementation->m_dragging)
	{
		if(k3d::distance(mouse, m_implementation->m_last_mouse) < 10)
			return false;

		m_implementation->m_dragging = true;
		m_implementation->m_timer.restart();

		record_command("start_drag", m_implementation->m_up_button_pressed ? "up" : "down");
	}

	// Update everything ...
	const double horizontal_length = m_implementation->m_last_mouse[0] - mouse[0];
	const double vertical_length = m_implementation->m_last_mouse[1] - mouse[1];
	double new_value = m_implementation->m_model->value();
	if(std::abs(horizontal_length) > std::abs(vertical_length))
	{
		// Dragging mostly horizontally : 1/10th unit increase
		new_value += m_implementation->m_drag_increment * 0.1 * (mouse[0] - m_implementation->m_last_mouse[0]);
	}
	else
	{
		// Dragging mostly vertically : one unit increase
		new_value += m_implementation->m_drag_increment * (m_implementation->m_last_mouse[1] - mouse[1]);
	}

	command_arguments arguments;
	arguments.append("timestamp", m_implementation->m_timer.elapsed());
	arguments.append("mouse_delta", mouse - m_implementation->m_last_mouse);
	arguments.append("value", new_value);
	record_command("drag_motion", arguments);

	m_implementation->m_model->set_value(new_value);
	m_implementation->m_last_mouse = mouse;

	// Wrap the mouse if it goes off the top-or-bottom of the screen ...
	const int screen_height = Gdk::Display::get_default()->get_default_screen()->get_height();
	const int border = 5;
	if(mouse[1] < border)
	{
		m_implementation->m_last_mouse = k3d::point2(mouse[0], screen_height - (border + 1));
		interactive::warp_pointer(m_implementation->m_last_mouse);
	}
	else if(screen_height - mouse[1] < border)
	{
		m_implementation->m_last_mouse = k3d::point2(mouse[0], (border + 1));
		interactive::warp_pointer(m_implementation->m_last_mouse);
	}

	// Wrap the mouse if it goes off the left-or-right of the screen ...
	const int screen_width = Gdk::Display::get_default()->get_default_screen()->get_width();
	if(mouse[0] < border)
	{
		m_implementation->m_last_mouse = k3d::point2(screen_width - (border + 1), mouse[1]);
		interactive::warp_pointer(m_implementation->m_last_mouse);
	}
	else if(screen_width - mouse[0] < border)
	{
		m_implementation->m_last_mouse = k3d::point2((border + 1), mouse[1]);
		interactive::warp_pointer(m_implementation->m_last_mouse);
	}

	return false;
}

bool control::on_drag_key_press_event(GdkEventKey* Event)
{
	if(!m_implementation->m_tap_started && Event->keyval == GDK_Shift_L || Event->keyval == GDK_Shift_R)
	{
		m_implementation->m_tap_started = true;
		m_implementation->m_drag_increment *= 10.0;
		record_command("increase_sensitivity");
		return true;
	}
	else if(!m_implementation->m_tap_started && Event->keyval == GDK_Control_L || Event->keyval == GDK_Control_R)
	{
		m_implementation->m_tap_started = true;
		m_implementation->m_drag_increment *= 0.1;
		record_command("decrease_sensitivity");
		return true;
	}

	return false;
}

bool control::on_drag_key_release_event(GdkEventKey* Event)
{
	m_implementation->m_tap_started = false;
	return false;
}

bool control::on_drag_timeout()
{
	// Step increment if the user doesn't move
	if(!m_implementation->m_dragging)
	{
		if(m_implementation->m_drag_first_timeout)
		{
			// Don't change value on first timeout
			m_implementation->m_drag_first_timeout = false;
			return true;
		}

		if(m_implementation->m_up_button_pressed)
		{
			increment();
		}
		else
		{
			decrement();
		}
	}

	return true;
}

void control::on_drag_released()
{
	// If the user really didn't drag anywhere and value wasn't changed yet
	if(m_implementation->m_dragging)
	{
		record_command("end_drag");
	}
	else if(!m_implementation->m_dragging && m_implementation->m_drag_first_timeout)
	{
		if(m_implementation->m_up_button_pressed)
		{
			increment();
			record_command("increment_value", k3d::string_cast(m_implementation->m_model->value()));
		}
		else
		{
			decrement();
			record_command("decrement_value", k3d::string_cast(m_implementation->m_model->value()));
		}
	}
	else
	{
		record_command("set_value", k3d::string_cast(m_implementation->m_model->value()));
	}

	// Disconnect idle timeout
	m_implementation->m_drag_timeout.disconnect();

	// Turn this into an undo/redo -able event ...
	if(m_implementation->m_state_recorder)
		m_implementation->m_state_recorder->commit_change_set(m_implementation->m_state_recorder->stop_recording(K3D_CHANGE_SET_CONTEXT), change_message(m_implementation->m_model->value()), K3D_CHANGE_SET_CONTEXT);

	m_implementation->m_up_button->unset_flags(Gtk::CAN_FOCUS);
	m_implementation->m_down_button->unset_flags(Gtk::CAN_FOCUS);
	m_implementation->m_dragging = false;
}

void control::increment()
{
	m_implementation->m_model->set_value(m_implementation->m_model->value() + m_implementation->m_step_increment);
}

void control::decrement()
{
	m_implementation->m_model->set_value(m_implementation->m_model->value() - m_implementation->m_step_increment);
}

const k3d::string_t control::change_message(const double Value)
{
	std::stringstream value_buffer;
	value_buffer << std::setprecision(3) << Value;

	return k3d::string_cast(boost::format(_("Change %1% to %2%")) % m_implementation->m_model->label().raw() % value_buffer.str());
}

} // namespace spin_button

} // namespace ngui

} // namespace k3d

