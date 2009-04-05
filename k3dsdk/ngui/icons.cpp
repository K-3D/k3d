// K-3D
// Copyright (c) 1995-2006, Timothy M. Shead
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

/**		\author Timothy M. Shead (tshead@k-3d.com)
*/

#include "icons.h"

#include <k3dsdk/log.h>
#include <k3dsdk/result.h>
#include <k3dsdk/share.h>

#include <gdkmm/display.h>
#include <gtkmm/enums.h>
#include <gtkmm/icontheme.h>

#include <iostream>

namespace k3d
{

namespace ngui
{

namespace detail
{

const k3d::filesystem::path scalable_path()
{
	return k3d::share_path() / k3d::filesystem::generic_path("ngui/scalable");
}

const k3d::filesystem::path rasterized_path()
{
	return k3d::share_path() / k3d::filesystem::generic_path("ngui/rasterized");
}

const k3d::filesystem::path pixmap_path()
{
	return k3d::share_path() / k3d::filesystem::generic_path("ngui/pixmap");
}

#include "unknown.xpm"

} // namespace detail

Glib::RefPtr<Gdk::Pixbuf> load_icon(const std::string& Name, const Gtk::IconSize& Size)
{
	static Glib::RefPtr<Gtk::IconTheme> theme;
	if(!theme)
	{
		theme = Gtk::IconTheme::create();

#ifdef K3D_HAVE_SVG_ICONS
		theme->append_search_path(detail::scalable_path().native_utf8_string().raw());
		k3d::log() << info << "Loading icons from " << detail::scalable_path().native_console_string() << std::endl;
#endif // K3D_HAVE_SVG_ICONS

		theme->append_search_path(detail::rasterized_path().native_utf8_string().raw());
		k3d::log() << info << "Loading icons from " << detail::rasterized_path().native_console_string() << std::endl;

		theme->append_search_path(detail::pixmap_path().native_utf8_string().raw());
		k3d::log() << info << "Loading icons from " << detail::pixmap_path().native_console_string() << std::endl;
	}

	int width = 0;
	int height = 0;
	return_val_if_fail(Gtk::IconSize::lookup(Size, width, height), Glib::RefPtr<Gdk::Pixbuf>());

	try
	{
		return theme->load_icon(Name, width, Gtk::ICON_LOOKUP_USE_BUILTIN);
	}
	catch(Glib::Exception& e)
	{
		k3d::log() << error << e.what() << std::endl;
	}

	static Glib::RefPtr<Gdk::Pixbuf> unknown_icon;
	if(!unknown_icon)
	{
		try
		{
			unknown_icon = Gdk::Pixbuf::create_from_xpm_data(detail::unknown_xpm);
		}
		catch(Glib::Exception& e)
		{
			k3d::log() << error << e.what() << std::endl;
		}
	}

	return unknown_icon;
}

Glib::RefPtr<Gdk::Pixbuf> quiet_load_icon(const std::string& Name, const Gtk::IconSize& Size)
{
#ifdef K3D_HAVE_SVG_ICONS
	if(k3d::filesystem::exists(detail::scalable_path() / k3d::filesystem::native_path(k3d::ustring::from_utf8(Name + ".svg"))))
		return load_icon(Name, Size);
#endif // !K3D_HAVE_SVG_ICONS

	if(k3d::filesystem::exists(detail::rasterized_path() / k3d::filesystem::native_path(k3d::ustring::from_utf8(Name + ".png"))))
		return load_icon(Name, Size);

	if(k3d::filesystem::exists(detail::pixmap_path() / k3d::filesystem::native_path(k3d::ustring::from_utf8(Name + ".xpm"))))
		return load_icon(Name, Size);


	static Glib::RefPtr<Gdk::Pixbuf> unknown_icon;
	if(!unknown_icon)
	{
		try
		{
			unknown_icon = Gdk::Pixbuf::create_from_xpm_data(detail::unknown_xpm);
		}
		catch(Glib::Exception& e)
		{
			k3d::log() << error << e.what() << std::endl;
		}
	}
	return unknown_icon;
}

} // namespace ngui

} // namespace k3d

