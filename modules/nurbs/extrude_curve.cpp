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
	\author Carsten Haubold (CarstenHaubold@web.de)
*/

#include "nurbs_patch_modifier.h"

#include <k3dsdk/data.h>
#include <k3dsdk/document_plugin_factory.h>
#include <k3dsdk/geometry.h>
#include <k3dsdk/log.h>
#include <k3dsdk/material_sink.h>
#include <k3dsdk/measurement.h>
#include <k3dsdk/mesh.h>
#include <k3dsdk/mesh_modifier.h>
#include <k3dsdk/mesh_selection_sink.h>
#include <k3dsdk/mesh_source.h>
#include <k3dsdk/module.h>
#include <k3dsdk/node.h>
#include <k3dsdk/nurbs_curve.h>
#include <k3dsdk/point3.h>
#include <k3dsdk/selection.h>
#include <k3dsdk/table_copier.h>

#include <boost/scoped_ptr.hpp>

#include <iostream>
#include <vector>

namespace module
{

namespace nurbs
{

namespace detail
{

/// TODO: Figure out a way to do this using k3d::equal
bool equal(const k3d::point3& p1, const k3d::point3& p2, float threshold = 0.000001)
{
	float point1[3], point2[3];

	point1[0] = static_cast<float>(p1[0]);
	point1[1] = static_cast<float>(p1[1]);
	point1[2] = static_cast<float>(p1[2]);

	point2[0] = static_cast<float>(p2[0]);
	point2[1] = static_cast<float>(p2[1]);
	point2[2] = static_cast<float>(p2[2]);

	if (fabs(point1[0] - point2[0]) < threshold
	    && fabs(point1[1] - point2[1]) < threshold
	    && fabs(point1[2] - point2[2]) < threshold)
		return true;
	return false;
}

void find_loops(const k3d::mesh::points_t& CurvePoints, const k3d::nurbs_curve::const_primitive& Curves, k3d::mesh::bools_t& IsInLoop, k3d::mesh::indices_t& LoopFirstCurves, k3d::mesh::indices_t& CurveLoops, k3d::mesh::indices_t& NextCurves, k3d::mesh::bools_t& LoopSelections, k3d::mesh::points_t& LoopCentroids)
{
	const k3d::uint_t curve_count = Curves.curve_first_points.size();
	k3d::mesh::indices_t curve_starts(curve_count);
	k3d::mesh::indices_t curve_ends(curve_count);
	for(k3d::uint_t curve = 0; curve != curve_count; ++curve)
	{
		curve_starts[curve] = Curves.curve_first_points[curve];
		curve_ends[curve] = curve_starts[curve] + Curves.curve_point_counts[curve] - 1;
	}

	// Get curve endpoint valences, since only valence 2 points are allowed
	k3d::mesh::counts_t valences(CurvePoints.size(), 0);
	for(k3d::uint_t curve = 0; curve != curve_count; ++curve)
	{
		valences[Curves.curve_points[curve_starts[curve]]]++;
		valences[Curves.curve_points[curve_ends[curve]]]++;
	}

	IsInLoop.assign(curve_count, false);
	NextCurves.assign(curve_count, 0);
	CurveLoops.assign(curve_count, 0);
	k3d::mesh::bools_t has_next_curve(curve_count, false);

	for(k3d::uint_t first_curve = 0; first_curve != curve_count; ++first_curve)
	{
		// check if there is a follow-on curve
		for(k3d::uint_t second_curve = 0; second_curve != curve_count; ++second_curve)
		{
			if(Curves.curve_points[curve_ends[first_curve]] == Curves.curve_points[curve_starts[second_curve]] && valences[Curves.curve_points[curve_ends[first_curve]]] == 2)
			{
				NextCurves[first_curve] = second_curve;
				has_next_curve[first_curve] = true;
			}
		}
	}

	// Find multi-curve loops
	for(k3d::uint_t curve = 0; curve != curve_count; ++curve)
	{
		if(IsInLoop[curve])
			continue;
		k3d::bool_t is_loop = true;
		k3d::point3 centroid(0,0,0);
		k3d::uint_t count = 0;
		k3d::uint_t selected_count = 0;
		k3d::uint_t point_count = 0;
		for(k3d::uint_t next_curve = curve; ;)
		{
			if(!has_next_curve[next_curve])
			{
				is_loop = false;
				break;
			}
			++count;
			if(Curves.curve_selections[next_curve])
				++selected_count;

			for(k3d::uint_t i = curve_starts[next_curve]; i != curve_ends[next_curve]; ++i)
			{
				const k3d::uint_t point = Curves.curve_points[i];
				++point_count;
				centroid += k3d::to_vector(CurvePoints[point]);
			}

			next_curve = NextCurves[next_curve];
			if(next_curve == curve)
				break;
		}
		if(is_loop)
		{
			for(k3d::uint_t next_curve = curve; ;)
			{
				IsInLoop[next_curve] = true;
				CurveLoops[next_curve] = LoopFirstCurves.size();

				next_curve = NextCurves[next_curve];
				if(next_curve == curve)
					break;
			}
			LoopCentroids.push_back(centroid / point_count);
			LoopSelections.push_back(selected_count == count);
			LoopFirstCurves.push_back(curve);
		}
	}
}

k3d::uint_t insert_point(k3d::mesh::points_t& Points, const k3d::point3& Point, const k3d::bool_t Reuse, const k3d::uint64_t Threshold)
{
	if(Reuse)
	{
		for(k3d::uint_t point_idx = Points.size(); point_idx != 0; --point_idx)
		{
			if(equal(Points[point_idx-1], Point))
				return point_idx-1;
		}
	}
	Points.push_back(Point);
	return Points.size() - 1;
}

void add_patch(k3d::mesh& Mesh,
		k3d::nurbs_patch::primitive& NurbsPatches,
		const k3d::mesh::points_t& ControlPoints,
		const k3d::mesh::weights_t& Weights,
		const k3d::mesh::knots_t& UKnots,
		const k3d::mesh::knots_t& VKnots,
		const k3d::uint_t UOrder,
		const k3d::uint_t VOrder,
		const k3d::bool_t ReusePoints = true,
		const k3d::uint64_t Threshold = 1) // Not used at this time
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
		NurbsPatches.patch_points.push_back(insert_point(points, ControlPoints[i], ReusePoints, Threshold));
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

/// Create a cap over a curve, if it is closed
void create_cap(k3d::mesh& Mesh, k3d::nurbs_patch::primitive& Patches, const k3d::mesh::points_t& CurvePoints, const k3d::nurbs_curve::const_primitive& Curves, const k3d::uint_t Curve, const k3d::point3& Centroid, const k3d::uint_t VSegments = 1)
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

/// Traverse each selected curve in SourceCurves along each selected curve in CurvesToTraverse
void traverse_curve(const k3d::mesh& SourceCurves, const k3d::mesh& CurvesToTraverse, k3d::mesh& OutputMesh, const k3d::bool_t CreateCaps)
{
	k3d::uint_t source_prim_idx = 0;
	for(k3d::mesh::primitives_t::const_iterator primitive = SourceCurves.primitives.begin(); primitive != SourceCurves.primitives.end(); ++primitive)
	{
		boost::scoped_ptr<k3d::nurbs_curve::const_primitive> source_curves(k3d::nurbs_curve::validate(SourceCurves, **primitive));
		if(!source_curves)
			continue;

		boost::scoped_ptr<k3d::nurbs_patch::primitive> output_patches(k3d::nurbs_patch::create(OutputMesh));
		output_patches->uniform_attributes = source_curves->curve_attributes.clone_types();
		k3d::table_copier uniform_copier(source_curves->curve_attributes, output_patches->uniform_attributes);

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
					add_patch(OutputMesh, *output_patches, new_points, new_weights, u_knots, v_knots, source_curves->curve_orders[source_curve], curves_to_traverse->curve_orders[curve_to_traverse]);
					uniform_copier.push_back(source_curve);
					output_patches->patch_materials.push_back(source_curves->material[0]);
					if(CreateCaps)
					{
						if(is_in_loop[source_curve] && loop_selections[curve_loops[source_curve]])
						{
							const k3d::point3& centroid = loop_centroids[curve_loops[source_curve]];
							// Cap over the original curve
							detail::create_cap(OutputMesh, *output_patches, *SourceCurves.points, *source_curves, source_curve, centroid);
							uniform_copier.push_back(source_curve);
							output_patches->patch_materials.push_back(source_curves->material[0]);

							// Cap over the opposite curve
							k3d::mesh tempmesh(SourceCurves);
							boost::scoped_ptr<k3d::nurbs_curve::primitive> tempcurve(k3d::nurbs_curve::validate(tempmesh, tempmesh.primitives[source_prim_idx]));
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

							boost::scoped_ptr<k3d::nurbs_curve::const_primitive> const_tempcurve(k3d::nurbs_curve::validate(tempmesh, *tempmesh.primitives[source_prim_idx]));
							detail::create_cap(OutputMesh, *output_patches, temp_points, *const_tempcurve, source_curve, centroid + delta_u);
							uniform_copier.push_back(source_curve);
							output_patches->patch_materials.push_back(source_curves->material[0]);
						}
					}
				}
			}
		}
		++source_prim_idx;
	}
}

/// Adds a straight line to the given NURBS curve set. New points are added to the OutputMesh
void straight_line(const k3d::point3& Start, const k3d::point3 End, const k3d::uint_t Segments, k3d::nurbs_curve::primitive& NurbsCurves, k3d::mesh& OutputMesh, const k3d::uint_t Order = 2)
{
	k3d::vector3 delta = (End - Start) / Segments;

	k3d::mesh::points_t points;
	for (k3d::uint_t i = 0; i <= Segments; i++)
	{
		points.push_back(Start + delta * i);
	}
	k3d::nurbs_curve::add_curve(OutputMesh, NurbsCurves, Order, points);
}

void update_indices(k3d::mesh::indices_t& Indices, const k3d::uint_t Start, const k3d::int32_t Offset)
{
	for(k3d::uint_t i = 0; i != Indices.size(); ++i)
	{
		if(Indices[i] >= Start)
			Indices[i] += Offset;
	}
}

// Remove the item at Index
template<typename ArrayT>
void erase(ArrayT& Array, const k3d::uint_t Index)
{
	return_if_fail(Index < Array.size());
	Array.erase(Array.begin() + Index);
}

void delete_curve(k3d::nurbs_curve::primitive& Curves, const k3d::uint_t Curve)
{
	const k3d::uint_t curve_count = Curves.curve_first_points.size();

	k3d::table curve_attributes = Curves.curve_attributes.clone_types();
	k3d::table_copier uniform_copier(Curves.curve_attributes, curve_attributes);
	for(k3d::uint_t i = 0; i != Curve; ++i)
		uniform_copier.push_back(i);
	for(k3d::uint_t i = Curve + 1; i != curve_count; ++i)
		uniform_copier.push_back(i);
	k3d::table varying_attributes = Curves.varying_attributes.clone_types();
	k3d::table_copier varying_copier(Curves.varying_attributes, varying_attributes);
	k3d::uint_t curve_start_index = Curves.curve_first_points[Curve];
	k3d::uint_t curve_end_index = curve_start_index + Curves.curve_point_counts[Curve];
	for(k3d::uint_t i = 0; i != curve_start_index; ++i)
		varying_copier.push_back(i);
	for(k3d::uint_t i = curve_end_index; i != Curves.curve_points.size(); ++i)
		varying_copier.push_back(i);

	return_if_fail(Curve < curve_count);
	// Erase point indices and weights
	k3d::mesh::indices_t::iterator points_begin = Curves.curve_points.begin() + Curves.curve_first_points[Curve];
	k3d::mesh::indices_t::iterator points_end = points_begin + Curves.curve_point_counts[Curve];
	Curves.curve_points.erase(points_begin, points_end);
	update_indices(Curves.curve_first_points, Curves.curve_first_points[Curve] + Curves.curve_point_counts[Curve], -Curves.curve_point_counts[Curve]);
	k3d::mesh::weights_t::iterator weights_begin = Curves.curve_point_weights.begin() + Curves.curve_first_points[Curve];
	k3d::mesh::weights_t::iterator weights_end = weights_begin + Curves.curve_point_counts[Curve];
	Curves.curve_point_weights.erase(weights_begin, weights_end);
	// Erase knots
	k3d::mesh::knots_t::iterator knots_begin = Curves.curve_knots.begin() + Curves.curve_first_knots[Curve];
	k3d::mesh::knots_t::iterator knots_end = knots_begin + Curves.curve_point_counts[Curve] + Curves.curve_orders[Curve];
	Curves.curve_knots.erase(knots_begin, knots_end);
	update_indices(Curves.curve_first_knots, Curves.curve_first_knots[Curve] + Curves.curve_point_counts[Curve] + Curves.curve_orders[Curve], -(Curves.curve_point_counts[Curve] + Curves.curve_orders[Curve]));

	erase(Curves.curve_first_knots, Curve);
	erase(Curves.curve_first_points, Curve);
	erase(Curves.curve_orders, Curve);
	erase(Curves.curve_point_counts, Curve);
	erase(Curves.curve_selections, Curve);
}

} // namespace detail

class extrude_curve :
	public k3d::mesh_selection_sink<k3d::mesh_modifier<k3d::node > >
{
	typedef k3d::mesh_selection_sink<k3d::mesh_modifier<k3d::node > > base;
public:
	extrude_curve(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_distance(init_owner(*this) + init_name(_("distance")) + init_label(_("Distance")) + init_description(_("How far to extrude the curve")) + init_step_increment(0.5) + init_units(typeid(k3d::measurement::scalar)) + init_value(1.0)),
		m_segments(init_owner(*this) + init_name("segments") + init_label(_("segments")) + init_description(_("Segments")) + init_value(1) + init_constraint(constraint::minimum<k3d::int32_t>(1)) + init_step_increment(1) + init_units(typeid(k3d::measurement::scalar))),
		m_along(init_owner(*this) + init_name("along") + init_label(_("Extrude along")) + init_description(_("Axis along which the curve gets extruded")) + init_value(k3d::Z) + init_enumeration(k3d::axis_values())),
		m_delete_original(init_owner(*this) + init_name(_("delete_original")) + init_label(_("Delete the Curve")) + init_description(_("Delete the original curve")) + init_value(true)),
		m_cap(init_owner(*this) + init_name(_("cap")) + init_label(_("Create Caps?")) + init_description(_("Create caps at both ends?")) + init_value(false))
	{
		m_mesh_selection.changed_signal().connect(make_update_mesh_slot());
		m_distance.changed_signal().connect(make_update_mesh_slot());
		m_segments.changed_signal().connect(make_update_mesh_slot());
		m_along.changed_signal().connect(make_update_mesh_slot());
		m_cap.changed_signal().connect(make_update_mesh_slot());
		m_delete_original.changed_signal().connect(make_update_mesh_slot());
	}

	void on_create_mesh(const k3d::mesh& Input, k3d::mesh& Output)
	{
		Output = Input;
	}

	void on_update_mesh(const k3d::mesh& Input, k3d::mesh& Output)
	{
		Output = Input;
		k3d::mesh input_with_selections = Input;
		k3d::geometry::selection::merge(m_mesh_selection.pipeline_value(), input_with_selections);
		k3d::geometry::selection::merge(m_mesh_selection.pipeline_value(), Output);

		double distance = m_distance.pipeline_value();
		k3d::axis axis = m_along.pipeline_value();
		bool create_cap =  m_cap.pipeline_value();
		int segments = m_segments.pipeline_value();

		// Determine the start and end point of the extrusion vector
		const k3d::point3 startpoint(0,0,0);
		k3d::point3 endpoint;
		switch (axis)
		{
		case k3d::X:
			endpoint = k3d::point3(distance, 0.0, 0.0);
			break;
		case k3d::Y:
			endpoint = k3d::point3(0.0, distance, 0.0);
			break;
		case k3d::Z:
			endpoint = k3d::point3(0.0, 0.0, distance);
			break;
		}

		// Create a straight curve from this vector to traverse along
		k3d::mesh extrusion_vector_mesh;
		extrusion_vector_mesh.point_selection.create();
		extrusion_vector_mesh.points.create();
		boost::scoped_ptr<k3d::nurbs_curve::primitive> extrusion_vector_primitive(k3d::nurbs_curve::create(extrusion_vector_mesh));
		detail::straight_line(startpoint, endpoint, segments, *extrusion_vector_primitive, extrusion_vector_mesh);
		extrusion_vector_primitive->curve_selections[0] = 1.0;
		extrusion_vector_primitive->material.push_back(0);

		detail::traverse_curve(input_with_selections, extrusion_vector_mesh, Output, m_cap.pipeline_value());

		if(m_delete_original.pipeline_value())
		{
			for(k3d::mesh::primitives_t::iterator primitive = Output.primitives.begin(); primitive != Output.primitives.end();)
			{
				boost::scoped_ptr<k3d::nurbs_curve::primitive> curves(k3d::nurbs_curve::validate(Output, *primitive));
				if(!curves)
				{
					++primitive;
					continue;
				}
				for(k3d::uint_t curve = 0; ;)
				{
					if(curves->curve_selections[curve])
					{
						detail::delete_curve(*curves, curve);
					}
					else
					{
						++curve;
					}
					if(curve == curves->curve_first_points.size())
						break;
				}
				if(!curves->curve_first_points.empty())
				{
					++primitive;
				}
				else
				{
					primitive = Output.primitives.erase(primitive);
				}
			}
		}
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<extrude_curve, k3d::interface_list<k3d::imesh_source, k3d::interface_list<k3d::imesh_sink > > > factory(
		  k3d::uuid(0x93c6f141, 0x934f333a, 0x8a027ab4, 0x59b1510c),
		  "NurbsExtrudeCurve",
		  _("Extrudes the selected curve along a coordinate axis and lets you create caps if one of the curves is closed"),
		  "NURBS",
		  k3d::iplugin_factory::EXPERIMENTAL);

		return factory;
	}

private:
	k3d_data(k3d::double_t, immutable_name, change_signal, with_undo, local_storage, no_constraint, measurement_property, with_serialization) m_distance;
	k3d_data(k3d::axis, immutable_name, change_signal, with_undo, local_storage, no_constraint, enumeration_property, with_serialization) m_along;
	k3d_data(k3d::bool_t, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_cap;
	k3d_data(k3d::bool_t, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_delete_original;
	k3d_data(k3d::int32_t, immutable_name, change_signal, with_undo, local_storage, with_constraint, measurement_property, with_serialization) m_segments;
};

//Create connect_curve factory
k3d::iplugin_factory& extrude_curve_factory()
{
	return extrude_curve::get_factory();
}

}//namespace nurbs
} //namespace module

