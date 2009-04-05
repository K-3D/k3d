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

#include "bounding_box.h"
#include "button.h"
#include "spin_button.h"
#include "widget_manip.h"

#include <k3d-i18n-config.h>
#include <k3dsdk/command_tree.h>
#include <k3dsdk/iproperty.h>
#include <k3dsdk/istate_recorder.h>
#include <k3dsdk/iwritable_property.h>
#include <k3dsdk/measurement.h>
#include <k3dsdk/state_change_set.h>

#include <gtkmm/label.h>

namespace k3d
{

namespace ngui
{

namespace bounding_box
{

namespace detail
{

/// Adapts a k3d::iproperty object for use with bounding_box::control
class data_proxy :
	public idata_proxy
{
public:
	data_proxy(k3d::iproperty& Property, k3d::istate_recorder* const StateRecorder, const Glib::ustring& ChangeMessage) :
		idata_proxy(StateRecorder, ChangeMessage),
		m_property(Property)
	{
		assert(Property.property_type() == typeid(k3d::bounding_box3));
	}

	const k3d::bounding_box3 value()
	{
		return boost::any_cast<k3d::bounding_box3>(m_property.property_internal_value());
	}

	void set_value(const k3d::bounding_box3& Value)
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

} // namespace detail

/// Adapts a spin button to control a single bounding_box coordinate
class spin_button_model :
	public spin_button::imodel
{
public:
	spin_button_model(bounding_box::idata_proxy& Data, double k3d::bounding_box3::* Value) :
		m_data(Data),
		m_value(Value)
	{
	}

	const Glib::ustring label()
	{
		return "";
	}

	const k3d::bool_t writable()
	{
		return true;
	}

	const k3d::double_t value()
	{
		return m_data.value().*m_value;
	}

	void set_value(const double Value)
	{
		k3d::bounding_box3 box = m_data.value();
		box.*m_value = Value;
		m_data.set_value(box);
	}

	sigc::connection connect_changed_signal(const sigc::slot<void>& Slot)
	{
		return m_data.changed_signal().connect(sigc::hide(Slot));
	}

	const k3d::double_t step_increment()
	{
		return 0.1;
	}

	const std::type_info& units()
	{
		return typeid(k3d::measurement::distance);
	}

private:
	bounding_box::idata_proxy& m_data;
	double k3d::bounding_box3::* m_value;
};

/////////////////////////////////////////////////////////////////////////////
// control

control::control(k3d::icommand_node& Parent, const std::string& Name, std::auto_ptr<idata_proxy> Data) :
	base(3, 3, false),
	m_data(Data)
{
	k3d::command_tree().add(*this, Name, &Parent);

	spin_button::control* const nx = new spin_button::control(*this, "nx", new spin_button_model(*m_data, &k3d::bounding_box3::nx), m_data->state_recorder);
	spin_button::control* const px = new spin_button::control(*this, "px", new spin_button_model(*m_data, &k3d::bounding_box3::px), m_data->state_recorder);
	spin_button::control* const ny = new spin_button::control(*this, "ny", new spin_button_model(*m_data, &k3d::bounding_box3::ny), m_data->state_recorder);
	spin_button::control* const py = new spin_button::control(*this, "py", new spin_button_model(*m_data, &k3d::bounding_box3::py), m_data->state_recorder);
	spin_button::control* const nz = new spin_button::control(*this, "nz", new spin_button_model(*m_data, &k3d::bounding_box3::nz), m_data->state_recorder);
	spin_button::control* const pz = new spin_button::control(*this, "pz", new spin_button_model(*m_data, &k3d::bounding_box3::pz), m_data->state_recorder);

	attach(*Gtk::manage(new Gtk::Label(_("X"))), 0, 1, 0, 1);
	attach(*Gtk::manage(nx), 0, 1, 1, 2);
	attach(*Gtk::manage(px), 0, 1, 2, 3);

	attach(*Gtk::manage(new Gtk::Label(_("Y"))), 1, 2, 0, 1);
	attach(*Gtk::manage(ny), 1, 2, 1, 2);
	attach(*Gtk::manage(py), 1, 2, 2, 3);

	attach(*Gtk::manage(new Gtk::Label(_("Z"))), 2, 3, 0, 1);
	attach(*Gtk::manage(nz), 2, 3, 1, 2);
	attach(*Gtk::manage(pz), 2, 3, 2, 3);
}

const k3d::icommand_node::result control::execute_command(const std::string& Command, const std::string& Arguments)
{
	return ui_component::execute_command(Command, Arguments);
}

/////////////////////////////////////////////////////////////////////////////
// proxy

std::auto_ptr<idata_proxy> proxy(k3d::iproperty& Property, k3d::istate_recorder* const StateRecorder, const Glib::ustring& ChangeMessage)
{
	return std::auto_ptr<idata_proxy>(new detail::data_proxy(Property, StateRecorder, ChangeMessage));
}

} // namespace bounding_box

} // namespace ngui

} // namespace k3d


