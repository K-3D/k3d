// K-3D
// Copyright (c) 2004-2007, Romain Behar
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
	\author Romain Behar (romainbehar@yahoo.com)
	\author Timothy M. Shead (tshead@k-3d.com)
*/

#include "helpers.h"

#include <k3dsdk/polyhedron.h>
#include <k3dsdk/triangulator.h>

#include <boost/scoped_ptr.hpp>

#include <vector>

namespace module
{

namespace gts
{

namespace detail
{

/// Converts a k3d::mesh to a GtsSurface as it is triangulated
class triangulator :
	public k3d::triangulator
{
	typedef k3d::triangulator base;

public:
	GtsSurface* process(const k3d::mesh& Mesh)
	{
		gts_surface = gts_surface_new(gts_surface_class(), gts_face_class(), gts_edge_class(), gts_vertex_class());

		mesh_points = Mesh.points.get();
		gts_vertices.assign(mesh_points->size(), 0);

		boost::scoped_ptr<k3d::polyhedron::const_primitive> polyhedron(k3d::polyhedron::validate(Mesh));
		if(polyhedron)
			base::process(Mesh, *polyhedron);

		gts_vertices.clear();
		mesh_points = 0;

		return gts_surface;
	}

private:
	void add_vertex(const k3d::point3& Coordinates, k3d::uint_t Vertices[4], k3d::uint_t Edges[4], double Weights[4], k3d::uint_t& NewVertex)
	{
		NewVertex = gts_vertices.size();
		gts_vertices.push_back(0);
	}

	void add_triangle(k3d::uint_t Vertices[3], k3d::uint_t Edges[3])
	{
		GtsVertex* const vertex1 = get_vertex(Vertices[0]);
		GtsVertex* const vertex2 = get_vertex(Vertices[1]);
		GtsVertex* const vertex3 = get_vertex(Vertices[2]);

		GtsEdge* const edge1 = gts_edge_new(gts_edge_class(), vertex1, vertex2);
		GtsEdge* const edge2 = gts_edge_new(gts_edge_class(), vertex2, vertex3);
		GtsEdge* const edge3 = gts_edge_new(gts_edge_class(), vertex3, vertex1);

		GtsFace* const face = gts_face_new(gts_face_class(), edge1, edge2, edge3);

		gts_surface_add_face(gts_surface, face);
	}

	GtsVertex* get_vertex(const k3d::uint_t Vertex)
	{
		if(!gts_vertices[Vertex])
		{
			const k3d::point3 mesh_point = (*mesh_points)[Vertex];
			gts_vertices[Vertex] = gts_vertex_new(gts_vertex_class(), mesh_point[0], mesh_point[1], mesh_point[2]);
		}

		return gts_vertices[Vertex];
	}

	GtsSurface* gts_surface;
	std::vector<GtsVertex*> gts_vertices;

	const k3d::mesh::points_t* mesh_points;
};

} // namespace detail

GtsSurface* convert(const k3d::mesh& Mesh)
{
	return detail::triangulator().process(Mesh);
}

/*
// Did you know that a GtsSurface cannot be converted into a K-3D mesh
// in a way that is compatible with the K-3D pipeline?  Unless you can
// prove that GTS has been fixed to provide deterministic output, we
// will-not accept any function that converts from GtsSurface to a
// K-3D mesh.
//
// See http://www.k-3d.org/wiki/GTS_Plugins_Removed for details.
*/

} // namespace gts

} // namespace module

