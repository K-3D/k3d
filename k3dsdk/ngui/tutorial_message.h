#ifndef K3DSDK_NGUI_TUTORIAL_MESSAGE_H
#define K3DSDK_NGUI_TUTORIAL_MESSAGE_H

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

/** \file
		\brief Declares functions for displaying a "Tutorial Message" dialog that prompts the user and provides feedback during interactive tutorial playback
		\author Tim Shead (tshead@k-3d.com)
*/

#include <k3dsdk/iuser_interface.h>
#include <k3dsdk/signal_system.h>

namespace k3d
{

namespace ngui
{

/// Singleton object that manages the global tutorial-message state
class tutorial_message
{
public:
	static tutorial_message& instance();

	/// Signals observers to display an interactive tutorial message
	bool show_message(const std::string& Message);
	/// Signals observers to hide any visible tutorial messages
	void hide_messages();
	/// Cancels execution of an interactive tutorial
	void cancel_message();
	/// Continues execution of an interactive tutorial
	void continue_message();

	/// Returns a connection to a signal that will be emitted to display a tutorial message
	sigc::connection connect_show_message_signal(const sigc::slot<void, const std::string&>& Slot);
	/// Returns a connection to a signal that will be emitted to move the mouse pointer over a tutorial message
	sigc::connection connect_move_pointer_signal(const sigc::slot<void>& Slot);
	/// Returns a connection to a signal that will be emitted to wait for user acknowledgement of a message
	sigc::connection connect_wait_signal(const sigc::slot<void>& Slot);
	/// Returns a connection to a signal that will be emitted to indicate user acknowledgement of a message
	sigc::connection connect_acknowledge_signal(const sigc::slot<void>& Slot);
	/// Returns a connection to a signal that will be emitted to hide any visible tutorial messages
	sigc::connection connect_hide_messages_signal(const sigc::slot<void>& Slot);
	
private:
	tutorial_message();
	tutorial_message(const tutorial_message&);
	tutorial_message& operator=(const tutorial_message&);
	~tutorial_message();
	
	class implementation;
	implementation* const m_implementation;
};

} // namespace ngui

} // namespace k3d

#endif // !K3DSDK_NGUI_TUTORIAL_MESSAGE_H

