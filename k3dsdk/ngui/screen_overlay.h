#ifndef NGUI_SCREEN_OVERLAY_H
#define NGUI_SCREEN_OVERLAY_H

// SDPGTK
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
		\brief Provides an interface for drawing transparent displays over the top of the user's screen
		\author Tim Shead (tshead@k-3d.com)
*/

#include <k3dsdk/color.h>
#include <gtkmm/window.h>

namespace libk3dngui
{

//////////////////////////////////////////////////////////////////////////
// screen_overlay

/// Provides an interface for drawing transparent displays over the top of the user's screen
class screen_overlay :
	public Gtk::Window
{
	typedef Gtk::Window base;

public:
	screen_overlay(const Gdk::Rectangle& Geometry, const k3d::color& Color);
	~screen_overlay();

	/// Updates the overlay color
	void set_color(const k3d::color& Color);
	/// Returns a mask that you can draw on to determine the shape of the overlay
	const Glib::RefPtr<Gdk::Bitmap>& mask() const;
	/// Returns a gc that you can use to draw on the overlay mask
	const Glib::RefPtr<Gdk::GC>& mask_gc() const;
	/// Called to update the overlay after changes have been using mask() and mask_gc()
	void update();

private:
	/// Stores the mask that controls the shape of the overlay
	Glib::RefPtr<Gdk::Bitmap> m_mask;
	/// Stores the gc for drawing on the overlay mask
	Glib::RefPtr<Gdk::GC> m_mask_gc;
};

} // namespace libk3dngui

#endif // !NGUI_SCREEN_OVERLAY_H

