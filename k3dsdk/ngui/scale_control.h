#ifndef NGUI_SCALE_H
#define NGUI_SCALE_H

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

#include <k3dsdk/signal_system.h>
#include <k3dsdk/vectors.h>

#include <gtkmm/table.h>

#include <memory>

// Forward declarations
namespace Gtk { class Button; }
namespace k3d { class iproperty; }
namespace k3d { class istate_recorder; }

namespace libk3dngui
{

namespace scale
{

/////////////////////////////////////////////////////////////////////////////
// idata_proxy

/// Abstract interface for an object that proxies a data source for a position control (i.e. the "model" in model-view-controller)
class idata_proxy
{
public:
	virtual ~idata_proxy() {}

	/// Called to return the underlying data value
	virtual const k3d::point3 value() = 0;
	/// Called to set a new data value
	virtual void set_value(const k3d::point3& Value) = 0;
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

/// Provides a UI for modifying the position of an object
class control :
	public Gtk::Table,
	public ui_component
{
	typedef Gtk::Table base;

public:
	control(k3d::icommand_node& Parent, const std::string& Name, std::auto_ptr<idata_proxy> Data);

	const k3d::icommand_node::result execute_command(const std::string& Command, const std::string& Arguments);

private:
	/// Called to reset the object position to the origin
	void on_reset();

	/// Stores a reference to the underlying data object
	std::auto_ptr<idata_proxy> m_data;
	/// Stores the reset button
	Gtk::Button* const m_reset_button;
};

/////////////////////////////////////////////////////////////////////////////
// proxy

/// Convenience factory function for creating k3d::spin_button::idata_proxy objects, specialized for k3d::iproperty
std::auto_ptr<idata_proxy> proxy(k3d::iproperty& Data, k3d::istate_recorder* const StateRecorder = 0, const Glib::ustring& ChangeMessage = Glib::ustring());

} // namespace scale

} // namespace libk3dngui

#endif // NGUI_SCALE_H


