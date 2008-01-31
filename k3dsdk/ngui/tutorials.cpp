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
	\author Tim Shead (tshead@k-3d.com)
*/

#include "tutorials.h"
#include <algorithm>

namespace k3d
{

namespace ngui
{

namespace tutorial
{

namespace detail
{

static int recording_count = 0;
static int playback_count = 0;

} // namespace detail

void recording_started()
{
	++detail::recording_count;
}

const bool recording()
{
	return detail::recording_count > 0;
}

void recording_finished()
{
	detail::recording_count = std::max(0, detail::recording_count - 1);
}

void playback_started()
{
	++detail::playback_count;
}

const bool playing()
{
	return detail::playback_count > 0;
}

void playback_finished()
{
	detail::playback_count = std::max(0, detail::playback_count - 1);
}

} // namespace tutorial

} // namespace ngui

} // namespace k3d

