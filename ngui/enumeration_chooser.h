#ifndef NGUI_ENUMERATION_CHOOSER_H
#define NGUI_ENUMERATION_CHOOSER_H

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
		\brief Declares k3d::enumeration_chooser classes, which provide a model-view-controller UI for text data that includes a finite set of default values
		\author Tim Shead (tshead@k-3d.com)
*/

#include "ui_component.h"

#include <k3dsdk/ienumeration_property.h>
#include <k3dsdk/iproperty.h>
#include <k3dsdk/iwritable_property.h>
#include <k3dsdk/string_cast.h>

#include <gtkmm/combobox.h>
#include <gtkmm/liststore.h>

// Forward declarations
namespace k3d { class istate_recorder; }

namespace libk3dngui
{

namespace enumeration_chooser
{

/////////////////////////////////////////////////////////////////////////////
// idata_proxy

/// Abstract interface for an object that proxies a data source for a enumeration_chooser control (i.e. the "model" in model-view-controller)
class idata_proxy
{
public:
	virtual ~idata_proxy() {}

	/// Returns the set of allowed values
	virtual const k3d::ienumeration_property::enumeration_values_t enumeration_values() = 0;
	/// Called to return the underlying data value
	virtual const std::string value() = 0;
	/// Called to set a new data value
	virtual void set_value(const std::string& Value) = 0;
	/// Makes a connection to a signal that will be emitted if the underlying data changes
	virtual sigc::connection connect_changed(const sigc::slot<void, k3d::iunknown*>& Slot) = 0;
	/// Makes a connection to a signal that will be emitted if the set of allowed values changes
	virtual sigc::connection connect_enumeration_values_changed(const sigc::slot<void>& Slot) = 0;

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
	public Gtk::ComboBox,
	public ui_component
{
	typedef Gtk::ComboBox base;

public:
	control(k3d::icommand_node& Parent, const std::string& Name, std::auto_ptr<idata_proxy> Data);

	const k3d::icommand_node::result execute_command(const std::string& Command, const std::string& Arguments);

private:
	/// Called whenever the underlying data changes
	void on_data_changed(k3d::iunknown*);
	/// Called whenever the set of allowable values changes
	void on_enumeration_values_changed();
	/// Called when the user picks an existing value using the dropdown list
	void on_list_changed();

	/// Stores a proxy for the underlying data source
	std::auto_ptr<idata_proxy> m_data;

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
		Gtk::TreeModelColumn<std::string> value;
		Gtk::TreeModelColumn<Glib::ustring> description;
	};

	/// Defines a data model for the underlying combo box widget
	columns m_columns;
	/// Stores data for the underlying combo box widget
	Glib::RefPtr<Gtk::ListStore> m_model;
};

/// Provides an implementation of k3d::enumeration_chooser::idata_proxy that supports any data source that supports the value(), set_value(), and changed_signal() concepts
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

	const k3d::ienumeration_property::enumeration_values_t enumeration_values()
	{
		return m_data.enumeration_values();
	}

	const std::string value()
	{
		return k3d::string_cast(m_data.value());
	}

	void set_value(const std::string& Value)
	{
		m_data.set_value(k3d::from_string(Value, static_cast<typename data_t::value_t>(0)));
	}

	sigc::connection connect_changed(const sigc::slot<void>& Slot)
	{
		return m_data.changed_signal().connect(Slot);
	}

	sigc::connection connect_enumeration_values_changed(const sigc::slot<void>& Slot)
	{
		return m_data.connect_enumeration_values_changed_signal(Slot);
	}

private:
	data_proxy(const data_proxy& RHS);
	data_proxy& operator=(const data_proxy& RHS);

	data_t& m_data;
};

/// Convenience factory function for creating enumeration_chooser::idata_proxy objects
template<typename data_t>
std::auto_ptr<idata_proxy> proxy(data_t& Data, k3d::istate_recorder* const StateRecorder = 0, const Glib::ustring& ChangeMessage = Glib::ustring())
{
	return std::auto_ptr<idata_proxy>(new data_proxy<data_t>(Data, StateRecorder, ChangeMessage));
}

/// Convenience factory function for creating enumeration_chooser::idata_proxy objects for use with k3d::iproperty
std::auto_ptr<idata_proxy> proxy(k3d::iproperty& Data, k3d::istate_recorder* const StateRecorder = 0, const Glib::ustring& ChangeMessage = Glib::ustring());

} // namespace enumeration_chooser

} // namespace libk3dngui

#endif // !NGUI_ENUMERATION_CHOOSER_H

