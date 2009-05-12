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

#include "entry.h"

#include <k3d-i18n-config.h>
#include <k3dsdk/command_tree.h>
#include <k3dsdk/inode.h>
#include <k3dsdk/iproperty.h>
#include <k3dsdk/istate_recorder.h>
#include <k3dsdk/iwritable_property.h>
#include <k3dsdk/result.h>
#include <k3dsdk/state_change_set.h>
#include <k3dsdk/string_cast.h>

#include <gtkmm/window.h>

#include <boost/format.hpp>
#include <boost/scoped_ptr.hpp>

namespace k3d
{

namespace ngui
{

namespace entry
{

/////////////////////////////////////////////////////////////////////////////
// property_model

/// Implementation of entry::imodel for use with k3d::iproperty objects
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
};

/////////////////////////////////////////////////////////////////////////////
// control

control::control(k3d::icommand_node& Parent, const k3d::string_t& Name, imodel* const Model, k3d::istate_recorder* const StateRecorder) :
	m_implementation(new implementation(Model, StateRecorder))
{
	k3d::command_tree().add(*this, Name, &Parent);

	set_name("k3d-entry");

	// Synchronize the view with the data source ...
	on_data_changed();

	// We want to be notified if the data source changes ...
	m_implementation->m_model->connect_changed_signal(sigc::mem_fun(*this, &control::on_data_changed));
}

control::~control()
{
	delete m_implementation;
}

bool control::on_focus_out_event(GdkEventFocus* Event)
{
	on_set_value();
	return base::on_focus_out_event(Event);
}

void control::on_activate()
{
	on_set_value();
	base::on_activate();
}

void control::on_set_value()
{
	// If the value didn't change, we're done ...
	const k3d::string_t new_value = get_text();
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

void control::on_data_changed()
{
	set_text(m_implementation->m_model->value());
}

const k3d::string_t control::change_message(const k3d::string_t& Value)
{
	return k3d::string_cast(boost::format(_("Change %1% to %2%")) % m_implementation->m_model->label().raw() % Value);
}

} // namespace entry

} // namespace ngui

} // namespace k3d


