#ifndef NGUI_ASYNCHRONOUS_UPDATE_H
#define NGUI_ASYNCHRONOUS_UPDATE_H

// K-3D
// Copyright (c) 1995-2005, Timothy M. Shead
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

#include <k3dsdk/signal_system.h>

namespace libk3dngui
{

/////////////////////////////////////////////////////////////////////////////
// asynchronous_update

/// Mixin class that can be used to schedule updates that will only occur when the UI is idle
class asynchronous_update :
	public virtual sigc::trackable
{
public:
	asynchronous_update();
	virtual ~asynchronous_update();

	/// Call this to schedule an update that will occur when the UI is idle ... multiple calls will result in only one update.
	void schedule_update();
	/// Call this to block any pending updates so that on_update() won't be called
	void block_updates();
	/// Call this to unblock updates so that on_update() will be called normally once again.  note: any updates that were pending when block_updates() was called are lost
	void unblock_updates();

	
	/// Implement this in your derived class to handle updates when the UI is idle.
	virtual void on_update() = 0;

private:
	/// Called by the signal system when the UI is idle
	bool on_idle();
	/// Used to track pending idle events
	sigc::connection m_idle_connection;
	/// If true, calls to on_update() are disabled
	bool m_block_updates;
};

} // namespace libk3dngui

#endif // !NGUI_ASYNCHRONOUS_UPDATE_H

