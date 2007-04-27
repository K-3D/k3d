#ifndef K3DSDK_LOG_H
#define K3DSDK_LOG_H

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

#include <iosfwd>

namespace k3d
{

/// Returns an output stream to be used for logging
std::ostream& log();

/// iostream manipulator that indicates a critical error has occurred, from which the application cannot recover
std::ostream& critical(std::ostream&);
/// iostream manipulator that indicates definite invalid input or unexpected behavior - the current operation will not succeed
std::ostream& error(std::ostream&);
/// iostream manipulator that indicates possible invalid input or incorrect behavior - the current operation may not succeed
std::ostream& warning(std::ostream&);
/// iostream manipulator that indicates an optional, informational message
std::ostream& info(std::ostream&);
/// iostream manipulator that indicates messages used for internal debugging by developers
std::ostream& debug(std::ostream&);

/// iostream manipulator that inserts a copy of the current backtrace into a stream
std::ostream& backtrace(std::ostream&);

/// Convenience function that prints a debug message to the log stream ... intended mainly for use with signals
void print_debug(const std::string& Message);
	
} // namespace k3d

using k3d::critical;
using k3d::error;
using k3d::warning;
using k3d::info;
using k3d::debug;

#define k3d_file_reference __FILE__ << " line " << __LINE__ 

#endif // !K3DSDK_LOG_H

