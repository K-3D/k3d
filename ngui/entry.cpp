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
		\brief Implements the k3d::edit_control::control class, which provides a UI for string quantities
		\author Tim Shead (tshead@k-3d.com)
*/

#include "entry.h"
#include "interactive.h"

#include <k3dsdk/istate_recorder.h>
#include <k3dsdk/result.h>
#include <k3dsdk/state_change_set.h>
#include <k3dsdk/types_ri.h>

#include <gtkmm/window.h>

namespace libk3dngui
{

namespace entry
{

/// Specialization of k3d::edit_control::data_proxy for use with k3d::iproperty objects
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
		const std::type_info& type = m_readable_data.property_type();

		if(type == typeid(std::string))
		{
			return boost::any_cast<std::string>(m_readable_data.property_value());
		}
		else if(type == typeid(k3d::ri::string))
		{
			return boost::any_cast<k3d::ri::string>(m_readable_data.property_value());
		}
		else
		{
			k3d::log() << error << k3d_file_reference << ": unknown property type: " << type.name() << std::endl;
			return std::string();
		}
	}

	void set_value(const std::string& Value)
	{
		return_if_fail(m_writable_data);

		const std::type_info& type = m_readable_data.property_type();

		if(type == typeid(std::string))
			m_writable_data->property_set_value(Value);
		else if(type == typeid(k3d::ri::string))
			m_writable_data->property_set_value(k3d::ri::string(Value));
		else
			k3d::log() << error << k3d_file_reference << ": unknown property type: " << type.name() << std::endl;
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
	ui_component(Name, &Parent),
	m_data(Data)
{
	set_name("k3d-entry");

	data_changed(0);

	if(m_data.get())
		m_data->changed_signal().connect(sigc::mem_fun(*this, &control::data_changed));
}

const k3d::icommand_node::result control::execute_command(const std::string& Command, const std::string& Arguments)
{
	if(Command == "set_value")
	{
		interactive::set_text(*this, Arguments);
		select_region(0, 0);
		set_value();
		return RESULT_CONTINUE;
	}

	return ui_component::execute_command(Command, Arguments);
}

bool control::on_focus_out_event(GdkEventFocus* Event)
{
	set_value();
	return base::on_focus_out_event(Event);
}

void control::on_activate()
{
	set_value();
	base::on_activate();
}

void control::set_value()
{
	if(!m_data.get())
		return;

	// If the value didn't change, we're done ...
	const std::string new_value = get_text();
	if(new_value == m_data->value())
		return;

	// Record the command for posterity (tutorials) ...
	record_command("set_value", new_value);

	// Turn this into an undo/redo -able event ...
	if(m_data->state_recorder)
		m_data->state_recorder->start_recording(k3d::create_state_change_set(__PRETTY_FUNCTION__), __PRETTY_FUNCTION__);

	// Update everything with the new value ...
	m_data->set_value(new_value);

	// Turn this into an undo/redo -able event ...
	if(m_data->state_recorder)
		m_data->state_recorder->commit_change_set(m_data->state_recorder->stop_recording(__PRETTY_FUNCTION__), m_data->change_message + " " + new_value, __PRETTY_FUNCTION__);
}

void control::data_changed(k3d::iunknown*)
{
	return_if_fail(m_data.get());
	set_text(m_data->value());
}

} // namespace edit_control

} // namespace libk3dngui


