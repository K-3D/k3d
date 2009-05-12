#ifndef K3DSDK_NGUI_CHECK_MENU_ITEM_H
#define K3DSDK_NGUI_CHECK_MENU_ITEM_H

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
		\brief Declares k3d::check_menu_item::control, which provides a standard MVC UI for boolean data that supports tutorials, undo/redo, etc.
		\author Tim Shead (tshead@k-3d.com)
*/

#include "ui_component.h"

#include <k3dsdk/iproperty.h>
#include <k3dsdk/iwritable_property.h>

#include <gtkmm/checkmenuitem.h>

#include <memory>

namespace k3d { class ihint; }
namespace k3d { class istate_recorder; }

namespace k3d
{

namespace ngui
{

namespace check_menu_item
{

/////////////////////////////////////////////////////////////////////////////
// idata_proxy

/// Abstract interface for an object that proxies a data source for a toggle button control (i.e. the "model" in model-view-controller)
class idata_proxy
{
public:
	virtual ~idata_proxy() {}

	/// Called to return the underlying data value
	virtual bool value() = 0;
	/// Called to set a new data value
	virtual void set_value(const bool Value) = 0;
	/// Signal emitted if the underlying data changes
	typedef sigc::signal<void, k3d::ihint*> changed_signal_t;
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
	public Gtk::CheckMenuItem,
	public ui_component
{
	typedef Gtk::CheckMenuItem base;

public:
	control(std::auto_ptr<idata_proxy> Data);
	control(std::auto_ptr<idata_proxy> Data, const Glib::ustring& label, bool mnemonic = false);

	void on_toggled();

private:
	/// Common construction code
	void attach();
	/// Called to update the widget state whenever the underlying data changes
	void update(k3d::ihint*);

	/// Stores a reference to the underlying data object
	const std::auto_ptr<idata_proxy> m_data;
};

/// Provides an implementation of k3d::check_menu_item::idata_proxy that supports any data source that supports the value(), set_value(), and changed_signal() concepts
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

/// Convenience factory function for creating check_menu_item::idata_proxy objects
template<typename data_t>
std::auto_ptr<idata_proxy> proxy(data_t& Data, k3d::istate_recorder* const StateRecorder = 0, const Glib::ustring& ChangeMessage = Glib::ustring())
{
	return std::auto_ptr<idata_proxy>(new data_proxy<data_t>(Data, StateRecorder, ChangeMessage));
}

/// Convenience factory function for creating check_menu_item::idata_proxy objects specialized for use with k3d::iproperty objects
std::auto_ptr<idata_proxy> proxy(k3d::iproperty& Data, k3d::istate_recorder* const StateRecorder = 0, const Glib::ustring& ChangeMessage = Glib::ustring());

/// Convenience factory function for creating empty check_menu_item::idata_proxy objects
inline std::auto_ptr<idata_proxy> proxy()
{
	return std::auto_ptr<idata_proxy>();
}

} // namespace check_menu_item

} // namespace ngui

} // namespace k3d

#endif // !K3DSDK_NGUI_CHECK_MENU_ITEM_H


