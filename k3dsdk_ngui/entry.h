#ifndef NGUI_ENTRY_H
#define NGUI_ENTRY_H

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
		\brief Declares the libk3dngui::entry control, which provides a standard MVC UI for string values
		\author Tim Shead (tshead@k-3d.com)
*/

#include "hotkey_entry.h"
#include "ui_component.h"

#include <k3dsdk/iproperty.h>
#include <k3dsdk/iwritable_property.h>

namespace k3d { class istate_recorder; }

namespace libk3dngui
{

namespace entry
{

/////////////////////////////////////////////////////////////////////////////
// idata_proxy

/// Abstract interface for an object that proxies a data source for an edit control (i.e. the "model" in model-view-controller)
class idata_proxy
{
public:
	virtual ~idata_proxy() {}

	/// Called to return the underlying data value
	virtual const std::string value() = 0;
	/// Called to set a new data value
	virtual void set_value(const std::string& Value) = 0;
	/// Signal emitted if the underlying data changes
	typedef sigc::signal<void, k3d::iunknown*> changed_signal_t;
	/// Signal emitted if the underlying data changes
	virtual changed_signal_t& changed_signal() = 0;

	/// Stores an optional state recorder for recording undo/redo data
	k3d::istate_recorder* const state_recorder;
	/// Stores an optional message for labelling undo/redo state changes
	const Glib::ustring change_message;

protected:
	idata_proxy(k3d::istate_recorder* const StateRecorder, const Glib::ustring& ChangeMessage) :
		state_recorder(StateRecorder),
		change_message(ChangeMessage)
	{
	}

private:
	idata_proxy(const idata_proxy&);
	idata_proxy& operator=(const idata_proxy&);
};

/////////////////////////////////////////////////////////////////////////////
// control

/// Provides a standard UI control for manipulating text fields (i.e. the view and the controller from model-view-controller)
class control :
	public hotkey_entry,
	public ui_component
{
	typedef hotkey_entry base;

public:
	control(k3d::icommand_node& Parent, const std::string& Name, std::auto_ptr<idata_proxy> Data);

	const k3d::icommand_node::result execute_command(const std::string& Command, const std::string& Arguments);

private:
	/// Called when the control loses the keyboard focus
	bool on_focus_out_event(GdkEventFocus* Event);
	/// Called when the control is activated
	void on_activate();
	/// Called when the user finishes editing data
	void set_value();
	/// Called whenever the underlying data changes
	void data_changed(k3d::iunknown*);

	/// Stores a reference to the underlying data object
	std::auto_ptr<idata_proxy> m_data;
};

/// Provides an implementation of k3d::edit_control::idata_proxy that supports any data source that supports the value(), set_value(), and changed_signal() concepts
template<typename data_t>
class data_proxy :
	public idata_proxy
{
public:
	data_proxy(data_t& Data, k3d::istate_recorder* const StateRecorder, const Glib::ustring& ChangeMessage) :
		idata_proxy(StateRecorder, ChangeMessage),
		m_data(Data)
	{
	}

	const std::string value()
	{
		return m_data.value();
	}

	void set_value(const std::string& Value)
	{
		m_data.set_value(Value);
	}

	changed_signal_t& changed_signal()
	{
		return m_data.changed_signal();
	}

private:
	data_proxy(const data_proxy& RHS);
	data_proxy& operator=(const data_proxy& RHS);

	data_t& m_data;
};

/// Convenience factory function for creating idata_proxy objects
template<typename data_t>
std::auto_ptr<idata_proxy> proxy(data_t& Data, k3d::istate_recorder* const StateRecorder = 0, const Glib::ustring& ChangeMessage = Glib::ustring())
{
	return std::auto_ptr<idata_proxy>(new data_proxy<data_t>(Data, StateRecorder, ChangeMessage));
}

/// Convenience factory function for creating idata_proxy objects
std::auto_ptr<idata_proxy> proxy(k3d::iproperty& Data, k3d::istate_recorder* const StateRecorder = 0, const Glib::ustring& ChangeMessage = Glib::ustring());

} // namespace edit_control

} // namespace libk3dngui

#endif // !NGUI_ENTRY_H

