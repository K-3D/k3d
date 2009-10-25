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

#include "nurbs_curves.h"
#include "nurbs_patches.h"

#include <k3dsdk/axis.h>
#include <k3dsdk/nurbs_curve.h>
#include <k3dsdk/table_copier.h>

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

void add_patch(k3d::mesh& OutputMesh, k3d::nurbs_patch::primitive& OutputPatches, const k3d::mesh& InputMesh, const k3d::nurbs_patch::const_primitive& InputPatches, const k3d::uint_t Patch)
{
	k3d::mesh::points_t points;
	k3d::mesh::weights_t weights;
	const k3d::uint_t u_order = InputPatches.patch_u_orders[Patch];
	const k3d::uint_t v_order = InputPatches.patch_v_orders[Patch];
	const k3d::uint_t points_begin = InputPatches.patch_first_points[Patch];
	const k3d::uint_t u_point_count = InputPatches.patch_u_point_counts[Patch];
	const k3d::uint_t v_point_count = InputPatches.patch_v_point_counts[Patch];
	const k3d::uint_t points_end = points_begin + u_point_count*v_point_count;
	for(k3d::uint_t i = 0; i != points_end; ++i)
	{
		points.push_back(InputMesh.points->at(InputPatches.patch_points[i]));
		weights.push_back(InputPatches.patch_point_weights[i]);
	}
	const k3d::uint_t u_knots_begin = InputPatches.patch_u_first_knots[Patch];
	const k3d::uint_t u_knots_end = u_knots_begin + u_order + u_point_count;
	k3d::mesh::knots_t u_knots(InputPatches.patch_u_knots.begin() + u_knots_begin, InputPatches.patch_u_knots.begin() + u_knots_end);
	const k3d::uint_t v_knots_begin = InputPatches.patch_v_first_knots[Patch];
	const k3d::uint_t v_knots_end = v_knots_begin + v_order + v_point_count;
	k3d::mesh::knots_t v_knots(InputPatches.patch_v_knots.begin() + v_knots_begin, InputPatches.patch_v_knots.begin() + v_knots_end);
	add_patch(OutputMesh, OutputPatches, points, weights, u_knots, v_knots, u_order, v_order);
	OutputPatches.patch_materials.push_back(InputPatches.patch_materials[Patch]);
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

void traverse_curve(const k3d::mesh& SourceCurves, const k3d::uint_t SourcePrimIdx, const k3d::mesh& CurvesToTraverse, k3d::mesh& OutputMesh, k3d::nurbs_patch::primitive& OutputPatches, const k3d::bool_t CreateCaps)
{
	boost::scoped_ptr<k3d::nurbs_curve::const_primitive> source_curves(k3d::nurbs_curve::validate(SourceCurves, *SourceCurves.primitives[SourcePrimIdx]));
	return_if_fail(source_curves);
	OutputPatches.patch_attributes = source_curves->curve_attributes.clone_types();
	k3d::table_copier uniform_copier(source_curves->curve_attributes, OutputPatches.patch_attributes);

	// Get the loops that exist in the source_curves primitive
	k3d::mesh::bools_t is_in_loop, loop_selections;
	k3d::mesh::indices_t loop_first_curves, next_curves, curve_loops;
	k3d::mesh::points_t loop_centroids;
	if(CreateCaps)
	{
		find_loops(*SourceCurves.points, *source_curves, is_in_loop, loop_first_curves, curve_loops, next_curves, loop_selections, loop_centroids);
	}

	for(k3d::mesh::primitives_t::const_iterator trav_primitive = CurvesToTraverse.primitives.begin(); trav_primitive != CurvesToTraverse.primitives.end(); ++trav_primitive)
	{
		boost::scoped_ptr<k3d::nurbs_curve::const_primitive> curves_to_traverse(k3d::nurbs_curve::validate(CurvesToTraverse, **trav_primitive));
		if(!curves_to_traverse)
			continue;

		const k3d::uint_t source_curves_begin = 0;
		const k3d::uint_t source_curves_end = source_curves->curve_first_knots.size();
		for(k3d::uint_t source_curve = source_curves_begin; source_curve != source_curves_end; ++source_curve)
		{
			if(!source_curves->curve_selections[source_curve])
				continue;
			const k3d::uint_t curves_to_traverse_begin = 0;
			const k3d::uint_t curves_to_traverse_end = curves_to_traverse->curve_first_knots.size();
			for(k3d::uint_t curve_to_traverse = curves_to_traverse_begin; curve_to_traverse != curves_to_traverse_end; ++curve_to_traverse)
			{
				if(!curves_to_traverse->curve_selections[curve_to_traverse])
					continue;

				//move the 1st curve along the 2nd
				const k3d::uint_t curve_points_begin[2] = {source_curves->curve_first_points[source_curve], curves_to_traverse->curve_first_points[curve_to_traverse]};
				const k3d::uint_t curve_points_end[2] = { curve_points_begin[0] + source_curves->curve_point_counts[source_curve], curve_points_begin[1] + curves_to_traverse->curve_point_counts[curve_to_traverse] };

				const k3d::uint_t curve_knots_begin[2] = { source_curves->curve_first_knots[source_curve], curves_to_traverse->curve_first_knots[curve_to_traverse]};
				const k3d::uint_t curve_knots_end[2] = { curve_knots_begin[0] + source_curves->curve_orders[source_curve] + source_curves->curve_point_counts[source_curve], curve_knots_begin[1] + curves_to_traverse->curve_orders[curve_to_traverse] + curves_to_traverse->curve_point_counts[curve_to_traverse]};

				k3d::mesh::points_t new_points;
				k3d::mesh::weights_t new_weights;

				k3d::mesh::knots_t u_knots;
				u_knots.insert(u_knots.end(), source_curves->curve_knots.begin() + curve_knots_begin[0], source_curves->curve_knots.begin() + curve_knots_end[0]);

				k3d::mesh::knots_t v_knots;
				v_knots.insert(v_knots.end(), curves_to_traverse->curve_knots.begin() + curve_knots_begin[1], curves_to_traverse->curve_knots.begin() + curve_knots_end[1]);

				k3d::uint_t u_order = source_curves->curve_orders[source_curve];
				k3d::uint_t v_order = curves_to_traverse->curve_orders[curve_to_traverse];

				const k3d::uint_t point_count = source_curves->curve_point_counts[source_curve];
				const k3d::mesh::points_t& source_mesh_points = *SourceCurves.points;
				const k3d::mesh::points_t& traverse_mesh_points = *CurvesToTraverse.points;

				for (int i = 0; i < curves_to_traverse->curve_point_counts[curve_to_traverse]; i++)
				{
					k3d::vector3 delta_u = traverse_mesh_points[curves_to_traverse->curve_points[curve_points_begin[1] + i]] - traverse_mesh_points[curves_to_traverse->curve_points[curve_points_begin[1]]];
					double w_u = curves_to_traverse->curve_point_weights[curve_points_begin[1] + i];

					for (int j = 0; j < point_count; j++)
					{
						k3d::point3 p_v = source_mesh_points[source_curves->curve_points[curve_points_begin[0] + j]];
						double w_v = source_curves->curve_point_weights[curve_points_begin[0] + j];

						new_points.push_back(p_v + delta_u);
						new_weights.push_back(w_u * w_v);
					}
				}
				add_patch(OutputMesh, OutputPatches, new_points, new_weights, u_knots, v_knots, source_curves->curve_orders[source_curve], curves_to_traverse->curve_orders[curve_to_traverse]);
				uniform_copier.push_back(source_curve);
				OutputPatches.patch_materials.push_back(source_curves->material[0]);
				if(CreateCaps)
				{
					const k3d::point3& centroid = is_in_loop[source_curve] ? loop_centroids[curve_loops[source_curve]] : module::nurbs::centroid(source_mesh_points, *source_curves, source_curve);
					// Cap over the original curve
					create_cap(OutputMesh, OutputPatches, *SourceCurves.points, *source_curves, source_curve, centroid);
					uniform_copier.push_back(source_curve);
					OutputPatches.patch_materials.push_back(source_curves->material[0]);

					// Cap over the opposite curve
					k3d::mesh tempmesh(SourceCurves);
					boost::scoped_ptr<k3d::nurbs_curve::primitive> tempcurve(k3d::nurbs_curve::validate(tempmesh, tempmesh.primitives[SourcePrimIdx]));
					return_if_fail(tempcurve); // shouldn't happen

					k3d::vector3 delta_u = traverse_mesh_points[curves_to_traverse->curve_points[curve_points_end[1] - 1]] - traverse_mesh_points[curves_to_traverse->curve_points[curve_points_begin[1]]];
					k3d::double_t weight = curves_to_traverse->curve_point_weights[curve_points_end[1] - 1];

					k3d::mesh::points_t& temp_points = tempmesh.points.writable();
					for (int i = curve_points_begin[0]; i != curve_points_end[0]; i++)
					{
						tempcurve->curve_point_weights[i] *= weight;
					}
					for(k3d::uint_t point = 0; point != temp_points.size(); ++point)
					{
						temp_points[point] += delta_u;
					}

					boost::scoped_ptr<k3d::nurbs_curve::const_primitive> const_tempcurve(k3d::nurbs_curve::validate(tempmesh, *tempmesh.primitives[SourcePrimIdx]));
					create_cap(OutputMesh, OutputPatches, temp_points, *const_tempcurve, source_curve, centroid + delta_u);
					uniform_copier.push_back(source_curve);
					OutputPatches.patch_materials.push_back(source_curves->material[0]);
				}
			}
		}
	}
}

void traverse_curve(const k3d::mesh& SourceCurves, const k3d::mesh& CurvesToTraverse, k3d::mesh& OutputMesh, const k3d::bool_t CreateCaps)
{
	for(k3d::uint_t prim_idx = 0; prim_idx != SourceCurves.primitives.size(); ++prim_idx)
	{
		boost::scoped_ptr<k3d::nurbs_curve::const_primitive> source_curves(k3d::nurbs_curve::validate(SourceCurves, *SourceCurves.primitives[prim_idx]));
		if(!source_curves)
			continue;

		boost::scoped_ptr<k3d::nurbs_patch::primitive> output_patches(k3d::nurbs_patch::create(OutputMesh));
		traverse_curve(SourceCurves, prim_idx, CurvesToTraverse, OutputMesh, *output_patches, CreateCaps);
	}
}

void extract_patch_curve(k3d::mesh& OutputMesh, k3d::nurbs_curve::primitive& OutputCurve, const k3d::mesh& InputMesh, const k3d::nurbs_patch::const_primitive& InputPatches, const k3d::uint_t Patch, const k3d::uint_t Curve, const k3d::bool_t UDirection)
{
	const k3d::uint_t curve_count = UDirection ? InputPatches.patch_v_point_counts[Patch] : InputPatches.patch_u_point_counts[Patch];
	const k3d::uint_t curve_point_count = UDirection ? InputPatches.patch_u_point_counts[Patch] : InputPatches.patch_v_point_counts[Patch];
	return_if_fail(Curve < curve_count);

	const k3d::uint_t point_step = UDirection ? 1 : curve_count;
	const k3d::uint_t points_begin = InputPatches.patch_first_points[Patch] + Curve * (UDirection ? curve_point_count : 1);
	const k3d::uint_t points_end = points_begin + point_step * curve_point_count;

	const k3d::mesh::knots_t& input_knots = UDirection ? InputPatches.patch_u_knots : InputPatches.patch_v_knots;
	const k3d::uint_t order = UDirection ? InputPatches.patch_u_orders[Patch] : InputPatches.patch_v_orders[Patch];
	const k3d::uint_t knot_count = curve_point_count + order;
	const k3d::uint_t knots_begin = UDirection ? InputPatches.patch_u_first_knots[Patch] : InputPatches.patch_v_first_knots[Patch];
	const k3d::uint_t knots_end = knots_begin + knot_count;
	k3d::mesh::knots_t knots(input_knots.begin() + knots_begin, input_knots.begin() + knots_end);
	k3d::mesh::points_t points;
	k3d::mesh::weights_t weights;

	for (int i = points_begin; i < points_end; i += point_step)
	{
		points.push_back(InputMesh.points->at(InputPatches.patch_points[i]));
		weights.push_back(InputPatches.patch_point_weights[i]);
	}

	k3d::nurbs_curve::add_curve(OutputMesh, OutputCurve, order, points, weights, knots);
}

// stores all curves of a patch in one direction in a curve primitive
void extract_curves(k3d::mesh& OutputMesh, k3d::nurbs_curve::primitive& OutputCurves, const k3d::mesh& InputMesh, const k3d::nurbs_patch::const_primitive& InputPatches, const k3d::uint_t Patch, const k3d::bool_t UDirection)
{
	OutputMesh.points.create();
	OutputMesh.point_selection.create();
	const k3d::uint_t curve_count = UDirection ? InputPatches.patch_v_point_counts[Patch] : InputPatches.patch_u_point_counts[Patch];
	for(k3d::uint_t curve = 0; curve != curve_count; ++curve)
	{
		extract_patch_curve(OutputMesh, OutputCurves, InputMesh, InputPatches, Patch, curve, UDirection);
	}
}

// Convert a series of curves back to a patch
void curves_to_patch(k3d::mesh& OutputMesh, k3d::nurbs_patch::primitive& OutputPatches, const k3d::mesh& InputMesh, const k3d::nurbs_curve::primitive& InputCurves, const k3d::nurbs_patch::const_primitive& InputPatches, const k3d::uint_t Patch, const k3d::bool_t UDirection)
{
	const k3d::uint_t u_order = UDirection ? InputCurves.curve_orders.front() : InputPatches.patch_u_orders[Patch];
	const k3d::uint_t v_order = UDirection ? InputPatches.patch_v_orders[Patch] : InputCurves.curve_orders.front();
	const k3d::uint_t curve_count = InputCurves.curve_first_points.size();

	if(UDirection)
	{
		// Because of the way elevate_curve_degree (and ultimately add_curve) works, the points are already ordered correctly in the U case
		k3d::mesh::points_t points(InputMesh.points->begin() + InputCurves.curve_points[InputCurves.curve_first_points.front()], InputMesh.points->end());
		k3d::mesh::knots_t u_knots(InputCurves.curve_knots.begin(), InputCurves.curve_knots.begin() + InputCurves.curve_point_counts.front() + InputCurves.curve_orders.front());
		const k3d::uint_t v_knots_begin = InputPatches.patch_v_first_knots[Patch];
		const k3d::uint_t v_knots_end = v_knots_begin + curve_count + v_order;
		k3d::mesh::knots_t v_knots(InputPatches.patch_v_knots.begin() + v_knots_begin, InputPatches.patch_v_knots.begin() + v_knots_end);
		add_patch(OutputMesh, OutputPatches, points, InputCurves.curve_point_weights, u_knots, v_knots, u_order, v_order);
	}
	else
	{
		k3d::mesh::points_t points;
		k3d::mesh::weights_t weights;
		const k3d::uint_t point_count = InputCurves.curve_point_counts.front();
		for(k3d::uint_t i = 0; i != point_count; ++i)
		{
			for(k3d::uint_t curve = 0; curve != curve_count; ++curve)
			{
				const k3d::uint_t point_idx = InputCurves.curve_first_points[curve] + i;
				points.push_back(InputMesh.points->at(InputCurves.curve_points[point_idx]));
				weights.push_back(InputCurves.curve_point_weights[point_idx]);
			}
		}
		k3d::mesh::knots_t v_knots(InputCurves.curve_knots.begin(), InputCurves.curve_knots.begin() + InputCurves.curve_point_counts.front() + InputCurves.curve_orders.front());
		const k3d::uint_t u_knots_begin = InputPatches.patch_u_first_knots[Patch];
		const k3d::uint_t u_knots_end = u_knots_begin + curve_count + u_order;
		k3d::mesh::knots_t u_knots(InputPatches.patch_u_knots.begin() + u_knots_begin, InputPatches.patch_u_knots.begin() + u_knots_end);
		add_patch(OutputMesh, OutputPatches, points, weights, u_knots, v_knots, u_order, v_order);
	}

	OutputPatches.patch_materials.push_back(InputPatches.patch_materials[Patch]);
	OutputPatches.patch_selections.back() = InputPatches.patch_selections[Patch];
}

void elevate_patch_degree(const k3d::mesh& InputMesh, const k3d::nurbs_patch::const_primitive& InputPatches, k3d::mesh& OutputMesh, k3d::nurbs_patch::primitive& OutputPatches, const k3d::uint_t Patch, const k3d::uint_t Elevations, const k3d::bool_t UDirection)
{
	// Extract the curves
	k3d::mesh curves_mesh;
	boost::scoped_ptr<k3d::nurbs_curve::primitive> curves_prim(k3d::nurbs_curve::create(curves_mesh));
	curves_prim->material.push_back(InputPatches.patch_materials[Patch]);
	extract_curves(curves_mesh, *curves_prim, InputMesh, InputPatches, Patch, UDirection);

	// Elevate them
	boost::scoped_ptr<k3d::nurbs_curve::const_primitive> const_curves_prim(k3d::nurbs_curve::validate(curves_mesh, *curves_mesh.primitives.front()));
	boost::scoped_ptr<k3d::nurbs_curve::primitive> elevated_curves_prim(k3d::nurbs_curve::create(curves_mesh));
	elevated_curves_prim->material.push_back(InputPatches.patch_materials[Patch]);
	const k3d::uint_t curve_count = UDirection ? InputPatches.patch_v_point_counts[Patch] : InputPatches.patch_u_point_counts[Patch];
	for(k3d::uint_t curve = 0; curve != curve_count; ++curve)
	{
		elevate_curve_degree(curves_mesh, *elevated_curves_prim, curves_mesh, *const_curves_prim, curve, Elevations);
	}

	curves_to_patch(OutputMesh, OutputPatches, curves_mesh, *elevated_curves_prim, InputPatches, Patch, UDirection);
}

void insert_knot(k3d::mesh& OutputMesh, k3d::nurbs_patch::primitive& OutputPatches, const k3d::mesh& InputMesh, const k3d::nurbs_patch::const_primitive& InputPatches, k3d::uint_t Patch, const k3d::double_t u, const k3d::uint_t r, const k3d::bool_t UDirection)
{
	// Extract the curves
	k3d::mesh curves_mesh;
	boost::scoped_ptr<k3d::nurbs_curve::primitive> curves_prim(k3d::nurbs_curve::create(curves_mesh));
	curves_prim->material.push_back(InputPatches.patch_materials[Patch]);
	extract_curves(curves_mesh, *curves_prim, InputMesh, InputPatches, Patch, UDirection);

	// Insert the new knot into every curve
	boost::scoped_ptr<k3d::nurbs_curve::const_primitive> const_curves_prim(k3d::nurbs_curve::validate(curves_mesh, *curves_mesh.primitives.front()));
	boost::scoped_ptr<k3d::nurbs_curve::primitive> knot_inserted_curves_prim(k3d::nurbs_curve::create(curves_mesh));
	knot_inserted_curves_prim->material.push_back(InputPatches.patch_materials[Patch]);
	const k3d::uint_t curve_count = UDirection ? InputPatches.patch_v_point_counts[Patch] : InputPatches.patch_u_point_counts[Patch];
	for(k3d::uint_t curve = 0; curve != curve_count; ++curve)
	{
		insert_knot(curves_mesh, *knot_inserted_curves_prim, curves_mesh, *const_curves_prim, curve, u, r);
	}

	curves_to_patch(OutputMesh, OutputPatches, curves_mesh, *knot_inserted_curves_prim, InputPatches, Patch, UDirection);
}

void split_patch(k3d::mesh& OutputMesh, k3d::nurbs_patch::primitive& OutputPatches, const k3d::mesh& InputMesh, const k3d::nurbs_patch::const_primitive& InputPatches, k3d::uint_t Patch, const k3d::double_t u, const k3d::bool_t UDirection)
{
	// Can't split at end
	if(u > 1 - 0.000001 || u < 0.000001)
	{
		add_patch(OutputMesh, OutputPatches, InputMesh, InputPatches, Patch);
		return;
	}

	// Extract the curves
	k3d::mesh curves_mesh;
	boost::scoped_ptr<k3d::nurbs_curve::primitive> curves_prim(k3d::nurbs_curve::create(curves_mesh));
	curves_prim->material.push_back(InputPatches.patch_materials[Patch]);
	extract_curves(curves_mesh, *curves_prim, InputMesh, InputPatches, Patch, UDirection);

	// Split all the curves
	boost::scoped_ptr<k3d::nurbs_curve::const_primitive> const_curves_prim(k3d::nurbs_curve::validate(curves_mesh, *curves_mesh.primitives.front()));
	boost::scoped_ptr<k3d::nurbs_curve::primitive> split_curves_prim(k3d::nurbs_curve::create(curves_mesh));
	split_curves_prim->material.push_back(InputPatches.patch_materials[Patch]);
	const k3d::uint_t curve_count = UDirection ? InputPatches.patch_v_point_counts[Patch] : InputPatches.patch_u_point_counts[Patch];
	for(k3d::uint_t curve = 0; curve != curve_count; ++curve)
	{
		split_curve(curves_mesh, *split_curves_prim, curves_mesh, *const_curves_prim, curve, u);
	}

	if(is_closed(*const_curves_prim, 0))
	{
		curves_to_patch(OutputMesh, OutputPatches, curves_mesh, *split_curves_prim, InputPatches, Patch, UDirection);
	}
	else
	{
		assert_warning(curve_count * 2 == split_curves_prim->curve_first_points.size());
		// separate each first and second split curve into different primitives and meshes
		k3d::mesh first_mesh, second_mesh;
		first_mesh.points.create();
		first_mesh.point_selection.create();
		second_mesh.points.create();
		second_mesh.point_selection.create();
		boost::scoped_ptr<k3d::nurbs_curve::primitive> first_prim(k3d::nurbs_curve::create(first_mesh));
		boost::scoped_ptr<k3d::nurbs_curve::primitive> second_prim(k3d::nurbs_curve::create(second_mesh));
		boost::scoped_ptr<k3d::nurbs_curve::const_primitive> const_split_prim(k3d::nurbs_curve::validate(curves_mesh, *curves_mesh.primitives.back()));
		for(k3d::uint_t curve = 0; curve != split_curves_prim->curve_first_points.size(); ++curve)
		{
			if(curve % 2 == 0)
			{
				add_curve(first_mesh, *first_prim, curves_mesh, *const_split_prim, curve);
			}
			else
			{
				add_curve(second_mesh, *second_prim, curves_mesh, *const_split_prim, curve);
			}
		}
		curves_to_patch(OutputMesh, OutputPatches, first_mesh, *first_prim, InputPatches, Patch, UDirection);
		curves_to_patch(OutputMesh, OutputPatches, second_mesh, *second_prim, InputPatches, Patch, UDirection);
	}
}

void revolve_curve(k3d::mesh& OutputMesh, k3d::nurbs_patch::primitive& OutputPatches, const k3d::mesh& InputMesh, const k3d::nurbs_curve::const_primitive& InputCurves, const k3d::uint_t Curve, const k3d::axis Axis, const k3d::double_t Angle, const k3d::uint_t Segments, const k3d::bool_t Caps)
{
	//create a circle with the given angle
	k3d::mesh::knots_t u_knots;
	k3d::mesh::weights_t u_weights;
	k3d::mesh::points_t u_control_points;
	switch (Axis)
	{
	case k3d::Z :
		k3d::nurbs_curve::circular_arc(k3d::vector3(1, 0, 0), k3d::vector3(0, 1, 0), 0, Angle, Segments, u_knots, u_weights, u_control_points);
		break;
	case k3d::X :
		k3d::nurbs_curve::circular_arc(k3d::vector3(0, 0, 1), k3d::vector3(0, 1, 0), 0, Angle, Segments, u_knots, u_weights, u_control_points);
		break;
	case k3d::Y :
		k3d::nurbs_curve::circular_arc(k3d::vector3(1, 0, 0), k3d::vector3(0, 0, 1), 0, Angle, Segments, u_knots, u_weights, u_control_points);
		break;
	}

	// New patch arrays
	k3d::mesh::points_t points;
	k3d::mesh::weights_t weights;

	const k3d::uint_t curve_points_begin = InputCurves.curve_first_points[Curve];
	const k3d::uint_t curve_points_end = InputCurves.curve_point_counts[Curve] + curve_points_begin;
	for (k3d::uint_t i = curve_points_begin; i != curve_points_end; ++i)
	{
		k3d::point3 p = InputMesh.points->at(InputCurves.curve_points[i]);
		double w = InputCurves.curve_point_weights[i];
		double distance = 1.0;
		switch (Axis)
		{
		case k3d::Z :
			distance = sqrt((p[0] * p[0]) + (p[1] * p[1])); // we want the distance to the z axis
			break;
		case k3d::X :
			distance = sqrt((p[2] * p[2]) + (p[1] * p[1])); // we want the distance to the x axis
			break;
		case k3d::Y :
			distance = sqrt((p[0] * p[0]) + (p[2] * p[2])); // we want the distance to the y axis
			break;
		}

		for (int j = 0; j < u_control_points.size(); j++)
		{
			k3d::point3 p_u;
			switch (Axis)
			{
			case k3d::Z :
				p_u = k3d::point3(u_control_points[j][0] * distance, u_control_points[j][1] * distance, p[2]);
				break;
			case k3d::X :
				p_u = k3d::point3(p[0], u_control_points[j][1] * distance, u_control_points[j][2] * distance);
				break;
			case k3d::Y :
				p_u = k3d::point3(u_control_points[j][0] * distance, p[1], u_control_points[j][2] * distance);
				break;
			}

			points.push_back(p_u);
			weights.push_back(w * u_weights[j]);
		}
	}

	const k3d::uint_t v_order = InputCurves.curve_orders[Curve];
	const k3d::uint_t knots_begin = InputCurves.curve_first_knots[Curve];
	const k3d::uint_t knots_end = knots_begin + v_order + InputCurves.curve_point_counts[Curve];
	k3d::mesh::knots_t v_knots(InputCurves.curve_knots.begin() + knots_begin, InputCurves.curve_knots.begin() + knots_end);

	add_patch(OutputMesh, OutputPatches, points, weights, u_knots, v_knots, 3, v_order);
	OutputPatches.patch_materials.push_back(InputCurves.material.front());
	if(Caps)
	{
		k3d::mesh::points_t first_circle_points(points.begin(), points.begin() + u_control_points.size());
		k3d::mesh::points_t last_circle_points(points.end() - u_control_points.size(), points.end());
		k3d::mesh curves_mesh;
		curves_mesh.points.create();
		curves_mesh.point_selection.create();
		boost::scoped_ptr<k3d::nurbs_curve::primitive> curves_prim(k3d::nurbs_curve::create(curves_mesh));
		k3d::nurbs_curve::add_curve(curves_mesh, *curves_prim, 3, first_circle_points, u_weights, u_knots);
		k3d::nurbs_curve::add_curve(curves_mesh, *curves_prim, 3, last_circle_points, u_weights, u_knots);
		curves_prim->material.push_back(InputCurves.material.front());
		k3d::point3 first_centroid(0,0,0);
		first_centroid[Axis] = points.front()[Axis];
		boost::scoped_ptr<k3d::nurbs_curve::const_primitive> const_curves_prim(k3d::nurbs_curve::validate(curves_mesh, *curves_mesh.primitives.front()));
		create_cap(OutputMesh, OutputPatches, *curves_mesh.points, *const_curves_prim, 0, first_centroid, Segments);
		OutputPatches.patch_materials.push_back(InputCurves.material.front());
		k3d::point3 second_centroid(0,0,0);
		second_centroid[Axis] = points.back()[Axis];
		create_cap(OutputMesh, OutputPatches, *curves_mesh.points, *const_curves_prim, 1, second_centroid, Segments);
		OutputPatches.patch_materials.push_back(InputCurves.material.front());
	}
}

} //namespace nurbs

} //namespace module
