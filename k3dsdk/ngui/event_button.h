#ifndef K3DSDK_NGUI_EVENT_BUTTON_H
#define K3DSDK_NGUI_EVENT_BUTTON_H

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

#include <gtkmm/button.h>

namespace libk3dngui
{

/// Defines a push-button that can handle any button event, not just LMB-click
class event_button :
	public Gtk::Button
{
	typedef Gtk::Button base;

public:
	event_button()
	{
	}

private:
	bool on_button_press_event(GdkEventButton* Event)
	{
		base::on_button_press_event(Event);
		return false;
	}

	bool on_button_release_event(GdkEventButton* Event)
	{
		base::on_button_release_event(Event);
		return false;
	}
};

} // namespace libk3dngui

#endif // !K3DSDK_NGUI_EVENT_BUTTON_H
