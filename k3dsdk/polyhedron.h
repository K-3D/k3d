#ifndef K3DSDK_POLYHEDRON_H
#define K3DSDK_POLYHEDRON_H

// K-3D
// Copyright (c) 1995-2009, Timothy M. Shead
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

/** \file
	\author Timothy M. Shead (tshead@k-3d.com)
	\author Bart Janssens (bart.janssens@lid.kviv.be)
*/

#include <k3dsdk/mesh.h>

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

class primitive;

/// Gathers the member arrays of a polyhedron primitive into a convenient package
class const_primitive
{
public:
	const_primitive(
		const mesh::indices_t& ShellFirstFaces,
		const mesh::counts_t& ShellFaceCounts,
		const typed_array<int32_t>& ShellTypes,
		const mesh::indices_t& FaceFirstLoops,
		const mesh::counts_t& FaceLoopCounts,
		const mesh::selection_t& FaceSelections,
		const mesh::materials_t& FaceMaterials,
		const mesh::indices_t& LoopFirstEdges,
		const mesh::indices_t& ClockwiseEdges,
		const mesh::selection_t& EdgeSelections,
		const mesh::indices_t& VertexPoints,
		const mesh::selection_t& VertexSelections,
		const mesh::table_t& ConstantAttributes,
		const mesh::table_t& FaceAttributes,
		const mesh::table_t& EdgeAttributes,
		const mesh::table_t& VertexAttributes
		);

	/// Implicit conversion
	const_primitive(const primitive& Primitive);

	const mesh::indices_t& shell_first_faces;
	const mesh::counts_t& shell_face_counts;
	const typed_array<int32_t>& shell_types;
	const mesh::indices_t& face_first_loops;
	const mesh::counts_t& face_loop_counts;
	const mesh::selection_t& face_selections;
	const mesh::materials_t& face_materials;
	const mesh::indices_t& loop_first_edges;
	const mesh::indices_t& clockwise_edges;
	const mesh::selection_t& edge_selections;
	const mesh::indices_t& vertex_points;
	const mesh::selection_t& vertex_selections;
	const mesh::table_t& constant_attributes;
	const mesh::table_t& face_attributes;
	const mesh::table_t& edge_attributes;
	const mesh::table_t& vertex_attributes;
};

/// Gathers the member arrays of a polyhedron primitive into a convenient package
class primitive
{
public:
	primitive(
		mesh::indices_t& ShellFirstFaces,
		mesh::counts_t& ShellFaceCounts,
		typed_array<int32_t>& ShellTypes,
		mesh::indices_t& FaceFirstLoops,
		mesh::counts_t& FaceLoopCounts,
		mesh::selection_t& FaceSelections,
		mesh::materials_t& FaceMaterials,
		mesh::indices_t& LoopFirstEdges,
		mesh::indices_t& ClockwiseEdges,
		mesh::selection_t& EdgeSelections,
		mesh::indices_t& VertexPoints,
		mesh::selection_t& VertexSelections,
		mesh::table_t& ConstantAttributes,
		mesh::table_t& FaceAttributes,
		mesh::table_t& EdgeAttributes,
		mesh::table_t& VertexAttributes
		);

	mesh::indices_t& shell_first_faces;
	mesh::counts_t& shell_face_counts;
	typed_array<int32_t>& shell_types;
	mesh::indices_t& face_first_loops;
	mesh::counts_t& face_loop_counts;
	mesh::selection_t& face_selections;
	mesh::materials_t& face_materials;
	mesh::indices_t& loop_first_edges;
	mesh::indices_t& clockwise_edges;
	mesh::selection_t& edge_selections;
	mesh::indices_t& vertex_points;
	mesh::selection_t& vertex_selections;
	mesh::table_t& constant_attributes;
	mesh::table_t& face_attributes;
	mesh::table_t& edge_attributes;
	mesh::table_t& vertex_attributes;
};

/// Creates a new polyhedron mesh primitive, returning references to its member arrays.
/// The caller is responsible for the lifetime of the returned object.
primitive* create(mesh::primitive& GenericPrimitive);

/// Creates a new polyhedron mesh primitive, returning references to its member arrays.
/// The caller is responsible for the lifetime of the returned object.
primitive* create(mesh& Mesh);

/// Creates a new polyhedron mesh primitive from a list of vertices and face indices, returning references to its member arrays.
/// The caller is responsible for the lifetime of the returned object.
primitive* create(mesh& Mesh, const mesh::points_t& Vertices, const mesh::counts_t& VertexCounts, const mesh::indices_t& VertexIndices, imaterial* const Material);

/// Creates a new polyhedron mesh primitive from a list of vertices, texture coordinates, and face indices, returning references to its member arrays.
/// The caller is responsible for the lifetime of the returned object.
primitive* create(mesh& Mesh, const mesh::points_t& Vertices, const mesh::counts_t& VertexCounts, const mesh::indices_t& VertexIndices, const mesh::texture_coordinates_t& TextureCoordinates, imaterial* const Material);

/// Tests the given mesh primitive to see if it is a valid polyhedron, returning references to its member arrays, or NULL.
/// The caller is responsible for the lifetime of the returned object.
const_primitive* validate(const mesh& Mesh, const mesh::primitive& GenericPrimitive);
/// Tests the given mesh primitive to see if it is a valid polyhedron, returning references to its member arrays, or NULL.
/// The caller is responsible for the lifetime of the returned object.
primitive* validate(const mesh& Mesh, mesh::primitive& GenericPrimitive);
/// Tests the given mesh primitive to see if it is a valid polyhedron, returning references to its member arrays, or NULL.
/// The caller is responsible for the lifetime of the returned object.
primitive* validate(const mesh& Mesh, pipeline_data<mesh::primitive>& GenericPrimitive);

/// Adds a triangle to an existing polyhedron.  Preconditions: the polyhedron must contain exactly one shell.
void add_triangle(mesh& Mesh, primitive& Polyhedron, uint_t V1, uint_t V2, uint_t V3, imaterial* const Material);
/// Adds a quadrilateral to an existing polyhedron.  Preconditions: the polyhedron must contain exactly one shell.
void add_quadrilateral(mesh& Mesh, primitive& Polyhedron, uint_t V1, uint_t V2, uint_t V3, uint_t V4, imaterial* const Material);
/// Adds a face to an existing polyhedron.  Preconditions: the polyhedron must contain exactly one shell.
void add_face(mesh& Mesh, primitive& Polyhedron, const mesh::points_t& Vertices, imaterial* const Material);
/// Adds a face with holes to an existing polyhedron.  Preconditions: the polyhedron must contain exactly one shell.
void add_face(mesh& Mesh, primitive& Polyhedron, const mesh::points_t& Vertices, const std::vector<mesh::points_t>& Holes, imaterial* const Material);
/// Adds a set of points and faces that form a topological "grid" with the given number of rows and columns to an existing polyhedron.
/// The geometry (vertex coordinates) of the grid is undefined, and must be set by the caller after add_grid() returns.
/// Preconditions: the polyhedron must contain exactly one shell.  Postconditions: the polyhedron contains Rows x Columns new faces and
/// Rows x Columns x 4 new half edges, and the mesh contains (Rows + 1) x (Columns + 1) new points.
void add_grid(mesh& Mesh, primitive& Polyhedron, const uint_t Rows, const uint_t Columns, imaterial* const Material);
/// Adds a set of points and faces that form a topological "cylinder" with the given number of rows and columns to an existing polyhedron.
/// The geometry (vertex coordinates) of the cylinder is undefined, and must be set by the caller after add_cylinder() returns.
/// Note that many surfaces-of-revolution can be represented as topological cylinders, including disks, cones, cylinders,
/// hyperboloids, paraboloids, and spheres.
/// Preconditions: the polyhedron must contain exactly one shell.  Postconditions: the polyhedron contains Rows x Columns new faces and
/// Rows x Columns x 4 new half edges, and the mesh contains (Rows + 1) x Columns new points.
void add_cylinder(mesh& Mesh, primitive& Polyhedron, const uint_t Rows, const uint_t Columns, imaterial* const Material);
/// Adds a set of points and faces that form a topological "torus" with the given number of rows and columns to an existing polyhedron.
/// The geometry (vertex coordinates) of the torus is undefined, and must be set by the caller after add_torus() returns.
/// Preconditions: the polyhedron must contain exactly one shell.  Postconditions: the polyhedron contains Rows x Columns new faces and
/// Rows x Columns x 4 new half edges, and the mesh contains Rows x Columns new points.
void add_torus(mesh& Mesh, primitive& Polyhedron, const uint_t Rows, const uint_t Columns, imaterial* const Material);

/// Returns true iff every face in the given polyhedron is a triangle.
bool_t is_triangles(const const_primitive& Polyhedron);
/// Returns true iff the given polyhedron is a solid (i.e. it has no topological holes).
bool_t is_solid(const const_primitive& Polyhedron);
/// Returns true iff the given polyhedron should be rendered as a Subdivision surface
/** \note The implementation looks at the flags set on the first (outer) shell.  We
should consider whether SDS should be set on a per-shell basis? */
bool_t is_sds(const const_primitive& Polyhedron);

/// Returns true iff the given edges are on the same loop.
bool_t same_loop(const mesh::indices_t& ClockwiseEdges, const uint_t EdgeA, const uint_t EdgeB);
/// Returns the previous (counterclockwise) edge on the same loop.
uint_t counterclockwise_edge(const mesh::indices_t& ClockwiseEdges, const uint_t Edge);

/// Calculates the center (average) for an edge loop (returns the origin for degenerate cases).
const point3 center(const mesh::indices_t& EdgePoints, const mesh::indices_t& ClockwiseEdges, const mesh::points_t& Points, const uint_t EdgeIndex);
/// Calculates the normal for an edge loop (returns a zero-length normal for degenerate cases).
const normal3 normal(const mesh::indices_t& EdgePoints, const mesh::indices_t& ClockwiseEdges, const mesh::points_t& Points, const uint_t EdgeIndex);
/// Calculates the normal for a triangle (returns a zero-length normal for degenerate cases).
const normal3 normal(const point3& A, const point3& B, const point3& C);

/// Initializes arrays for constant-time lookup from an edge to the adjacent edge (if any)
void create_edge_adjacency_lookup(const mesh::indices_t& EdgePoints, const mesh::indices_t& ClockwiseEdges, mesh::bools_t& BoundaryEdges, mesh::indices_t& AdjacentEdges);

/// Initializes an array for constant-time lookup from an edge to the face that owns it
void create_edge_face_lookup(const mesh::indices_t& FaceFirstLoops, const mesh::indices_t& FaceLoopCounts, const mesh::indices_t& LoopFirstEdges, const mesh::indices_t& ClockwiseEdges, mesh::indices_t& EdgeFaces);

/// Initializes an array for constant-time lookup from a loop to its edge count
void create_edge_count_lookup(const mesh::indices_t& LoopFirstEdges, const mesh::indices_t& ClockwiseEdges, mesh::counts_t& Counts);

/// Initialize arrays for fast lookup from a vertex to its adjacent faces
void create_vertex_face_lookup(const mesh::indices_t& FaceFirstLoops, const mesh::indices_t& FaceLoopCounts, const mesh::indices_t& LoopFirstEdges, const mesh::indices_t& EdgePoints, const mesh::indices_t& ClockwiseEdges, const mesh::points_t& Points, mesh::indices_t& PointFirstFaces, mesh::counts_t& PointFaceCounts, mesh::indices_t& PointFaces);
/// Initialize arrays for fast lookup from a point index to all edges that start from it. If PointEdgeCounts is filled (by create_vertex_valence_lookup) it is used, otherwise it is created
void create_vertex_edge_lookup(const mesh::indices_t& EdgePoints, mesh::indices_t& PointEdges, mesh::indices_t& PointFirstEdges, mesh::counts_t& PointEdgeCounts);

/// Initialize Valences array for constant time lookup of vertex valence (number of incoming edges)
/**
 * \param PointCount Initial guess for the number of points. Valences will be expanded to the correct size if this is too small
 * \param EdgePoints The indices of the edge points
 * \param Valences will store the number of edges for each point
 */
void create_vertex_valence_lookup(const uint_t PointCount, const mesh::indices_t& EdgePoints, mesh::counts_t& Valences);

/// Initialise boundary_faces array for constant time lookup of faces that are on the mesh boundary. BoundaryEdges and AdjacentEdges can be created using create_edge_adjacency_lookup
void create_boundary_face_lookup(const mesh::indices_t& FaceFirstLoops, const mesh::indices_t& FaceLoopCounts, const mesh::indices_t& LoopFirstEdges, const mesh::indices_t& ClockwiseEdges, const mesh::bools_t& BoundaryEdges, const mesh::indices_t& AdjacentEdges, mesh::bools_t& BoundaryFaces);

/// Adds edges that are collinear and with points of valence 1 for boundary edges or valence 2 otherwise to EdgeList
void mark_collinear_edges(mesh::indices_t& RedundantEdges, const mesh::selection_t& EdgeSelection, const mesh::points_t& Points, const mesh::indices_t& EdgePoints, const mesh::indices_t& ClockwiseEdges, const mesh::counts_t& VertexValences, const mesh::bools_t& BoundaryEdges, const mesh::indices_t& AdjacentEdges, const double_t Threshold = 1e-8);

/// Marks edges that are shared by coplanar faces among the selected faces
void mark_coplanar_edges(const mesh::indices_t& Companions,
		const mesh::bools_t& BoundaryEdges,
		const mesh::normals_t& Normals,
		const mesh::indices_t& EdgeFaces,
		const mesh::selection_t& FaceSelection,
		mesh::indices_t& RedundantEdges,
		const double_t Threshold = 1e-8);

/// Triangulates the input polyhedron, storing the resulting primitive and storing point data in Output
mesh::primitive* triangulate(const mesh& Input, const const_primitive& Polyhedron, mesh& Output);

} // namespace polyhedron

} // namespace k3d

#endif // !K3DSDK_POLYHEDRON_H
