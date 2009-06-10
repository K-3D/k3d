// K-3D
// Copyright (c) 1995-2009, Timothy M. Shead
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
#include <k3dsdk/iuser_interface.h>
#include <k3dsdk/module.h>
#include <k3dsdk/ngui/application_window.h>
#include <k3dsdk/result.h>
#include <k3dsdk/user_interface.h>

#include <gtkmm/label.h>

#include <glibmm/dispatcher.h>
#include <glibmm/thread.h>

#include <boost/scoped_ptr.hpp>
#include <boost/assign/list_of.hpp>

#include <ostream>

#include <spnav.h>

using namespace k3d::ngui;

namespace module
{

namespace ngui
{

namespace space_navigator
{

/// Encapsulates threaded asynchronous handling of SpaceNavigator hardware events.
class events
{
public:
	events()
	{
		static k3d::bool_t initialized = false;
		if(!initialized)
		{
			initialized = true;

			if(-1 == spnav_open())
			{
				k3d::log() << error << "Error opening connection to spnav daemon." << std::endl;
				return;
			}

			if(!Glib::thread_supported())
				Glib::thread_init();

			g_dispatcher.reset(new Glib::Dispatcher());
			g_dispatcher->connect(sigc::ptr_fun(handle_event));

			g_mutex.reset(new Glib::Mutex());

			Glib::Thread::create(sigc::ptr_fun(&event_loop), false);
		}
	}

	/// Connects a slot to a signal that will be emitted whenever a button event is received.
	sigc::connection connect_button_signal(const sigc::slot<void, int, int>& ButtonSlot)
	{
		return g_button_signal.connect(ButtonSlot);
	}

	/// Connects a slot to a signal that will be emitted whenever a motion event is received.
	sigc::connection connect_motion_signal(const sigc::slot<void, int, int, int, int, int, int>& MotionSlot)
	{
		return g_motion_signal.connect(MotionSlot);
	}

private:
	/// This method executes within the context of a separate thread that
	/// blocks waiting for events from the SpaceNavigator hardware.
	static void event_loop()
	{
		while(true)
		{
			if(0 == spnav_wait_event(&g_event))
			{
				k3d::log() << error << "Error waiting for event." << std::endl;
				continue;
			}

			// Signal the main thread that an event has occurred
			g_dispatcher->emit();

			// Wait until the main thread has handled the event
			Glib::Mutex::Lock lock(*g_mutex);
		}
	}

	/// This methods executes within the context of the main thread
	/// whenever a hardware event has been received.
	static void handle_event()
	{
		Glib::Mutex::Lock lock(*g_mutex);

		switch(g_event.type)
		{
			case SPNAV_EVENT_BUTTON:
				g_button_signal.emit(g_event.button.press, g_event.button.bnum);
				break;
			case SPNAV_EVENT_MOTION:
				g_motion_signal.emit(g_event.motion.x, g_event.motion.y, g_event.motion.z, g_event.motion.rx, g_event.motion.ry, g_event.motion.rz);
				break;
			default:
				break;
		}
	}

	/// Dispatcher used for threaded event notification
	static boost::scoped_ptr<Glib::Dispatcher> g_dispatcher;
	/// Used to make the watching thread wait until the event has been handled
	static boost::scoped_ptr<Glib::Mutex> g_mutex;
	/// Shared event datastructure
	static spnav_event g_event;
	/// Used to broadcast button events
	static sigc::signal<void, int, int> g_button_signal;
	/// Used to broadcast motion events
	static sigc::signal<void, int, int, int, int, int, int> g_motion_signal;
};

boost::scoped_ptr<Glib::Dispatcher> events::g_dispatcher;
boost::scoped_ptr<Glib::Mutex> events::g_mutex;
spnav_event events::g_event;
sigc::signal<void, int, int> events::g_button_signal;
sigc::signal<void, int, int, int, int, int, int> events::g_motion_signal;

class dialog :
	public application_window,
	public k3d::iunknown
{
	typedef application_window base;

public:
	dialog() :
		m_stream(k3d::log())
	{
		Gtk::Label* const label = new Gtk::Label(_("Logging SpaceNavigator events to stderr ...\nClose window to cancel logging."));
		add(*Gtk::manage(label));

		set_border_width(10);

		set_role("space_navigator");
		show_all();

		m_events.connect_button_signal(sigc::mem_fun(*this, &dialog::on_button));
		m_events.connect_motion_signal(sigc::mem_fun(*this, &dialog::on_motion));
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::application_plugin_factory<dialog> factory(
			k3d::uuid(0xa58ff8d7, 0xfa4525b4, 0x500167bc, 0x4b4b22b4),
			"NGUISpaceNavigatorDialog",
			_("Sends a stream of SpaceNavigator events to stderr, intended mainly for debugging."),
			"NGUI Dialog",
			k3d::iplugin_factory::EXPERIMENTAL,
			boost::assign::map_list_of("ngui:component-type", "dialog")
			);

		return factory;
	}

private:
	void on_button(int Press, int Button)
	{
		m_stream << Press << " " << Button << std::endl;
	}

	void on_motion(int X, int Y, int Z, int RX, int RY, int RZ)
	{
		m_stream << X << " " << Y << " " << Z << " " << RX << " " << RY << " " << RZ << std::endl;
	}

	/// Stores the output file stream
	std::ostream& m_stream;
	/// Broadcasts device events
	events m_events;
};

} // namespace space_navigator

} // namespace ngui

} // namespace module

K3D_MODULE_START(Registry)
	Registry.register_factory(module::ngui::space_navigator::dialog::get_factory());
K3D_MODULE_END

