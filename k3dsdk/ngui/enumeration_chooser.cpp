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
		\brief Implements the k3d::enumeration_chooser classes, which provide a model-view-controller UI for enumerated (fixed set of choices) types
		\author Tim Shead (tshead@k-3d.com)
*/

#include "enumeration_chooser.h"
#include "interactive.h"
#include "options.h"
#include "utility.h"

#include <k3dsdk/command_tree.h>
#include <k3dsdk/istate_recorder.h>
#include <k3dsdk/state_change_set.h>

#include <gtkmm/entry.h>

namespace libk3dngui
{

namespace enumeration_chooser
{

/// Specialization of k3d::enumeration_chooser::data_proxy for use with k3d::iproperty objects
template<>
class data_proxy<k3d::iproperty> :
	public idata_proxy
{
public:
	typedef k3d::iproperty data_t;

	data_proxy(data_t& Data, k3d::istate_recorder* const StateRecorder, const Glib::ustring& ChangeMessage) :
		idata_proxy(StateRecorder, ChangeMessage),
		m_readable_data(Data),
		m_enumeration_data(dynamic_cast<k3d::ienumeration_property*>(&Data)),
		m_writable_data(dynamic_cast<k3d::iwritable_property*>(&Data))
	{
	}

	const k3d::ienumeration_property::enumeration_values_t enumeration_values()
	{
		return_val_if_fail(m_enumeration_data, k3d::ienumeration_property::enumeration_values_t());
		return m_enumeration_data->enumeration_values();
	}

	const std::string value()
	{
		return boost::any_cast<std::string>(m_readable_data.property_value());
	}

	void set_value(const std::string& Value)
	{
		return_if_fail(m_writable_data);
		return_if_fail(m_writable_data->property_set_value(Value));
	}

	sigc::connection connect_changed(const sigc::slot<void, k3d::iunknown*>& Slot)
	{
		return m_readable_data.property_changed_signal().connect(Slot);
	}

	sigc::connection connect_enumeration_values_changed(const sigc::slot<void>& Slot)
	{
		return_val_if_fail(m_enumeration_data, sigc::connection());
		return m_enumeration_data->connect_enumeration_values_changed(Slot);
	}

private:
	data_proxy(const data_proxy& RHS);
	data_proxy& operator=(const data_proxy& RHS);

	data_t& m_readable_data;
	k3d::ienumeration_property* const m_enumeration_data;
	k3d::iwritable_property* const m_writable_data;
};

std::auto_ptr<idata_proxy> proxy(k3d::iproperty& Data, k3d::istate_recorder* const StateRecorder, const Glib::ustring& ChangeMessage)
{
	return std::auto_ptr<idata_proxy>(new data_proxy<k3d::iproperty>(Data, StateRecorder, ChangeMessage));
}

/////////////////////////////////////////////////////////////////////////////
// control

control::control(k3d::icommand_node& Parent, const std::string& Name, std::auto_ptr<idata_proxy> Data) :
	ui_component(Name, &Parent),
	m_data(Data)
{
	if(m_data.get())
	{
		m_data->connect_changed(sigc::mem_fun(*this, &control::on_data_changed));
		m_data->connect_enumeration_values_changed(sigc::mem_fun(*this, &control::on_enumeration_values_changed));
	}

	m_model = Gtk::ListStore::create(m_columns);
	set_model(m_model);

	Gtk::CellRendererText* const cell_renderer = new Gtk::CellRendererText();
	pack_start(*manage(cell_renderer), true);
	add_attribute(cell_renderer->property_text(), m_columns.label);

	on_enumeration_values_changed();
	on_data_changed(0);

	signal_changed().connect(sigc::mem_fun(*this, &control::on_list_changed));
}

const k3d::icommand_node::result control::execute_command(const std::string& Command, const std::string& Arguments)
{
	/** \todo Improve the tutorial interactivity of this thing */
	if(Command == "value")
	{
		const double speed = options::tutorial_speed();
		return_val_if_fail(speed, RESULT_ERROR);

		interactive::show(*this);
		interactive::move_pointer(*this);

		popup();

		const Gtk::TreeNodeChildren children = m_model->children();
		for(Gtk::TreeNodeChildren::const_iterator child = children.begin(); child != children.end(); ++child)
		{
			const std::string child_value = (*child)[m_columns.value];

			if(Arguments == child_value)
			{
				set_active(child);

				non_blocking_sleep(0.5 / speed);
				popdown();

				return RESULT_CONTINUE;
			}
		}

		k3d::log() << error << "Enumeration value [" << Arguments << "] does not match any allowed values" << std::endl;
		return RESULT_ERROR;
	}

	return ui_component::execute_command(Command, Arguments);
}

void control::on_data_changed(k3d::iunknown*)
{
	return_if_fail(m_data.get());

	const std::string value = m_data->value();

	const Gtk::TreeNodeChildren children = m_model->children();
	for(Gtk::TreeNodeChildren::const_iterator child = children.begin(); child != children.end(); ++child)
	{
		const std::string child_value = (*child)[m_columns.value];

		if(value == child_value)
		{
			set_active(child);
			return;
		}
	}

	k3d::log() << error << "Current enumeration value [" << value << "] does not match any allowed values" << std::endl;
}

void control::on_enumeration_values_changed()
{
	return_if_fail(m_data.get());

	m_model->clear();

	const k3d::ienumeration_property::enumeration_values_t values = m_data->enumeration_values();
	for(k3d::ienumeration_property::enumeration_values_t::const_iterator choice = values.begin(); choice != values.end(); ++choice)
	{
		Gtk::TreeRow row = *m_model->append();
		row[m_columns.label] = choice->label;
		row[m_columns.value] = choice->value;
		row[m_columns.description] = choice->description;
	}

	set_active(0);
}

void control::on_list_changed()
{
	Gtk::TreeIter active = get_active();
	return_if_fail(active != m_model->children().end());

	Gtk::TreeRow row = *active;
	const std::string value = row[m_columns.value];

	k3d::command_tree().command_signal().emit(*this, k3d::icommand_node::COMMAND_INTERACTIVE, "value", value);

	return_if_fail(m_data.get());

	if(value == m_data->value())
		return;

	k3d::istate_recorder* const state_recorder = m_data->state_recorder;
	const Glib::ustring change_message = m_data->change_message;

	if(state_recorder)
		state_recorder->start_recording(k3d::create_state_change_set(K3D_CHANGE_SET_CONTEXT), K3D_CHANGE_SET_CONTEXT);

	m_data->set_value(value);

	if(state_recorder)
		state_recorder->commit_change_set(state_recorder->stop_recording(K3D_CHANGE_SET_CONTEXT), change_message + " " + value, K3D_CHANGE_SET_CONTEXT);
}

} // namespace enumeration_chooser

} // namespace libk3dngui

