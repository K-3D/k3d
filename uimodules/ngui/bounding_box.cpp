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
#include <k3dsdk/iproperty.h>
#include <k3dsdk/istate_recorder.h>
#include <k3dsdk/iwritable_property.h>
#include <k3dsdk/measurement.h>
#include <k3dsdk/state_change_set.h>

#include <gtkmm/label.h>

namespace libk3dngui
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
		return boost::any_cast<k3d::bounding_box3>(m_property.property_value());
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

/// Adapts a spin button to control a single bounding_box coordinate
class spin_button_proxy_t :
	public spin_button::idata_proxy
{
public:
	spin_button_proxy_t(bounding_box::idata_proxy& Data, double k3d::bounding_box3::* Value) :
		spin_button::idata_proxy(Data.state_recorder, Data.change_message),
		m_data(Data),
		m_value(Value)
	{
	}

	bool writable()
	{
		return true;
	}

	double value()
	{
		return m_data.value().*m_value;
	}

	void set_value(const double Value)
	{
		k3d::bounding_box3 box = m_data.value();
		box.*m_value = Value;
		m_data.set_value(box);
	}

	changed_signal_t& changed_signal()
	{
		return m_data.changed_signal();
	}

private:
	bounding_box::idata_proxy& m_data;
	double k3d::bounding_box3::* m_value;
};

/// Convenience factory method for creating spin_button_proxy_t objects
std::auto_ptr<spin_button::idata_proxy> spin_button_proxy(bounding_box::idata_proxy& Data, double k3d::bounding_box3::* Value)
{
	return std::auto_ptr<spin_button::idata_proxy>(new spin_button_proxy_t(Data, Value));
}

} // namespace detail

/////////////////////////////////////////////////////////////////////////////
// control

control::control(k3d::icommand_node& Parent, const std::string& Name, std::auto_ptr<idata_proxy> Data) :
	base(3, 3, false),
	ui_component(Name, &Parent),
	m_data(Data)
{
	spin_button::control* const nx = new spin_button::control(*this, "nx", detail::spin_button_proxy(*m_data, &k3d::bounding_box3::nx));
	spin_button::control* const px = new spin_button::control(*this, "px", detail::spin_button_proxy(*m_data, &k3d::bounding_box3::px));
	spin_button::control* const ny = new spin_button::control(*this, "ny", detail::spin_button_proxy(*m_data, &k3d::bounding_box3::ny));
	spin_button::control* const py = new spin_button::control(*this, "py", detail::spin_button_proxy(*m_data, &k3d::bounding_box3::py));
	spin_button::control* const nz = new spin_button::control(*this, "nz", detail::spin_button_proxy(*m_data, &k3d::bounding_box3::nz));
	spin_button::control* const pz = new spin_button::control(*this, "pz", detail::spin_button_proxy(*m_data, &k3d::bounding_box3::pz));

	nx->set_units(typeid(k3d::measurement::distance));
	px->set_units(typeid(k3d::measurement::distance));
	ny->set_units(typeid(k3d::measurement::distance));
	py->set_units(typeid(k3d::measurement::distance));
	nz->set_units(typeid(k3d::measurement::distance));
	pz->set_units(typeid(k3d::measurement::distance));

	nx->set_step_increment(0.1);
	px->set_step_increment(0.1);
	ny->set_step_increment(0.1);
	py->set_step_increment(0.1);
	nz->set_step_increment(0.1);
	pz->set_step_increment(0.1);

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

} // namespace k3d


