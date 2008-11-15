#ifndef K3DSDK_MESH_TOPOLOGY_DATA_H
#define K3DSDK_MESH_TOPOLOGY_DATA_H

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

#include "mesh.h"

namespace k3d
{

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

/// Initialize an array to mark unused mesh points (points not used by any primitive).
void lookup_unused_points(const mesh& Mesh, mesh::bools_t& UnusedPoints);
/// Remove unused points from a mesh, adjusting point indices in all remaining primitives.
void delete_unused_points(mesh& Mesh);

/// Iterates over every array in every generic primitive in the given mesh, passing the array name and array to a functor.
template<typename FunctorT>
void visit_primitive_arrays(const mesh& Mesh, FunctorT Functor)
{
	for(mesh::primitives_t::const_iterator p = Mesh.primitives.begin(); p != Mesh.primitives.end(); ++p)
	{
		const mesh::primitive& primitive = **p;
		for(mesh::named_arrays_t::const_iterator array = primitive.topology.begin(); array != primitive.topology.end(); ++array)
			Functor(array->first, array->second);

		for(mesh::named_attribute_arrays_t::const_iterator attributes = primitive.attributes.begin(); attributes != primitive.attributes.end(); ++attributes)
		{
			for(mesh::attribute_arrays_t::const_iterator array = attributes->second.begin(); array != attributes->second.end(); ++array)
				Functor(array->first, array->second);
		}
	}
}

/// Iterates over every array in every generic primitive in the given mesh, passing the array name and array to a functor.
template<typename FunctorT>
void visit_primitive_arrays(mesh& Mesh, FunctorT Functor)
{
	for(mesh::primitives_t::iterator p = Mesh.primitives.begin(); p != Mesh.primitives.end(); ++p)
	{
		mesh::primitive& primitive = p->writable();
		for(mesh::named_arrays_t::iterator array = primitive.topology.begin(); array != primitive.topology.end(); ++array)
			Functor(array->first, array->second);

		for(mesh::named_attribute_arrays_t::iterator attributes = primitive.attributes.begin(); attributes != primitive.attributes.end(); ++attributes)
		{
			for(mesh::attribute_arrays_t::iterator array = attributes->second.begin(); array != attributes->second.end(); ++array)
				Functor(array->first, array->second);
		}
	}
}

} // namespace k3d

#endif // !K3DSDK_MESH_TOPOLOGY_DATA_H
