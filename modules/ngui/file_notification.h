#ifndef MODULES_NGUI_FILE_NOTIFICATION_H
#define MODULES_NGUI_FILE_NOTIFICATION_H

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

#include <k3dsdk/signal_system.h>
#include <k3dsdk/types.h>

namespace k3d { namespace filesystem { class path; } }

namespace module
{

namespace ngui
{

/// Implements all of the logic for file-change-notification.  Based on the current configuration, 
/// file-notification may be disabled altogether, implemented using timeout-based polling, or
/// implementing using glibmm threads.
class file_notification
{
public:
	file_notification();
	~file_notification();

	/// Start the file notification system.
	void start();
	k3d::uint_t watch_path(const k3d::filesystem::path& Path, const sigc::slot<void>& Slot);
	void unwatch_path(const k3d::uint_t WatchID);
	/// Stop the file notification system and cleanup resources.
	void stop();

private:
	void on_notify_file_changes();
	void on_notify_file_changes_threaded();
	void notify_file_change_thread_function();

	class implementation;
	implementation* const m_implementation;
};

} // namespace ngui

} // namespace module

#endif // !MODULES_NGUI_FILE_NOTIFICATION_H
