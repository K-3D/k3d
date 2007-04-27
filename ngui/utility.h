#ifndef NGUI_UTILITY_H
#define NGUI_UTILITY_H

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

#include <k3dsdk/color.h>
#include <glibmm/refptr.h>
#include <gdkmm/color.h>

namespace Gdk { class Pixbuf; }
namespace k3d { namespace filesystem { class path; } }

namespace libk3dngui
{

/// Processes any pending user interface events
void handle_pending_events();
/// Provides a portable sleep() that doesn't block processing of UI events
void non_blocking_sleep(const double Seconds);
/// Loads a pixbuf from disk, with error-handling
Glib::RefPtr<Gdk::Pixbuf> load_pixbuf(const k3d::filesystem::path& Path);
/// Loads a pixbuf from disk, with error-handling
Glib::RefPtr<Gdk::Pixbuf> load_pixbuf(const k3d::filesystem::path& SharePath, const k3d::filesystem::path& Path);
/// Converts a k3d::color to a Gdk::Color
const Gdk::Color convert(const k3d::color& RHS);
/// Converts a Gdk::Color to a k3d::color
const k3d::color convert(const Gdk::Color& RHS);

} // namespace libk3dngui

#endif // !NGUI_UTILITY_H

