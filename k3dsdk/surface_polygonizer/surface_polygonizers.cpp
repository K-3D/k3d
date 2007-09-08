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
		\brief Implements surface polygonizers interfaces
		\author Romain Behar (romainbehar@yahoo.com)
*/

#include "surface_polygonizers.h"

#include "jules_bloomenthal.h"

#include <k3dsdk/result.h>

// Marching Cubes
marching_cubes::marching_cubes(
		const double voxel_size,
		const double threshold,
		const int xmin, const int xmax,
		const int ymin, const int ymax,
		const int zmin, const int zmax,
		const vertex_t& origin,
		implicit_functor& functor,
		vertices_t& surface_vertices,
		vertices_t& surface_normals,
		polygons_t& surface_polygons)
{
	m_bloomenthal_polygonizer = new bloomenthal_polygonizer(
		bloomenthal_polygonizer::MARCHINGCUBES,
		voxel_size, threshold, xmin, xmax, ymin, ymax, zmin, zmax,
		origin, functor,
		surface_vertices, surface_normals, surface_polygons);

	assert_warning(m_bloomenthal_polygonizer);
}

marching_cubes::~marching_cubes()
{
	delete m_bloomenthal_polygonizer;
}

bool marching_cubes::polygonize_from_inside_point(const vertex_t& starting_point)
{
	return m_bloomenthal_polygonizer->polygonize_from_inside_point(starting_point);
}

void marching_cubes::polygonize_whole_grid()
{
	m_bloomenthal_polygonizer->polygonize_whole_grid();
}


// Marching Tetrahedra
marching_tetrahedra::marching_tetrahedra(
		const double voxel_size,
		const double threshold,
		const int xmin, const int xmax,
		const int ymin, const int ymax,
		const int zmin, const int zmax,
		const vertex_t& origin,
		implicit_functor& functor,
		vertices_t& surface_vertices,
		vertices_t& surface_normals,
		polygons_t& surface_polygons)
{
	m_bloomenthal_polygonizer = new bloomenthal_polygonizer(
		bloomenthal_polygonizer::TETRAHEDRAL,
		voxel_size, threshold, xmin, xmax, ymin, ymax, zmin, zmax,
		origin, functor,
		surface_vertices, surface_normals, surface_polygons);

	assert_warning(m_bloomenthal_polygonizer);
}

marching_tetrahedra::~marching_tetrahedra()
{
	delete m_bloomenthal_polygonizer;
}

bool marching_tetrahedra::polygonize_from_inside_point(const vertex_t& starting_point)
{
	return m_bloomenthal_polygonizer->polygonize_from_inside_point(starting_point);
}

void marching_tetrahedra::polygonize_whole_grid()
{
	m_bloomenthal_polygonizer->polygonize_whole_grid();
}


