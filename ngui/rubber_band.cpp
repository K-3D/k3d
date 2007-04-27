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

#include "rubber_band.h"
#include "utility.h"

#include <k3dsdk/color.h>

#include <gdkmm/gc.h>
#include <gtkmm/widget.h>

namespace libk3dngui
{

/////////////////////////////////////////////////////////////////////////////
// rubber_band

rubber_band::rubber_band(const k3d::color& Color) :
	box(0, 0, 0, 0),
	color(Color)
{
}

void rubber_band::draw(Gtk::Widget& Widget)
{
	if(!gc)
	{
		Gdk::Color color = convert(k3d::color(1.0 - this->color.red, 1.0 - this->color.green, 1.0 - this->color.blue));
		Widget.get_default_colormap()->alloc_color(color);
		gc = Gdk::GC::create(Widget.get_window());
		gc->set_foreground(color);
		gc->set_function(Gdk::XOR);
		gc->set_line_attributes(1, Gdk::LINE_ON_OFF_DASH, Gdk::CAP_BUTT, Gdk::JOIN_MITER);
	}
	
	const k3d::rectangle b = k3d::rectangle(
		std::min(box.left, box.right),
		std::max(box.left, box.right),
		std::min(box.top, box.bottom),
		std::max(box.top, box.bottom));
	
	Widget.get_window()->draw_rectangle(
		gc,
		false,
		static_cast<int>(b.left),
		static_cast<int>(b.top),
		static_cast<int>(b.width()),
		static_cast<int>(b.height()));
}

} // namespace libk3dngui

