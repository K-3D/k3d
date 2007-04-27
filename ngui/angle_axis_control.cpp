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

#include "angle_axis_control.h"
#include "button.h"
#include "interactive.h"
#include "spin_button.h"
#include "widget_manip.h"

#include <k3dsdk/i18n.h>
#include <k3dsdk/iproperty.h>
#include <k3dsdk/istate_recorder.h>
#include <k3dsdk/iwritable_property.h>
#include <k3dsdk/measurement.h>
#include <k3dsdk/state_change_set.h>

#include <gtkmm/label.h>

namespace libk3dngui
{

namespace angle_axis
{

namespace detail
{

/// Adapts a k3d::iproperty object for use with angle_axis::control
class data_proxy :
	public idata_proxy
{
public:
	data_proxy(k3d::iproperty& Property, k3d::istate_recorder* const StateRecorder, const Glib::ustring& ChangeMessage) :
		idata_proxy(StateRecorder, ChangeMessage),
		m_property(Property)
	{
		assert(Property.property_type() == typeid(k3d::angle_axis));
	}

	const k3d::angle_axis value()
	{
		return boost::any_cast<k3d::angle_axis>(m_property.property_value());
	}

	void set_value(const k3d::angle_axis& Value)
	{
		k3d::iwritable_property* const writable_property = dynamic_cast<k3d::iwritable_property*>(&m_property);
		return_if_fail(writable_property);

		writable_property->property_set_value(boost::any(Value));
	}

	changed_signal_t& changed_signal()
	{
		return m_property.property_changed_signal();
	}

private:
	k3d::iproperty& m_property;
};

/// Adapts a spin button to control a single angle_axis coordinate (indentified by index)
class spin_button_proxy_t :
	public spin_button::idata_proxy
{
public:
	spin_button_proxy_t(angle_axis::idata_proxy& Data, const unsigned int Index) :
		spin_button::idata_proxy(Data.state_recorder, Data.change_message),
		m_data(Data),
		m_index(Index)
	{
		// Sanity checks ...
		assert_warning(m_index < 3);
	}

	bool writable()
	{
		return true;
	}

	double value()
	{
		k3d::quaternion quat(m_data.value());
		return k3d::euler_angles(quat, k3d::euler_angles::XYZstatic)[m_index];
	}

	void set_value(const double Value)
	{
		k3d::quaternion quaternion(m_data.value());
		k3d::euler_angles eulerangles(quaternion, k3d::euler_angles::XYZstatic);
		eulerangles[m_index] = Value;
		m_data.set_value(k3d::angle_axis(k3d::quaternion(eulerangles)));
	}

	changed_signal_t& changed_signal()
	{
		return m_data.changed_signal();
	}

private:
	angle_axis::idata_proxy& m_data;
	const unsigned int m_index;
};

/// Convenience factory method for creating spin_button_proxy_t objects
std::auto_ptr<spin_button::idata_proxy> spin_button_proxy(angle_axis::idata_proxy& Data, const unsigned int Index)
{
	return std::auto_ptr<spin_button::idata_proxy>(new spin_button_proxy_t(Data, Index));
}

} // namespace detail

/////////////////////////////////////////////////////////////////////////////
// control

control::control(k3d::icommand_node& Parent, const std::string& Name, std::auto_ptr<idata_proxy> Data) :
	base(3, 3, false),
	ui_component(Name, &Parent),
	m_data(Data),
	m_reset_button(new Gtk::Button(_("Reset")))
{
	spin_button::control* const x = new spin_button::control(*this, "x", detail::spin_button_proxy(*m_data, 0));
	spin_button::control* const y = new spin_button::control(*this, "y", detail::spin_button_proxy(*m_data, 1));
	spin_button::control* const z = new spin_button::control(*this, "z", detail::spin_button_proxy(*m_data, 2));

	x->set_units(typeid(k3d::measurement::angle));
	y->set_units(typeid(k3d::measurement::angle));
	z->set_units(typeid(k3d::measurement::angle));

	x->set_step_increment(k3d::radians(1.0));
	y->set_step_increment(k3d::radians(1.0));
	z->set_step_increment(k3d::radians(1.0));

	attach(*Gtk::manage(new Gtk::Label(_("X"))), 0, 1, 0, 1);
	attach(*Gtk::manage(x), 1, 2, 0, 1);

	attach(*Gtk::manage(new Gtk::Label(_("Y"))), 0, 1, 1, 2);
	attach(*Gtk::manage(y), 1, 2, 1, 2);

	attach(*Gtk::manage(new Gtk::Label(_("Z"))), 0, 1, 2, 3);
	attach(*Gtk::manage(z), 1, 2, 2, 3);

	attach(*Gtk::manage(m_reset_button << connect_button(sigc::mem_fun(*this, &control::on_reset))), 2, 3, 1, 2);
}

const k3d::icommand_node::result control::execute_command(const std::string& Command, const std::string& Arguments)
{
	if(Command == "reset")
	{
		interactive::activate(*m_reset_button);
		return RESULT_CONTINUE;
	}

	return ui_component::execute_command(Command, Arguments);
}

void control::on_reset()
{
	return_if_fail(m_data.get());

	record_command("reset");

	// Turn this into an undo/redo -able event ...
	if(m_data->state_recorder)
		m_data->state_recorder->start_recording(k3d::create_state_change_set(__PRETTY_FUNCTION__), __PRETTY_FUNCTION__);

	m_data->set_value(k3d::angle_axis(0, k3d::point3(1, 0, 0)));

	// Turn this into an undo/redo -able event ...
	if(m_data->state_recorder)
		m_data->state_recorder->commit_change_set(m_data->state_recorder->stop_recording(__PRETTY_FUNCTION__), m_data->change_message + " reset", __PRETTY_FUNCTION__);
}

/////////////////////////////////////////////////////////////////////////////
// proxy

std::auto_ptr<idata_proxy> proxy(k3d::iproperty& Property, k3d::istate_recorder* const StateRecorder, const Glib::ustring& ChangeMessage)
{
	return std::auto_ptr<idata_proxy>(new detail::data_proxy(Property, StateRecorder, ChangeMessage));
}

} // namespace angle_axis

} // namespace k3d

