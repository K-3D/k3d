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

#include "mesh_operations.h"
#include "polyhedra.h"

namespace k3d
{

namespace polyhedron
{

const uint_t count(const mesh& Mesh)
{
	if(!validate_polyhedra(Mesh))
		return 0;

	return Mesh.polyhedra->first_faces->size();
}

const bool_t is_solid(const mesh& Mesh, const uint_t Polyhedron)
{
	// K-3D uses a split-edge data structure to represent polyhedra.
	// We test for solidity by counting the number of edges that
	// connect each pair of points in the polyhedron.  A polyhedron is 
	// solid if-and-only-if each pair of points is used by two edges.

	return_val_if_fail(Polyhedron < count(Mesh), false);

	const mesh::indices_t& first_faces = *Mesh.polyhedra->first_faces;
	const mesh::counts_t& face_counts = *Mesh.polyhedra->face_counts;
	const mesh::indices_t& face_first_loops = *Mesh.polyhedra->face_first_loops;
	const mesh::counts_t& face_loop_counts = *Mesh.polyhedra->face_loop_counts;
	const mesh::indices_t& loop_first_edges = *Mesh.polyhedra->loop_first_edges;
	const mesh::indices_t& edge_points = *Mesh.polyhedra->edge_points;
	const mesh::indices_t& clockwise_edges = *Mesh.polyhedra->clockwise_edges;

	typedef std::map<std::pair<uint_t, uint_t>, uint_t> adjacent_edges_t;
	adjacent_edges_t adjacent_edges;

	const uint_t face_begin = first_faces[Polyhedron];
	const uint_t face_end = face_begin + face_counts[Polyhedron];
	for(uint_t face = face_begin; face != face_end; ++face)
	{
		const uint_t loop_begin = face_first_loops[face];
		const uint_t loop_end = loop_begin + face_loop_counts[face];
		for(uint_t loop = loop_begin; loop != loop_end; ++loop)
		{
			const uint_t first_edge = loop_first_edges[loop];
			for(uint_t edge = first_edge; ;)
			{
				const uint_t vertex1 = std::min(edge_points[edge], edge_points[clockwise_edges[edge]]);
				const uint_t vertex2 = std::max(edge_points[edge], edge_points[clockwise_edges[edge]]);
				adjacent_edges[std::make_pair(vertex1, vertex2)] += 1;

				edge = clockwise_edges[edge];
				if(edge == first_edge)
					break;
			}
		}
	}

	for(adjacent_edges_t::iterator edges = adjacent_edges.begin(); edges != adjacent_edges.end(); ++edges)
	{
		if(edges->second != 2)
			return false;
	}

	return true;
}

} // namespace polyhedron

} // namespace k3d

