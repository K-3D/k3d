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

const bool_t validate_nurbs_curve_groups(const mesh& Mesh)
{
	if(!Mesh.nurbs_curve_groups)
		return false;

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

} // namespace k3d

