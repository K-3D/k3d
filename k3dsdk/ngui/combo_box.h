#ifndef K3DSDK_NGUI_COMBO_BOX_H
#define K3DSDK_NGUI_COMBO_BOX_H

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
		\brief Declares k3d::combo_box classes, which provide a model-view-controller UI for text data that includes a finite set of default values
		\author Tim Shead (tshead@k-3d.com)
*/

#include "ui_component.h"

#include <k3dsdk/iproperty.h>
#include <k3dsdk/iwritable_property.h>

#include <gtkmm/comboboxentry.h>
#include <gtkmm/liststore.h>

#include <memory>

// Forward declarations
namespace k3d { class istate_recorder; }

namespace k3d
{

namespace ngui
{

namespace combo_box
{

/////////////////////////////////////////////////////////////////////////////
// idata_proxy

/// Abstract interface for an object that proxies a data source for a combo_box control (i.e. the "model" in model-view-controller)
class idata_proxy
{
public:
	virtual ~idata_proxy() {}

	/// Called to return the underlying data value
	virtual const std::string value() = 0;
	/// Called to set a new data value
	virtual void set_value(const std::string& Value) = 0;
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

/// Provides a UI for manipulating enumerated quantities (i.e. the view and the controller from model-view-controller)
class control :
	public Gtk::ComboBoxEntry,
	public ui_component
{
	typedef Gtk::ComboBoxEntry base;

public:
	control(k3d::icommand_node& Parent, const std::string& Name, std::auto_ptr<idata_proxy> Data);

	const k3d::icommand_node::result execute_command(const std::string& Command, const std::string& Arguments);

	/// Defines a predefined (but not exclusive) list of values that should be available to the user
	typedef std::vector<std::string> values_t;
	/// Specifies a predefined (but not exclusive) list of values that should be available to the user
	void set_values(const values_t& Values);

private:
	/// Called whenever the underlying data changes
	void data_changed(k3d::ihint*);
	/// Called when the user picks an existing value using the dropdown list
	void on_list_changed();
	/// Called when the edit control receives the focus
	bool on_entry_focus_in_event(GdkEventFocus* Event);
	/// Called when the user tabs off the edit control
	bool on_entry_focus_out_event(GdkEventFocus* Event);
	/// Called when the user activates the edit control
	void on_entry_activate();
	/// Called to add new values to the list that aren't already part of the default set
	void add_impromptu_value(const std::string& Value);
	/// Called to update the underlying data when the control is modified by the user
	void set_new_value();

	/// Stores a proxy for the underlying data source
	std::auto_ptr<idata_proxy> m_data;

	/// Defines a data model for the underlying combo box widget
	class columns :
		public Gtk::TreeModelColumnRecord
	{
	public:
		columns()
		{
			add(value);
		}

		Gtk::TreeModelColumn<Glib::ustring> value;
	};

	/// Defines a data model for the underlying combo box widget
	columns m_columns;
	/// Stores data for the underlying combo box widget
	Glib::RefPtr<Gtk::ListStore> m_model;
	/// Stores the global accel group while the control has the keyboard focus
	Glib::RefPtr<Gtk::AccelGroup> m_disabled_accel_group;
};

/// Provides an implementation of k3d::combo_box::idata_proxy that supports any data source that supports the value(), set_value(), and changed_signal() concepts
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

/// Convenience factory function for creating combo_box::idata_proxy objects
template<typename data_t>
std::auto_ptr<idata_proxy> proxy(data_t& Data, k3d::istate_recorder* const StateRecorder = 0, const Glib::ustring& ChangeMessage = Glib::ustring())
{
	return std::auto_ptr<idata_proxy>(new data_proxy<data_t>(Data, StateRecorder, ChangeMessage));
}

/// Convenience factory function for creating combo_box::idata_proxy objects for use with k3d::iproperty
std::auto_ptr<idata_proxy> proxy(k3d::iproperty& Data, k3d::istate_recorder* const StateRecorder = 0, const Glib::ustring& ChangeMessage = Glib::ustring());

} // namespace combo_box

} // namespace ngui

} // namespace k3d

#endif // !K3DSDK_NGUI_COMBO_BOX_H

