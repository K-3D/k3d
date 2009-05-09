#ifndef K3DSDK_EXTENSION_GL_H
#define K3DSDK_EXTENSION_GL_H

// K-3D
// Copyright (c) 1995-2007, Timothy M. Shead
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

#include "types.h"
#include <set>

namespace k3d
{

namespace gl
{

namespace extension
{

/// "Disables" an OpenGL extension, so that query() will always return false (regardless of the underlying hardware)
void disable(const k3d::string_t& Extension);
/// "Enables" an OpenGL extension, so that query() will always return true (regardless of the underlying hardware)
void enable(const k3d::string_t& Extension);
/// Returns the list of OpenGL extensions supported by underlying hardware
const std::set<k3d::string_t> list();

/// Returns true iff the requested OpenGL extension is available
bool_t query(const k3d::string_t& Extension);

/// Convenience function that returns true iff the VBO extension is supported
bool_t query_vbo();

} // namespace extension

} // namespace gl

} // namespace k3d

#endif // !K3DSDK_EXTENSION_GL_H

