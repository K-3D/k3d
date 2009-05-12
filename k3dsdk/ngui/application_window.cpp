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

#include "application_state.h"
#include "application_window.h"

#include <k3dsdk/iapplication.h>
#include <k3dsdk/application.h>

#include <gdk/gdkkeysyms.h>

namespace k3d
{

namespace ngui
{

/////////////////////////////////////////////////////////////////////////////
// application_window

application_window::application_window() :
	base(Gtk::WINDOW_TOPLEVEL)
{
	k3d::application().connect_close_signal(sigc::mem_fun(*this, &application_window::close));
}

bool application_window::on_key_press_event(GdkEventKey* event)
{
	if(event->keyval == GDK_Escape)
	{
		close();
		return true;
	}

	return base::on_key_press_event(event);
}

bool application_window::on_delete_event(GdkEventAny* event)
{
	close();
	return true;
}

void application_window::close()
{
	on_close();
	delete this;
}

void application_window::on_close()
{
}

} // namespace ngui

} // namespace k3d

