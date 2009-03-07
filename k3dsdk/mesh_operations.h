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

/// Returns true iff the given mesh contains valid nurbs curve group data (i.e. every array is defined).
const bool_t validate_nurbs_curve_groups(const mesh& Mesh);
/// Returns true iff the given mesh contains valid nurbs patch data (i.e. every array is defined).
const bool_t validate_nurbs_patches(const mesh& Mesh);
/** \deprecated */
const bool_t legacy_validate_polyhedra(const mesh& Mesh);

/// Traverse polygonal mesh, visiting faces, loops, and points.
template<typename visitor_t>
void traverse_polyhedra(const mesh& Mesh, visitor_t& Visitor)
{
	return_if_fail(legacy_validate_polyhedra(Mesh));
	const mesh::points_t& points = *Mesh.points;
	const mesh::indices_t& face_first_loops = *Mesh.polyhedra->face_first_loops;
	const mesh::counts_t& face_loop_counts = *Mesh.polyhedra->face_loop_counts;
	const mesh::indices_t& loop_first_edges = *Mesh.polyhedra->loop_first_edges;
	const mesh::indices_t& edge_points = *Mesh.polyhedra->edge_points;
	const mesh::indices_t& clockwise_edges = *Mesh.polyhedra->clockwise_edges;
	
	const uint_t face_count = face_first_loops.size();
	for(uint_t face = 0; face != face_count; ++face)
	{
		Visitor.on_face_start(face);
		const uint_t loop_begin = face_first_loops[face];
		const uint_t loop_end = loop_begin + face_loop_counts[face];

		for(uint_t loop = loop_begin; loop != loop_end; ++loop)
		{
			Visitor.on_loop(loop);
			const uint_t first_edge = loop_first_edges[loop];

			for(uint_t edge = first_edge; ; )
			{
				Visitor.on_edge(edge, edge_points[edge], edge_points[clockwise_edges[edge]], points[edge_points[edge]], points[edge_points[clockwise_edges[edge]]]);
				edge = clockwise_edges[edge];
				if(edge == first_edge)
					break;
			}
		}
		Visitor.on_face_end(face);
	}
}

/// Traverse selected mesh points
template<typename visitor_t>
void traverse_selected_points(const mesh& Mesh, visitor_t& Visitor)
{
	for (uint_t point = 0; point != Mesh.points->size(); ++point)
	{
		if (Mesh.point_selection->at(point))
		{
			Visitor(point, Mesh.points->at(point));
		}
	}
}

/// For each selected edge, visit the start and end point (multiple visits per point possible!)
template<typename visitor_t>
void traverse_selected_edge_points(const mesh& Mesh, visitor_t& Visitor)
{
	return_if_fail(legacy_validate_polyhedra(Mesh));
	const mesh::points_t& points = *Mesh.points;
	const mesh::indices_t& edge_points = *Mesh.polyhedra->edge_points;
	const mesh::indices_t& clockwise_edges = *Mesh.polyhedra->clockwise_edges;
	const mesh::selection_t& edge_selection = *Mesh.polyhedra->edge_selection;
	for (uint_t edge = 0; edge != edge_points.size(); ++edge)
	{
		if (edge_selection[edge])
		{
			Visitor(edge_points[edge], points[edge_points[edge]]);
			Visitor(edge_points[clockwise_edges[edge]], points[edge_points[clockwise_edges[edge]]]);
		}
	}
}

// For each selected face, visit all of its points (multiple visits per point possible!)
template<typename visitor_t>
void traverse_selected_face_points(const mesh& Mesh, visitor_t& Visitor)
{
	return_if_fail(legacy_validate_polyhedra(Mesh));
	const mesh::points_t& points = *Mesh.points;
	const mesh::indices_t& face_first_loops = *Mesh.polyhedra->face_first_loops;
	const mesh::counts_t& face_loop_counts = *Mesh.polyhedra->face_loop_counts;
	const mesh::indices_t& loop_first_edges = *Mesh.polyhedra->loop_first_edges;
	const mesh::indices_t& edge_points = *Mesh.polyhedra->edge_points;
	const mesh::indices_t& clockwise_edges = *Mesh.polyhedra->clockwise_edges;
	const mesh::selection_t& face_selection = *Mesh.polyhedra->face_selection;
	for(uint_t face = 0; face != face_first_loops.size(); ++face)
	{
		if (!face_selection[face])
			continue;
		
		const uint_t loop_begin = face_first_loops[face];
		const uint_t loop_end = loop_begin + face_loop_counts[face];
		for(uint_t loop = loop_begin; loop != loop_end; ++loop)
		{
			const uint_t first_edge = loop_first_edges[loop];
			for(uint_t edge = first_edge; ; )
			{
				Visitor(edge_points[edge], points[edge_points[edge]]);

				edge = clockwise_edges[edge];
				if(edge == first_edge)
					break;
			}
		}
	}
}

} // namespace k3d

#endif // !K3DSDK_MESH_OPERATIONS_H

