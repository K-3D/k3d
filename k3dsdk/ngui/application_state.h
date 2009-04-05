#ifndef K3DSDK_NGUI_APPLICATION_STATE_H
#define K3DSDK_NGUI_APPLICATION_STATE_H

// K-3D
// Copyright (c) 1995-2004, Timothy M. Shead
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

#include <k3dsdk/data.h>
#include <set>

namespace Gtk { class Window; }

namespace k3d { namespace ngui { class unsaved_document; } }

namespace k3d
{

namespace ngui
{

/// This ugly little fellow stores global (application-wide) user interface state in one place to minimize coupling between UI components
class application_state
{
public:
	/// Singleton access to the global application_state object
	static application_state& instance();

	/// Connects a slot to a signal that will be called prior to safe shutdown.
	sigc::connection connect_safe_close_signal(const sigc::slot<k3d::ngui::unsaved_document*>& Slot);
	/// Attempts to safely close the application, saving unsaved documents and giving the user a chance to cancel.  Returns true iff the application will close.
	bool safe_close(Gtk::Window& Parent);

	/// Enables / disables use of user-customized UI layouts
	void enable_custom_layouts(const bool Enabled = true);
	/// Returns true if user-customized UI layouts are enabled
	const bool custom_layouts();

	/// Enables / disables interactive hotkey assignment
	void enable_hotkey_assignment(const bool Enabled = true);
	/// Returns true if dynamic hotkey assignment is enabled
	const bool assign_hotkeys();

private:
	application_state();
	application_state(const application_state&);
	application_state& operator=(const application_state&);
	~application_state();

	class implementation;
	implementation* const m_implementation;
};

} // namespace ngui

} // namespace k3d

#endif // !K3DSDK_NGUI_APPLICATION_STATE_H

