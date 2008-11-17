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

#include "euler_operations.h"
#include "polyhedron.h"

#include <set>

namespace k3d
{

namespace euler
{

const uint_t number(const mesh& Mesh, const uint_t Polyhedron)
{
	return_val_if_fail(Polyhedron < polyhedron::count(Mesh), 0);
	return_val_if_fail(polyhedron::is_solid(Mesh, Polyhedron), 0);

	const mesh::indices_t& first_faces = *Mesh.polyhedra->first_faces;
	const mesh::counts_t& face_counts = *Mesh.polyhedra->face_counts;
	const mesh::indices_t& face_first_loops = *Mesh.polyhedra->face_first_loops;
	const mesh::counts_t& face_loop_counts = *Mesh.polyhedra->face_loop_counts;
	const mesh::indices_t& loop_first_edges = *Mesh.polyhedra->loop_first_edges;
	const mesh::indices_t& edge_points = *Mesh.polyhedra->edge_points;
	const mesh::indices_t& clockwise_edges = *Mesh.polyhedra->clockwise_edges;

	uint_t vertex_count = 0;
	uint_t edge_count = 0;
	uint_t face_count = face_counts[Polyhedron];
	uint_t loop_count = 0;

	std::set<uint_t> vertices;
	const uint_t face_begin = first_faces[Polyhedron];
	const uint_t face_end = face_begin + face_counts[Polyhedron];
	for(uint_t face = face_begin; face != face_end; ++face)
	{
		loop_count += face_loop_counts[face];

		const uint_t loop_begin = face_first_loops[face];
		const uint_t loop_end = loop_begin + face_loop_counts[face];
		for(uint_t loop = loop_begin; loop != loop_end; ++loop)
		{
			const uint_t first_edge = loop_first_edges[loop];
			for(uint_t edge = first_edge; ;)
			{
				vertices.insert(edge_points[edge]);

				++edge_count;

				edge = clockwise_edges[edge];
				if(edge == first_edge)
					break;
			}
		}
	}
	vertex_count = vertices.size();
	edge_count /= 2;

	return vertex_count - edge_count + face_count - (loop_count - face_count);
}

} // namespace euler

} // namespace k3d

