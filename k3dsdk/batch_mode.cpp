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

#include "log.h"
#include "result.h"
#include "batch_mode_init.h"
#include "batch_mode.h"

namespace k3d
{

namespace detail
{

/// Stores the global batch (no user intervention) mode state
bool g_batch_mode = false;
	
} // namespace detail
	
void set_batch_mode(const bool Mode)
{
	detail::g_batch_mode = Mode;
}

const bool batch_mode()
{
	return detail::g_batch_mode;
}

} // namespace k3d

