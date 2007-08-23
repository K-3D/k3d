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
		\brief Implements common mesh functions
		\author Romain Behar (romainbehar@yahoo.com)
*/

#include <k3dsdk/result.h>

#include "helpers.h"

#include <set>
#include <utility>

namespace libk3dmesh
{

namespace helpers
{

/// Counts edges in edge loop
const unsigned long edge_number(k3d::legacy::split_edge* Edge)
{
	k3d::legacy::split_edge* edge = Edge;

	unsigned long edges = 0;
	do
	{
		++edges;
		edge = edge->face_clockwise;
	}
	while(edge != Edge);

	return edges;
}

/// Returns whether Edge belongs to Face
bool edge_in_face(k3d::legacy::split_edge* Edge, k3d::legacy::split_edge* Face)
{
	k3d::legacy::split_edge* edge = Face;
	do
	{
		if(edge == Edge)
			return true;

		edge = edge->face_clockwise;
	}
	while(edge != Face);

	return false;
}

/// Returns vertex valency for interior points, 0 for border points
const unsigned long vertex_valency(k3d::legacy::split_edge* Edge)
{
	// Cycle through edges around vertex
	unsigned long valency = 0;
	k3d::legacy::split_edge* current_edge = Edge;
	do
	{
		// Return 0 if a surface boundary is found
		if(!current_edge->companion)
			return 0;

		++valency;

		current_edge = current_edge->companion->face_clockwise;
	}
	while(current_edge != Edge);

	return valency;
}

/// Inserts a new edge after Edge, returns new point
k3d::legacy::point* append_edge(k3d::legacy::split_edge* Edge)
{
	// Create new point
	k3d::legacy::point* new_point = new k3d::legacy::point(Edge->vertex->position);

	// Subdivide edge
	k3d::legacy::split_edge* new_edge = new k3d::legacy::split_edge(new_point);
	new_edge->face_clockwise = Edge->face_clockwise;
	Edge->face_clockwise = new_edge;

	if(!Edge->companion)
		return new_point;

	// Subdivide companion
	k3d::legacy::split_edge* companion = Edge->companion;

	k3d::legacy::split_edge* new_companion = new k3d::legacy::split_edge(new_point);
	new_companion->face_clockwise = companion->face_clockwise;
	companion->face_clockwise = new_companion;

	// Join edges
	k3d::legacy::join_edges(*Edge, *new_companion);
	k3d::legacy::join_edges(*new_edge, *companion);

	return new_point;
}

/// Calculates the center point (as an average) for a face
k3d::point3 center_point(const k3d::legacy::face& Face)
{
	k3d::point3 center(0, 0, 0);

	unsigned long count = 0;
	for(k3d::legacy::split_edge* edge = Face.first_edge; edge; edge = edge->face_clockwise)
	{
		count++;
		center += k3d::to_vector(edge->vertex->position);
		if(Face.first_edge == edge->face_clockwise)
			break;
	}
	if(count)
		center /= count;

	return center;
}

} // namespace helpers

} // namespace libk3dmesh

