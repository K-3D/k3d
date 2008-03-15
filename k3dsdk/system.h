#ifndef K3DSDK_SYSTEM
#define K3DSDK_SYSTEM

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
	\author Romain Behar (romainbehar@yahoo.com)
	\author Timothy M. Shead (tshead@k-3d.com)
*/

#include "path.h"
#include "types.h"

#include <vector>

namespace k3d
{

namespace system
{

/// Safely returns an environment variable (returns empty string if the variable doesn't exist)
const string_t getenv(const string_t& Variable);
/// Safely sets an environment variable using separate name and value strings
void setenv(const string_t& Name, const string_t& Value);
/// Safely sets an environment variable using "NAME=VALUE" syntax
void setenv(const string_t& Variable);

/// Safely returns the user's home directory
const filesystem::path get_home_directory();
/// Safely returns the user's temp directory
const filesystem::path get_temp_directory();
/// Returns a unique temporary file path
const filesystem::path generate_temp_file();

/// Returns the path to a binary executable by searching the contents of the PATH environment variable, or an empty path
const k3d::filesystem::path find_executable(const string_t& Executable);

/// Returns the most recent modification time of a file
bool file_modification_time(const filesystem::path& File, time_t& ModificationTime);

/// Runs an external process asynchronously.  Note: execs the process directly, do not use shell features!  The child process will have the same environment as its parent, and the PATH environment variable will be used to lookup the binary to be executed.
bool spawn_async(const string_t& CommandLine);
/// Runs an external process synchronously, blocking until it returns.  Note: execs the process directly, do not use shell features!  The child process will have the same environment as its parent, and the PATH environment variable will be used to lookup the binary to be executed.
bool spawn_sync(const string_t& CommandLine);

/// Defines a collection of paths
typedef std::vector<filesystem::path> paths_t;
/// Split a string containing zero-or-more paths separated by delimiters into a collection of paths ...
const paths_t decompose_path_list(const string_t Input);

/// Returns the correct name for an executable on the local platform
const string_t executable_name(const string_t& Executable);

/// Blocks the calling thread for the given number of seconds
void sleep(const double Seconds);

} // namespace system

} // namespace k3d

#endif // !K3DSDK_SYSTEM

