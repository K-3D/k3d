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
	\author Timothy M. Shead (tshead@k-3d.com)
*/

#include "inotify-cxx.h"

#include <k3d-i18n-config.h>
#include <k3dsdk/application_plugin_factory.h>
#include <k3dsdk/ifile_change_notifier.h>
#include <k3dsdk/log.h>
#include <k3dsdk/path.h>
#include <k3dsdk/result.h>

#include <boost/scoped_ptr.hpp>
#include <boost/shared_ptr.hpp>

#include <stdexcept>

namespace module
{

namespace inotify
{

class file_change_notifier :
	public k3d::ifile_change_notifier
{
public:
	file_change_notifier()
	{
		try
		{
			inotify.reset(new Inotify());
			inotify->SetNonBlock(true);
		}
		catch(InotifyException& e)
		{
			inotify.reset();
			k3d::log() << error << e.GetMessage() << " - Inotify-based change notification disabled." << std::endl;
		}
	}
	
	k3d::uint_t watch_file(const k3d::filesystem::path& Path, const sigc::slot<void>& Slot)
	{
		try
		{
			if(!inotify)
				throw std::runtime_error("Inotify-based change notification disabled.");

			if(Path.empty())
				throw std::runtime_error("Cannot watch empty path.");

			boost::shared_ptr<InotifyWatch> watch;
			if(paths.count(Path))
			{
				watch = paths[Path];
			}
			else
			{
				watch.reset(new InotifyWatch(Path.native_filesystem_string(), IN_CLOSE_WRITE));
				inotify->Add(*watch);
				paths[Path] = watch;
				k3d::log() << debug << "Adding watch on " << Path.native_console_string() << std::endl;
			}

			for(k3d::uint_t i = 0; i != watches.size(); ++i)
			{
				if(!watches[i])
				{
					watches[i] = watch.get();
					slots[i] = Slot;
					return i + 1;
				}
			}

			watches.push_back(watch.get());
			slots.push_back(Slot);
			return watches.size();
		}
		catch(InotifyException& e)
		{
			k3d::log() << error << e.GetMessage() << " code: " << e.GetErrorNumber() << std::endl;
			return 0;
		}
		catch(std::exception& e)
		{
			k3d::log() << error << e.what() << std::endl;
			return 0;
		}
	}
	
	void unwatch_file(const k3d::uint_t WatchID)
	{
		if(!inotify)
			return;

		if(WatchID == 0)
			return;

		try
		{
			const k3d::uint_t watch_index = WatchID - 1;

			if(watch_index >= watches.size())
				throw std::runtime_error("Watch ID out-of-range.");

			if(!watches[watch_index])
				throw std::runtime_error("Invalid Watch ID.");

			InotifyWatch* const watch = watches[watch_index];
			watches[watch_index] = 0;
			slots[watch_index] = sigc::slot<void>();

			if(0 == std::count(watches.begin(), watches.end(), watch))
			{
				for(paths_t::iterator path = paths.begin(); path != paths.end(); ++path)
				{
					if(path->second.get() == watch)
					{	
						k3d::log() << debug << "Removing watch on " << path->first.native_console_string() << std::endl;
						inotify->Remove(*watch);
						paths.erase(path);
						return;
					}
				}
			}

		}
		catch(InotifyException& e)
		{
			k3d::log() << error << e.GetMessage() << " code: " << e.GetErrorNumber() << std::endl;
		}
		catch(std::exception& e)
		{
			k3d::log() << error << e.what() << std::endl;
		}
	}
	
	const k3d::bool_t pending_changes()
	{
		if(!inotify)
			return false;

		inotify->WaitForEvents();
		return inotify->GetEventCount();
	}
	
	void notify_change()
	{
		if(!inotify)
			return;

		if(!inotify->GetEventCount())
		{
			inotify->SetNonBlock(false);
			inotify->WaitForEvents();
			inotify->SetNonBlock(true);
		}
		
		handle_event();
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::application_plugin_factory<file_change_notifier,
			k3d::interface_list<k3d::ifile_change_notifier> > factory(
				k3d::uuid(0x37e5155b, 0xed45e9aa, 0xd1f0ed88, 0xb582ea76),
				"InotifyFileChangeNotifier",
				_("Monitors files for changes, using the Linux inotify system"),
				"Desktop",
				k3d::iplugin_factory::STABLE);

		return factory;
	}
	
private:
	/// Assumes an event is on the queue, and handles it
	void handle_event()
	{
		InotifyEvent event;
		if(!inotify->GetEvent(&event))
		{
			k3d::log() << debug << "file_change_notifier::handle_event: Did not get an event" << std::endl;
			return;
		}

		InotifyWatch* watch = event.GetWatch();
		k3d::log() << debug << "Triggering event for " << watch->GetPath() << std::endl;

		for(k3d::uint_t i = 0; i != watches.size(); ++i)
		{
			if(watches[i] == watch)
				slots[i]();
		}
	}

	boost::scoped_ptr<Inotify> inotify;

	typedef std::map<k3d::filesystem::path, boost::shared_ptr<InotifyWatch> > paths_t;
	paths_t paths;

	std::vector<InotifyWatch*> watches;
	std::vector<sigc::slot<void> > slots;
};


/////////////////////////////////////////////////////////////////////////////
// file_change_notifier_factory

k3d::iplugin_factory& file_change_notifier_factory()
{
	return file_change_notifier::get_factory();
}

} // namespace inotify

} // namespace module

