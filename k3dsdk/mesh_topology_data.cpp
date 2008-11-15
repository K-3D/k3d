// K-3D
// Copyright (c) 1995-2008, Timothy M. Shead
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

/** \file Declares structures to add extra "convenience" data to a mesh
	\author Bart Janssens (bart.janssens@lid.kviv.be)
*/

#include "array_metadata.h"
#include "mesh_topology_data.h"

#include "parallel/blocked_range.h"
#include "parallel/parallel_for.h"
#include "parallel/threads.h"

#include <iterator>

namespace k3d
{

namespace detail
{

class find_companion_worker
{
public:
	find_companion_worker(
		const mesh::indices_t& EdgePoints,
		const mesh::indices_t& ClockwiseEdges,
		const mesh::counts_t& Valences,
		const mesh::indices_t& FirstEdges,
		const mesh::indices_t& PointEdges,
		mesh::bools_t& BoundaryEdges,
		mesh::indices_t& AdjacentEdges) :
			m_edge_points(EdgePoints),
			m_clockwise_edges(ClockwiseEdges),
			m_valences(Valences),
			m_first_edges(FirstEdges),
			m_point_edges(PointEdges),
			m_boundary_edges(BoundaryEdges),
			m_adjacent_edges(AdjacentEdges)
		{
		}
	
	void operator()(const k3d::parallel::blocked_range<k3d::uint_t>& range) const
	{
		const k3d::uint_t edge_begin = range.begin();
		const k3d::uint_t edge_end = range.end();
		for(size_t edge = edge_begin; edge != edge_end; ++edge)
		{
			const uint_t vertex1 = m_edge_points[edge];
			const uint_t vertex2 = m_edge_points[m_clockwise_edges[edge]];

			const uint_t first_index = m_first_edges[vertex2];
			const uint_t last_index = first_index + m_valences[vertex2];
			m_adjacent_edges[edge] = edge;
			for(uint_t i = first_index; i != last_index; ++i)
			{
				const uint_t companion = m_point_edges[i];
				if(m_edge_points[m_clockwise_edges[companion]] == vertex1)
				{
					m_boundary_edges[edge] = false;
					m_adjacent_edges[edge] = companion;
					break;
				}
			}
		}
	}
	
private:
	const mesh::indices_t& m_edge_points;
	const mesh::indices_t& m_clockwise_edges;
	const mesh::counts_t& m_valences;
	const mesh::indices_t& m_first_edges;
	const mesh::indices_t& m_point_edges;
	mesh::bools_t& m_boundary_edges;
	mesh::indices_t& m_adjacent_edges;
};

} // namespace detail

void create_edge_adjacency_lookup(const mesh::indices_t& EdgePoints, const mesh::indices_t& ClockwiseEdges, mesh::bools_t& BoundaryEdges, mesh::indices_t& AdjacentEdges)
{
	mesh::counts_t valences;
	create_vertex_valence_lookup(0, EdgePoints, valences);
	mesh::counts_t found_edges(valences.size(), 0);
	mesh::indices_t first_edges(valences.size(), 0); // first edge in point_edges for each point
	mesh::indices_t point_edges(EdgePoints.size(), 0);
	uint_t count = 0;
	for(uint_t point = 0; point != valences.size(); ++point)
	{
		first_edges[point] = count;
		count += valences[point];
	}
	BoundaryEdges.assign(EdgePoints.size(), true);
	AdjacentEdges.assign(EdgePoints.size(), 0);

	const uint_t edge_begin = 0;
	const uint_t edge_end = edge_begin + EdgePoints.size();
	for(uint_t edge = edge_begin; edge != edge_end; ++edge)
	{
		const uint_t point = EdgePoints[edge];
		point_edges[first_edges[point] + found_edges[point]] = edge;
		++found_edges[point];
	}
	
	// Making this parallel decreases running time by 20 % on a Pentium D. 
	k3d::parallel::parallel_for(
				k3d::parallel::blocked_range<k3d::uint_t>(edge_begin, edge_end, k3d::parallel::grain_size()),
				detail::find_companion_worker(EdgePoints, ClockwiseEdges, valences, first_edges, point_edges, BoundaryEdges, AdjacentEdges));
}

void create_edge_face_lookup(const mesh::indices_t& FaceFirstLoops, const mesh::indices_t& FaceLoopCounts, const mesh::indices_t& LoopFirstEdges, const mesh::indices_t& ClockwiseEdges, mesh::indices_t& EdgeFaces)
{
	EdgeFaces.assign(ClockwiseEdges.size(), 0);

	const size_t face_begin = 0;
	const size_t face_end = face_begin + FaceFirstLoops.size();
	for(size_t face = face_begin; face != face_end; ++face)
	{
		const size_t loop_begin = FaceFirstLoops[face];
		const size_t loop_end = loop_begin + FaceLoopCounts[face];
		for(size_t loop = loop_begin; loop != loop_end; ++loop)
		{
			const size_t first_edge = LoopFirstEdges[loop];
			for(size_t edge = first_edge; ;)
			{
				EdgeFaces[edge] = face;

				edge = ClockwiseEdges[edge];
				if(edge == first_edge)
					break;
			}
		}
	}
}

void create_vertex_face_lookup(const mesh::indices_t& FaceFirstLoops, const mesh::indices_t& FaceLoopCounts, const mesh::indices_t& LoopFirstEdges, const mesh::indices_t& EdgePoints, const mesh::indices_t& ClockwiseEdges, const mesh::points_t& Points, mesh::indices_t& PointFirstFaces, mesh::counts_t& PointFaceCounts, mesh::indices_t& PointFaces)
{
	std::vector<std::vector<size_t> > adjacency_list(Points.size());

	const size_t face_begin = 0;
	const size_t face_end = face_begin + FaceFirstLoops.size();
	for(size_t face = face_begin; face != face_end; ++face)
	{
		const size_t loop_begin = FaceFirstLoops[face];
		const size_t loop_end = loop_begin + FaceLoopCounts[face];
		for(size_t loop = loop_begin; loop != loop_end; ++loop)
		{
			const size_t first_edge = LoopFirstEdges[loop];
			for(size_t edge = first_edge; ;)
			{
				adjacency_list[EdgePoints[edge]].push_back(face);

				edge = ClockwiseEdges[edge];
				if(edge == first_edge)
					break;
			}
		}
	}

	PointFirstFaces.assign(Points.size(), 0);
	PointFaceCounts.assign(Points.size(), 0);
	PointFaces.clear();

	const size_t point_begin = 0;
	const size_t point_end = point_begin + Points.size();
	for(size_t point = point_begin; point != point_end; ++point)
	{
		PointFirstFaces[point] = PointFaces.size();
		PointFaceCounts[point] = adjacency_list[point].size();
		PointFaces.insert(PointFaces.end(), adjacency_list[point].begin(), adjacency_list[point].end());
	}
}

void create_vertex_valence_lookup(const uint_t PointCount, const mesh::indices_t& EdgePoints, mesh::counts_t& Valences)
{
	Valences.assign(PointCount, 0);
	
	// Add 1 for each edge that starts at a point
	uint_t edge_count = EdgePoints.size();
	for (uint_t edge = 0; edge != edge_count; ++edge)
	{
		const uint_t edge_point = EdgePoints[edge];
		if(edge_point >= Valences.size()) // In case PointCount was not known to the caller
			Valences.resize(edge_point + 1, 0);
		++Valences[edge_point];
	}
}

void create_boundary_face_lookup(const mesh::indices_t& FaceFirstLoops, const mesh::indices_t& FaceLoopCounts, const mesh::indices_t& LoopFirstEdges, const mesh::indices_t& ClockwiseEdges, const mesh::bools_t& BoundaryEdges, const mesh::indices_t& AdjacentEdges, mesh::bools_t& BoundaryFaces)
{
	BoundaryFaces.clear();
	BoundaryFaces.resize(FaceFirstLoops.size());
	
	const size_t face_begin = 0;
	const size_t face_end = face_begin + FaceFirstLoops.size();
	for(size_t face = face_begin; face != face_end; ++face)
	{
		const size_t loop_begin = FaceFirstLoops[face];
		const size_t loop_end = loop_begin + FaceLoopCounts[face];
		for(size_t loop = loop_begin; loop != loop_end; ++loop)
		{
			const size_t first_edge = LoopFirstEdges[loop];
			for(size_t edge = first_edge; ;)
			{
				if (BoundaryEdges[edge])
				{
					BoundaryFaces[face] = true;
					break;
				}

				edge = ClockwiseEdges[edge];
				if(edge == first_edge)
					break;
			}
		}
	}
}

namespace detail
{

/// Helper function used by lookup_unused_points()
void mark_used_points(const mesh::indices_t& PrimitivePoints, mesh::bools_t& UnusedPoints)
{
	const uint_t begin = 0;
	const uint_t end = PrimitivePoints.size();
	for(uint_t i = begin; i != end; ++i)
		UnusedPoints[PrimitivePoints[i]] = false;
}

/// Helper object used by lookup_unused_points()
struct mark_used_primitive_points
{
	mark_used_primitive_points(mesh::bools_t& UnusedPoints) :
		unused_points(UnusedPoints)
	{
	}

	void operator()(const string_t&, const pipeline_data<array>& Array)
	{
		if(Array->get_metadata_value(k3d::mesh_point_indices()) != "true")
			return;

		if(const mesh::indices_t* const array = dynamic_cast<const mesh::indices_t*>(Array.get()))
			mark_used_points(*array, unused_points);
	}

	mesh::bools_t& unused_points;
};

} // namespace detail

void lookup_unused_points(const mesh& Mesh, mesh::bools_t& UnusedPoints)
{
	UnusedPoints.assign(Mesh.points ? Mesh.points->size() : 0, true);

	// Mark points used by legacy primitives ...
	if(Mesh.nurbs_curve_groups && Mesh.nurbs_curve_groups->curve_points)
		detail::mark_used_points(*Mesh.nurbs_curve_groups->curve_points, UnusedPoints);

	if(Mesh.bilinear_patches && Mesh.bilinear_patches->patch_points)
		detail::mark_used_points(*Mesh.bilinear_patches->patch_points, UnusedPoints);

	if(Mesh.bicubic_patches && Mesh.bicubic_patches->patch_points)
		detail::mark_used_points(*Mesh.bicubic_patches->patch_points, UnusedPoints);

	if(Mesh.nurbs_patches && Mesh.nurbs_patches->patch_points)
		detail::mark_used_points(*Mesh.nurbs_patches->patch_points, UnusedPoints);

	if(Mesh.polyhedra && Mesh.polyhedra->edge_points)
		detail::mark_used_points(*Mesh.polyhedra->edge_points, UnusedPoints);

	// Mark points used by generic mesh primtiives ...
	visit_primitive_arrays(Mesh, detail::mark_used_primitive_points(UnusedPoints));
}

namespace detail
{

/// Helper function used by delete_unused_points()
void remap_points(mesh::indices_t& PrimitivePoints, const mesh::indices_t& PointMap)
{
	const uint_t begin = 0;
	const uint_t end = PrimitivePoints.size();
	for(uint_t i = begin; i != end; ++i)
		PrimitivePoints[i] = PointMap[PrimitivePoints[i]];
}

/// Helper object used by delete_unused_points()
struct remap_primitive_points
{
	remap_primitive_points(mesh::indices_t& PointMap) :
		point_map(PointMap)
	{
	}

	void operator()(const string_t&, pipeline_data<array>& Array)
	{
		if(Array->get_metadata_value(k3d::mesh_point_indices()) != "true")
			return;

		if(mesh::indices_t* const array = dynamic_cast<mesh::indices_t*>(&Array.writable()))
			remap_points(*array, point_map);
	}

	mesh::indices_t& point_map;
};

} // namespace detail

void delete_unused_points(mesh& Mesh)
{
	// Create a bitmap marking which points are unused ...
	mesh::bools_t unused_points;
	lookup_unused_points(Mesh, unused_points);

	// Count how many points will be left when we're done ...
	const uint_t points_remaining = std::count(unused_points.begin(), unused_points.end(), false);

	// Create an array that will map from current-point-indices to new-point-indices,
	// taking into account the points that will be removed.
	mesh::indices_t point_map(unused_points.size());

	const uint_t begin = 0;
	const uint_t end = unused_points.size();
	for(uint_t current_index = begin, new_index = begin; current_index != end; ++current_index)
	{
		point_map[current_index] = new_index;
		if(!unused_points[current_index])
			++new_index;
	}

	// Move leftover points (and point selections) into their final positions ...
	mesh::points_t& points = Mesh.points.writable();
	mesh::selection_t& point_selection = Mesh.point_selection.writable();
	for(uint_t i = begin; i != end; ++i)
	{
		if(!unused_points[i])
		{
			points[point_map[i]] = points[i];
			point_selection[point_map[i]] = point_selection[i];
		}
	}

	// Update legacy mesh primitives so they use the correct indices ...
	if(Mesh.nurbs_curve_groups && Mesh.nurbs_curve_groups->curve_points)
		detail::remap_points(Mesh.nurbs_curve_groups.writable().curve_points.writable(), point_map);

	if(Mesh.bilinear_patches && Mesh.bilinear_patches->patch_points)
		detail::remap_points(Mesh.bilinear_patches.writable().patch_points.writable(), point_map);

	if(Mesh.bicubic_patches && Mesh.bicubic_patches->patch_points)
		detail::remap_points(Mesh.bicubic_patches.writable().patch_points.writable(), point_map);

	if(Mesh.nurbs_patches && Mesh.nurbs_patches->patch_points)
		detail::remap_points(Mesh.nurbs_patches.writable().patch_points.writable(), point_map);

	if(Mesh.polyhedra && Mesh.polyhedra->edge_points)
		detail::remap_points(Mesh.polyhedra.writable().edge_points.writable(), point_map);

	// Update generic mesh primitives so they use the correct indices ...
	visit_primitive_arrays(Mesh, detail::remap_primitive_points(point_map));

	// Free leftover memory ...
	points.resize(points_remaining);
	point_selection.resize(points_remaining);
}

} // namespace k3d

