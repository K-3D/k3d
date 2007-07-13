#ifndef NGUI_CHECK_BUTTON_H
#define NGUI_CHECK_BUTTON_H

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
		\brief Declares k3d::check_button::control, which provides a standard MVC UI for boolean data that supports tutorials, undo/redo, etc.
		\author Tim Shead (tshead@k-3d.com)
*/

#include "ui_component.h"

#include <k3dsdk/iproperty.h>
#include <k3dsdk/iwritable_property.h>

#include <gtkmm/checkbutton.h>

// Forward declarations
namespace k3d { class istate_recorder; }

namespace libk3dngui
{

namespace check_button
{

/////////////////////////////////////////////////////////////////////////////
// idata_proxy

/// Abstract interface for an object that proxies a data source for a check button control (i.e. the "model" in model-view-controller)
class idata_proxy
{
public:
	virtual ~idata_proxy() {}

	/// Returns true iff the underlying data is writable
	virtual const bool writable() = 0;
	/// Called to return the underlying data value
	virtual bool value() = 0;
	/// Called to set a new data value
	virtual void set_value(const bool Value) = 0;
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
	idata_proxy(const idata_proxy& RHS);
	idata_proxy& operator=(const idata_proxy& RHS);
};

/////////////////////////////////////////////////////////////////////////////
// control

/// Provides a UI for manipulating boolean quantities (i.e. the view and the controller from model-view-controller)
class control :
	public Gtk::CheckButton,
	public ui_component
{
	typedef Gtk::CheckButton base;

public:
	control(k3d::icommand_node& Parent, const std::string& Name, std::auto_ptr<idata_proxy> Data);
	control(k3d::icommand_node& Parent, const std::string& Name, std::auto_ptr<idata_proxy> Data, const Glib::ustring& label, bool mnemonic = false);

	const k3d::icommand_node::result execute_command(const std::string& Command, const std::string& Arguments);
	void on_toggled();

private:
	/// Common construction code
	void attach();
	/// Called to update the state of the widget when the underlying data source changes
	void update(k3d::iunknown*);
	/// Storeas a reference to the underlying data object
	const std::auto_ptr<idata_proxy> m_data;
};

/// Provides an implementation of k3d::check_button::idata_proxy that supports any data source that supports the value(), set_value(), and changed_signal() concepts
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

	const bool writable()
	{
		return true;
	}

	bool value()
	{
		return m_data.internal_value();
	}

	void set_value(const bool Value)
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

/// Convenience factory function for creating check_button::idata_proxy objects
template<typename data_t>
std::auto_ptr<idata_proxy> proxy(data_t& Data, k3d::istate_recorder* const StateRecorder = 0, const Glib::ustring& ChangeMessage = Glib::ustring())
{
	return std::auto_ptr<idata_proxy>(new data_proxy<data_t>(Data, StateRecorder, ChangeMessage));
}

/// Convenience factory function for creating check_button::idata_proxy objects specialized for use with k3d::iproperty
std::auto_ptr<idata_proxy> proxy(k3d::iproperty& Data, k3d::istate_recorder* const StateRecorder = 0, const Glib::ustring& ChangeMessage = Glib::ustring());

/// Convenience factory function for creating empty check_button::idata_proxy objects
inline std::auto_ptr<idata_proxy> proxy()
{
	return std::auto_ptr<idata_proxy>(0);
}

} // namespace check_button

} // namespace libk3dngui

#endif // !NGUI_CHECK_BUTTON_H

