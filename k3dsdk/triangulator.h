#ifndef K3DSDK_TRIANGULATOR_H
#define K3DSDK_TRIANGULATOR_H

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

#include "mesh.h"

namespace k3d
{

/// Provides a template design pattern object for triangulating polygons.
/// To generate triangulated data, derive from k3d::triangulator and
/// override the private virtual methods to process triangles
class triangulator
{
public:
	triangulator();
	~triangulator();

	/// Generates triangles for every polyhedron face in a mesh
	void process(const mesh& SourceMesh);

	/// Generates triangles for a single polyhedron face
	void process(
		const mesh::points_t& Points,
		const mesh::indices_t& FaceFirstLoops,
		const mesh::counts_t& FaceLoopCounts,
		const mesh::indices_t& LoopFirstEdges,
		const mesh::indices_t& EdgePoints,
		const mesh::indices_t& ClockwiseEdges,
		const uint_t Face);

private:
	/// Called once before processin begins on the given mesh
	virtual void start_processing(const mesh& SourceMesh);
	/// Called once before processing begins on the given polygon face
	virtual void start_face(const uint_t Face);
	/// Called anytime the triangulation process needs to create a new vertex (e.g: when edges cross within a self-intersecting polygon)
	virtual void add_vertex(const point3& Coordinates, uint_t Vertices[4], uint_t Edges[4], double_t Weights[4], uint_t& NewVertex);
	/// Called once for each triangle generated
	virtual void add_triangle(uint_t Vertices[3], uint_t Edges[3]);
	/// Called once after processing for the given face has been completed
	virtual void finish_face(const uint_t Face);
	/// Called once after the entire mesh has been processed
	virtual void finish_processing(const mesh& SourceMesh);

	class implementation;
	implementation* const m_implementation;

	friend class implementation;
};

} // namespace k3d

#endif // K3DSDK_TRIANGULATOR_H

