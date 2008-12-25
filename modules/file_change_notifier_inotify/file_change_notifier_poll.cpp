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

#include <glibmm.h>

#include "file_change_notifier_base.h"

#define FILE_CHANGE_NOTIFIER_POLL

namespace module
{

namespace file_change_notifier
{

namespace inotify
{

class file_change_notifier_poll : public detail::file_change_notifier_base<sigc::signal<void> >
{
	typedef detail::file_change_notifier_base<sigc::signal<void> > base;
public:
	file_change_notifier_poll()
	{
		m_inotify.SetNonBlock(true);
		// Poll about every 100ms
		m_idle_connection = Glib::signal_timeout().connect(sigc::mem_fun(*this, &file_change_notifier_poll::poll_changes), 100);
	}
	
	~file_change_notifier_poll()
	{
		m_idle_connection.disconnect();
	}
	
	void notify_change()
	{
		k3d::bool_t blocking = false;
		if(m_inotify.GetEventCount() == 0) // make sure we block if there are no events
		{
			m_inotify.SetNonBlock(false);
			blocking = true;
		}
		base::notify_change();
		if(blocking)
			m_inotify.SetNonBlock(true);
	}
	
	static k3d::iplugin_factory& get_factory()
	{
		static k3d::application_plugin_factory<file_change_notifier_poll,
			k3d::interface_list<k3d::ifile_change_notifier> > factory(
				k3d::uuid(0x37e5155b, 0xed45e9aa, 0xd1f0ed88, 0xb582ea76),
				"FileChangeNotifierInotifyPoll",
				_("Allows monitoring files for changes, using the Linux inotify system"),
				"Desktop",
				k3d::iplugin_factory::STABLE);

		return factory;
	}
	
private:
	/// Executed to poll for events
	bool poll_changes()
	{
		while(pending_changes())
		{
			notify_change();
		}
		return true;
	}
	
	sigc::connection m_idle_connection;
};


/////////////////////////////////////////////////////////////////////////////
// file_change_notifier_factory

k3d::iplugin_factory& file_change_notifier_poll_factory()
{
	return file_change_notifier_poll::get_factory();
}

} // namespace inotify

} // namespace file_change_notifier

} // namespace module

