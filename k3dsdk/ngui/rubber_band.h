#ifndef RUBBER_BAND_H
#define RUBBER_BAND_H

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

#include <glibmm/refptr.h>

#include <k3dsdk/color.h>
#include <k3dsdk/rectangle.h>

namespace Gdk { class GC; }
namespace Gtk { class Widget; }

namespace libk3dngui
{

/////////////////////////////////////////////////////////////////////////////
// rubber_band

class rubber_band
{
public:
	rubber_band(const k3d::color& Color);

	/// Draws the rubber band by xor-ing it into the given widget
	void draw(Gtk::Widget& Widget);

	/// Stores the rubber band dimensions
	k3d::rectangle box;

private:
	const k3d::color color;
	/// Stores a GC for drawing the rubber band
	Glib::RefPtr<Gdk::GC> gc;
};

} // namespace libk3dngui

#endif // !RUBBER_BAND_H
