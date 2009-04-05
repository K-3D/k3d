#ifndef K3DSDK_NGUI_HOTKEY_ENTRY_H
#define K3DSDK_NGUI_HOTKEY_ENTRY_H

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

#include <gtkmm/entry.h>

namespace k3d
{

namespace ngui
{

/////////////////////////////////////////////////////////////////////////////
// hotkey_entry

/// Gtk::Entry derivative that automatically disables accelerators while it has the focus
class hotkey_entry :
	public Gtk::Entry
{
	typedef Gtk::Entry base;

public:
	/// Called when the control gets the keyboard focus
	bool on_focus_in_event(GdkEventFocus* Event);
	/// Called when the control loses the keyboard focus
	bool on_focus_out_event(GdkEventFocus* Event);

private:
	/// Stores the global accel group while the control has the keyboard focus
	Glib::RefPtr<Gtk::AccelGroup> m_disabled_accel_group;
};

} // namespace ngui

} // namespace k3d

#endif // !K3DSDK_NGUI_HOTKEY_ENTRY_H


