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

#include "imaterial.h"
#include "mesh_operations.h"
#include "mesh_selection.h"
#include "polyhedron.h"
#include "vectors.h"

namespace k3d
{

/////////////////////////////////////////////////////////////////////////////
// bounds

const bounding_box3 bounds(const k3d::mesh& Mesh)
{
	bounding_box3 results;

	if(Mesh.points)
        return bounds(*Mesh.points);

	return results;
}

/////////////////////////////////////////////////////////////////////////////
// bounds

const bounding_box3 bounds(const k3d::mesh::points_t& Points)
{
	bounding_box3 results;

    const uint_t point_begin = 0;
    const uint_t point_end = point_begin + Points.size();
    for(uint_t point = point_begin; point != point_end; ++point)
        results.insert(Points[point]);

	return results;
}

//////////////////////////////////////////////////////////////////////////////
// center

const point3 center(const mesh::indices_t& EdgePoints, const mesh::indices_t& ClockwiseEdges, const mesh::points_t& Points, const uint_t EdgeIndex)
{
	point3 result(0, 0, 0);

	uint_t count = 0;
	for(uint_t edge = EdgeIndex; ; )
	{
		result += to_vector(Points[EdgePoints[edge]]);
		++count;

		edge = ClockwiseEdges[edge];
		if(edge == EdgeIndex)
			break;
	}

	if(count)
		result /= static_cast<double>(count);

	return result;
}

//////////////////////////////////////////////////////////////////////////////
// normal

const normal3 normal(const mesh::indices_t& EdgePoints, const mesh::indices_t& ClockwiseEdges, const mesh::points_t& Points, const uint_t EdgeIndex)
{
	// Calculates the normal for an edge loop using the summation method, which is more robust than the three-point methods (handles zero-length edges)
	normal3 result(0, 0, 0);

	for(uint_t edge = EdgeIndex; ; )
	{
		const point3& i = Points[EdgePoints[edge]];
		const point3& j = Points[EdgePoints[ClockwiseEdges[edge]]];

		result[0] += (i[1] + j[1]) * (j[2] - i[2]);
		result[1] += (i[2] + j[2]) * (j[0] - i[0]);
		result[2] += (i[0] + j[0]) * (j[1] - i[1]);

		edge = ClockwiseEdges[edge];
		if(edge == EdgeIndex)
			break;
	}

	return 0.5 * result;
}

////////////////////////////////////////////////////////////////////////////////
// deep_copy

void deep_copy(const mesh& From, mesh& To)
{
	To = From;
	assert_not_implemented(); // Need to ensure that all storage is unique
}


////////////////////////////////////////////////////////////////////////////////
// validate

/** \todo Handle all gprim types */
const bool_t validate(mesh& Mesh)
{
	bool_t result = true;

	if(Mesh.points && !validate_points(Mesh))
		result = false;

	if(Mesh.nurbs_curve_groups && !validate_nurbs_curve_groups(Mesh))
		result = false;

	if(Mesh.nurbs_patches && !validate_nurbs_patches(Mesh))
		result = false;

	if(Mesh.polyhedra && !legacy_validate_polyhedra(Mesh))
		result = false;

	return result;
}

const bool_t validate_points(const mesh& Mesh)
{
	if(!Mesh.points)
		return false;

	// If the mesh has points, it must have storage for point selections
	return_val_if_fail(Mesh.point_selection, false);

	// The point and point selection arrays must be the same length
	return_val_if_fail(Mesh.points->size() == Mesh.point_selection->size(), false);

	return true;
}

const bool_t validate_nurbs_curve_groups(const mesh& Mesh)
{
	if(!Mesh.nurbs_curve_groups)
		return false;

	return_val_if_fail(validate_points(Mesh), false);
	return_val_if_fail(Mesh.nurbs_curve_groups->first_curves, false);
	return_val_if_fail(Mesh.nurbs_curve_groups->curve_counts, false);
	return_val_if_fail(Mesh.nurbs_curve_groups->curve_first_points, false);
	return_val_if_fail(Mesh.nurbs_curve_groups->curve_point_counts, false);
	return_val_if_fail(Mesh.nurbs_curve_groups->curve_orders, false);
	return_val_if_fail(Mesh.nurbs_curve_groups->curve_first_knots, false);
	return_val_if_fail(Mesh.nurbs_curve_groups->curve_points, false);
	return_val_if_fail(Mesh.nurbs_curve_groups->curve_point_weights, false);
	return_val_if_fail(Mesh.nurbs_curve_groups->curve_knots, false);
	return_val_if_fail(Mesh.nurbs_curve_groups->curve_selection, false);
	return_val_if_fail(Mesh.nurbs_curve_groups->materials, false);

	return true;
}

const bool_t validate_nurbs_patches(const mesh& Mesh)
{
	if(!Mesh.nurbs_patches)
		return false;

	return_val_if_fail(validate_points(Mesh), false);
	return_val_if_fail(Mesh.nurbs_patches->patch_first_points, false);
	return_val_if_fail(Mesh.nurbs_patches->patch_u_point_counts, false);
	return_val_if_fail(Mesh.nurbs_patches->patch_v_point_counts, false);
	return_val_if_fail(Mesh.nurbs_patches->patch_u_orders, false);
	return_val_if_fail(Mesh.nurbs_patches->patch_v_orders, false);
	return_val_if_fail(Mesh.nurbs_patches->patch_u_first_knots, false);
	return_val_if_fail(Mesh.nurbs_patches->patch_v_first_knots, false);
	return_val_if_fail(Mesh.nurbs_patches->patch_points, false);
	return_val_if_fail(Mesh.nurbs_patches->patch_point_weights, false);
	return_val_if_fail(Mesh.nurbs_patches->patch_u_knots, false);
	return_val_if_fail(Mesh.nurbs_patches->patch_v_knots, false);
	return_val_if_fail(Mesh.nurbs_patches->patch_selection, false);
	return_val_if_fail(Mesh.nurbs_patches->patch_materials, false);

	return true;
}

const bool_t legacy_validate_polyhedra(const mesh& Mesh)
{
	if(!Mesh.polyhedra)
		return false;

	return_val_if_fail(validate_points(Mesh), false);
	return_val_if_fail(Mesh.polyhedra->first_faces, false);
	return_val_if_fail(Mesh.polyhedra->face_counts, false);
	return_val_if_fail(Mesh.polyhedra->types, false);
	return_val_if_fail(Mesh.polyhedra->face_first_loops, false);
	return_val_if_fail(Mesh.polyhedra->face_loop_counts, false);
	return_val_if_fail(Mesh.polyhedra->face_selection, false);
	return_val_if_fail(Mesh.polyhedra->face_materials, false);
	return_val_if_fail(Mesh.polyhedra->loop_first_edges, false);
	return_val_if_fail(Mesh.polyhedra->edge_points, false);
	return_val_if_fail(Mesh.polyhedra->clockwise_edges, false);
	return_val_if_fail(Mesh.polyhedra->edge_selection, false);

	return_val_if_fail(Mesh.polyhedra->first_faces->size() == Mesh.polyhedra->face_counts->size(), false);
	return_val_if_fail(Mesh.polyhedra->first_faces->size() == Mesh.polyhedra->types->size(), false);

	return_val_if_fail(Mesh.polyhedra->face_first_loops->size() == Mesh.polyhedra->face_loop_counts->size(), false);
	return_val_if_fail(Mesh.polyhedra->face_first_loops->size() == Mesh.polyhedra->face_selection->size(), false);
	return_val_if_fail(Mesh.polyhedra->face_first_loops->size() == Mesh.polyhedra->face_materials->size(), false);

	return_val_if_fail(Mesh.polyhedra->edge_points->size() == Mesh.polyhedra->clockwise_edges->size(), false);
	return_val_if_fail(Mesh.polyhedra->edge_points->size() == Mesh.polyhedra->edge_selection->size(), false);

	// Check for infinite loops
	// TODO:  Catch segmentation faults
	const mesh::indices_t& loop_first_edges = *Mesh.polyhedra->loop_first_edges;
	const mesh::indices_t& edge_points = *Mesh.polyhedra->edge_points;
	const mesh::indices_t& clockwise_edges = *Mesh.polyhedra->clockwise_edges;

	const uint_t loop_begin = 0;
	const uint_t loop_end = loop_begin + loop_first_edges.size();
	for(uint_t loop = loop_begin; loop != loop_end; ++loop)
	{
		const uint_t first_edge = loop_first_edges[loop];
		uint_t edge_slow = first_edge;
		uint_t edge_fast = first_edge;
		uint_t cycle_count = 0;
		while(true)
		{
			edge_slow = clockwise_edges[edge_slow];
			edge_fast = clockwise_edges[clockwise_edges[edge_fast]];

			if(edge_slow == edge_fast)
				++cycle_count;

			if(cycle_count > 2)
			{
				log() << error << "infinite loop at loop index " << loop << std::endl;
				return false;
			}

			if(edge_slow == first_edge)
				break;
		}
	}
	return true;
}

const bool_t is_sds(const mesh& Mesh)
{
	if(!legacy_validate_polyhedra(Mesh))
		return false;

	const mesh::polyhedra_t::types_t& types = *Mesh.polyhedra->types;
	for(uint_t type = 0; type != types.size(); ++type)
	{
		if(types[type] == mesh::polyhedra_t::CATMULL_CLARK)
			return true;
	}
	return false;
}

} // namespace k3d

