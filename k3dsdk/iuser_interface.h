#ifndef K3DSDK_IUSER_INTERFACE_H
#define K3DSDK_IUSER_INTERFACE_H

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

#include "ipath_property.h"
#include "iunknown.h"
#include "keyboard.h"
#include "signal_system.h"

#include <string>
#include <vector>

namespace k3d
{

namespace filesystem { class path; }

/// Abstract interface to common graphical-user-interface operations for use by objects
class iuser_interface :
	public virtual iunknown
{
public:
	/// Specifies batch-mode (no user intervention, good for executing scripts)
	virtual bool batch_mode() = 0;
	/// Displays a URL in the user's preferred web browser
	virtual void browser_navigate(const std::string& URL) = 0;
	/// Displays an informational message
	virtual void message(const std::string& Message) = 0;
	/// Displays a warning message
	virtual void warning_message(const std::string& Message) = 0;
	/// Displays an error message
	virtual void error_message(const std::string& Message) = 0;
	/**
		 \brief Prompts the user to choose one of several options
		 \param Message text to be displayed
		 \param DefaultOption one-based index of the option that is selected by default.  If DefaultOption is 0, no option is selected by default.
		 \return one-based index of the option selected by the user, or "0" if a choice was not made (e.g. user clicked WM "close" button)
	*/
	virtual unsigned int query_message(const std::string& Message, const unsigned int DefaultOption, const std::vector<std::string>& Options) = 0;

	/**
		\brief Displays a message (for tutorial purposes) in a dialog box
		\param Message the text to display in the dialog box
		\return true iff the user wants to continue the tutorial, false to quit
	*/
	virtual bool tutorial_message(const std::string& Message) = 0;

	/**
		\brief Prompts the user for a filepath, checking for old choices, and storing the current choice for reuse
		\param Prompt message to display in the file selection dialog
		\param OldPath initial file path to display in the file selection dialog
		\param Result returns the chosen file path
		\return true iff the user confirms the file path choice, false if they wish to cancel the pending operation
	*/
	virtual bool get_file_path(const ipath_property::mode_t Mode, const std::string& Type, const std::string& Prompt, const filesystem::path& OldPath, filesystem::path& Result) = 0;

	/// Displays the given object using a graphical user interface
	virtual bool show(iunknown& Object) = 0;

	/// Returns a connection to a signal that will be emitted at the requested frame rate (could return an empty connection, if the UI doesn't support timers)
	virtual sigc::connection get_timer(const double FrameRate, sigc::slot<void> Slot) = 0;

protected:
	iuser_interface() {}
	iuser_interface(const iuser_interface&) {}
	iuser_interface& operator = (const iuser_interface&) { return *this; }
	virtual ~iuser_interface() {}
};

} // namespace k3d

#endif // K3DSDK_IUSER_INTERFACE_H

