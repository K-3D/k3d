#ifndef K3DSDK_NGUI_PATH_CHOOSER_H
#define K3DSDK_NGUI_PATH_CHOOSER_H

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
		\author Tim Shead (tshead@k-3d.com)
*/

#include "ui_component.h"

#include <k3dsdk/ipath_property.h>
#include <k3dsdk/iproperty.h>
#include <k3dsdk/iwritable_property.h>

#include <gtkmm/box.h>

#include <memory>

namespace Gtk { class Button; }
namespace Gtk { class ComboBox; }
namespace k3d { class istate_recorder; }

namespace libk3dngui
{

class hotkey_entry;

namespace path_chooser
{

/////////////////////////////////////////////////////////////////////////////
// idata_proxy

/// Abstract interface for an object that proxies a data source for an path control (i.e. the "model" in model-view-controller)
class idata_proxy
{
public:
	virtual ~idata_proxy() {}

	/// Returns the path "mode" (input or output)
	virtual const k3d::ipath_property::mode_t mode() = 0;
	/// Returns the path "type" (category)
	virtual const std::string type() = 0;
	virtual const k3d::ipath_property::reference_t reference() = 0;
	virtual void set_reference(const k3d::ipath_property::reference_t Reference) = 0;
	virtual const k3d::ipath_property::pattern_filters_t pattern_filters() = 0;
	/// Called to return the underlying data value
	virtual const k3d::filesystem::path value() = 0;
	/// Called to set a new data value
	virtual void set_value(const k3d::filesystem::path& Value) = 0;
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

/// Provides a standard UI control for manipulating filepath data (i.e. the view and the controller from model-view-controller)
class control :
	public Gtk::HBox,
	public ui_component
{
	typedef Gtk::HBox base;

public:
	control(k3d::icommand_node& Parent, const std::string& Name, std::auto_ptr<idata_proxy> Data);

	const k3d::icommand_node::result execute_command(const std::string& Command, const std::string& Arguments);

private:
	/// Called when the control loses the keyboard focus
	bool on_focus_out_event(GdkEventFocus* Event);
	/// Called when the control is activated
	void on_activate();
	/// Called when the user clicks the "browse" button
	void on_browse();
	/// Called when the user changes the reference type
	void on_pick_reference_type();
	/// Called when the user finishes editing data
	void set_value();
	/// Called whenever the underlying data changes
	void data_changed(k3d::ihint*);
	/// Called whenever the underlying data reference changes
	void on_reference_type_changed();

	/// Stores an entry widget for displaying the current value
	hotkey_entry* const m_entry;
	/// Stores a button widget for browsing the filesystem
	Gtk::Button* const m_button;
	/// Stores a combo-box for picking absolute, relative, or inline path behavior
	Gtk::ComboBox* const m_combo;
	/// Storeas a reference to the underlying data object
	std::auto_ptr<idata_proxy> m_data;
	/// Prevent set_value() from being called recursively (a hack)
	bool m_disable_set_value;

	class reference_columns :
		public Gtk::TreeModelColumnRecord
	{
	public:
		reference_columns()
		{
			add(reference);
			add(label);
		}

		Gtk::TreeModelColumn<k3d::ipath_property::reference_t> reference;
		Gtk::TreeModelColumn<Glib::ustring> label;
	};

	reference_columns m_columns;
};

/// Provides an implementation of k3d::path_chooser::idata_proxy that supports any data source that supports the value(), set_value(), and changed_signal() concepts
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

	const k3d::filesystem::path value()
	{
		return m_data.value();
	}

	void set_value(const k3d::filesystem::path& Value)
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

} // namespace path_chooser

} // namespace libk3dngui

#endif // !K3DSDK_NGUI_PATH_CHOOSER_H

