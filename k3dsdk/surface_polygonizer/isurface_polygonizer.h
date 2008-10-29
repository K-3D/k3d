#ifndef K3DSDK_SURFACE_POLYGONIZER_ISURFACE_POLYGONIZER_H
#define K3DSDK_SURFACE_POLYGONIZER_ISURFACE_POLYGONIZER_H

// K-3D
// Copyright (c) 2004-2006, Romain Behar
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
		\brief Declares isurface_polygonizer, an abstract interface for implicit surface polygonizers
		\author Romain Behar (romainbehar@yahoo.com)
*/

#include <k3dsdk/vectors.h>

#include <vector>

// Implicit surface functor
//  implicit_value(k3d::point3&) returns >= threshold for inside, < threshold for outside
class implicit_functor
{
public:
	virtual ~implicit_functor() {}

	virtual double implicit_value(const k3d::point3& point) = 0;
};

// Surface polygonizer
class isurface_polygonizer
{
public:
	isurface_polygonizer(
		const double voxel_size,
		const double threshold,
		const int xmin, const int xmax,
		const int ymin, const int ymax,
		const int zmin, const int zmax,
		const k3d::point3& origin,
		implicit_functor& functor,
		std::vector<k3d::point3>& surface_vertices,
		std::vector<k3d::point3>& surface_normals,
		std::vector< std::vector<unsigned long> >& surface_polygons) {}

	virtual bool polygonize_from_inside_point(const k3d::point3& startingpoint) = 0;

	virtual void polygonize_whole_grid() = 0;

protected:
	isurface_polygonizer() {}
	virtual ~isurface_polygonizer() {}
};

#endif // !K3DSDK_SURFACE_POLYGONIZER_ISURFACE_POLYGONIZER_H

