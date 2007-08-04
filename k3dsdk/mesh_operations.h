#ifndef K3DSDK_MESH_OPERATIONS_H
#define K3DSDK_MESH_OPERATIONS_H

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

#include "mesh.h"

namespace k3d
{

class imaterial;

/// Returns a mesh containing a topological "grid" of polygons with the given number of polys in each parametric direction
const mesh create_grid(const size_t Rows, const size_t Columns, imaterial* const Material = 0);
/// Returns a mesh containing a topological "cylinder" of polygons with the given number of polys in each parametric direction
const mesh create_cylinder(const size_t Rows, const size_t Columns, imaterial* const Material = 0);

/// Returns true iff the given mesh is composed of solid volumes
const bool is_solid(const mesh& Mesh);

} // namespace k3d

#endif // K3DSDK_MESH_OPERATIONS_H

