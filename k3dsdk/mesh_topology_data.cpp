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

#include "mesh_topology_data.h"

namespace k3d
{

void create_edge_adjacency_lookup(const mesh::indices_t& EdgePoints, const mesh::indices_t& ClockwiseEdges, mesh::bools_t& BoundaryEdges, mesh::indices_t& AdjacentEdges)
{
	BoundaryEdges.assign(EdgePoints.size(), true);
	AdjacentEdges.assign(EdgePoints.size(), 0);

	typedef std::map<std::pair<size_t, size_t>, size_t> adjacent_edges_t;
	adjacent_edges_t adjacent_edges;

	const size_t edge_begin = 0;
	const size_t edge_end = edge_begin + EdgePoints.size();
	for(size_t edge = edge_begin; edge != edge_end; ++edge)
	{
		const size_t vertex1 = EdgePoints[edge];
		const size_t vertex2 = EdgePoints[ClockwiseEdges[edge]];
		adjacent_edges.insert(std::make_pair(std::make_pair(vertex1, vertex2), edge));
	}

	for(size_t edge = edge_begin; edge != edge_end; ++edge)
	{
		const size_t vertex1 = EdgePoints[edge];
		const size_t vertex2 = EdgePoints[ClockwiseEdges[edge]];

		const adjacent_edges_t::const_iterator adjacent_edge_iterator = adjacent_edges.find(std::make_pair(vertex2, vertex1));
		if(adjacent_edge_iterator != adjacent_edges.end())
		{
			const size_t adjacent_edge = adjacent_edge_iterator->second;

			BoundaryEdges[adjacent_edge] = false;
			AdjacentEdges[adjacent_edge] = edge;
		}
	}
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
	// Default to 0 for all points
	Valences.clear();
	Valences.resize(PointCount, 0);
		
	// Add 1 for each edge that starts at a point
	uint_t edge_count = EdgePoints.size();
	for (uint_t edge = 0; edge != edge_count; ++edge)
	{
		++Valences[EdgePoints[edge]];
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
