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

#include "bounding_box3.h"
#include "mesh.h"
#include "mesh_selection.h"
#include "normal3.h"
#include "point3.h"

namespace k3d
{

class imaterial;

/// Returns a mesh containing a topological "grid" of polygons with the given number of polys in each parametric direction
const mesh create_grid(const uint_t Rows, const uint_t Columns, imaterial* const Material = 0);
/// Returns a mesh containing a topological "cylinder" of polygons with the given number of polys in each parametric direction
const mesh create_cylinder(const uint_t Rows, const uint_t Columns, imaterial* const Material = 0);

/// Returns true iff every polyhedron in the given mesh is solid volume
const bool_t is_solid(const mesh& Mesh);
/// Returns true iff every face in the given mesh is a triangle
const bool_t is_triangles(const mesh& Mesh);

/// Copies the selection state of a mesh into a mesh_selection
void store_selection(const mesh& Mesh, mesh_selection& Selection);
/// Merges a mesh_selection with the current selection state in the given mesh
void merge_selection(const mesh_selection& MeshSelection, mesh& Mesh);
/// Merges a set of mesh_selection records with the current selection state in the given array
void merge_selection(const mesh_selection::records_t& Records, mesh::selection_t& Selection);

/// Returns a bounding-box containing every point in the given mesh
const bounding_box3 bounds(const mesh& Mesh);
/// Returns a bounding-box containing every point in the given collection
const bounding_box3 bounds(const mesh::points_t& Points);
/// Calculates the center (average) for an edge loop (returns the origin for degenerate cases)
const point3 center(const mesh::indices_t& EdgePoints, const mesh::indices_t& ClockwiseEdges, const mesh::points_t& Points, const uint_t EdgeIndex);
/// Calculates the normal for an edge loop (returns a zero-length normal for degenerate cases)
const normal3 normal(const mesh::indices_t& EdgePoints, const mesh::indices_t& ClockwiseEdges, const mesh::points_t& Points, const uint_t EdgeIndex);

/// Performs a deep-copy from one mesh to another (the new mesh doesn't share any memory with the old)
void deep_copy(const mesh& From, mesh& To);

/// Performs sanity-checking on a mesh, validating all constraints - returns true iff the mesh is valid
const bool_t validate(mesh& Mesh);

/// Returns true iff the given mesh contains valid point data (i.e. both point and point_selection arrays are defined)
const bool_t validate_points(const mesh& Mesh);
/// Returns true iff the given mesh contains valid point group data (i.e. every array is defined)
const bool_t validate_point_groups(const mesh& Mesh);
/// Returns true iff the given mesh contains valid linear curve group data (i.e. every array is defined)
const bool_t validate_linear_curve_groups(const mesh& Mesh);
/// Returns true iff the given mesh contains valid cubic curve group data (i.e. every array is defined)
const bool_t validate_cubic_curve_groups(const mesh& Mesh);
/// Returns true iff the given mesh contains valid nurbs curve group data (i.e. every array is defined)
const bool_t validate_nurbs_curve_groups(const mesh& Mesh);
/// Returns true iff the given mesh contains valid bilinear patch data (i.e. every array is defined)
const bool_t validate_bilinear_patches(const mesh& Mesh);
/// Returns true iff the given mesh contains valid bicubic patch data (i.e. every array is defined)
const bool_t validate_bicubic_patches(const mesh& Mesh);
/// Returns true iff the given mesh contains valid nurbs patch data (i.e. every array is defined)
const bool_t validate_nurbs_patches(const mesh& Mesh);
/// Returns true iff the given mesh contains valid polyhedron data (i.e. every array is defined)
const bool_t validate_polyhedra(const mesh& Mesh);
/// Returns true iff the given mesh contains valid blobby data (i.e. every array is defined)
const bool_t validate_blobbies(const mesh& Mesh);

/// Returns true iff the given mesh should be rendered as SDS
const bool_t is_sds(const mesh& Mesh);

} // namespace k3d

#endif // K3DSDK_MESH_OPERATIONS_H

