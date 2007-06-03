#ifndef MESH_HELPERS_H
#define MESH_HELPERS_H

// K-3D
// Copyright (c) 2005, Romain Behar
//
// Contact: romainbehar@yahoo.com
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
		\brief Gathers common mesh functions
		\author Romain Behar (romainbehar@yahoo.com)
*/

#include <k3dsdk/legacy_mesh.h>
#include <k3dsdk/result.h>

#include <map>

namespace libk3dpolyhedra
{

namespace helpers
{

/// Counts edges in edge loop
const unsigned long edge_number(k3d::legacy::split_edge* Edge);

/// Returns whether Edge belongs to Face
bool edge_in_face(k3d::legacy::split_edge* Edge, k3d::legacy::split_edge* Face);

/// Returns vertex valency for interior points, 0 for border points
const unsigned long vertex_valency(k3d::legacy::split_edge* Edge);

/// Inserts a new edge after Edge, returns new point
k3d::legacy::point* append_edge(k3d::legacy::split_edge* Edge);

/// Returns the center point (as an average) for a face
k3d::point3 center_point(const k3d::legacy::face& Face);

/// Creates a new face to fill a hole given its surrounding edge loop
// Edge list follows outer edges, parameters must be a reverse list (.rbegin(), .rend)
template<typename ReverseIteratorType>
inline k3d::legacy::face* fill_hole(ReverseIteratorType RBegin, ReverseIteratorType REnd, k3d::imaterial* Material)
{
	return_val_if_fail(RBegin != REnd, 0);

	typedef std::vector<k3d::legacy::split_edge*> edges_t;
	edges_t new_edges;

	ReverseIteratorType i = RBegin;
	do
	{
		k3d::legacy::split_edge* new_edge = new k3d::legacy::split_edge((*i)->face_clockwise->vertex);

		// Set companions
		k3d::legacy::join_edges(**i, *new_edge);

		new_edges.push_back(new_edge);
	}
	while(++i != REnd);

	// Create new face
	k3d::legacy::loop_edges(new_edges.begin(), new_edges.end());
	k3d::legacy::face* face = new k3d::legacy::face(new_edges.front(), Material);

	return face;
}

} // namespace helpers

} // namespace libk3dpolyhedra

#endif // MESH_HELPERS_H

