#ifndef MODULES_OPENGL_PAINTERS_UTILITY_H
#define MODULES_OPENGL_PAINTERS_UTILITY_H

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

/** \file
	\author Timothy M. Shead (tshead@k-3d.com)
*/

#include <k3dsdk/mesh.h>
#include <k3dsdk/polyhedron.h>

#include <boost/scoped_ptr.hpp>

namespace module
{

namespace opengl
{

namespace painters
{

/// Functor object that returns "true" for selected points only
class selected_points
{
public:
	selected_points(const k3d::mesh& Mesh) :
		point_selection(*Mesh.point_selection)
	{
	}

	bool operator()(const size_t Point) const
	{
		return point_selection[Point];
	}

private:
	const k3d::mesh::selection_t& point_selection;	
};

/// Functor object that returns "true" for unselected points only
class unselected_points
{
public:
	unselected_points(const k3d::mesh& Mesh) :
		point_selection(*Mesh.point_selection)
	{
	}

	bool operator()(const size_t Point) const
	{
		return !point_selection[Point];
	}

private:
	const k3d::mesh::selection_t& point_selection;	
};

/// Functor object that returns "true" for selected edges only
class selected_edges
{
public:
	selected_edges(const k3d::polyhedron::const_primitive& Polyhedron) :
		edge_selections(Polyhedron.edge_selections)
	{
	}

	bool operator()(const size_t Edge) const
	{
		return edge_selections[Edge];
	}

private:
	const k3d::mesh::selection_t& edge_selections;	
};

/// Functor object that returns "true" for unselected edges only
class unselected_edges
{
public:
	unselected_edges(const k3d::polyhedron::const_primitive& Polyhedron) :
		edge_selections(Polyhedron.edge_selections)
	{
	}

	bool operator()(const size_t Edge) const
	{
		return !edge_selections[Edge];
	}

private:
	const k3d::mesh::selection_t& edge_selections;	
};

/// Functor object that returns "true" for selected faces only
class selected_faces
{
public:
	selected_faces(const k3d::polyhedron::const_primitive& Polyhedron) :
		face_selections(Polyhedron.face_selections)
	{
	}

	bool operator()(const size_t Face) const
	{
		return face_selections[Face];
	}

private:
	const k3d::mesh::selection_t& face_selections;	
};

/// Functor object that returns "true" for unselected faces only
class unselected_faces
{
public:
	unselected_faces(const k3d::polyhedron::const_primitive& Polyhedron) :
		face_selections(Polyhedron.face_selections)
	{
	}

	bool operator()(const size_t Face) const
	{
		return !face_selections[Face];
	}

private:
	const k3d::mesh::selection_t& face_selections;	
};

/// Returns true if the given mesh contains non-empty polyhedra
k3d::bool_t has_non_empty_polyhedra(const k3d::mesh& Mesh);

/// Returns true if the given mesh contains non-SDS polyhedra
k3d::bool_t has_non_sds_polyhedra(const k3d::mesh& Mesh);

/// Returns true if the given mesh contains SDS polyhedra
k3d::bool_t has_sds_polyhedra(const k3d::mesh& Mesh);

} // namespace painters

} // namespace opengl

} // namespace module

#endif // !MODULES_OPENGL_PAINTERS_UTILITY_H

