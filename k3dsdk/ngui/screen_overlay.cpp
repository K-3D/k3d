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
		\brief Implements the screen_overlay class
		\author Tim Shead (tshead@k-3d.com)
*/

#include "screen_overlay.h"
#include "utility.h"

#include <gdkmm/bitmap.h>
#include <gtkmm/window.h>

namespace k3d
{

namespace ngui
{

/////////////////////////////////////////////////////////////////////////////////////////////
// screen_overlay

screen_overlay::screen_overlay(const Gdk::Rectangle& Geometry, const k3d::color& Color) :
	base(Gtk::WINDOW_POPUP)
{
//	const unsigned long width = get_screen()->get_width();
//	const unsigned long height = get_screen()->get_height();
//	resize(width, height);

	move(Geometry.get_x(), Geometry.get_y());
	resize(Geometry.get_width(), Geometry.get_height());

	set_color(Color);
	
	Gdk::Color white;
	white.set_rgb(0xffff, 0xffff, 0xffff);

	Gdk::Color black;
	black.set_rgb(0x0000, 0x0000, 0x0000);

	realize();

	std::vector<char> buffer(Geometry.get_width() * Geometry.get_height(), 0x00);
	m_mask = Gdk::Bitmap::create(&buffer[0], Geometry.get_width(), Geometry.get_height());
	
	m_mask_gc = Gdk::GC::create(m_mask);
	m_mask_gc->set_function(Gdk::SET);
	m_mask_gc->set_foreground(white);
	m_mask_gc->set_background(black);
	
//m_mask->draw_rectangle(m_mask_gc, true, 100, 100, 100, 100);

	shape_combine_mask(m_mask, 0, 0);
}

screen_overlay::~screen_overlay()
{
	hide();
}

void screen_overlay::set_color(const k3d::color& Color)
{

	Gdk::Color color;
	color.set_rgb_p(Color.red, Color.green, Color.blue);

	modify_bg(Gtk::STATE_NORMAL, color);
	modify_bg(Gtk::STATE_ACTIVE, color);
	modify_bg(Gtk::STATE_PRELIGHT, color);
	modify_bg(Gtk::STATE_SELECTED, color);
	modify_bg(Gtk::STATE_INSENSITIVE, color);
	
	queue_draw();
	handle_pending_events();
}

const Glib::RefPtr<Gdk::Bitmap>& screen_overlay::mask() const
{
	return m_mask;
}

const Glib::RefPtr<Gdk::GC>& screen_overlay::mask_gc() const
{
	return m_mask_gc;
}

void screen_overlay::update()
{
	get_window()->shape_combine_mask(m_mask, 0, 0);
	queue_draw();
	handle_pending_events();
}

} // namespace ngui

} // namespace k3d

