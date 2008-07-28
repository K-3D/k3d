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

#include <k3d-i18n-config.h>
#include <k3dsdk/command_tree.h>
#include <k3dsdk/inode.h>
#include <k3dsdk/istate_recorder.h>
#include <k3dsdk/state_change_set.h>

#include <gtkmm/entry.h>

#include <boost/scoped_ptr.hpp>

namespace libk3dngui
{

namespace enumeration_chooser
{

/////////////////////////////////////////////////////////////////////////////
// property_model

/// Implementation of enumeration_chooser::imodel for use with k3d::iproperty objects
class property_model :
	public imodel
{
public:
	property_model(k3d::iproperty& Data) :
		m_readable_data(Data),
		m_enumeration_data(dynamic_cast<k3d::ienumeration_property*>(&Data)),
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

	const k3d::ienumeration_property::enumeration_values_t enumeration_values()
	{
		return_val_if_fail(m_enumeration_data, k3d::ienumeration_property::enumeration_values_t());
		return m_enumeration_data->enumeration_values();
	}

	const k3d::string_t value()
	{
		return boost::any_cast<k3d::string_t>(m_readable_data.property_internal_value());
	}

	void set_value(const k3d::string_t& Value)
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
	property_model(const property_model&);
	property_model& operator=(const property_model&);

	k3d::iproperty& m_readable_data;
	k3d::ienumeration_property* const m_enumeration_data;
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

		m_list_model = Gtk::ListStore::create(m_columns);
	}

	/// Stores the underlying data object
	boost::scoped_ptr<imodel> m_model;
	/// Stores a referenence to the (optional) object for recording undo/redo data
	k3d::istate_recorder* const m_state_recorder;

	/// Defines a data model for the underlying combo box widget
	class columns :
		public Gtk::TreeModelColumnRecord
	{
	public:
		columns()
		{
			add(label);
			add(value);
			add(description);
		}

		Gtk::TreeModelColumn<Glib::ustring> label;
		Gtk::TreeModelColumn<k3d::string_t> value;
		Gtk::TreeModelColumn<Glib::ustring> description;
	};

	/// Defines a data model for the underlying combo box widget
	columns m_columns;
	/// Stores data for the underlying combo box widget
	Glib::RefPtr<Gtk::ListStore> m_list_model;
};

/////////////////////////////////////////////////////////////////////////////
// control

control::control(k3d::icommand_node& Parent, const k3d::string_t& Name, imodel* const Model, k3d::istate_recorder* const StateRecorder) :
	m_implementation(new implementation(Model, StateRecorder))
{
	k3d::command_tree().add(*this, Name, &Parent);

	if(Model)
	{
		Model->connect_changed(sigc::mem_fun(*this, &control::on_data_changed));
		Model->connect_enumeration_values_changed(sigc::mem_fun(*this, &control::on_enumeration_values_changed));
	}

	set_model(m_implementation->m_list_model);

	Gtk::CellRendererText* const cell_renderer = new Gtk::CellRendererText();
	pack_start(*manage(cell_renderer), true);
	add_attribute(cell_renderer->property_text(), m_implementation->m_columns.label);

	on_enumeration_values_changed();
	on_data_changed(0);

	signal_changed().connect(sigc::mem_fun(*this, &control::on_list_changed));
}

const k3d::icommand_node::result control::execute_command(const k3d::string_t& Command, const k3d::string_t& Arguments)
{
	/** \todo Improve the tutorial interactivity of this thing */
	if(Command == "value")
	{
		const double speed = options::tutorial_speed();
		return_val_if_fail(speed, RESULT_ERROR);

		interactive::show(*this);
		interactive::move_pointer(*this);

		popup();

		const Gtk::TreeNodeChildren children = m_implementation->m_list_model->children();
		for(Gtk::TreeNodeChildren::const_iterator child = children.begin(); child != children.end(); ++child)
		{
			const k3d::string_t child_value = (*child)[m_implementation->m_columns.value];

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

control::~control()
{
	delete m_implementation;
}

void control::on_data_changed(k3d::iunknown*)
{
	return_if_fail(m_implementation->m_model.get());

	const k3d::string_t value = m_implementation->m_model->value();

	const Gtk::TreeNodeChildren children = m_implementation->m_list_model->children();
	for(Gtk::TreeNodeChildren::const_iterator child = children.begin(); child != children.end(); ++child)
	{
		const k3d::string_t child_value = (*child)[m_implementation->m_columns.value];

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
	return_if_fail(m_implementation->m_model.get());

	m_implementation->m_list_model->clear();

	const k3d::ienumeration_property::enumeration_values_t values = m_implementation->m_model->enumeration_values();
	for(k3d::ienumeration_property::enumeration_values_t::const_iterator choice = values.begin(); choice != values.end(); ++choice)
	{
		Gtk::TreeRow row = *m_implementation->m_list_model->append();
		row[m_implementation->m_columns.label] = choice->label;
		row[m_implementation->m_columns.value] = choice->value;
		row[m_implementation->m_columns.description] = choice->description;
	}

	set_active(0);
}

void control::on_list_changed()
{
	Gtk::TreeIter active = get_active();
	return_if_fail(active != m_implementation->m_list_model->children().end());

	Gtk::TreeRow row = *active;
	const k3d::string_t value = row[m_implementation->m_columns.value];

	k3d::command_tree().command_signal().emit(*this, k3d::icommand_node::COMMAND_INTERACTIVE, "value", value);

	return_if_fail(m_implementation->m_model.get());

	if(value == m_implementation->m_model->value())
		return;

	if(m_implementation->m_state_recorder)
		m_implementation->m_state_recorder->start_recording(k3d::create_state_change_set(K3D_CHANGE_SET_CONTEXT), K3D_CHANGE_SET_CONTEXT);

	m_implementation->m_model->set_value(value);

	if(m_implementation->m_state_recorder)
	{
		const k3d::string_t change_message = k3d::string_cast(boost::format(_("Change %1% to %2%")) % m_implementation->m_model->label().raw() % value);
		m_implementation->m_state_recorder->commit_change_set(m_implementation->m_state_recorder->stop_recording(K3D_CHANGE_SET_CONTEXT), change_message, K3D_CHANGE_SET_CONTEXT);
	}
}

} // namespace enumeration_chooser

} // namespace libk3dngui

