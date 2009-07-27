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
#include <k3dsdk/log.h>
#include <k3dsdk/module.h>
#include <k3dsdk/ngui/application_window.h>
#include <k3dsdk/result.h>
#include <k3dsdk/string_cast.h>
#include <k3dsdk/type_registry.h>

#include <atkmm/object.h>

#include <gtkmm/label.h>
#include <gtk/gtk.h>

#include <boost/algorithm/string.hpp>
#include <boost/assign/list_of.hpp>
#include <stack>

namespace module
{

namespace ngui
{

namespace atk_event_recorder
{

namespace detail
{

const k3d::string_t script_name(const k3d::string_t& Name)
{
	return boost::replace_all_copy(Name, " ", "_");
}

/// Returns the index in a "per role" array of children, for the given child number of the parent Object
const k3d::uint_t get_index_in_role(AtkObject* Object)
{
	const k3d::uint_t index_in_parent = atk_object_get_index_in_parent(Object);
	const AtkRole role = atk_object_get_role(Object);
	AtkObject* parent = atk_object_get_parent(Object);
	return_val_if_fail(parent, 0);
	k3d::uint_t result = 0;
	for(k3d::uint_t i = 0; i != index_in_parent; ++i)
	{
		if(atk_object_get_role(atk_object_ref_accessible_child(parent, i)) == role)
			++result;
	}
	return result;
}

} // namespace detail

class dialog :
	public k3d::ngui::application_window,
	public k3d::iunknown
{
	typedef k3d::ngui::application_window base;

public:
	dialog()
	{
		set_title(_("ATK Event Recorder"));
		set_role("event_recorder");
		set_border_width(10);

		add(*manage(new Gtk::Label(_("Recording ATK events to stderr ...\nClose window to end recording"))));
		show_all();

		k3d::log() << debug << "Listening to " << atk_get_toolkit_name() << " " << atk_get_toolkit_version() << std::endl;

//		m_event_listeners.push_back(atk_add_global_event_listener(raw_event_listener, "window:create"));
//		m_event_listeners.push_back(atk_add_global_event_listener(raw_event_listener, "window:destroy"));
//		m_event_listeners.push_back(atk_add_global_event_listener(raw_event_listener, "window:minimize"));
//		m_event_listeners.push_back(atk_add_global_event_listener(raw_event_listener, "window:maximize"));
//		m_event_listeners.push_back(atk_add_global_event_listener(raw_event_listener, "window:restore"));
//		m_event_listeners.push_back(atk_add_global_event_listener(raw_event_listener, "window:activate"));
//		m_event_listeners.push_back(atk_add_global_event_listener(raw_event_listener, "window:deactivate"));
//		m_event_listeners.push_back(atk_add_global_event_listener(raw_event_listener, "Gtk:AtkObject:state-change"));
//		m_event_listeners.push_back(atk_add_global_event_listener(raw_event_listener, "Gtk:AtkObject:property-change"));
////		m_event_listeners.push_back(atk_add_global_event_listener(raw_event_listener, "Gtk:AtkObject:visible-data-changed"));
//		m_event_listeners.push_back(atk_add_global_event_listener(raw_event_listener, "Gtk:GtkButton:enter"));
//		m_event_listeners.push_back(atk_add_global_event_listener(raw_event_listener, "Gtk:GtkButton:pressed"));
//		m_event_listeners.push_back(atk_add_global_event_listener(raw_event_listener, "Gtk:GtkButton:released"));
//		m_event_listeners.push_back(atk_add_global_event_listener(raw_event_listener, "Gtk:GtkButton:leave"));
		m_event_listeners.push_back(atk_add_global_event_listener(click_event_listener, "Gtk:GtkButton:clicked"));
		m_event_listeners.push_back(atk_add_global_event_listener(click_event_listener, "Gtk:GtkMenuItem:activate"));
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
		Glib::RefPtr<Atk::Object> object;
		if(GTK_IS_WIDGET(g_value_get_object(param_values)))
		{
			Gtk::Widget* widget = Glib::wrap(GTK_WIDGET(g_value_get_object(param_values)));
			object = widget->get_accessible();
		}
		else
		{
			object = Glib::wrap(ATK_OBJECT(g_value_get_object(param_values + 0)), true);
		}
		return_val_if_fail(object, true);

		k3d::log() << debug << "AtkEvent: signal=" << signal_query.signal_name << ", object=" << object->get_name();

		k3d::log() << debug << ", type=";
		if(ATK_IS_ACTION(object->gobj()))
		{
			k3d::log() << debug << "action, with actions:";
			AtkAction* action = ATK_ACTION(object->gobj());
			for(k3d::uint_t i = 0; i != atk_action_get_n_actions(action); ++i)
				k3d::log() << debug << " (" << atk_action_get_name(action, i) << ")";
		}

		k3d::log() << debug << ", parameters:";

		for(k3d::uint_t param = 1; param != n_param_values; ++param)
		{
			k3d::log() << debug << " [";
			const GValue* val = param_values + param;
			if(G_VALUE_HOLDS_STRING(val))
				k3d::log() << debug << g_value_get_string(val);
			if(G_VALUE_HOLDS_INT(val))
				k3d::log() << debug << g_value_get_int(val);
			k3d::log() << debug << "]";
		}

		k3d::log() << debug << std::endl;


		return true;
	}

	static gboolean click_event_listener(GSignalInvocationHint* signal_hint, guint n_param_values, const GValue* param_values, gpointer data)
	{
		AtkObject* root = atk_get_root();
		Glib::RefPtr<Atk::Object> object;
		if(GTK_IS_WIDGET(g_value_get_object(param_values)))
		{
			Gtk::Widget* widget = Glib::wrap(GTK_WIDGET(g_value_get_object(param_values)));
			object = widget->get_accessible();
		}
		else
		{
			object = Glib::wrap(ATK_OBJECT(g_value_get_object(param_values)), true);
		}
		return_val_if_fail(object, true);
		k3d::bool_t click_found = false;
		if(ATK_IS_ACTION(object->gobj()))
		{
			AtkAction* action = ATK_ACTION(object->gobj());
			for(k3d::uint_t i = 0; i != atk_action_get_n_actions(action); ++i)
			{
				if(std::string(atk_action_get_name(action, i)) == "click")
					click_found = true;
			}
		}
		return_val_if_fail(click_found, true);
		std::stack<Glib::RefPtr<Atk::Object> > click_trace;
		while(object && object->get_index_in_parent() > -1)
		{
			click_trace.push(object);
			object = object->get_parent();
		}
		k3d::log() << debug << "k3d.atk.root()";
		while(!click_trace.empty())
		{
			Glib::RefPtr<Atk::Object> obj = click_trace.top();
			k3d::log() << debug << "." << detail::script_name(atk_role_get_name(atk_object_get_role(obj->gobj())));
			if(obj->get_name() == "")
				k3d::log() << debug << "(" << detail::get_index_in_role(obj->gobj()) << ")";
			else
				k3d::log() << debug << "(\"" << obj->get_name() << "\")";
			click_trace.pop();
		}
		k3d::log() << debug << ".click()" << std::endl;
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

