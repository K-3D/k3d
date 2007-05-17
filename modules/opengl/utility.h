#ifndef LIBK3DOPENGL_UTILITY_H
#define LIBK3DOPENGL_UTILITY_H

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

namespace libk3dopengl
{

/// Functor object that returns "true" for selected points only
class selected_points
{
public:
	selected_points(const k3d::dev::mesh& Mesh) :
		point_selection(*Mesh.point_selection)
	{
	}

	bool operator()(const size_t Point) const
	{
		return point_selection[Point];
	}

private:
	const k3d::dev::mesh::selection_t& point_selection;	
};

/// Functor object that returns "true" for unselected points only
class unselected_points
{
public:
	unselected_points(const k3d::dev::mesh& Mesh) :
		point_selection(*Mesh.point_selection)
	{
	}

	bool operator()(const size_t Point) const
	{
		return !point_selection[Point];
	}

private:
	const k3d::dev::mesh::selection_t& point_selection;	
};

/// Functor object that returns "true" for selected faces only
class selected_faces
{
public:
	selected_faces(const k3d::dev::mesh& Mesh) :
		face_selection(*Mesh.polyhedra->face_selection)
	{
	}

	bool operator()(const size_t Face) const
	{
		return face_selection[Face];
	}

private:
	const k3d::dev::mesh::selection_t& face_selection;	
};

/// Functor object that returns "true" for unselected faces only
class unselected_faces
{
public:
	unselected_faces(const k3d::dev::mesh& Mesh) :
		face_selection(*Mesh.polyhedra->face_selection)
	{
	}

	bool operator()(const size_t Face) const
	{
		return !face_selection[Face];
	}

private:
	const k3d::dev::mesh::selection_t& face_selection;	
};

} // namespace libk3dopengl

#endif // !LIBK3DOPENGL_UTILITY_H

