#ifndef K3DSDK_FRAMES_H
#define K3DSDK_FRAMES_H

// K-3D
// Copyright (c) 1995-2008, Timothy M. Shead
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
	\author Timothy M. Shead (tshead@k-3d.com)
*/

#include "path.h"

namespace k3d
{

/////////////////////////////////////////////////////////////////////////////
// frame

/// Encapsulates an animation frame (a range of time) to be rendered
class frame
{
public:
	frame();
	frame(const double_t BeginTime, const double_t EndTime);
	frame(const double_t BeginTime, const double_t EndTime, const filesystem::path& Destination);

	/// Stores the beginning of the frame
	double_t begin_time;
	/// Stores the end of the frame
	double_t end_time;
	/// Stores the filesystem location where the rendered frame should be stored
	filesystem::path destination;
};

std::ostream& operator<<(std::ostream& Stream, const frame& RHS);

/////////////////////////////////////////////////////////////////////////////
// frames

/// Stores a collection of animation frames to be rendered
typedef std::vector<frame> frames;

std::ostream& operator<<(std::ostream& Stream, const frames& RHS);

} // namespace k3d

#endif // !K3DSDK_FRAMES_H

