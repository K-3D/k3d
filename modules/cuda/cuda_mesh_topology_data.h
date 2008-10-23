#ifndef MODULES_CUDA_CUDA_MESH_TOPOLOGY_DATA_H
#define MODULES_CUDA_CUDA_MESH_TOPOLOGY_DATA_H

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

#include <k3dsdk/mesh.h>

typedef k3d::typed_array<unsigned char> cuda_bools_t;

namespace k3d
{

/// Initializes arrays for constant-time lookup from an edge to the adjacent edge (if any)
void cuda_create_edge_adjacency_lookup(const k3d::uint32_t* pdev_edgePoints, const k3d::uint32_t* pdev_clockwiseEdges, unsigned char* pdev_boundaryEdges, k3d::uint32_t* pdev_adjacentEdges, int num_edges, int num_points);

/// Initializes an array for constant-time lookup from an edge to the face that owns it
void create_edge_face_lookup(const mesh::indices_t& FaceFirstLoops, const mesh::indices_t& FaceLoopCounts, const mesh::indices_t& LoopFirstEdges, const mesh::indices_t& ClockwiseEdges, mesh::indices_t& EdgeFaces);

/// Initialize arrays for fast lookup from a vertex to its adjacent faces
void create_vertex_face_lookup(const mesh::indices_t& FaceFirstLoops, const mesh::indices_t& FaceLoopCounts, const mesh::indices_t& LoopFirstEdges, const mesh::indices_t& EdgePoints, const mesh::indices_t& ClockwiseEdges, const mesh::points_t& Points, mesh::indices_t& PointFirstFaces, mesh::counts_t& PointFaceCounts, mesh::indices_t& PointFaces);

/// Initialize Valences array for constant time lookup of vertex valence (number of incoming edges)
/**
 * \param PointCount Initial guess for the number of points. Valences will be expanded to the correct size if this is too small
 * \param EdgePoints The indices of the edge points
 * \param Valences will store the number of edges for each point
 */
void create_vertex_valence_lookup(const uint_t PointCount, const mesh::indices_t& EdgePoints, mesh::counts_t& Valences);

/// Initialise boundary_faces array for constant time lookup of faces that are on the mesh boundary. BoundaryEdges and AdjacentEdges can be created using create_edge_adjacency_lookup
void create_boundary_face_lookup(const mesh::indices_t& FaceFirstLoops, const mesh::indices_t& FaceLoopCounts, const mesh::indices_t& LoopFirstEdges, const mesh::indices_t& ClockwiseEdges, const mesh::bools_t& BoundaryEdges, const mesh::indices_t& AdjacentEdges, mesh::bools_t& BoundaryFaces);

} // namespace k3d

#endif // !MODULES_CUDA_CUDA_MESH_TOPOLOGY_DATA_H

