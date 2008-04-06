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

#include "frames.h"

#include <algorithm>
#include <iterator>

namespace k3d
{

///////////////////////////////////////////////////////////////////
// frame

frame::frame() :
	begin_time(0),
	end_time(0)
{
}

frame::frame(const double_t BeginTime, const double_t EndTime) :
	begin_time(BeginTime),
	end_time(EndTime)
{
}

frame::frame(const double_t BeginTime, const double_t EndTime, const filesystem::path& Destination) :
	begin_time(BeginTime),
	end_time(EndTime),
	destination(Destination)
{
}

std::ostream& operator<<(std::ostream& Stream, const frame& RHS)
{
	Stream << "frame: " << RHS.begin_time << " " << RHS.end_time << " " << RHS.destination.native_console_string();
	return Stream;
}

///////////////////////////////////////////////////////////////////
// frames

std::ostream& operator<<(std::ostream& Stream, const frames& RHS)
{
	std::copy(RHS.begin(), RHS.end(), std::ostream_iterator<frame>(Stream, "\n"));
	return Stream;
}

} // namespace k3d

