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

#include "hotkey_entry.h"
#include <gtkmm/window.h>

namespace libk3dngui
{

/////////////////////////////////////////////////////////////////////////////
// hotkey_entry

bool hotkey_entry::on_focus_in_event(GdkEventFocus* Event)
{
	// Disable accelerators for this window
	Gtk::Window* const window = dynamic_cast<Gtk::Window*>(get_toplevel());
	if(window)
		{
			m_disabled_accel_group = window->get_accel_group();
			window->remove_accel_group(window->get_accel_group());
		}

	return base::on_focus_in_event(Event);
}

bool hotkey_entry::on_focus_out_event(GdkEventFocus* Event)
{
	// Enable accelerators for this window
	Gtk::Window* const window = dynamic_cast<Gtk::Window*>(get_toplevel());
	if(window)
		{
			window->add_accel_group(m_disabled_accel_group);
			m_disabled_accel_group.clear();
		}

	return base::on_focus_out_event(Event);
}

} // namespace libk3dngui


