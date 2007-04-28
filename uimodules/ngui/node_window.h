#ifndef NGUI_OBJECT_WINDOW_H
#define NGUI_OBJECT_WINDOW_H

// K-3D
// Copyright (c) 1995-2004, Timothy M. Shead
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

#include <gtkmm/window.h>

// Forward declarations
namespace k3d { class inode; }

namespace libk3dngui
{

///////////////////////////////////////////////////////////////////////////////
// node_window

/// Provides a standard node-level window that is interactive (can participate in tutorials and macros)
class node_window :
        public Gtk::Window,
	public ui_component
{
	typedef Gtk::Window base;

public:
	node_window(k3d::inode& Object, k3d::icommand_node& Parent, const std::string& Name);

	k3d::inode& node();

	bool on_key_press_event(GdkEventKey* event);
	bool on_delete_event(GdkEventAny* event);

	/// Call to determine whether it is safe to close the window
	bool safe_to_close();
	/// Closes the window safely, giving derived classes a chance to cancel
	void safe_close();
	/// Closes the window
	void close();

private:
	/// Override in derived classes to determine whether it is safe to close the window
	virtual bool on_safe_to_close();
	/// Override in derived classes to handle cleanup when the window is closed
	virtual void on_close();
	/// Stores the owning node
	k3d::inode& m_node;
};

} // namespace libk3dngui

#endif // NGUI_OBJECT_WINDOW_H

