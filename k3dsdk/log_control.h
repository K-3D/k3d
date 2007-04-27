#ifndef K3DSDK_LOG_CONTROL_H
#define K3DSDK_LOG_CONTROL_H

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

#include "signal_system.h"

namespace k3d
{

/// Enumerates available log levels
typedef enum
{
	K3D_LOG_LEVEL_CRITICAL = 1,
	K3D_LOG_LEVEL_ERROR = 2,
	K3D_LOG_LEVEL_WARNING = 3,
	K3D_LOG_LEVEL_INFO = 4,
	K3D_LOG_LEVEL_DEBUG = 5
} log_level_t;

/// Enables logging of timestamps (default: off)
void log_show_timestamps(bool Enable);
/// Sets a "tag" that will be prepended to log messages (default: empty)
void log_set_tag(const std::string& Tag);
/// Enables color output based on log level (default: off)
void log_color_level(bool Enable);
/// Prepends the log level to messages (default: off)
void log_show_level(bool Enable);
/// Enables logging to the syslog on systems that support it (default: off)
void log_syslog(bool Enable);
/// Sets the minimum level of message to be logged (default: K3D_LOG_LEVEL_WARNING)
void log_minimum_level(const log_level_t Level);


/// Connects a slot to a signal that will be emitted whenever messages are added to the log 
sigc::connection connect_log_message(const sigc::slot<void, const time_t, const log_level_t, const std::string&>& Slot);
/// Retrieves the current contents of the log cache by repeatedly calling the given slot
void get_log_cache(const sigc::slot<void, const time_t, const log_level_t, const std::string&>& Slot);

} // namespace k3d

#endif // !K3DSDK_LOG_CONTROL_H

