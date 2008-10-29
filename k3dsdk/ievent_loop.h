#ifndef K3DSDK_IEVENT_LOOP_H
#define K3DSDK_IEVENT_LOOP_H

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

#include "iunknown.h"

#include <boost/program_options/parsers.hpp>

#include <string>
#include <vector>

namespace boost { namespace filesystem { class path; } }
namespace boost { namespace program_options { class options_description; } }

namespace k3d
{

/// Abstract interface for event loop (user interface) plugins
class ievent_loop :
	public virtual iunknown
{
public:
	virtual ~ievent_loop() {}

	/// Called by the host application to retrieve descriptions of command-line arguments for this plugin
	virtual void get_command_line_arguments(boost::program_options::options_description& Description) = 0;
	/// Defines storage for parsed command-line arguments
	typedef std::vector<boost::program_options::basic_option<char> > arguments_t;
	/// Called by the host application so the plugin can handle command-line arguments during startup
	virtual const arguments_t parse_startup_arguments(const arguments_t& Arguments, bool& Quit, bool& Error) = 0;
	/// Called by the host application to display informational messages during startup
	virtual void startup_message_handler(const std::string& Message) = 0;
	/// Called by the host application to display the "normal" user interface (and hide any startup / splash screens, etc)
	virtual void display_user_interface() = 0;
	/// Called by the host application so the plugin can handle command-line arguments after startup is complete
	virtual const arguments_t parse_runtime_arguments(const arguments_t& Arguments, bool& Quit, bool& Error) = 0;
	/// Called by the host application to start the main event-loop (if any)
	virtual void start_event_loop() = 0;
	/// Called by the host application to stop the main event-loop (if any)
	virtual void stop_event_loop() = 0;

protected:
	ievent_loop() {}
	ievent_loop(const ievent_loop&) {}
	ievent_loop& operator = (const ievent_loop&) { return *this; }
};

} // namespace k3d

#endif // !K3DSDK_IEVENT_LOOP_H

