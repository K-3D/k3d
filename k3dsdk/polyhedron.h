#ifndef K3DSDK_POLYHEDRON_H
#define K3DSDK_POLYHEDRON_H

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
	\author Timothy M. Shead (tshead@k-3d.com)
	\author Bart Janssens (bart.janssens@lid.kviv.be)
*/

#include "mesh.h"

namespace k3d
{

namespace polyhedron
{

/// Defines allowable polyhedron types
enum polyhedron_type
{
	POLYGONS = 0,
	CATMULL_CLARK = 1,
};

/// Gathers the member arrays of a polyhedron primitive into a convenient package
class const_primitive
{
public:
	const_primitive(
		const mesh::indices_t& FirstFaces,
		const mesh::counts_t& FaceCounts,
//		const typed_array<int32_t>& PolyhedronTypes,
		const mesh::polyhedra_t::types_t& PolyhedronTypes,
		const mesh::indices_t& FaceFirstLoops,
		const mesh::counts_t& FaceLoopCounts,
		const mesh::selection_t& FaceSelections,
		const mesh::materials_t& FaceMaterials,
		const mesh::indices_t& LoopFirstEdges,
		const mesh::indices_t& EdgePoints,
		const mesh::indices_t& ClockwiseEdges,
		const mesh::selection_t& EdgeSelections,
		const mesh::attribute_arrays_t& ConstantData,
		const mesh::attribute_arrays_t& UniformData,
		const mesh::attribute_arrays_t& FaceVaryingData
		);

	const mesh::indices_t& first_faces;
	const mesh::counts_t& face_counts;
//	const typed_array<int32_t>& polyhedron_types;
	const mesh::polyhedra_t::types_t& polyhedron_types;
	const mesh::indices_t& face_first_loops;
	const mesh::counts_t& face_loop_counts;
	const mesh::selection_t& face_selections;
	const mesh::materials_t& face_materials;
	const mesh::indices_t& loop_first_edges;
	const mesh::indices_t& edge_points;
	const mesh::indices_t& clockwise_edges;
	const mesh::selection_t& edge_selections;
	const mesh::attribute_arrays_t& constant_data;
	const mesh::attribute_arrays_t& uniform_data;
	const mesh::attribute_arrays_t& face_varying_data;
};

/// Gathers the member arrays of a polyhedron primitive into a convenient package
class primitive
{
public:
	primitive(
		mesh::indices_t& FirstFaces,
		mesh::counts_t& FaceCounts,
//		typed_array<int32_t>& PolyhedronTypes,
		mesh::polyhedra_t::types_t& PolyhedronTypes,
		mesh::indices_t& FaceFirstLoops,
		mesh::counts_t& FaceLoopCounts,
		mesh::selection_t& FaceSelections,
		mesh::materials_t& FaceMaterials,
		mesh::indices_t& LoopFirstEdges,
		mesh::indices_t& EdgePoints,
		mesh::indices_t& ClockwiseEdges,
		mesh::selection_t& EdgeSelections,
		mesh::attribute_arrays_t& ConstantData,
		mesh::attribute_arrays_t& UniformData,
		mesh::attribute_arrays_t& FaceVaryingData
		);

	mesh::indices_t& first_faces;
	mesh::counts_t& face_counts;
//	typed_array<int32_t>& polyhedron_types;
	mesh::polyhedra_t::types_t& polyhedron_types;
	mesh::indices_t& face_first_loops;
	mesh::counts_t& face_loop_counts;
	mesh::selection_t& face_selections;
	mesh::materials_t& face_materials;
	mesh::indices_t& loop_first_edges;
	mesh::indices_t& edge_points;
	mesh::indices_t& clockwise_edges;
	mesh::selection_t& edge_selections;
	mesh::attribute_arrays_t& constant_data;
	mesh::attribute_arrays_t& uniform_data;
	mesh::attribute_arrays_t& face_varying_data;
};

/// Creates a new polyhedron mesh primitive, returning references to its member arrays.
/// The caller is responsible for the lifetime of the returned object.
primitive* create(mesh& Mesh);

/// Creates a new polyhedron mesh primitive from a list of vertices and face indices, returning references to its member arrays.
/// The caller is responsible for the lifetime of the returned object.
primitive* create(mesh& Mesh, const mesh::points_t& Vertices, const mesh::counts_t& VertexCounts, const mesh::indices_t& VertexIndices);

/// Initializes arrays for constant-time lookup from an edge to the adjacent edge (if any)
void create_edge_adjacency_lookup(const mesh::indices_t& EdgePoints, const mesh::indices_t& ClockwiseEdges, mesh::bools_t& BoundaryEdges, mesh::indices_t& AdjacentEdges);

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

/// Returns the number of polyhedra (if any) in a mesh
const uint_t count(const mesh& Mesh);

/// Returns true iff a polyhedron is a solid volume.
const bool_t is_solid(const mesh& Mesh, const uint_t Polyhedron);

} // namespace polyhedron

} // namespace k3d

#endif // !K3DSDK_POLYHEDRON_H
