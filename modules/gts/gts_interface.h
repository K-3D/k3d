#ifndef GTS_INTERFACE_H
#define GTS_INTERFACE_H

// K-3D
// Copyright (c) 2004-2005, Romain Behar
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
		\brief Defines GTS interface: data types and functions
		\author Romain Behar (romainbehar@yahoo.com)
*/

#include <k3dsdk/legacy_mesh.h>

#include <gts.h>

namespace module
{

namespace gts
{

typedef std::vector<unsigned long> edge_t;
typedef std::vector<unsigned long> triangle_t;
typedef std::vector<edge_t> edges_t;
typedef std::vector<triangle_t> triangles_t;

/// Copies a surface from a GTS structure into a K-3D mesh
bool copy_surface(GtsSurface* Surface, k3d::legacy::mesh& Mesh);

/// Loads a surface from a K-3D mesh to a GTS structure
GtsSurface* gts_surface(const k3d::legacy::mesh& Mesh);

/// Copies a surface from a GTS structure into a K-3D mesh
bool copy_polygonal_surface(GtsSurface* Surface, const k3d::legacy::mesh& OldMesh, k3d::legacy::mesh& Mesh);

/// Loads a surface from a K-3D mesh to a GTS structure
// This version of gts_surface() triangulates polygons and inserts
// polygonal information in overloaded GTS data structures
GtsSurface* gts_polygonal_surface(const k3d::legacy::mesh& Mesh, unsigned long& PolyhedronIndex);

} // namespace gts

} // namespace module

#endif // GTS_INTERFACE_H


