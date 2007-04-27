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
		\brief Implements the k3d::spin_button class, which provides a UI for numeric quantities
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

#include <k3dsdk/basic_math.h>
#include <k3dsdk/i18n.h>
#include <k3dsdk/iproperty.h>
#include <k3dsdk/istate_recorder.h>
#include <k3dsdk/iwritable_property.h>
#include <k3dsdk/log.h>
#include <k3dsdk/measurement.h>
#include <k3dsdk/result.h>
#include <k3dsdk/state_change_set.h>
#include <k3dsdk/string_cast.h>

#include <iomanip>
#include <sstream>

using namespace k3d::measurement;

namespace libk3dngui
{

namespace spin_button
{

namespace detail
{

/////////////////////////////////////////////////////////////////////////////
// data_proxy

/// Specialization of k3d::spin_button::data_proxy for use with k3d::iproperty objects
class property_proxy :
	public idata_proxy
{
public:
	typedef k3d::iproperty data_t;

	explicit property_proxy(data_t& Data, k3d::istate_recorder* const StateRecorder, const Glib::ustring& ChangeMessage) :
		idata_proxy(StateRecorder, ChangeMessage),
		m_readable_data(Data),
		m_writable_data(dynamic_cast<k3d::iwritable_property*>(&Data))
	{
	}

	bool writable()
	{
		return m_writable_data ? true : false;
	}

	double value()
	{
		const std::type_info& type = m_readable_data.property_type();
		if(type == typeid(double))
			return boost::any_cast<double>(m_readable_data.property_value());
		else if(type == typeid(float))
			return boost::any_cast<float>(m_readable_data.property_value());
		else if(type == typeid(long))
			return boost::any_cast<long>(m_readable_data.property_value());
		else if(type == typeid(unsigned long))
			return boost::any_cast<unsigned long>(m_readable_data.property_value());
		else if(type == typeid(int))
			return boost::any_cast<int>(m_readable_data.property_value());
		else if(type == typeid(unsigned int))
			return boost::any_cast<unsigned int>(m_readable_data.property_value());
		else
			k3d::log() << error << k3d_file_reference << ": unknown property type: " << type.name() << std::endl;

		return 0;
	}

	void set_value(const double Value)
	{
		return_if_fail(m_writable_data);

		const std::type_info& type = m_readable_data.property_type();
		if(type == typeid(double))
			m_writable_data->property_set_value(Value);
		else if(type == typeid(float))
			m_writable_data->property_set_value(static_cast<float>(Value));
		else if(type == typeid(long))
			m_writable_data->property_set_value(static_cast<long>(k3d::round(Value)));
		else if(type == typeid(unsigned long))
			m_writable_data->property_set_value(static_cast<unsigned long>(k3d::round(Value)));
		else if(type == typeid(int))
			m_writable_data->property_set_value(static_cast<int>(k3d::round(Value)));
		else if(type == typeid(unsigned int))
			m_writable_data->property_set_value(static_cast<unsigned int>(k3d::round(Value)));
		else
			k3d::log() << error << k3d_file_reference << ": unknown property type: " << type.name() << std::endl;
	}

	changed_signal_t& changed_signal()
	{
		return m_readable_data.property_changed_signal();
	}

private:
	property_proxy(const property_proxy& RHS);
	property_proxy& operator=(const property_proxy& RHS);
	~property_proxy() {}

	data_t& m_readable_data;
	k3d::iwritable_property* const m_writable_data;
};

} // namespace detail

std::auto_ptr<idata_proxy> proxy(k3d::iproperty& Data, k3d::istate_recorder* const StateRecorder, const Glib::ustring& ChangeMessage)
{
	return std::auto_ptr<idata_proxy>(new detail::property_proxy(Data, StateRecorder, ChangeMessage));
}

/////////////////////////////////////////////////////////////////////////////
// control

control::control(k3d::icommand_node& Parent, const std::string& Name, std::auto_ptr<idata_proxy> Data) :
	base(2, 8, true),
	ui_component(Name, &Parent),
	m_entry(new hotkey_entry()),
	m_up_button(new Gtk::Button()),
	m_down_button(new Gtk::Button()),
	m_data(Data),
	m_step_increment(0.01),
	m_units(&typeid(scalar)),
	m_dragging(false),
	m_tap_started(false)
{
	set_name("k3d-spin-button");

	m_entry->set_name("entry");
	m_entry->set_width_chars(8);
	m_entry->signal_focus_out_event().connect(sigc::mem_fun(*this, &control::on_entry_focus_out_event));
	m_entry->signal_activate().connect(sigc::mem_fun(*this, &control::on_entry_activated));
	attach(*manage(m_entry), 0, 6, 0, 2);

	if(m_data.get() && m_data->writable())
	{
		// Setup up and down buttons
		setup_arrow_button(m_up_button, Gtk::ARROW_UP, true);
		setup_arrow_button(m_down_button, Gtk::ARROW_DOWN, false);

		// Setup VBox containing arrows
		attach(*manage(m_up_button), 6, 7, 0, 1);
		attach(*manage(m_down_button), 6, 7, 1, 2);

		tooltips().set_tip(*m_entry, _("Enter a new value.  Real-world units and simple math expressions are allowed."));
		tooltips().set_tip(*m_up_button, _("LMB-Drag to modify, LMB-Click to step, Tap Shift and Control while dragging to change sensitivity."));
		tooltips().set_tip(*m_down_button, _("LMB-Drag to modify, LMB-Click to step, Tap Shift and Control while dragging to change sensitivity."));

		// Make sure buttons can't get the focus (makes tabbing difficult)
		m_up_button->unset_flags(Gtk::CAN_FOCUS);
		m_down_button->unset_flags(Gtk::CAN_FOCUS);
	}
	else
	{
		m_entry->set_editable(false);
	}

	// Synchronize the view with the data source ...
	on_data_changed(0);

	// We want to be notified if the data source changes ...
	if(m_data.get())
		m_data->changed_signal().connect(sigc::mem_fun(*this, &control::on_data_changed));
}

void control::set_step_increment(const double StepIncrement)
{
	m_step_increment = StepIncrement;
	on_data_changed(0);
}

void control::set_units(const std::type_info& Units)
{
	m_units = &Units;
	on_data_changed(0);
}

const k3d::icommand_node::result control::execute_command(const std::string& Command, const std::string& Arguments)
{
	try
	{
		if(Command == "set_value")
		{
			interactive::set_text(*m_entry, Arguments);
			return RESULT_CONTINUE;
		}
		else if(Command == "increment_value")
		{
			return_val_if_fail(m_data.get(), RESULT_ERROR);

			const double current_value = m_data->value();
			const double new_value = k3d::from_string<double>(Arguments, 0.0);
			const double old_step_increment = m_step_increment;
			m_step_increment = new_value - current_value;
			interactive::activate(*m_up_button);
			m_step_increment = old_step_increment;

			return RESULT_CONTINUE;
		}
		else if(Command == "decrement_value")
		{
			return_val_if_fail(m_data.get(), RESULT_ERROR);

			const double current_value = m_data->value();
			const double new_value = k3d::from_string<double>(Arguments, 0.0);
			const double old_step_increment = m_step_increment;
			m_step_increment = current_value - new_value;
			interactive::activate(*m_down_button);
			m_step_increment = old_step_increment;

			return RESULT_CONTINUE;
		}
		else if(Command == "start_drag")
		{
			return_val_if_fail(m_data.get(), RESULT_ERROR);

			if(Arguments == "down")
			{
				interactive::move_pointer(*m_down_button);
			}
			else
			{
				interactive::move_pointer(*m_up_button);
			}

			if(m_data->state_recorder)
				m_data->state_recorder->start_recording(k3d::create_state_change_set(__PRETTY_FUNCTION__), __PRETTY_FUNCTION__);

			m_timer.restart();

			return RESULT_CONTINUE;
		}
		else if(Command == "drag_motion")
		{
			return_val_if_fail(m_data.get(), RESULT_ERROR);

			command_arguments arguments(Arguments);
			const double timestamp = arguments.get_double("timestamp");
			const k3d::point2 mouse_delta = arguments.get_point2("mouse_delta");
			const double value = arguments.get_double("value");

			interactive::warp_pointer(interactive::get_pointer() + mouse_delta, timestamp, m_timer);
			m_data->set_value(value);

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
			return_val_if_fail(m_data.get(), RESULT_ERROR);

			if(m_data->state_recorder)
			{
				std::stringstream buffer;
				buffer << std::setprecision(3) << m_data->value();
				m_data->state_recorder->commit_change_set(m_data->state_recorder->stop_recording(__PRETTY_FUNCTION__), m_data->change_message + ' ' + buffer.str(), __PRETTY_FUNCTION__);
			}

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
	Gtk::Arrow* const arrow = manage(new Gtk::Arrow(ArrowType, Gtk::SHADOW_NONE));
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

void control::on_data_changed(k3d::iunknown*)
{
	return_if_fail(m_data.get());
	display_value(m_data->value());
}

void control::display_value(const double Value)
{
	std::ostringstream buffer;

	if(*m_units == typeid(k3d::measurement::angle)) // Display angles in degrees, even though the internal unit is radians
		buffer << k3d::string_cast(boost::format("%g") % k3d::measurement::convert(k3d::measurement::quantity<k3d::measurement::angle>(Value, k3d::measurement::angle_units("rad")), k3d::measurement::angle_units("deg")).value()) << " deg";
	else if(*m_units == typeid(k3d::measurement::area))
		buffer << k3d::string_cast(boost::format("%g") % Value) << " m^2";
	else if(*m_units == typeid(k3d::measurement::distance))
		buffer << k3d::string_cast(boost::format("%g") % Value) << " m";
	else if(*m_units == typeid(k3d::measurement::force))
		buffer << k3d::string_cast(boost::format("%g") % Value) << " N";
	else if(*m_units == typeid(k3d::measurement::mass))
		buffer << k3d::string_cast(boost::format("%g") % Value) << " Kg";
	else if(*m_units == typeid(k3d::measurement::pressure))
		buffer << k3d::string_cast(boost::format("%g") % Value) << " Pa";
	else if(*m_units == typeid(k3d::measurement::time))
		buffer << k3d::string_cast(boost::format("%g") % Value) << " s";
	else if(*m_units == typeid(k3d::measurement::volume))
		buffer << k3d::string_cast(boost::format("%g") % Value) << " m^3";
	else if(*m_units == typeid(scalar))
		buffer << k3d::string_cast(boost::format("%g") % Value);
	else
		k3d::log() << error << "Unknown physical unit - " << m_units->name() << std::endl;

	m_entry->set_text(buffer.str());
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
	m_down_button->set_flags(Gtk::CAN_FOCUS);
	m_down_button->grab_focus();
	m_down_button->unset_flags(Gtk::CAN_FOCUS);
}

void control::on_manual_value()
{
	return_if_fail(m_data.get());

	m_entry->select_region(0, 0);
	const std::string new_text = m_entry->get_text();

	// Default our results to the current value, in case it doesn't parse ...
	const double original_value = m_data->value();
	double new_value = original_value;

	// Parse the input expression into value converting it to SI units automatically (it can do mathematical expressions, too, for fun)
	if(!k3d::measurement::parse(new_text, new_value, m_units))
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
		if(m_data->state_recorder)
			m_data->state_recorder->start_recording(k3d::create_state_change_set(__PRETTY_FUNCTION__), __PRETTY_FUNCTION__);

		m_data->set_value(new_value);

		// Turn this into an undo/redo -able event ...
		if(m_data->state_recorder)
			m_data->state_recorder->commit_change_set(m_data->state_recorder->stop_recording(__PRETTY_FUNCTION__), m_data->change_message + ' ' + new_text, __PRETTY_FUNCTION__);
	}
	else
	{
		display_value(new_value);
	}
}

void control::on_drag_pressed(const bool Up)
{
	return_if_fail(m_data.get());

	// Save which button was pressed
	m_up_button_pressed = Up;

	m_up_button->set_flags(Gtk::CAN_FOCUS);
	m_down_button->set_flags(Gtk::CAN_FOCUS);
	m_up_button->grab_focus();
	m_down_button->grab_focus();

	// Get the current mouse coordinates ...
	m_last_mouse = interactive::get_pointer();

	// Calculate the increment we should use while dragging ...
	m_drag_increment = std::abs(m_step_increment) * 0.2;
	if(!m_drag_increment)
		m_drag_increment = 0.002;

	// Connect idle timeout handler, called every 200ms
	m_drag_timeout = Glib::signal_timeout().connect(sigc::mem_fun(*this, &control::on_drag_timeout), 200);
	m_drag_first_timeout = true;

	// Turn this into an undo/redo -able event ...
	if(m_data->state_recorder)
		m_data->state_recorder->start_recording(k3d::create_state_change_set(__PRETTY_FUNCTION__), __PRETTY_FUNCTION__);
}

bool control::on_drag_motion_notify_event(GdkEventMotion* Event)
{
	return_val_if_fail(m_data.get(), false);

	// Get new mouse coordinates
	const k3d::point2 mouse = interactive::get_pointer();

	// Don't switch to drag mode until the mouse really moved
	if(!m_dragging)
	{
		if(k3d::distance(mouse, m_last_mouse) < 10)
			return false;

		m_dragging = true;
		m_timer.restart();

		record_command("start_drag", m_up_button_pressed ? "up" : "down");
	}

	// Update everything ...
	const double horizontal_length = m_last_mouse[0] - mouse[0];
	const double vertical_length = m_last_mouse[1] - mouse[1];
	double new_value = m_data->value();
	if(std::abs(horizontal_length) > std::abs(vertical_length))
	{
		// Dragging mostly horizontally : 1/10th unit increase
		new_value += m_drag_increment * 0.1 * (mouse[0] - m_last_mouse[0]);
	}
	else
	{
		// Dragging mostly vertically : one unit increase
		new_value += m_drag_increment * (m_last_mouse[1] - mouse[1]);
	}

	command_arguments arguments;
	arguments.append("timestamp", m_timer.elapsed());
	arguments.append("mouse_delta", mouse - m_last_mouse);
	arguments.append("value", new_value);
	record_command("drag_motion", arguments);

	m_data->set_value(new_value);
	m_last_mouse = mouse;

	// Wrap the mouse if it goes off the top-or-bottom of the screen ...
	const int screen_height = Gdk::Display::get_default()->get_default_screen()->get_height();
	const int border = 5;
	if(mouse[1] < border)
	{
		m_last_mouse = k3d::point2(mouse[0], screen_height - (border + 1));
		interactive::warp_pointer(m_last_mouse);
	}
	else if(screen_height - mouse[1] < border)
	{
		m_last_mouse = k3d::point2(mouse[0], (border + 1));
		interactive::warp_pointer(m_last_mouse);
	}

	// Wrap the mouse if it goes off the left-or-right of the screen ...
	const int screen_width = Gdk::Display::get_default()->get_default_screen()->get_width();
	if(mouse[0] < border)
	{
		m_last_mouse = k3d::point2(screen_width - (border + 1), mouse[1]);
		interactive::warp_pointer(m_last_mouse);
	}
	else if(screen_width - mouse[0] < border)
	{
		m_last_mouse = k3d::point2((border + 1), mouse[1]);
		interactive::warp_pointer(m_last_mouse);
	}

	return false;
}

bool control::on_drag_key_press_event(GdkEventKey* Event)
{
	if(!m_tap_started && Event->keyval == GDK_Shift_L || Event->keyval == GDK_Shift_R)
	{
		m_tap_started = true;
		m_drag_increment *= 10.0;
		record_command("increase_sensitivity");
		return true;
	}
	else if(!m_tap_started && Event->keyval == GDK_Control_L || Event->keyval == GDK_Control_R)
	{
		m_tap_started = true;
		m_drag_increment *= 0.1;
		record_command("decrease_sensitivity");
		return true;
	}

	return false;
}

bool control::on_drag_key_release_event(GdkEventKey* Event)
{
	m_tap_started = false;
	return false;
}

bool control::on_drag_timeout()
{
	// Step increment if the user doesn't move
	if(!m_dragging)
	{
		if(m_drag_first_timeout)
		{
			// Don't change value on first timeout
			m_drag_first_timeout = false;
			return true;
		}

		if(m_up_button_pressed)
			increment();
		else
			decrement();
	}

	return true;
}

void control::on_drag_released()
{
	return_if_fail(m_data.get());

	// If the user really didn't drag anywhere and value wasn't changed yet
	if(m_dragging)
	{
		record_command("end_drag");
	}
	else if(!m_dragging && m_drag_first_timeout)
	{
		if(m_up_button_pressed)
		{
			increment();
			record_command("increment_value", k3d::string_cast(m_data->value()));
		}
		else
		{
			decrement();
			record_command("decrement_value", k3d::string_cast(m_data->value()));
		}
	}
	else
	{
		record_command("set_value", k3d::string_cast(m_data->value()));
	}

	// Disconnect idle timeout
	m_drag_timeout.disconnect();

	// Turn this into an undo/redo -able event ...
	if(m_data->state_recorder)
	{
		// Format a limited-precision version of the new value, so we we don't create unreadably-long undo-node labels ...
		std::stringstream buffer;
		buffer << std::setprecision(3) << m_data->value();
		m_data->state_recorder->commit_change_set(m_data->state_recorder->stop_recording(__PRETTY_FUNCTION__), m_data->change_message + ' ' + buffer.str(), __PRETTY_FUNCTION__);
	}

	m_up_button->unset_flags(Gtk::CAN_FOCUS);
	m_down_button->unset_flags(Gtk::CAN_FOCUS);
	m_dragging = false;
}

void control::increment()
{
	return_if_fail(m_data.get());
	m_data->set_value(m_data->value() + m_step_increment);
}

void control::decrement()
{
	return_if_fail(m_data.get());
	m_data->set_value(m_data->value() - m_step_increment);
}

} // namespace spin_button

} // namespace libk3dngui

