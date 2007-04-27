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
	\author Tim Shead (tshead@k-3d.com)
*/

#include "application_window.h"
#include "console.h"

#include <k3dsdk/i18n.h>
#include <k3dsdk/log_control.h>

#include <gtkmm/texttag.h>

#include <sstream>

namespace libk3dngui
{

/////////////////////////////////////////////////////////////////////////////
// log_window

/// Implements a menu of available tutorials
class log_window :
	public application_window
{
	typedef application_window base;

public:
	log_window() :
		base("log_window", 0),
		console(Gtk::manage(new console::control(*this, "console")))
	{
		critical_tag = Gtk::TextTag::create("critical");
		critical_tag->property_foreground() = "#ff0000";
		critical_tag->property_weight() = Pango::WEIGHT_BOLD;

		error_tag = Gtk::TextTag::create("error");
		error_tag->property_foreground() = "#ff0000";

		warning_tag = Gtk::TextTag::create("warning");
		warning_tag->property_foreground() = "#ff8000";

		debug_tag = Gtk::TextTag::create("debug");
		debug_tag->property_foreground() = "#00bb00";

		set_title(_("K-3D Log Window"));
		set_role("log_window");
		set_border_width(0);
		resize(800, 400);
		set_position(Gtk::WIN_POS_CENTER);

		add(*console);

		k3d::get_log_cache(sigc::mem_fun(*this, &log_window::on_log_message));
		k3d::connect_log_message(sigc::mem_fun(*this, &log_window::on_log_message));

		show_all();
	}

private:
	void on_log_message(const time_t Timestamp, const k3d::log_level_t Level, const std::string& Message)
	{
		std::string timestamp(256, '\0');
		timestamp.resize(strftime(&timestamp[0], timestamp.size(), "%m/%d/%Y %H:%M:%S ", localtime(&Timestamp)));
		console->set_current_format(default_tag);
		console->print_string(timestamp);

		switch(Level)
		{
		case k3d::K3D_LOG_LEVEL_CRITICAL:
			console->set_current_format(critical_tag);
			break;
		case k3d::K3D_LOG_LEVEL_ERROR:
			console->set_current_format(error_tag);
			break;
		case k3d::K3D_LOG_LEVEL_WARNING:
			console->set_current_format(warning_tag);
			break;
		case k3d::K3D_LOG_LEVEL_INFO:
			console->set_current_format(default_tag);
			break;
		case k3d::K3D_LOG_LEVEL_DEBUG:
			console->set_current_format(debug_tag);
			break;
		default:
			console->set_current_format(default_tag);
			break;
		}

		switch(Level)
		{
		case k3d::K3D_LOG_LEVEL_CRITICAL:
			console->print_string("CRITICAL: ");
			break;
		case k3d::K3D_LOG_LEVEL_ERROR:
			console->print_string("ERROR: ");
			break;
		case k3d::K3D_LOG_LEVEL_WARNING:
			console->print_string("WARNING: ");
			break;
		case k3d::K3D_LOG_LEVEL_INFO:
			console->print_string("INFO: ");
			break;
		case k3d::K3D_LOG_LEVEL_DEBUG:
			console->print_string("DEBUG: ");
			break;
		}

		console->print_string(Message);
	}

	console::control* const console;
	Glib::RefPtr<Gtk::TextTag> critical_tag;
	Glib::RefPtr<Gtk::TextTag> error_tag;
	Glib::RefPtr<Gtk::TextTag> warning_tag;
	Glib::RefPtr<Gtk::TextTag> debug_tag;
	Glib::RefPtr<Gtk::TextTag> default_tag;
};

void create_log_window()
{
	new log_window();
}

} // namespace libk3dngui

