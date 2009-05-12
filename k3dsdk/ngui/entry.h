#ifndef K3DSDK_NGUI_ENTRY_H
#define K3DSDK_NGUI_ENTRY_H

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

#include "hotkey_entry.h"
#include "ui_component.h"

#include <k3dsdk/types.h>

namespace k3d { class iproperty; }
namespace k3d { class istate_recorder; }

namespace k3d
{

namespace ngui
{

namespace entry
{

/////////////////////////////////////////////////////////////////////////////
// imodel

/// Abstract data model for an entry::control
class imodel
{
public:
	virtual ~imodel() {}

	/// Returns a human-readable label for the underlying data
	virtual const Glib::ustring label() = 0;
	/// Called to return the underlying data value
	virtual const k3d::string_t value() = 0;
	/// Called to set a new data value
	virtual void set_value(const k3d::string_t& Value) = 0;
	/// Connects a slot to a signal that will be emitted if the underlying data changes
	virtual sigc::connection connect_changed_signal(const sigc::slot<void>& Slot) = 0;

protected:
	imodel() {}

private:
	imodel(const imodel&);
	imodel& operator=(const imodel&);
};

/// Factory method for creating an imodel object given a suitably-typed property
imodel* const model(k3d::iproperty& Property);

/// Provides an implementation of entry::imodel that can "wrap" any data source that supports the internal_value(), set_value(), and changed_signal() concepts
template<typename data_t>
class generic_model_t :
	public imodel
{
public:
	generic_model_t(data_t& Data, const Glib::ustring& Label) :
		m_data(Data),
		m_label(Label)
	{
	}

	const Glib::ustring label()
	{
		return m_label;
	}

	const k3d::string_t value()
	{
		return m_data.internal_value();
	}

	void set_value(const k3d::string_t& Value)
	{
		m_data.set_value(Value);
	}

	sigc::connection connect_changed_signal(const sigc::slot<void>& Slot)
	{
		return m_data.changed_signal().connect(sigc::hide(Slot));
	}

private:
	data_t& m_data;
	const Glib::ustring m_label;
};

/// Convenience factory function for creating generic_model_t objects
template<typename data_t>
imodel* model(data_t& Data, const Glib::ustring& Label = "")
{
	return new generic_model_t<data_t>(Data, Label);
}

/////////////////////////////////////////////////////////////////////////////
// control

/// Provides a standard UI control for manipulating text fields (i.e. the view and the controller from model-view-controller)
class control :
	public hotkey_entry,
	public ui_component
{
	typedef hotkey_entry base;

public:
	control(k3d::icommand_node& Parent, const k3d::string_t& Name, imodel* const Model, k3d::istate_recorder* const StateRecorder);
	~control();

private:
	/// Called when the control loses the keyboard focus
	bool on_focus_out_event(GdkEventFocus* Event);
	/// Called when the control is activated
	void on_activate();
	/// Called when the user finishes editing data
	void on_set_value();
	/// Called whenever the underlying data changes
	void on_data_changed();

	const k3d::string_t change_message(const k3d::string_t& Value);

	class implementation;
	implementation* const m_implementation;
};

} // namespace entry

} // namespace ngui

} // namespace k3d

#endif // !K3DSDK_NGUI_ENTRY_H

