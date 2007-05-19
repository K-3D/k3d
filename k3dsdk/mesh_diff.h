#ifndef K3DSDK_MESH_DIFF_H
#define K3DSDK_MESH_DIFF_H

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
#include <boost/cstdint.hpp>
#include <iosfwd>

namespace k3d
{

/// Returns true iff two point groups are equivalent (handles fuzzy floating-point comparisons)
const bool equal(const mesh::point_groups_t& A, const mesh::point_groups_t& B, const boost::uint64_t Threshold);
/// Returns true iff two lineary curve groups are equivalent (handles fuzzy floating-point comparisons)
const bool equal(const mesh::linear_curve_groups_t& A, const mesh::linear_curve_groups_t& B, const boost::uint64_t Threshold);
/// Returns true iff two cubic curve groups are equivalent (handles fuzzy floating-point comparisons)
const bool equal(const mesh::cubic_curve_groups_t& A, const mesh::cubic_curve_groups_t& B, const boost::uint64_t Threshold);
/// Returns true iff two nurbs curve groups are equivalent (handles fuzzy floating-point comparisons)
const bool equal(const mesh::nurbs_curve_groups_t& A, const mesh::nurbs_curve_groups_t& B, const boost::uint64_t Threshold);
/// Returns true iff two sets of bilinear patches are equivalent (handles fuzzy floating-point comparisons)
const bool equal(const mesh::bilinear_patches_t& A, const mesh::bilinear_patches_t& B, const boost::uint64_t Threshold);
/// Returns true iff two sets of bicubic patches are equivalent (handles fuzzy floating-point comparisons)
const bool equal(const mesh::bicubic_patches_t& A, const mesh::bicubic_patches_t& B, const boost::uint64_t Threshold);
/// Returns true iff two sets of nurbs are equivalent (handles fuzzy floating-point comparisons)
const bool equal(const mesh::nurbs_patches_t& A, const mesh::nurbs_patches_t& B, const boost::uint64_t Threshold);
/// Returns true iff two sets of polyhedra are equivalent (handles fuzzy floating-point comparisons)
const bool equal(const mesh::polyhedra_t& A, const mesh::polyhedra_t& B, const boost::uint64_t Threshold);
/// Returns true iff two sets of blobbies are equivalent (handles fuzzy floating-point comparisons)
const bool equal(const mesh::blobbies_t& A, const mesh::blobbies_t& B, const boost::uint64_t Threshold);
/// Returns true iff two meshes are equivalent (handles fuzzy floating-point comparisons)
const bool equal(const mesh& A, const mesh& B, const boost::uint64_t Threshold);
/// Prints the difference between two meshes to a stream
void print_diff(std::ostream& Stream, const mesh& A, const mesh& B, const boost::uint64_t Threshold);

} // namespace k3d

#endif // K3DSDK_MESH_DIFF_H

