#ifndef FILE_CHANGE_NOTIFIER_BASE_H_
#define FILE_CHANGE_NOTIFIER_BASE_H_

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

/** \file file_change_notifier_base.h
		\author Bart Janssens (bart.janssens@lid.kviv.be)
		\created Dec 19, 2008
*/

#include <k3dsdk/ifile_change_notifier.h>
#include <k3dsdk/result.h>
#include <k3dsdk/signal_system.h>
#include <k3dsdk/types.h>

#include <boost/shared_ptr.hpp>

#include "inotify-cxx.h"

namespace module
{

namespace file_change_notifier
{

namespace inotify
{

namespace detail
{

/// Base class for both the threaded and non-threaded variants
/**
 * Note: SignalT is a template, because in the threaded variant Glib::Dispatcher must be used as
 * signal in order to guarantee thread safety.
 */
template<typename SignalT>
class file_change_notifier_base : public k3d::ifile_change_notifier
{
	typedef std::map<k3d::int32_t, boost::shared_ptr<InotifyWatch> > watch_list_t;
	typedef std::map<k3d::int32_t, sigc::connection> connection_list_t;
	typedef std::map<InotifyWatch*, boost::shared_ptr<SignalT > > signal_map_t;
public:
	file_change_notifier_base() : m_watch_id_counter(1), m_mask(IN_CLOSE_WRITE) {}
	
	~file_change_notifier_base()
	{
		clear();
	}
	
	virtual k3d::uint_t watch_file(const k3d::filesystem::path& Path, const sigc::slot<void>& Slot)
	{
		const k3d::int32_t id = m_watch_id_counter;
		try
		{
			boost::shared_ptr<InotifyWatch> watch(new InotifyWatch(Path.native_filesystem_string(), m_mask));
			boost::shared_ptr<SignalT> changed_signal(new SignalT());
			m_inotify.Add(watch.get());
			m_watch_list[id] = watch;
			m_connection_list[id] = changed_signal->connect(Slot);
			m_signal_map[watch.get()] = changed_signal;
			++m_watch_id_counter;
			k3d::log() << debug << "adding watch on " << Path.native_console_string() << std::endl;
			return id;
		}
		catch(InotifyException& E)
		{
			const std::string error_message = E.GetMessage();
			if(error_message.find("path already watched") != std::string::npos) // file already watched
			{
				return_val_if_fail(id == m_watch_id_counter, 0);
				for(watch_list_t::iterator watch_it = m_watch_list.begin(); watch_it != m_watch_list.end(); ++watch_it)
				{
					boost::shared_ptr<InotifyWatch> watch = watch_it->second;
					if(watch->GetPath() == Path.native_filesystem_string())
					{
						m_watch_list[id] = watch;
						m_connection_list[id] = m_signal_map[watch.get()]->connect(Slot);
						++m_watch_id_counter;
						return id;
					}
				}
			}
			k3d::log() << error << "file_change_notifier::watch_file: Error adding watch: " << E.GetMessage() << " code: " << E.GetErrorNumber() << std::endl;
			return 0;
		}
	}
	
	virtual void unwatch_file(const k3d::uint_t WatchID)
	{
		if(WatchID < 0 || !m_watch_list.count(WatchID))
			return;
		
		// Disconnect from the client
		sigc::connection connection = m_connection_list[WatchID];
		connection.disconnect();
		
		// Get the watch, and check if it is shared
		k3d::uint_t watch_count = 0;
		boost::shared_ptr<InotifyWatch> watch = m_watch_list[WatchID];
		for(watch_list_t::iterator watch_it = m_watch_list.begin(); watch_it != m_watch_list.end(); ++watch_it)
		{
			if(watch_it->second == watch)
			{
				++watch_count;
			}
		}
		
		if(watch_count == 1)
		{
			m_signal_map.erase(watch.get());
			k3d::log() << debug << "Removing watch on " << watch->GetPath() << " (" << m_inotify.GetWatchCount() - 1 << " watches remaining)" << std::endl;
			m_inotify.Remove(watch.get());
		}
		
		m_watch_list.erase(WatchID);
	}
	
	virtual const k3d::bool_t pending_changes()
	{
		try
		{
			m_inotify.WaitForEvents();
			return m_inotify.GetEventCount() != 0;
		}
		catch(...)
		{
			k3d::log() << error << "file_change_notifier::pending_changes: Error getting event count" << std::endl;
			return false;
		}
	}
	
	virtual void notify_change()
	{
		const k3d::uint_t count = m_inotify.GetEventCount();
		if(count > 0)
		{
			handle_event();
		}
		else
		{
			m_inotify.WaitForEvents();
			handle_event();
		}
	}

protected:
	/// Removes all watches
	void clear()
	{
		for(connection_list_t::iterator connection = m_connection_list.begin(); connection != m_connection_list.end(); ++connection)
			connection->second.disconnect();
		m_watch_list.clear();
		m_connection_list.clear();
		m_signal_map.clear();
	}
	Inotify m_inotify;
	
	/// Assumes an event is on the queue, and handles it
	virtual void handle_event()
	{
		InotifyEvent event;
		k3d::bool_t got_event = m_inotify.GetEvent(&event);

		if (got_event)
		{
			if(!event.IsType(m_mask))
				return;
			InotifyWatch* watch = event.GetWatch();
			k3d::log() << debug << "triggering event for " << watch->GetPath() << std::endl;
			m_signal_map[watch]->emit();
		}
		else
		{
			k3d::log() << debug << "file_change_notifier::handle_event: Did not get an event" << std::endl;
		}
	}
private:
	k3d::int32_t m_watch_id_counter;
	watch_list_t m_watch_list;
	connection_list_t m_connection_list;
	signal_map_t m_signal_map;
	const k3d::uint_t m_mask;
};

} // namespace detail

} // namespace inotify

} // namespace file_change_notifier

} // namespace module

#endif /* FILE_CHANGE_NOTIFIER_BASE_H_ */
