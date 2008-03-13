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
#include "path.h"
#include "result.h"
#include "shader_cache_detail.h"
#include "shader_cache.h"

namespace k3d
{

namespace detail
{

/// Stores the global shader cache directory
filesystem::path g_shader_cache_path;

} // namespace detail

void set_shader_cache_path(const filesystem::path& ShaderCachePath)
{
	return_if_fail(detail::g_shader_cache_path.empty());
	detail::g_shader_cache_path = ShaderCachePath;
}

const filesystem::path shader_cache_path()
{
	if(detail::g_shader_cache_path.empty())
		k3d::log() << error << "shader cache path must be set before use!" << std::endl;

	k3d::filesystem::create_directories(detail::g_shader_cache_path);
	if(!k3d::filesystem::exists(detail::g_shader_cache_path))
		k3d::log() << error << "shader cache does not exist" << std::endl;

	return detail::g_shader_cache_path;
}

} // namespace k3d

