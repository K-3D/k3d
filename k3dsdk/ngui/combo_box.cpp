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
		\brief Implements the k3d::combo_box classes, which provide a model-view-controller UI for enumerated (fixed set of choices) types
		\author Tim Shead (tshead@k-3d.com)
*/

#include "combo_box.h"
#include "interactive.h"
#include "utility.h"

#include <k3dsdk/command_tree.h>
#include <k3dsdk/istate_recorder.h>
#include <k3dsdk/state_change_set.h>

#include <gtkmm/entry.h>
#include <gtkmm/window.h>

namespace libk3dngui
{

namespace combo_box
{

/// Specialization of k3d::combo_box::data_proxy for use with k3d::iproperty objects
template<>
class data_proxy<k3d::iproperty> :
	public idata_proxy
{
public:
	typedef k3d::iproperty data_t;

	data_proxy(data_t& Data, k3d::istate_recorder* const StateRecorder, const Glib::ustring& ChangeMessage) :
		idata_proxy(StateRecorder, ChangeMessage),
		m_readable_data(Data),
		m_writable_data(dynamic_cast<k3d::iwritable_property*>(&Data))
	{
	}

	const std::string value()
	{
		return boost::any_cast<std::string>(m_readable_data.property_internal_value());
	}

	void set_value(const std::string& Value)
	{
		return_if_fail(m_writable_data);
		return_if_fail(m_writable_data->property_set_value(Value));
	}

	changed_signal_t& changed_signal()
	{
		return m_readable_data.property_changed_signal();
	}

private:
	data_proxy(const data_proxy& RHS);
	data_proxy& operator=(const data_proxy& RHS);

	data_t& m_readable_data;
	k3d::iwritable_property* const m_writable_data;
};

std::auto_ptr<idata_proxy> proxy(k3d::iproperty& Data, k3d::istate_recorder* const StateRecorder, const Glib::ustring& ChangeMessage)
{
	return std::auto_ptr<idata_proxy>(new data_proxy<k3d::iproperty>(Data, StateRecorder, ChangeMessage));
}

/////////////////////////////////////////////////////////////////////////////
// control

control::control(k3d::icommand_node& Parent, const std::string& Name, std::auto_ptr<idata_proxy> Data) :
	m_data(Data)
{
	k3d::command_tree().add(*this, Name, &Parent);

	if(m_data.get())
		m_data->changed_signal().connect(sigc::mem_fun(*this, &control::data_changed));

	m_model = Gtk::ListStore::create(m_columns);
	set_model(m_model);
	set_text_column(m_columns.value);

	Gtk::Entry* const entry = dynamic_cast<Gtk::Entry*>(get_child());

	signal_changed().connect(sigc::mem_fun(*this, &control::on_list_changed));
	entry->signal_focus_in_event().connect(sigc::mem_fun(*this, &control::on_entry_focus_in_event));
	entry->signal_focus_out_event().connect(sigc::mem_fun(*this, &control::on_entry_focus_out_event));
	entry->signal_activate().connect(sigc::mem_fun(*this, &control::on_entry_activate));
}

const k3d::icommand_node::result control::execute_command(const std::string& Command, const std::string& Arguments)
{
	/** \todo Improve the tutorial interactivity of this thing */
	if(Command == "value")
	{
		return_val_if_fail(m_data.get(), RESULT_ERROR);

		// If the value hasn't changed, we're done ...
		if(Arguments == m_data->value())
			return RESULT_CONTINUE;

		Gtk::Entry* const entry = dynamic_cast<Gtk::Entry*>(get_child());
		return_val_if_fail(entry, RESULT_ERROR);

		interactive::set_text(*entry, Arguments);
		return RESULT_CONTINUE;
	}

	return ui_component::execute_command(Command, Arguments);
}

void control::set_values(const values_t& Values)
{
	m_model->clear();
	for(values_t::const_iterator value = Values.begin(); value != Values.end(); ++value)
	{
		Gtk::TreeRow row = *m_model->append();
		row[m_columns.value] = *value;
	}

	data_changed(0);
}

void control::data_changed(k3d::ihint*)
{
	return_if_fail(m_data.get());

	const std::string value = m_data->value();
	add_impromptu_value(value);

	Gtk::Entry* const entry = dynamic_cast<Gtk::Entry*>(get_child());
	entry->set_text(value);
}

void control::on_list_changed()
{
	set_new_value();
}

bool control::on_entry_focus_in_event(GdkEventFocus* Event)
{
	// Disable accelerators for this window
	Gtk::Window* const window = dynamic_cast<Gtk::Window*>(get_toplevel());
	if(window)
	{
		m_disabled_accel_group = window->get_accel_group();
		window->remove_accel_group(window->get_accel_group());
	}
	return false;
}

bool control::on_entry_focus_out_event(GdkEventFocus* Event)
{
	// Enable accelerators for this window
	Gtk::Window* const window = dynamic_cast<Gtk::Window*>(get_toplevel());
	if(window)
	{
		window->add_accel_group(m_disabled_accel_group);
		m_disabled_accel_group.clear();
	}
	set_new_value();
	return false;
}

void control::on_entry_activate()
{
	child_focus(Gtk::DIR_TAB_FORWARD);
}

void control::add_impromptu_value(const std::string& Value)
{
	// Never add empty string to the list ...
	if(Value.empty())
		return;

	// Never add duplicates to the list ...
	const Gtk::TreeNodeChildren children = m_model->children();
	for(Gtk::TreeNodeChildren::const_iterator child = children.begin(); child != children.end(); ++child)
	{
		if(Value == (*child)[m_columns.value])
			return;
	}

	Gtk::TreeRow row = *m_model->append();
	row[m_columns.value] = Value;
}

void control::set_new_value()
{
	Gtk::Entry* const entry = dynamic_cast<Gtk::Entry*>(get_child());
	const std::string value = entry->get_text();

	return_if_fail(m_data.get());

	add_impromptu_value(value);
	if(value == m_data->value())
		return;

	record_command("value", value);

	k3d::istate_recorder* const state_recorder = m_data->state_recorder;
	const Glib::ustring change_message = m_data->change_message;

	if(state_recorder)
		state_recorder->start_recording(k3d::create_state_change_set(K3D_CHANGE_SET_CONTEXT), K3D_CHANGE_SET_CONTEXT);

	m_data->set_value(value);

	if(state_recorder)
		state_recorder->commit_change_set(state_recorder->stop_recording(K3D_CHANGE_SET_CONTEXT), change_message + " " + value, K3D_CHANGE_SET_CONTEXT);
}

} // namespace combo_box

} // namespace libk3dngui
