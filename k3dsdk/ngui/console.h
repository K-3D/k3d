#ifndef K3DSDK_NGUI_CONSOLE_H
#define K3DSDK_NGUI_CONSOLE_H

// K-3D
// Copyright (c) 1995-2006, Timothy M. Shead
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

#include "ui_component.h"

#include <k3dsdk/signal_system.h>
#include <k3dsdk/types.h>

#include <gtkmm/frame.h>

namespace Gtk { class TextTag; }

namespace k3d
{

namespace ngui
{

namespace console
{

/////////////////////////////////////////////////////////////////////////////
// control

/// Provides a "console" control that can be used for line-oriented output
class control :
        public Gtk::Frame
{
	typedef Gtk::Frame base;

public:
	control();
	~control();

	/// Clears the contents of the console.
	void clear();
	/// Sets the text formatting tag that will be applied to text passed to print_string() and prompt_string().
	void set_current_format(Glib::RefPtr<Gtk::TextTag>& Tag);
	/// Writes the supplied text to the console.
	void print_string(const string_t& String);
	/// Writes the supplied user prompt text to the console.
	void prompt_string(const string_t& String);

	/// Connects a slot to a signal that will be emitted whenever the user enters a command.
	sigc::connection connect_command_signal(const sigc::slot<void, const string_t&>& Slot);

private:
	class implementation;
	implementation* const m_implementation;
};

} // namespace console

} // namespace ngui

} // namespace k3d

#endif // !K3DSDK_NGUI_CONSOLE_H

