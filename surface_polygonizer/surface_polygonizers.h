#ifndef SURFACE_POLYGONIZERS_H
#define SURFACE_POLYGONIZERS_H

// K-3D
// Copyright (c) 1995-2004, Timothy M. Shead
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
		\brief Declares surface polygonizers available
		\author Romain Behar (romainbehar@yahoo.com)
*/

#include "isurface_polygonizer.h"

#include "jules_bloomenthal.h"

// Marching Cubes
class marching_cubes :
	public isurface_polygonizer
{
public:
	marching_cubes(
		const double voxel_size,
		const double threshold,
		const int xmin, const int xmax,
		const int ymin, const int ymax,
		const int zmin, const int zmax,
		const k3d::point3& origin,
		implicit_functor& functor,
		std::vector<k3d::point3>& surface_vertices,
		std::vector<k3d::point3>& surface_normals,
		std::vector< std::vector<unsigned long> >& surface_polygons);

	virtual ~marching_cubes();

	virtual bool polygonize_from_inside_point(const k3d::point3& startingpoint);

	virtual void polygonize_whole_grid();

private:
	bloomenthal_polygonizer* m_bloomenthal_polygonizer;
};

// Marching Tetrahedra
class marching_tetrahedra :
	public isurface_polygonizer
{
public:
	marching_tetrahedra(
		const double voxel_size,
		const double threshold,
		const int xmin, const int xmax,
		const int ymin, const int ymax,
		const int zmin, const int zmax,
		const k3d::point3& origin,
		implicit_functor& functor,
		std::vector<k3d::point3>& surface_vertices,
		std::vector<k3d::point3>& surface_normals,
		std::vector< std::vector<unsigned long> >& surface_polygons);

	virtual ~marching_tetrahedra();

	virtual bool polygonize_from_inside_point(const k3d::point3& startingpoint);

	virtual void polygonize_whole_grid();

private:
	bloomenthal_polygonizer* m_bloomenthal_polygonizer;
};

#endif // SURFACE_POLYGONIZERS_H


