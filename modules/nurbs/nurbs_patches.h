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

/// Create a cap over a curve, if it is closed
void create_cap(k3d::mesh& Mesh, k3d::nurbs_patch::primitive& Patches, const k3d::mesh::points_t& CurvePoints, const k3d::nurbs_curve::const_primitive& Curves, const k3d::uint_t Curve, const k3d::point3& Centroid, const k3d::uint_t VSegments = 1);

/// Apply a modifier to the selected curves in OutputMesh, outputting to patches
template <typename FunctorT>
void selected_curves_to_patches(k3d::mesh& OutputMesh, FunctorT Modifier)
{
	const k3d::uint_t prim_count = OutputMesh.primitives.size();
	for(k3d::uint_t prim_idx = 0; prim_idx != prim_count; ++prim_idx)
	{
		boost::scoped_ptr<k3d::nurbs_curve::const_primitive> curves(k3d::nurbs_curve::validate(OutputMesh, *OutputMesh.primitives[prim_idx]));
		if(curves.get())
		{
			boost::scoped_ptr<k3d::nurbs_patch::primitive> patches(k3d::nurbs_patch::create(OutputMesh));
			for(k3d::uint_t curve = 0; curve != curves->curve_selections.size(); ++curve)
			{
				if(curves->curve_selections[curve])
					Modifier(OutputMesh, *curves, *patches, curve);
			}
		}
	}
}

} //namespace nurbs

} //namespace module

#endif // !MODULES_NURBS_NURBS_PATCHES_H
