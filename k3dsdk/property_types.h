#ifndef K3DSDK_PROPERTY_TYPES_H
#define K3DSDK_PROPERTY_TYPES_H

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

#include "color.h"
#include "imaterial.h"
#include "imesh_painter_gl.h"
#include "imesh_painter_ri.h"
#include "inode.h"
#include "itexture_ri.h"
#include "legacy_mesh.h"
#include "mesh.h"
#include "path.h"
#include "types.h"

#include <boost/mpl/vector/vector20.hpp>

namespace k3d
{

namespace property
{

/// Enumerates all of the types that can be stored using user properties.
typedef boost::mpl::vector18<
	k3d::bool_t,
	k3d::color,
	k3d::double_t,
	k3d::filesystem::path,
	k3d::gl::imesh_painter*,
	k3d::imaterial*,
	k3d::inode*,
	k3d::int32_t,
	k3d::legacy::mesh*,
	k3d::matrix4,
	k3d::mesh*,
	k3d::normal3,
	k3d::point3,
	k3d::point4,
	k3d::ri::imesh_painter*,
	k3d::ri::itexture*,
	k3d::string_t,
	k3d::vector3
	> types;

} // namespace property

} // namespace k3d

#endif // !K3DSDK_PROPERTY_TYPES_H

