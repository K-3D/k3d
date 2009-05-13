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

#include "text.h"
#include "widget_manip.h"

#include <k3d-i18n-config.h>
#include <k3dsdk/inode.h>
#include <k3dsdk/iproperty.h>
#include <k3dsdk/istate_recorder.h>
#include <k3dsdk/iwritable_property.h>
#include <k3dsdk/result.h>
#include <k3dsdk/state_change_set.h>
#include <k3dsdk/string_cast.h>

#include <gtkmm/buttonbox.h>
#include <gtkmm/scrolledwindow.h>
#include <gtkmm/textview.h>
#include <gtkmm/window.h>

#include <boost/format.hpp>
#include <boost/scoped_ptr.hpp>

namespace k3d
{

namespace ngui
{

namespace text
{

/////////////////////////////////////////////////////////////////////////////
// property_model

/// Implementation of text::imodel for use with k3d::iproperty objects
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

	const k3d::bool_t writable()
	{
		return m_writable_data ? true : false;
	}

	const k3d::string_t value()
	{
		const std::type_info& type = m_readable_data.property_type();

		if(type == typeid(k3d::string_t))
			return boost::any_cast<k3d::string_t>(m_readable_data.property_internal_value());
		k3d::log() << error << k3d_file_reference << ": unknown property type: " << type.name() << std::endl;
		return k3d::string_t();
	}

	void set_value(const k3d::string_t& Value)
	{
		return_if_fail(m_writable_data);

		const std::type_info& type = m_readable_data.property_type();

		if(type == typeid(k3d::string_t))
			m_writable_data->property_set_value(Value);
		else
			k3d::log() << error << k3d_file_reference << ": unknown property type: " << type.name() << std::endl;
	}

	sigc::connection connect_changed_signal(const sigc::slot<void>& Slot)
	{
		return m_readable_data.property_changed_signal().connect(sigc::hide(Slot));
	}

private:
	property_model(const property_model& RHS);
	property_model& operator=(const property_model& RHS);

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
		m_state_recorder(StateRecorder)
	{
		assert(m_model.get());
	}

	/// Stores a reference to the underlying data object
	const boost::scoped_ptr<imodel> m_model;
	/// Stores a reference to the (optional) object for recording undo/redo data
	k3d::istate_recorder* const m_state_recorder;
	/// Stores the global accel group while the control has the keyboard focus
	Glib::RefPtr<Gtk::AccelGroup> m_disabled_accel_group;
	/// Provides the main view widget
	Gtk::TextView m_text_view;
};

/////////////////////////////////////////////////////////////////////////////
// control

control::control(imodel* const Model, k3d::istate_recorder* const StateRecorder) :
	m_implementation(new implementation(Model, StateRecorder))
{
	set_name("k3d-text");

	Gtk::ScrolledWindow* const scrolled_window = new Gtk::ScrolledWindow();
	scrolled_window->set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
	scrolled_window->add(m_implementation->m_text_view);
	pack_start(*Gtk::manage(scrolled_window), Gtk::PACK_EXPAND_WIDGET);

	if(m_implementation->m_model->writable())
	{
		m_implementation->m_text_view.set_editable(true);

		Gtk::Button* const apply_button =
			new Gtk::Button(_("Apply"))
				<< connect_button(sigc::mem_fun(*this, &control::on_apply))
				<< set_tooltip(_("Apply modifications."));

		Gtk::Button* const reset_button =
			new Gtk::Button(_("Reset"))
				<< connect_button(sigc::mem_fun(*this, &control::on_reset))
				<< set_tooltip(_("Reset modifications."));

		Gtk::HButtonBox* const bbox = new Gtk::HButtonBox(Gtk::BUTTONBOX_END);
		bbox->pack_start(*Gtk::manage(apply_button));
		bbox->pack_start(*Gtk::manage(reset_button));

		pack_start(*Gtk::manage(bbox));
	}
	else
	{
		m_implementation->m_text_view.set_editable(false);
	}

	// Keep track of when the TextView control receives/loses the keyboard focus, so we can disable/enable hotkeys ...
	m_implementation->m_text_view.signal_focus_in_event().connect(sigc::mem_fun(*this, &control::on_focus_in_event));
	m_implementation->m_text_view.signal_focus_out_event().connect(sigc::mem_fun(*this, &control::on_focus_out_event));

	// We want to be notified if the data source changes ...
	m_implementation->m_model->connect_changed_signal(sigc::mem_fun(*this, &control::on_reset));

	// Synchronize the view with the data source ...
	on_reset();
}

control::~control()
{
	delete m_implementation;
}

bool control::on_focus_in_event(GdkEventFocus* Event)
{
	// Disable accelerators for this window
	if(Gtk::Window* const window = dynamic_cast<Gtk::Window*>(get_toplevel()))
	{
		m_implementation->m_disabled_accel_group = window->get_accel_group();
		window->remove_accel_group(window->get_accel_group());
	}

	return base::on_focus_in_event(Event);
}

bool control::on_focus_out_event(GdkEventFocus* Event)
{
	// Enable accelerators for this window
	if(Gtk::Window* const window = dynamic_cast<Gtk::Window*>(get_toplevel()))
	{
		window->add_accel_group(m_implementation->m_disabled_accel_group);
		m_implementation->m_disabled_accel_group.clear();
	}

	return base::on_focus_out_event(Event);
}

void control::on_apply()
{
	// If the value didn't change, we're done ...
	const k3d::string_t new_value = m_implementation->m_text_view.get_buffer()->get_text();
	if(new_value == m_implementation->m_model->value())
		return;

	// Turn this into an undo/redo -able event ...
	if(m_implementation->m_state_recorder)
		m_implementation->m_state_recorder->start_recording(k3d::create_state_change_set(K3D_CHANGE_SET_CONTEXT), K3D_CHANGE_SET_CONTEXT);

	// Update everything with the new value ...
	m_implementation->m_model->set_value(new_value);

	// Turn this into an undo/redo -able event ...
	if(m_implementation->m_state_recorder)
		m_implementation->m_state_recorder->commit_change_set(m_implementation->m_state_recorder->stop_recording(K3D_CHANGE_SET_CONTEXT), change_message(m_implementation->m_model->value()), K3D_CHANGE_SET_CONTEXT);
}

void control::on_reset()
{
	m_implementation->m_text_view.get_buffer()->set_text(m_implementation->m_model->value());
}

const k3d::string_t control::change_message(const k3d::string_t& Value)
{
	return k3d::string_cast(boost::format(_("Change %1%")) % m_implementation->m_model->label().raw());
}

} // namespace text

} // namespace ngui

} // namespace k3d

