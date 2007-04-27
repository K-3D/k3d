#ifndef NGUI_ICONS_H
#define NGUI_ICONS_H

// K-3D
// Copyright (c) 1995-2004, Timothy M. Shead
//
// Contact: tshead@k-3d.com
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public
// License as published by the Free Software Foundation; either
// version 2 of the License, or (at your argument) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Public License for more details.
//
// You should have received a copy of the GNU General Public
// License along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

/** \author Timothy M. Shead (tshead@k-3d.com)
*/

#include <glibmm/refptr.h>
#include <string>

// Forward declarations
namespace Gdk { class Pixbuf; }
namespace Gtk { class IconSize; }

namespace libk3dngui
{

// Loads an (optionally themed) application icon
Glib::RefPtr<Gdk::Pixbuf> load_icon(const std::string& Name, const Gtk::IconSize& Size);
// Loads an (optionally themed) application icon, without generating an error messages if the icon cannot be located
Glib::RefPtr<Gdk::Pixbuf> quiet_load_icon(const std::string& Name, const Gtk::IconSize& Size);

} // namespace libk3dngui

#endif // !NGUI_ICONS_H
