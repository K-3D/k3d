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
 * 		From k3dsdk/mesh_topology data.  CUDA implementation
	\author Evan Lezar (evanlezar@gmail.com)
*/

#include "cuda_mesh_topology_data.h"
#include "cuda_entry_points.h"

#include <k3dsdk/parallel/blocked_range.h>
#include <k3dsdk/parallel/parallel_for.h>
#include <k3dsdk/parallel/threads.h>

namespace k3d
{

namespace detail
{

void cuda_find_companion_worker(const mesh::indices_t& EdgePoints,
								const mesh::indices_t& ClockwiseEdges,
								const mesh::counts_t& Valences,
								const mesh::indices_t& FirstEdges,
								const mesh::indices_t& PointEdges,
								mesh::bools_t& BoundaryEdges,
								mesh::indices_t& AdjacentEdges);

class find_companion_worker
{
public:
	find_companion_worker(const mesh::indices_t& EdgePoints,
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
			{}

	void operator()(const k3d::parallel::blocked_range<k3d::uint_t>& range) const
	{
		const k3d::uint_t edge_begin = range.begin();
		const k3d::uint_t edge_end = range.end();
		for(size_t edge = edge_begin; edge != edge_end; ++edge)
		{
			const uint_t vertex1 = m_edge_points[edge];
			const uint_t vertex2 = m_edge_points[m_clockwise_edges[edge]];
			/// Calculates the split points positions for each edge

			const uint_t first_index = m_first_edges[vertex2];
			const uint_t last_index = first_index + m_valences[vertex2];
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

void cuda_create_edge_adjacency_lookup(const k3d::uint32_t* pdev_edgePoints, const k3d::uint32_t* pdev_clockwiseEdges, unsigned char* pdev_boundaryEdges, k3d::uint32_t* pdev_adjacentEdges, int num_edges, int num_points)
{
	k3d::uint32_t* pdev_valences;
	k3d::uint32_t* pdev_found_edges;
	k3d::uint32_t* pdev_first_edges;
	k3d::uint32_t* pdev_point_edges;
	// since it is not known how many valence points the edges refer to, allocate maximum (all edges disconnected)
	allocate_device_memory((void**)&pdev_valences, num_edges*2*sizeof(k3d::uint32_t));

	int valence_size = create_vertex_valence_lookup_kernel_entry ( pdev_valences, (const unsigned int*)pdev_edgePoints, num_edges );


	allocate_device_memory((void**)&pdev_first_edges, valence_size*sizeof(k3d::uint32_t));

	allocate_device_memory((void**)&pdev_found_edges, valence_size*sizeof(k3d::uint32_t));
	allocate_device_memory((void**)&pdev_point_edges, num_edges*sizeof(k3d::uint32_t));

	synchronize_threads();
	calculate_first_edge_entry ( (unsigned int*)pdev_first_edges, ( const unsigned int*)pdev_valences, valence_size );

	calculate_point_edges_entry ((unsigned int*) pdev_point_edges,
								(unsigned int*) pdev_found_edges,
								(const unsigned int*) pdev_edgePoints,
								(const unsigned int*) pdev_first_edges,
								num_edges,
								valence_size);

	find_companion_kernel_entry ( pdev_boundaryEdges,
								  pdev_adjacentEdges,
								  num_edges,
								  pdev_edgePoints,
								  pdev_clockwiseEdges,
								  pdev_first_edges,
								  (unsigned int*) pdev_valences,
								  pdev_point_edges );

	free_device_memory(pdev_valences);
	free_device_memory(pdev_found_edges);
	free_device_memory(pdev_first_edges);
	free_device_memory(pdev_point_edges);

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

} // namespace k3d
