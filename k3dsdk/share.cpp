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
#include "share_detail.h"
#include "share.h"

namespace k3d
{

namespace detail
{

/// Stores the global shared-data directory
filesystem::path g_share_path;
	
} // namespace detail
	
void set_share_path(const filesystem::path& SharePath)
{
	return_if_fail(detail::g_share_path.empty());
	detail::g_share_path = SharePath;
}

const filesystem::path share_path()
{
	static bool nag = true;
	if(detail::g_share_path.empty() && nag)
	{
		k3d::log() << error << "share path must be set before use!" << std::endl;
		nag = false;
	}

	return detail::g_share_path;
}

} // namespace k3d

