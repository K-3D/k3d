// K-3D
// Copyright (c) 1995-2008, Timothy M. Shead
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

/** \file
	\author Tim Shead (tshead@k-3d.com)
*/

#include "file_notification.h"

#include <k3dsdk/ifile_change_notifier.h>
#include <k3dsdk/plugins.h>

#include <glibmm/dispatcher.h>
#include <glibmm/thread.h>

#include <boost/scoped_ptr.hpp>

namespace module
{

namespace ngui
{

/////////////////////////////////////////////////////////////////////////////
// file_notification::implementation

class file_notification::implementation
{
public:
	/// Stores an object that provides an abstraction over platform-specific file-change-notification APIs.
	boost::scoped_ptr<k3d::ifile_change_notifier> m_notifier;
	/// Controls a timeout loop or thread that handles file-notification events.
	sigc::connection m_file_notification_connection;
	/// Dispatcher used for threaded file change notification
	boost::scoped_ptr<Glib::Dispatcher> m_file_notification_dispatcher;
	/// Mutex and condition, used to make the watching thread wait until the event has been handled
	boost::scoped_ptr<Glib::Mutex> m_file_notification_mutex;
	boost::scoped_ptr<Glib::Cond> m_file_notification_cond;
};
	
/////////////////////////////////////////////////////////////////////////////
// file_notification

file_notification::file_notification() :
	m_implementation(new implementation())
{
}

file_notification::~file_notification()
{
	delete m_implementation;
}

void file_notification::start()
{
	// Use the first file change notifier found
	const k3d::plugin::factory::collection_t factories = k3d::plugin::factory::lookup<k3d::ifile_change_notifier>();
	if(factories.size())
		m_implementation->m_notifier.reset(k3d::plugin::create<k3d::ifile_change_notifier>(**factories.begin()));

	if(!m_implementation->m_notifier)
	{
		k3d::log() << info << "File change notification disabled." << std::endl;
		return;
	}

#ifdef G_THREADS_ENABLED
	if(!Glib::thread_supported())
		Glib::thread_init();
	m_implementation->m_file_notification_dispatcher.reset(new Glib::Dispatcher());
	m_implementation->m_file_notification_mutex.reset(new Glib::Mutex());
	m_implementation->m_file_notification_cond.reset(new Glib::Cond());
	m_implementation->m_file_notification_connection = m_implementation->m_file_notification_dispatcher->connect(sigc::mem_fun(*this, &file_notification::on_notify_file_changes_threaded));
	Glib::Thread::create(sigc::mem_fun(*this, &file_notification::notify_file_change_thread_function), false);
#else
	m_implementation->m_file_notification_connection = Glib::signal_timeout().connect(sigc::bind_return(sigc::mem_fun(*this, &file_notification::on_notify_file_changes), true), 1000);
#endif
}

k3d::uint_t file_notification::watch_path(const k3d::filesystem::path& Path, const sigc::slot<void>& Slot)
{
	if(!m_implementation->m_notifier)
		return 0;

	return m_implementation->m_notifier->watch_file(Path, Slot);
}

void file_notification::unwatch_path(const k3d::uint_t WatchID)
{
	if(!m_implementation->m_notifier)
		return;

	m_implementation->m_notifier->unwatch_file(WatchID);
}
	
void file_notification::stop()
{
	m_implementation->m_file_notification_connection.disconnect();
}

void file_notification::on_notify_file_changes()
{
	while(m_implementation->m_notifier->change_count())
		m_implementation->m_notifier->signal_change();
}

void file_notification::on_notify_file_changes_threaded()
{
	Glib::Mutex::Lock lock(*m_implementation->m_file_notification_mutex);
	on_notify_file_changes();
	m_implementation->m_file_notification_cond->signal();
}

void file_notification::notify_file_change_thread_function()
{
	while(true)
	{
		m_implementation->m_notifier->wait_for_changes();
		// Signal the main thread that events have occurred
		m_implementation->m_file_notification_dispatcher->emit();
		// Wait until the main thread has handled the events
		Glib::Mutex::Lock lock(*m_implementation->m_file_notification_mutex);
		m_implementation->m_file_notification_cond->wait(*m_implementation->m_file_notification_mutex);
	}
}

} // namespace ngui

} // namespace module

