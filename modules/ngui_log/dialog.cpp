// K-3D
// Copyright (c) 1995-2008, Timothy M. Shead
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

#include <k3d-i18n-config.h>
#include <k3dsdk/application_plugin_factory.h>
#include <k3dsdk/command_tree.h>
#include <k3dsdk/log_control.h>
#include <k3dsdk/module.h>
#include <k3dsdk/ngui/application_window.h>
#include <k3dsdk/ngui/console.h>

#include <gtkmm/texttag.h>

#include <boost/assign/list_of.hpp>

#include <sstream>

namespace module
{

namespace ngui
{

namespace log
{

/////////////////////////////////////////////////////////////////////////////
// dialog

/// Provides a window for viewing the contents of the K-3D log
class dialog :
	public libk3dngui::application_window
{
	typedef libk3dngui::application_window base;

public:
	dialog() :
		console(Gtk::manage(new k3d::ngui::console::control(*this, "console")))
	{
		k3d::command_tree().add(*this, "log_window");

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

		k3d::get_log_cache(sigc::mem_fun(*this, &dialog::on_log_message));
		k3d::connect_log_message(sigc::mem_fun(*this, &dialog::on_log_message));

		show_all();
	}

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

	k3d::ngui::console::control* const console;
	Glib::RefPtr<Gtk::TextTag> critical_tag;
	Glib::RefPtr<Gtk::TextTag> error_tag;
	Glib::RefPtr<Gtk::TextTag> warning_tag;
	Glib::RefPtr<Gtk::TextTag> debug_tag;
	Glib::RefPtr<Gtk::TextTag> default_tag;

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::application_plugin_factory<dialog> factory(
			k3d::uuid(0xe552df91, 0x264b17ad, 0x6e65cd9d, 0x208bdfe7),
			"NGUILogDialog",
			_("Displays the contents of the K-3D log"),
			"NGUI Dialog",
			k3d::iplugin_factory::STABLE,
			boost::assign::map_list_of("ngui:component-type", "dialog"));

		return factory;
	}
};

} // namespace log

} // namespace ngui

} // namespace module

K3D_MODULE_START(Registry)
	Registry.register_factory(module::ngui::log::dialog::get_factory());
K3D_MODULE_END

