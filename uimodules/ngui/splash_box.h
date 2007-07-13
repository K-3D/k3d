#ifndef NGUI_SPLASH_BOX_H
#define NGUI_SPLASH_BOX_H

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
	\brief Declares the splash_box class, which provides a custom startup "splash" box for K-3D
	\author Tim Shead (tshead@k-3d.com)
*/

#include <gtkmm/window.h>

namespace k3d { namespace filesystem { class path; } }
namespace Gtk { class Label; }

namespace module
{

namespace ngui
{

/////////////////////////////////////////////////////////////////////////////
// splash_box

/// Creates a modal "splash" box that displays program version and copyright
class splash_box :
	public Gtk::Window
{
	typedef Gtk::Window base;

public:
	splash_box(const k3d::filesystem::path& SharePath);
	void on_startup_message(const std::string& Message);

private:
	Gtk::Label* const m_startup_message;
};

} // namespace ngui

} // namespace module

#endif // NGUI_SPLASH_BOX_H

