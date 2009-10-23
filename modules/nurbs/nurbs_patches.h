#ifndef MODULES_NURBS_NURBS_PATCHES_H
#define MODULES_NURBS_NURBS_PATCHES_H

// K-3D
// Copyright (c) 1995-2009, Timothy M. Shead
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
  \author Carsten Haubold (CarstenHaubold@web.de)
	\author Bart Janssens (bart.janssens@lid.kviv.be)
*/

#include <k3dsdk/types.h>
#include <k3dsdk/mesh.h>
#include <k3dsdk/metadata.h>
#include <k3dsdk/metadata_keys.h>
#include <k3dsdk/nurbs_curve.h>
#include <k3dsdk/nurbs_patch.h>

#include <boost/scoped_ptr.hpp>
#include <stdexcept>

namespace module
{

namespace nurbs
{

/// Adds a patch with the given points, weights and knots
void add_patch(k3d::mesh& Mesh,
		k3d::nurbs_patch::primitive& NurbsPatches,
		const k3d::mesh::points_t& ControlPoints,
		const k3d::mesh::weights_t& Weights,
		const k3d::mesh::knots_t& UKnots,
		const k3d::mesh::knots_t& VKnots,
		const k3d::uint_t UOrder,
		const k3d::uint_t VOrder);

/// Copies a patch from one primitive to another
void add_patch(k3d::mesh& OutputMesh, k3d::nurbs_patch::primitive& OutputPatches, const k3d::mesh& InputMesh, const k3d::nurbs_patch::const_primitive& InputPatches, const k3d::uint_t Patch);

/// Create a cap over a curve, if it is closed
void create_cap(k3d::mesh& Mesh, k3d::nurbs_patch::primitive& Patches, const k3d::mesh::points_t& CurvePoints, const k3d::nurbs_curve::const_primitive& Curves, const k3d::uint_t Curve, const k3d::point3& Centroid, const k3d::uint_t VSegments = 1);

/// Traverse each selected curve in SourceCurves along each selected curve in CurvesToTraverse
void traverse_curve(const k3d::mesh& SourceCurves, const k3d::mesh& CurvesToTraverse, k3d::mesh& OutputMesh, const k3d::bool_t CreateCaps);

/// Traverse curves in the source mesh stored in the primitive given by SourcePrimIdx, storing the resulting patches in the given patch primitives
void traverse_curve(const k3d::mesh& SourceCurves, const k3d::uint_t SourcePrimIdx, const k3d::mesh& CurvesToTraverse, k3d::mesh& OutputMesh, k3d::nurbs_patch::primitive& OutputPatches, const k3d::bool_t CreateCaps);

/// Extract a curve from the given patch, appending it to the output structures.
void extract_patch_curve(k3d::mesh& OutputMesh, k3d::nurbs_curve::primitive& OutputCurve, const k3d::mesh& InputMesh, const k3d::nurbs_patch::const_primitive& InputPatches, const k3d::uint_t Patch, const k3d::uint_t Curve, const k3d::bool_t UDirection);

/// Elevate patch degree by the requested number of elevations
void elevate_patch_degree(const k3d::mesh& InputMesh, const k3d::nurbs_patch::const_primitive& InputPatches, k3d::mesh& OutputMesh, k3d::nurbs_patch::primitive& OutputPatches, const k3d::uint_t Patch, const k3d::uint_t Elevations, const k3d::bool_t UDirection);

/// Insert a knot in one directrion of the patch
void insert_knot(k3d::mesh& OutputMesh, k3d::nurbs_patch::primitive& OutputPatches, const k3d::mesh& InputMesh, const k3d::nurbs_patch::const_primitive& InputPatches, k3d::uint_t Patch, const k3d::double_t u, const k3d::uint_t r, const k3d::bool_t UDirection);

/// Splits the patch at the requested parameter value u
void split_patch(k3d::mesh& OutputMesh, k3d::nurbs_patch::primitive& OutputPatches, const k3d::mesh& InputMesh, const k3d::nurbs_patch::const_primitive& InputPatches, k3d::uint_t Patch, const k3d::double_t u, const k3d::bool_t UDirection);

} //namespace nurbs

} //namespace module

#endif // !MODULES_NURBS_NURBS_PATCHES_H
