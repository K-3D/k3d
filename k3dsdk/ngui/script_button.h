#ifndef K3DSDK_NGUI_SCRIPT_BUTTON_H
#define K3DSDK_NGUI_SCRIPT_BUTTON_H

// K-3D
// Copyright (c) 1995-2009, Timothy M. Shead
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

#include <k3dsdk/iproperty.h>
#include <k3dsdk/iwritable_property.h>

#include <gtkmm/box.h>
#include <gtkmm/button.h>

#include <memory>

namespace k3d
{

class ihint;
class iproperty;
class istate_recorder;

namespace ngui
{

namespace script_button
{

/////////////////////////////////////////////////////////////////////////////
// imodel

/// Abstract interface for an object that proxies a data source for a script button control (i.e. the "model" in model-view-controller)
class imodel
{
public:
	virtual ~imodel() {}

	/// Called to return the underlying data property (if any, could return NULL)
	virtual iproperty* property() = 0;
	/// Called to return the underlying data value
	virtual const string_t value() = 0;
	/// Called to set a new data value
	virtual void set_value(const string_t& Value) = 0;
	/// Signal emitted if the underlying data changes
	typedef sigc::signal<void, ihint*> changed_signal_t;
	/// Signal emitted if the underlying data changes
	virtual changed_signal_t& changed_signal() = 0;

protected:
	imodel() {}

private:
	imodel(const imodel&);
	imodel& operator=(const imodel&);
};

/// Factory method for creating an imodel object given a suitably-typed property
imodel* const model(iproperty& Property);

/////////////////////////////////////////////////////////////////////////////
// control

/// Provides a UI for manipulating script data (i.e. the view and the controller from model-view-controller)
class control :
	public Gtk::HBox,
	public ui_component
{
	typedef Gtk::HBox base;

public:
	control(icommand_node& Parent, const string_t& Name, imodel* const Model, istate_recorder* const StateRecorder, const Glib::ustring& ChangeMessage);
	~control();

private:
	Gtk::Button m_load;
	Gtk::Button m_save;
	Gtk::Button m_edit;

	/// Called to update the state of the widget when the underlying data source changes
	void update(ihint*);
	/// Called to replace the current data with a script from disk
	void on_load();
	/// Called to store the current data to disk
	void on_save();
	/// Called to edit the current data
	void on_edit();

	/// Storeas a reference to the underlying data object
	imodel* const m_model;
	/// Stores an optional state recorder for recording undo/redo data
	istate_recorder* const m_state_recorder;
	/// Stores an optional message for labelling undo/redo state changes
	const Glib::ustring m_change_message;
};

} // namespace script_button

} // namespace ngui

} // namespace k3d

#endif // !K3DSDK_NGUI_SCRIPT_BUTTON_H

