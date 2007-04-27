// K-3D
// Copyright (c) 1995-2004, Timothy M. Shead
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
		\brief Declares helpful functions for geometry readers/writers
		\author Romain Behar (romainbehar@yahoo.com)
*/

#include <k3dsdk/idocument.h>
#include <k3dsdk/inode_collection.h>
#include <k3dsdk/legacy_mesh.h>

#include <map>

namespace libk3dgeometry
{

namespace detail
{

/// Creates a new mesh and its instance
k3d::legacy::mesh* create_mesh(k3d::idocument& Document, const std::string Name,
	k3d::inode*& ReturnFrozenMesh, k3d::inode*& ReturnMeshInstance);

/// Get meshes with their object from MeshInstances
typedef std::map<k3d::legacy::mesh*, k3d::inode*> mesh_instances_t;
void get_mesh_instances(k3d::idocument& Document, mesh_instances_t& MeshInstances);

/// Returns true iff mesh contains only triangles
bool triangle_test(const k3d::legacy::mesh& Mesh);

} // namespace detail

} // namespace libk3dgeometry

