#ifndef K3DSDK_HINTS_H
#define K3DSDK_HINTS_H

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
	\author Timothy M. Shead (tshead@k-3d.com)
*/

#include "algebra.h"
#include "ihint.h"
#include "mesh_selection.h"
#include "mesh.h"

#include <boost/any.hpp>

#include <iosfwd>

namespace k3d
{

class mesh_selection;
	
/// Namespace reserved for "hint" object that pass metadata about an upstream change to downstream nodes
namespace hint
{

/// Hint object that indicates that an object's selection state has changed
class selection_changed_t :
	public ihint
{
public:
	void print(std::ostream& Stream);
};

/// Convenience function that returns a reference to a static instance of selection_changed_t
selection_changed_t* selection_changed();

/// Hint object that indicates that a mesh's geometry (the locations of its points) has changed
class mesh_geometry_changed_t :
	public ihint
{
public:
	void print(std::ostream& Stream);
	/// Indices of the points affected by the change
	k3d::mesh::indices_t changed_points;
	/// Transformation matrix used for the change
	k3d::matrix4 transformation_matrix;
};

/// Convenience function that returns a reference to a static instance of mesh_geometry_changed_t
mesh_geometry_changed_t* mesh_geometry_changed();

/// Hint object that indicates that a mesh's topology has changed
class mesh_topology_changed_t :
	public ihint
{
public:
	void print(std::ostream& Stream);
};

/// Convenience function that returns a reference to a static instance of mesh_topology_changed_t
mesh_topology_changed_t* mesh_topology_changed();

/// Hint object that indicates a mesh was deleted
class mesh_deleted_t :
	public ihint
{
public:
	void print(std::ostream& Stream);
};

/// Convenience function that returns a reference to a static instance of mesh_deleted_t
mesh_deleted_t* mesh_deleted();

/// iostream-compatible manipulator object that serializes information about a hint object
class print
{
public:
	print(ihint* Hint);

	ihint* const hint;
};

/// Stream serialization
std::ostream& operator<<(std::ostream& Stream, const print& RHS);

} // namespace hint

} // namespace k3d

#endif // K3DSDK_HINTS_H

