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

#include <gtkmm/label.h>
#include <gtk/gtk.h>

#include <ostream>

using namespace libk3dngui;

namespace module
{

namespace ngui
{

namespace event_recorder
{

std::ostream& operator<<(std::ostream& Stream, const GdkEventButton& RHS)
{
	Stream << "window: " << RHS.window << " send_event: " << static_cast<int>(RHS.send_event) << " time: " << RHS.time;
	Stream << " x: " << RHS.x << " y: " << RHS.y << " axes: " << RHS.axes << " state: " << RHS.state;
	Stream << " button: " << RHS.button << " device: " << RHS.device << " x_root: " << RHS.x_root << " y_root: " << RHS.y_root;

	return Stream;
}

std::ostream& operator<<(std::ostream& Stream, const GdkEventKey& RHS)
{
	Stream << "window: " << RHS.window << " send_event: " << static_cast<int>(RHS.send_event) << " time: " << RHS.time;
	Stream << " state: " << RHS.state << " keyval: 0x" << std::hex << RHS.keyval << std::dec << " length: " << RHS.length;
	Stream << " string: [" << RHS.string << "] hw keycode: " << RHS.hardware_keycode << " group: " << RHS.group;
	
	return Stream;
}

class dialog :
	public application_window
{
	typedef application_window base;

public:
	dialog() :
		base("event_recorder", 0),
		m_stream(k3d::log())
	{
		Gtk::Label* const label = new Gtk::Label(_("Recording GTK+ events and K-3D commands to stderr ...\nClose window to cancel recording"));
		add(*Gtk::manage(label));

		set_border_width(10);

		set_role("event_recorder");
		show_all();

		k3d::command_tree().command_signal().connect(sigc::mem_fun(*this, &dialog::on_command));
		gdk_event_handler_set(raw_on_gdk_event, this, NULL);
	}

	~dialog()
	{
		gdk_event_handler_set(GdkEventFunc(gtk_main_do_event), NULL, NULL);
	}

	void on_command(k3d::icommand_node& Node, const k3d::icommand_node::type Type, const std::string& Command, const std::string& Arguments)
	{
		// Sanity checks ...
		return_if_fail(Command.size());

		m_stream << "command: " << k3d::command_node::path(Node) << " " << Command << " " << Arguments << std::endl;
	}

	void on_gdk_event(const GdkEvent& Event)
	{
		switch(Event.type)
		{
			case GDK_NOTHING:
//				m_stream << "GDK_NOTHING" << std::endl;
				break;
			case GDK_DELETE:
				m_stream << "GDK_DELETE" << std::endl;
				break;
			case GDK_DESTROY:
				m_stream << "GDK_DESTROY" << std::endl;
				break;
			case GDK_EXPOSE:
//				m_stream << "GDK_EXPOSE" << std::endl;
				break;
			case GDK_MOTION_NOTIFY:
//				m_stream << "GDK_MOTION_NOTIFY" << std::endl;
				break;
			case GDK_BUTTON_PRESS:
				m_stream << "GDK_BUTTON_PRESS " << Event.button << std::endl;
				break;
			case GDK_2BUTTON_PRESS:
				m_stream << "GDK_2BUTTON_PRESS " << Event.button << std::endl;
				break;
			case GDK_3BUTTON_PRESS:
				m_stream << "GDK_3BUTTON_PRESS " << Event.button << std::endl;
				break;
			case GDK_BUTTON_RELEASE:
				m_stream << "GDK_BUTTON_RELEASE " << Event.button << std::endl;
				break;
			case GDK_KEY_PRESS:
				m_stream << "GDK_KEY_PRESS " << Event.key << std::endl;
				break;
			case GDK_KEY_RELEASE:
				m_stream << "GDK_KEY_RELEASE " << Event.key << std::endl;
				break;
			case GDK_ENTER_NOTIFY:
//				m_stream << "GDK_ENTER_NOTIFY" << std::endl;
				break;
			case GDK_LEAVE_NOTIFY:
//				m_stream << "GDK_LEAVE_NOTIFY" << std::endl;
				break;
			case GDK_FOCUS_CHANGE:
//				m_stream << "GDK_FOCUS_CHANGE" << std::endl;
				break;
			case GDK_CONFIGURE:
//				m_stream << "GDK_CONFIGURE" << std::endl;
				break;
			case GDK_MAP:
//				m_stream << "GDK_MAP" << std::endl;
				break;
			case GDK_UNMAP:
//				m_stream << "GDK_UNMAP" << std::endl;
				break;
			case GDK_PROPERTY_NOTIFY:
				m_stream << "GDK_PROPERTY_NOTIFY" << std::endl;
				break;
			case GDK_SELECTION_CLEAR:
				m_stream << "GDK_SELECTION_CLEAR" << std::endl;
				break;
			case GDK_SELECTION_REQUEST:
				m_stream << "GDK_SELECTION_REQUEST" << std::endl;
				break;
			case GDK_SELECTION_NOTIFY:
				m_stream << "GDK_SELECTION_NOTIFY" << std::endl;
				break;
			case GDK_PROXIMITY_IN:
				m_stream << "GDK_PROXIMITY_IN" << std::endl;
				break;
			case GDK_PROXIMITY_OUT:
				m_stream << "GDK_PROXIMITY_OUT" << std::endl;
				break;
			case GDK_DRAG_ENTER:
				m_stream << "GDK_DRAG_ENTER" << std::endl;
				break;
			case GDK_DRAG_LEAVE:
				m_stream << "GDK_DRAG_LEAVE" << std::endl;
				break;
			case GDK_DRAG_MOTION:
				m_stream << "GDK_DRAG_MOTION" << std::endl;
				break;
			case GDK_DRAG_STATUS:
				m_stream << "GDK_DRAG_STATUS" << std::endl;
				break;
			case GDK_DROP_START:
				m_stream << "GDK_DROP_START" << std::endl;
				break;
			case GDK_DROP_FINISHED:
				m_stream << "GDK_DROP_FINISHED" << std::endl;
				break;
			case GDK_CLIENT_EVENT:
				m_stream << "GDK_CLIENT_EVENT" << std::endl;
				break;
			case GDK_VISIBILITY_NOTIFY:
//				m_stream << "GDK_VISIBILITY_NOTIFY" << std::endl;
				break;
			case GDK_NO_EXPOSE:
				m_stream << "GDK_NO_EXPOSE" << std::endl;
				break;
			case GDK_SCROLL:
				m_stream << "GDK_SCROLL" << std::endl;
				break;
			case GDK_WINDOW_STATE:
//				m_stream << "GDK_WINDOW_STATE" << std::endl;
				break;
			case GDK_SETTING:
				m_stream << "GDK_SETTING" << std::endl;
				break;
			default:
				m_stream << "UNKNOWN" << std::endl;
				break;
		}
	}

	static void raw_on_gdk_event(GdkEvent* Event, gpointer Data)
	{
		if(Event && Data)
			reinterpret_cast<dialog*>(Data)->on_gdk_event(*Event);

		gtk_main_do_event(Event);
	}

	/// Stores the output file stream
	std::ostream& m_stream;

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::application_plugin_factory<dialog> factory(
			k3d::uuid(0xbabb3819, 0x244163c5, 0xdec79daa, 0xf579ab3e),
			"NGUIEventRecorderDialog",
			_("Sends a stream of user interface events to stderr, primarily intended for troubleshooting"),
			"NGUI Dialogs",
			k3d::iplugin_factory::EXPERIMENTAL);

		return factory;
	}
};

} // namespace event_recorder

} // namespace ngui

} // namespace module

K3D_MODULE_START(Registry)
	Registry.register_factory(module::ngui::event_recorder::dialog::get_factory());
K3D_MODULE_END

