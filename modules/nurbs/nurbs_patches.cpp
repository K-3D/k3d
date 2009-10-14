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

#include "nurbs_patches.h"

namespace module
{

namespace nurbs
{

void add_patch(k3d::mesh& Mesh,
		k3d::nurbs_patch::primitive& NurbsPatches,
		const k3d::mesh::points_t& ControlPoints,
		const k3d::mesh::weights_t& Weights,
		const k3d::mesh::knots_t& UKnots,
		const k3d::mesh::knots_t& VKnots,
		const k3d::uint_t UOrder,
		const k3d::uint_t VOrder)
{
	// Sanity checking ...
	return_if_fail(Mesh.points);
	return_if_fail(Mesh.point_selection);

	return_if_fail(ControlPoints.size() == Weights.size());

	return_if_fail(UOrder >= 2);
	return_if_fail(VOrder >= 2);

	k3d::mesh::points_t& points = Mesh.points.writable();
	k3d::mesh::selection_t& point_selection = Mesh.point_selection.writable();

	NurbsPatches.patch_first_points.push_back(NurbsPatches.patch_points.size());

	for(k3d::uint_t i = 0; i != ControlPoints.size(); ++i)
	{
		NurbsPatches.patch_points.push_back(points.size());
		points.push_back(ControlPoints[i]);
	}
	point_selection.resize(points.size(), 0.0);

	NurbsPatches.patch_selections.push_back(0.0);
	NurbsPatches.patch_u_first_knots.push_back(NurbsPatches.patch_u_knots.size());
	NurbsPatches.patch_v_first_knots.push_back(NurbsPatches.patch_v_knots.size());
	NurbsPatches.patch_u_orders.push_back(UOrder);
	NurbsPatches.patch_v_orders.push_back(VOrder);
	NurbsPatches.patch_u_point_counts.push_back(UKnots.size() - UOrder);
	NurbsPatches.patch_v_point_counts.push_back(VKnots.size() - VOrder);
	NurbsPatches.patch_trim_loop_counts.push_back(0);
	NurbsPatches.patch_first_trim_loops.push_back(0);
	NurbsPatches.patch_point_weights.insert(NurbsPatches.patch_point_weights.end(), Weights.begin(), Weights.end());
	NurbsPatches.patch_u_knots.insert(NurbsPatches.patch_u_knots.end(), UKnots.begin(), UKnots.end());
	NurbsPatches.patch_v_knots.insert(NurbsPatches.patch_v_knots.end(), VKnots.begin(), VKnots.end());
}

void create_cap(k3d::mesh& Mesh, k3d::nurbs_patch::primitive& Patches, const k3d::mesh::points_t& CurvePoints, const k3d::nurbs_curve::const_primitive& Curves, const k3d::uint_t Curve, const k3d::point3& Centroid, const k3d::uint_t VSegments)
{
	return_if_fail(Curve < Curves.curve_first_points.size());
	k3d::mesh::points_t control_points;
	k3d::mesh::weights_t weights;
	k3d::mesh::knots_t u_knots;
	k3d::mesh::knots_t v_knots;

	const k3d::uint_t curve_knots_begin = Curves.curve_first_knots[Curve];
	const k3d::uint_t curve_knots_end = curve_knots_begin + Curves.curve_point_counts[Curve] + Curves.curve_orders[Curve];

	u_knots.assign(Curves.curve_knots.begin() + curve_knots_begin, Curves.curve_knots.begin() + curve_knots_end);

	const k3d::uint_t curve_points_begin = Curves.curve_first_points[Curve];
	const k3d::uint_t curve_points_end = curve_points_begin + Curves.curve_point_counts[Curve];

	// Distance from each point to the centroid
	std::vector<k3d::double_t> radii;
	for(k3d::uint_t i = curve_points_begin; i != curve_points_end; ++i)
	{
		radii.push_back((CurvePoints[Curves.curve_points[i]] - Centroid).length());
	}

	for(k3d::uint_t i = 0; i <= VSegments; ++i)
	{
		for(k3d::uint_t j = curve_points_begin; j != curve_points_end; ++j)
		{
			const k3d::double_t scale_factor = static_cast<k3d::double_t>(i) / static_cast<k3d::double_t>(VSegments);
			const k3d::point3 control_point(Centroid + (scale_factor * (CurvePoints[Curves.curve_points[j]] - Centroid)));
			control_points.push_back(control_point);
			weights.push_back(Curves.curve_point_weights[j]);
		}
	}

	v_knots.push_back(0);
	v_knots.push_back(0);
	for(k3d::int32_t i = 1; i != VSegments; ++i)
		v_knots.push_back(i);
	v_knots.push_back(VSegments);
	v_knots.push_back(VSegments);

	add_patch(Mesh, Patches, control_points, weights, u_knots, v_knots, Curves.curve_orders[Curve], 2);
}

} //namespace nurbs

} //namespace module
