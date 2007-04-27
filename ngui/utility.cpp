// K-3D
// Copyright (c) 1995-2005, Timothy M. Shead
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

#include "utility.h"

#include <k3dsdk/share.h>
#include <k3dsdk/signal_system.h>

#include <gdkmm/pixbuf.h>
#include <gtkmm/main.h>

namespace libk3dngui
{

namespace detail
{

bool sleep_callback(bool* Done)
{
	*Done = true;
	return false;
}

const k3d::filesystem::path share_path()
{
	return k3d::share_path() / k3d::filesystem::generic_path("ngui/pixmap");
}

} // namespace detail

void handle_pending_events()
{
	while(Gtk::Main::events_pending())
		Gtk::Main::iteration();
}

void non_blocking_sleep(const double Seconds)
{
	bool done = false;

	Glib::signal_timeout().connect(
		sigc::bind(sigc::ptr_fun(detail::sleep_callback), &done), static_cast<unsigned long>(1000 * Seconds));

	while(!done)
		handle_pending_events();
}

Glib::RefPtr<Gdk::Pixbuf> load_pixbuf(const k3d::filesystem::path& Path)
{
	return load_pixbuf(detail::share_path(), Path);
}

Glib::RefPtr<Gdk::Pixbuf> load_pixbuf(const k3d::filesystem::path& SharePath, const k3d::filesystem::path& Path)
{
	Glib::RefPtr<Gdk::Pixbuf> result;
	const k3d::filesystem::path path = SharePath / Path;
	try
	{
		result = Gdk::Pixbuf::create_from_file(path.native_filesystem_string());
	}
	catch(Glib::Exception& e)
	{
		k3d::log() << error << e.what() << std::endl;
	}
	catch(std::exception& e)
	{
		k3d::log() << error << e.what() << std::endl;
	}
	catch(...)
	{
		k3d::log() << error << "Unknown exception loading [" << path.native_console_string() << "]" << std::endl;
	}

	return result;
}

/// Converts a k3d::color to a Gdk::Color
const Gdk::Color convert(const k3d::color& RHS)
{
	Gdk::Color result;
	result.set_rgb_p(RHS.red, RHS.green, RHS.blue);

	return result;
}

/// Converts a Gdk::Color to a k3d::color
const k3d::color convert(const Gdk::Color& RHS)
{
	return k3d::color(RHS.get_red_p(), RHS.get_green_p(), RHS.get_blue_p());
}

} // namespace libk3dngui

