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
	\author Tim Shead (tshead@k-3d.com)
*/

#include "button.h"
#include "point3.h"
#include "spin_button.h"
#include "widget_manip.h"

#include <k3d-i18n-config.h>
#include <k3dsdk/command_tree.h>
#include <k3dsdk/iproperty.h>
#include <k3dsdk/istate_recorder.h>
#include <k3dsdk/iwritable_property.h>
#include <k3dsdk/measurement.h>
#include <k3dsdk/types_ri.h>
#include <k3dsdk/state_change_set.h>
#include <k3dsdk/vectors.h>

#include <gtkmm/label.h>

namespace k3d
{

namespace ngui
{

namespace point
{

namespace detail
{

/// Adapts a k3d::iproperty object for use with point::control
class data_proxy :
	public idata_proxy
{
public:
	data_proxy(k3d::iproperty& Data, k3d::istate_recorder* const StateRecorder, const Glib::ustring& ChangeMessage) :
		idata_proxy(StateRecorder, ChangeMessage),
		m_readable_data(Data),
		m_writable_data(dynamic_cast<k3d::iwritable_property*>(&Data))
	{
	}

	bool writable()
	{
		return m_writable_data ? true : false;
	}
	
	const k3d::point3 value()
	{
		const std::type_info& type = m_readable_data.property_type();
		if(type == typeid(k3d::point3))
			return boost::any_cast<k3d::point3>(m_readable_data.property_internal_value());
		else if(type == typeid(k3d::vector3))
			return k3d::to_point(boost::any_cast<k3d::vector3>(m_readable_data.property_internal_value()));
		else if(type == typeid(k3d::normal3))
			return k3d::to_point(boost::any_cast<k3d::normal3>(m_readable_data.property_internal_value()));
		else
			k3d::log() << error << k3d_file_reference << "unknown property type: " << type.name() << std::endl;

		return k3d::point3(0, 0, 0);
	}

	void set_value(const k3d::point3& Value)
	{
		return_if_fail(m_writable_data);

		const std::type_info& type = m_readable_data.property_type();
		if(type == typeid(k3d::point3))
			m_writable_data->property_set_value(Value);
		else if(type == typeid(k3d::vector3))
			m_writable_data->property_set_value(k3d::to_vector(Value));
		else if(type == typeid(k3d::normal3))
			m_writable_data->property_set_value(k3d::to_normal(Value));
		else
			k3d::log() << error << k3d_file_reference << "unknown property type: " << type.name() << std::endl;
	}

	changed_signal_t& changed_signal()
	{
		return m_readable_data.property_changed_signal();
	}

private:
	k3d::iproperty& m_readable_data;
	k3d::iwritable_property* const m_writable_data;
};

} // namespace detail

/// Adapts a spin button to control a single point coordinate (indentified by index)
class spin_button_model :
	public spin_button::imodel
{
public:
	spin_button_model(point::idata_proxy& Data, const unsigned int Index) :
		m_data(Data),
		m_index(Index)
	{
		assert_warning(m_index < 3);
	}

	const Glib::ustring label()
	{
		return "";
	}

	k3d::bool_t writable()
	{
		return m_data.writable();
	}

	k3d::double_t value()
	{
		return m_data.value()[m_index];
	}

	void set_value(const double Value)
	{
		k3d::point3 coords = m_data.value();
		coords[m_index] = Value;
		m_data.set_value(coords);
	}

	sigc::connection connect_changed_signal(const sigc::slot<void>& Slot)
	{
		return m_data.changed_signal().connect(sigc::hide(Slot));
	}

	k3d::double_t step_increment()
	{
		return 0.1;
	}

	const std::type_info& units()
	{
		return typeid(k3d::measurement::distance);
	}

private:
	point::idata_proxy& m_data;
	const int m_index;
};

/////////////////////////////////////////////////////////////////////////////
// control

control::control(k3d::icommand_node& Parent, const std::string& Name, std::auto_ptr<idata_proxy> Data) :
	base(3, 3, false),
	m_data(Data),
	m_reset_button(0)
{
	k3d::command_tree().add(*this, Name, &Parent);

	spin_button::control* const x = new spin_button::control(*this, "x", new spin_button_model(*m_data, 0), m_data->state_recorder);
	spin_button::control* const y = new spin_button::control(*this, "y", new spin_button_model(*m_data, 1), m_data->state_recorder);
	spin_button::control* const z = new spin_button::control(*this, "z", new spin_button_model(*m_data, 2), m_data->state_recorder);

	attach(*Gtk::manage(new Gtk::Label(_("X"))), 0, 1, 0, 1);
	attach(*Gtk::manage(x), 1, 2, 0, 1);

	attach(*Gtk::manage(new Gtk::Label(_("Y"))), 0, 1, 1, 2);
	attach(*Gtk::manage(y), 1, 2, 1, 2);

	attach(*Gtk::manage(new Gtk::Label(_("Z"))), 0, 1, 2, 3);
	attach(*Gtk::manage(z), 1, 2, 2, 3);

	if(m_data.get() && m_data->writable())
	{
		m_reset_button = new Gtk::Button(_("Reset"));
		attach(*Gtk::manage(m_reset_button << connect_button(sigc::mem_fun(*this, &control::on_reset))), 2, 3, 1, 2);
	}
}

void control::on_reset()
{
	return_if_fail(m_data.get());

	// Turn this into an undo/redo -able event ...
	if(m_data->state_recorder)
		m_data->state_recorder->start_recording(k3d::create_state_change_set(K3D_CHANGE_SET_CONTEXT), K3D_CHANGE_SET_CONTEXT);

	m_data->set_value(k3d::point3(0, 0, 0));

	// Turn this into an undo/redo -able event ...
	if(m_data->state_recorder)
		m_data->state_recorder->commit_change_set(m_data->state_recorder->stop_recording(K3D_CHANGE_SET_CONTEXT), m_data->change_message + " reset", K3D_CHANGE_SET_CONTEXT);
}

/////////////////////////////////////////////////////////////////////////////
// proxy

std::auto_ptr<idata_proxy> proxy(k3d::iproperty& Property, k3d::istate_recorder* const StateRecorder, const Glib::ustring& ChangeMessage)
{
	return std::auto_ptr<idata_proxy>(new detail::data_proxy(Property, StateRecorder, ChangeMessage));
}

} // namespace point

} // namespace ngui

} // namespace k3d

