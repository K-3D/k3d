#ifndef K3DSDK_NGUI_ENUMERATION_CHOOSER_H
#define K3DSDK_NGUI_ENUMERATION_CHOOSER_H

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

#include "ui_component.h"

#include <k3dsdk/ienumeration_property.h>
#include <k3dsdk/iproperty.h>
#include <k3dsdk/iwritable_property.h>
#include <k3dsdk/string_cast.h>

#include <gtkmm/combobox.h>
#include <gtkmm/liststore.h>

namespace k3d { class iproperty; }
namespace k3d { class istate_recorder; }

namespace k3d
{

namespace ngui
{

namespace enumeration_chooser
{

/////////////////////////////////////////////////////////////////////////////
// imodel

/// Abstract data model for an enumeration_chooser::control
class imodel
{
public:
	virtual ~imodel() {}

	/// Returns a human-readible label for the underlying data
	virtual const Glib::ustring label() = 0;
	/// Returns the set of allowed values
	virtual const k3d::ienumeration_property::enumeration_values_t enumeration_values() = 0;
	/// Called to return the underlying data value
	virtual const k3d::string_t value() = 0;
	/// Called to set a new data value
	virtual void set_value(const k3d::string_t& Value) = 0;
	/// Makes a connection to a signal that will be emitted if the underlying data changes
	virtual sigc::connection connect_changed(const sigc::slot<void, k3d::iunknown*>& Slot) = 0;
	/// Makes a connection to a signal that will be emitted if the set of allowed values changes
	virtual sigc::connection connect_enumeration_values_changed(const sigc::slot<void>& Slot) = 0;

protected:
	imodel() {}

private:
	imodel(const imodel&);
	imodel& operator=(const imodel&);
};

/// Factory method for creating an imodel instance given a suitably-typed property (could return NULL)
imodel* const model(k3d::iproperty& Property);

/////////////////////////////////////////////////////////////////////////////
// control

/// Provides a UI for manipulating enumerated quantities (i.e. the view and the controller from model-view-controller)
class control :
	public Gtk::ComboBox,
	public ui_component
{
	typedef Gtk::ComboBox base;

public:
	control(k3d::icommand_node& Parent, const k3d::string_t& Name, imodel* const Model, k3d::istate_recorder* const StateRecorder = 0);
	~control();

private:
	/// Called whenever the underlying data changes
	void on_data_changed(k3d::iunknown*);
	/// Called whenever the set of allowable values changes
	void on_enumeration_values_changed();
	/// Called when the user picks an existing value using the dropdown list
	void on_list_changed();

	class implementation;
	implementation* const m_implementation;
};

} // namespace enumeration_chooser

} // namespace ngui

} // namespace k3d

#endif // !K3DSDK_NGUI_ENUMERATION_CHOOSER_H

