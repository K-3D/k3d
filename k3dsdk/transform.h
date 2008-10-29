#ifndef K3DSDK_TRANSFORM_H
#define K3DSDK_TRANSFORM_H

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

#include "algebra.h"

namespace k3d
{

class iunknown;
class point3;

/// Returns a node's position in world coordinates (returns the origin for non-transformable nodes)
const point3 world_position(iunknown& Node);
/// Returns a complete, concatenated, node-to-world transformation matrix for the given transformable node.  Returns an identity matrix for non-transformable node.
const matrix4 node_to_world_matrix(iunknown& Node);
/// Returns a complete, concatenated, parent-node-to-world transformation matrix for the given transformable node.  Returns an identity matrix for non-transformable nodes, or transformable nodes without a parent
const matrix4 parent_to_world_matrix(iunknown& Node);

/// Sets the transformation matrix (in world coordinates) for a node, automatically adjusts for the parentage of a node. Returns upstream transformation pointer when created, returns 0 if the existing one was updated.
k3d::inode* set_matrix(iunknown& Node, const k3d::matrix4& Matrix);

} // namespace k3d

#endif // !K3DSDK_TRANSFORM_H

