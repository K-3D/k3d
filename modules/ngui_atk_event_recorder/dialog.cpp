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
#include <k3dsdk/log.h>
#include <k3dsdk/module.h>
#include <k3dsdk/ngui/application_window.h>
#include <k3dsdk/result.h>
#include <k3dsdk/string_cast.h>

#include <atkmm/object.h>

#include <gtkmm/label.h>
#include <gtk/gtk.h>

#include <boost/assign/list_of.hpp>

namespace module
{

namespace ngui
{

namespace atk_event_recorder
{

class dialog :
	public k3d::ngui::application_window
{
	typedef k3d::ngui::application_window base;

public:
	dialog()
	{
		k3d::command_tree().add(*this, "atk_event_recorder");

		set_title(_("ATK Event Recorder"));
		set_role("event_recorder");
		set_border_width(10);

		add(*manage(new Gtk::Label(_("Recording ATK events to stderr ...\nClose window to end recording"))));
		show_all();

		k3d::log() << debug << "Listening to " << atk_get_toolkit_name() << " " << atk_get_toolkit_version() << std::endl;

		m_event_listeners.push_back(atk_add_global_event_listener(raw_event_listener, "window:create"));
		m_event_listeners.push_back(atk_add_global_event_listener(raw_event_listener, "window:destroy"));
		m_event_listeners.push_back(atk_add_global_event_listener(raw_event_listener, "window:minimize"));
		m_event_listeners.push_back(atk_add_global_event_listener(raw_event_listener, "window:maximize"));
		m_event_listeners.push_back(atk_add_global_event_listener(raw_event_listener, "window:restore"));
		m_event_listeners.push_back(atk_add_global_event_listener(raw_event_listener, "window:activate"));
		m_event_listeners.push_back(atk_add_global_event_listener(raw_event_listener, "window:deactivate"));
		m_event_listeners.push_back(atk_add_global_event_listener(raw_event_listener, "Gtk:AtkObject:state-change"));
//		m_event_listeners.push_back(atk_add_global_event_listener(raw_event_listener, "Gtk:AtkObject:visible-data-changed"));
		m_event_listeners.push_back(atk_add_global_event_listener(raw_event_listener, "Gtk:GtkButton:enter"));
		m_event_listeners.push_back(atk_add_global_event_listener(raw_event_listener, "Gtk:GtkButton:pressed"));
		m_event_listeners.push_back(atk_add_global_event_listener(raw_event_listener, "Gtk:GtkButton:released"));
		m_event_listeners.push_back(atk_add_global_event_listener(raw_event_listener, "Gtk:GtkButton:leave"));
	}

	~dialog()
	{
		std::for_each(m_event_listeners.begin(), m_event_listeners.end(), atk_remove_global_event_listener);
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::application_plugin_factory<dialog> factory(
			k3d::uuid(0xc8ac6ef4, 0xce48212a, 0xed0006b2, 0x8f29e761),
			"NGUIATKEventRecorderDialog",
			_("Sends a stream of ATK user interface events to stderr, primarily intended for troubleshooting"),
			"NGUI Dialog",
			k3d::iplugin_factory::EXPERIMENTAL,
			boost::assign::map_list_of("ngui:component-type", "dialog")
			);

		return factory;
	}

private:
	static gboolean raw_event_listener(GSignalInvocationHint* signal_hint, guint n_param_values, const GValue* param_values, gpointer data)
	{
		GSignalQuery signal_query;
		g_signal_query(signal_hint->signal_id, &signal_query);
		const k3d::string_t signal_name = signal_query.signal_name;
		Glib::RefPtr<Atk::Object> object = Glib::wrap(ATK_OBJECT(g_value_get_object(param_values + 0)), true);

		k3d::log() << debug << signal_query.signal_name << " [" << object->get_name() << "]";

		if(signal_name == "state-change")
		{
			k3d::log() << " [" << g_value_get_string(param_values + 1) << "]";
		}

		k3d::log() << std::endl;

		return true;
	}

	std::vector<guint> m_event_listeners;
};

} // namespace atk_event_recorder

} // namespace ngui

} // namespace module

K3D_MODULE_START(Registry)
	Registry.register_factory(module::ngui::atk_event_recorder::dialog::get_factory());
K3D_MODULE_END

