#ifndef K3DSDK_THREADS_H
#define K3DSDK_THREADS_H

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

#include <k3dsdk/types.h>

namespace k3d
{

namespace parallel
{

enum
{
	/// Set the number of threads to be used for parallel operations automatically (e.g: based on the number of hardware threads)
	automatic = -1
};

/// Set the number of threads to be used for parallel operations
void set_thread_count(const int32_t Count);
/// Set the preferred grainsize to be used for parallel operations
void set_grain_size(const uint_t GrainSize);
/// Get the preferred grainsize to be used for parallel operations
const uint_t grain_size();

} // namespace parallel

} // namespace k3d

#endif // K3DSDK_THREADS_H

