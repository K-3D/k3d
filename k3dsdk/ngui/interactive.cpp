// K-3D
// Copyright (c) 1995-2006, Timothy M. Shead
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
		\brief Implements functions and helpers for simulating user input and driving interactive tutorials
		\author Tim Shead (tshead@k-3d.com)
*/

#include "collapsible_frame.h"
#include "interactive.h"
#include "options.h"
#include "screen_overlay.h"
#include "utility.h"

#include <k3d-platform-config.h>

#include <k3dsdk/basic_math.h>
#include <k3dsdk/bezier.h>
#include <k3dsdk/high_res_timer.h>
#include <k3dsdk/rectangle.h>
#include <k3dsdk/system.h>
#include <k3dsdk/vectors.h>

#include <gtkmm/button.h>
#include <gtkmm/checkbutton.h>
#include <gtkmm/combobox.h>
#include <gtkmm/entry.h>
#include <gtkmm/menu.h>
#include <gtkmm/menubar.h>
#include <gtkmm/menuitem.h>
#include <gtkmm/menushell.h>
#include <gtkmm/notebook.h>
#include <gtkmm/scrolledwindow.h>
#include <gtkmm/treeview.h>
#include <gtkmm/widget.h>
#include <gtkmm/window.h>

#include <gtk/gtk.h>

#if defined K3D_API_WIN32

	//#include <gdkwin32.h>
	#undef min
	#undef max

#else // K3D_API_WIN32

	#include <gdk/gdkx.h>

#endif // !K3D_API_WIN32

namespace k3d
{

namespace ngui
{

namespace interactive
{

namespace detail
{

void warp_pointer(const k3d::point2& Offset)
{
#ifdef K3D_API_WIN32

	// Make that pointer jump!
	SetCursorPos(static_cast<int>(Offset[0]), static_cast<int>(Offset[1]));

#else // K3D_API_WIN32

	// Get the X display ...
	Display* xdisplay = GDK_WINDOW_XDISPLAY(Gdk::Display::get_default()->get_default_screen()->get_root_window()->gobj());
	return_if_fail(xdisplay);

	// Get our X window ...
	Window xwindow = GDK_WINDOW_XWINDOW(Gdk::Display::get_default()->get_default_screen()->get_root_window()->gobj());
	return_if_fail(xwindow);

	// Move that pointer!
	XWarpPointer(xdisplay, None, xwindow, 0, 0, 0, 0, static_cast<int>(Offset[0]), static_cast<int>(Offset[1]));
	XFlush(xdisplay);

#endif // !K3D_API_WIN32
}

void warp_pointer(Glib::RefPtr<Gdk::Window> Window, const k3d::point2& Offset)
{
	return_if_fail(Window);

	int left = 0;
	int top = 0;
	Window->get_origin(left, top);

	warp_pointer(k3d::point2(left, top) + Offset);
}

const k3d::point2 screen_coordinates(Gtk::Widget& Widget)
{
	// Sanity check
	return_val_if_fail(Widget.get_window(), k3d::point2(0, 0));

	int left = 0;
	int top = 0;
	Widget.get_window()->get_origin(left, top);

	if(Widget.has_no_window())
	{
		left += Widget.get_allocation().get_x();
		top += Widget.get_allocation().get_y();
	}

	return k3d::point2(left, top);
}

} // namespace detail

const k3d::point2 get_pointer()
{
	int x = 0;
	int y = 0;
	Gdk::ModifierType modifiers;
	Gdk::Display::get_default()->get_pointer(x, y, modifiers);

	return k3d::point2(x, y);
}

void warp_pointer(const k3d::point2& Coords)
{
	detail::warp_pointer(Coords);
}

void warp_pointer(const k3d::point2& Coords, const double Timestamp, const k3d::timer& Timer)
{
	handle_pending_events();
	k3d::system::sleep(Timestamp - Timer.elapsed());
	detail::warp_pointer(Coords);
}

void warp_pointer(Gtk::Widget& Widget, const k3d::point2& Coords)
{
	detail::warp_pointer(detail::screen_coordinates(Widget) + Coords);
}

void warp_pointer(Gtk::Widget& Widget, const k3d::point2& Coords, const double Timestamp, const k3d::timer& Timer)
{
	warp_pointer(detail::screen_coordinates(Widget) + Coords, Timestamp, Timer);
}

void warp_pointer(Gtk::Widget& Widget)
{
	detail::warp_pointer(detail::screen_coordinates(Widget) + k3d::point2(Widget.get_width() / 2, Widget.get_height() / 2));
}

} // namespace interactive

} // namespace ngui

} // namespace k3d

