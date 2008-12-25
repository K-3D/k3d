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
		\author Bart Janssens (bart.janssens@lid.kviv.be)
*/

#include <k3d-i18n-config.h>
#include <k3dsdk/application_plugin_factory.h>
#include <k3dsdk/path.h>
#include <k3dsdk/ifile_change_notifier.h>
#include <k3dsdk/log.h>

#include <boost/scoped_ptr.hpp>

#include <glibmm.h>

#include "file_change_notifier_base.h"

namespace module
{

namespace file_change_notifier
{

namespace inotify
{

namespace detail
{

class file_watch_thread
{
public:
	file_watch_thread(k3d::ifile_change_notifier& Notifier) :
		m_notifier(Notifier),
		m_thread(0)
	{}
	
	void start()
	{
		return_if_fail(!m_thread);
		k3d::log() << debug << "starting thread" << std::endl;
		m_thread = Glib::Thread::create(sigc::mem_fun(*this, &file_watch_thread::thread_function), false);
	}
	
private:
	void thread_function()
	{
		while(true)
		{
			m_notifier.notify_change();
		}
	}
	k3d::ifile_change_notifier& m_notifier;
	Glib::Thread* m_thread;
};

} // namespace detail

/// Thread-safe version of the file_change_notifier_base. Note we require the use of Glib::Dispatcher as signal type here
class file_change_notifier_thread : public detail::file_change_notifier_base<Glib::Dispatcher>
{
	typedef detail::file_change_notifier_base<Glib::Dispatcher> base;
public:
	file_change_notifier_thread()
	{
		if(!Glib::thread_supported()) Glib::thread_init();
		m_thread.reset(new detail::file_watch_thread(*this));
		m_mutex.reset(new Glib::Mutex());
		m_thread->start();
	}
	
	~file_change_notifier_thread()
	{
		Glib::Mutex::Lock lock(*m_mutex);
		base::clear();
	}
	
	k3d::uint_t watch_file(const k3d::filesystem::path& Path, const sigc::slot<void>& Slot)
	{
		Glib::Mutex::Lock lock(*m_mutex);
		return base::watch_file(Path, Slot);
	}
	
	void unwatch_file(const k3d::uint_t WatchID)
	{
		Glib::Mutex::Lock lock(*m_mutex);
		base::unwatch_file(WatchID);
	}
	
	void notify_change()
	{
		fd_set read_descriptors;
		FD_ZERO(&read_descriptors);
		FD_SET(m_inotify.GetDescriptor(), &read_descriptors);
		// We use select() to wait for changes, making sure base::notify_change executes immediately and while we are holding the lock
		if(select(m_inotify.GetDescriptor() + 1, &read_descriptors, NULL, &read_descriptors, NULL) > 0)
		{
			Glib::Mutex::Lock lock(*m_mutex);
			base::notify_change();
		}
	}
	
	static k3d::iplugin_factory& get_factory()
	{
		static k3d::application_plugin_factory<file_change_notifier_thread,
			k3d::interface_list<k3d::ifile_change_notifier> > factory(
				k3d::uuid(0x04de1312, 0xc247fab8, 0xf16f8380, 0x0f41112b),
				"FileChangeNotifierInotifyThread",
				_("Allows monitoring files for changes, using the Linux inotify system"),
				"Desktop",
				k3d::iplugin_factory::STABLE);

		return factory;
	}
	
private:
	boost::scoped_ptr<detail::file_watch_thread> m_thread;
	boost::scoped_ptr<Glib::Mutex> m_mutex;
};


/////////////////////////////////////////////////////////////////////////////
// file_change_notifier_factory

k3d::iplugin_factory& file_change_notifier_thread_factory()
{
	return file_change_notifier_thread::get_factory();
}

} // namespace inotify

} // namespace file_change_notifier

} // namespace module

