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

#include <k3dsdk/mesh_operations.h>
#include <k3dsdk/triangulator.h>

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

		if(k3d::validate_polyhedra(Mesh))
		{
			mesh_points = Mesh.points.get();
			gts_vertices.assign(mesh_points->size(), 0);

			base::process(Mesh);

			gts_vertices.clear();
			mesh_points = 0;
		}

		return gts_surface;
	}

private:
	void add_vertex(const k3d::point3& Coordinates, k3d::uint_t Vertices[4], double Weights[4], k3d::uint_t& NewVertex)
	{
		NewVertex = gts_vertices.size();
		gts_vertices.push_back(0);
	}

	void add_triangle(const k3d::uint_t Vertex1, const k3d::uint_t Vertex2, const k3d::uint_t Vertex3)
	{
		GtsVertex* const vertex1 = get_vertex(Vertex1);
		GtsVertex* const vertex2 = get_vertex(Vertex2);
		GtsVertex* const vertex3 = get_vertex(Vertex3);

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

/*
/// Helper struct used to provide callback data when iterating over the vertices in a GtsSurface
struct vertex_data
{
	vertex_data(k3d::mesh::points_t& Points, k3d::mesh::selection_t& PointSelection) :
		points(Points),
		point_selection(PointSelection)
	{
	}

	k3d::mesh::points_t& points;
	k3d::mesh::selection_t& point_selection;
	std::map<GtsVertex*, k3d::uint_t> vertex_map;
};

/// Callback function used to convert GtsVertex objects into K-3D mesh points
int convert_vertices(gpointer Item, gpointer Data)
{
	GtsVertex* const gts_vertex = GTS_VERTEX(Item);
	vertex_data& data = *reinterpret_cast<vertex_data*>(Data);

	data.vertex_map.insert(std::make_pair(gts_vertex, data.points.size()));
	data.points.push_back(k3d::point3(gts_vertex->p.x, gts_vertex->p.y, gts_vertex->p.z));
	data.point_selection.push_back(0.0);

	return 0;
}

/// Helper struct used to provide callback data when iterating over the faces in a GtsSurface
struct face_data
{
	face_data(
		vertex_data& Vertices,
		k3d::mesh::indices_t& FaceFirstLoops,
		k3d::mesh::counts_t& FaceLoopCounts,
		k3d::mesh::selection_t& FaceSelection,
		k3d::mesh::materials_t& FaceMaterials,
		k3d::mesh::indices_t& LoopFirstEdges,
		k3d::mesh::indices_t& EdgePoints,
		k3d::mesh::indices_t& ClockwiseEdges,
		k3d::mesh::selection_t& EdgeSelection) :

		vertices(Vertices),
		face_first_loops(FaceFirstLoops),
		face_loop_counts(FaceLoopCounts),
		face_selection(FaceSelection),
		face_materials(FaceMaterials),
		loop_first_edges(LoopFirstEdges),
		edge_points(EdgePoints),
		clockwise_edges(ClockwiseEdges),
		edge_selection(EdgeSelection)
	{
	}

	vertex_data& vertices;
	k3d::mesh::indices_t& face_first_loops;
	k3d::mesh::counts_t& face_loop_counts;
	k3d::mesh::selection_t& face_selection;
	k3d::mesh::materials_t& face_materials;
	k3d::mesh::indices_t& loop_first_edges;
	k3d::mesh::indices_t& edge_points;
	k3d::mesh::indices_t& clockwise_edges;
	k3d::mesh::selection_t& edge_selection;
};

/// Callback function used to convert GtsFace objects into K-3D faces
int convert_faces(gpointer Item, gpointer Data)
{
	GtsFace* const gts_face = GTS_FACE(Item);
	face_data& data = *reinterpret_cast<face_data*>(Data);

	GtsEdge* const edge1 = gts_face->triangle.e1;
	GtsEdge* const edge2 = gts_face->triangle.e2;
	GtsEdge* const edge3 = gts_face->triangle.e3;

	GtsVertex* const vertex1 = edge1->segment.v1;
	GtsVertex* const vertex2 = edge1->segment.v2;
	GtsVertex* const vertex3 = edge2->segment.v1 == edge1->segment.v2 ? edge2->segment.v2 : edge2->segment.v1;

	data.face_first_loops.push_back(data.loop_first_edges.size());
	data.face_loop_counts.push_back(1);
	data.face_selection.push_back(0.0);
	data.face_materials.push_back(0);
	data.loop_first_edges.push_back(data.edge_points.size());

	data.edge_points.push_back(data.vertices.vertex_map[vertex1]);
	data.clockwise_edges.push_back(data.edge_points.size());
	data.edge_selection.push_back(0.0);

	data.edge_points.push_back(data.vertices.vertex_map[vertex2]);
	data.clockwise_edges.push_back(data.edge_points.size());
	data.edge_selection.push_back(0.0);

	data.edge_points.push_back(data.vertices.vertex_map[vertex3]);
	data.clockwise_edges.push_back(data.loop_first_edges.back());
	data.edge_selection.push_back(0.0);

	return 0;
}
*/

} // namespace detail

GtsSurface* convert(const k3d::mesh& Mesh)
{
	return detail::triangulator().process(Mesh);
}

/*
const k3d::mesh convert(GtsSurface* Surface)
{
	// Did you know that a GtsSurface cannot be converted into a K-3D mesh
	// in a way that is compatible with the K-3D pipeline?  Unless you can
	// prove that GTS has been fixed to provide deterministic output, you
	// don't want to use this function.
	//
	// See http://www.k-3d.org/wiki/GTS_Plugins_Removed for details.
	assert(0);

	k3d::mesh result;

	return_val_if_fail(Surface, result);
	
	boost::shared_ptr<k3d::mesh::polyhedra_t> polyhedra(new k3d::mesh::polyhedra_t());
	boost::shared_ptr<k3d::mesh::indices_t> first_faces(new k3d::mesh::indices_t());
	boost::shared_ptr<k3d::mesh::counts_t> face_counts(new k3d::mesh::counts_t());
	boost::shared_ptr<k3d::mesh::polyhedra_t::types_t> types(new k3d::mesh::polyhedra_t::types_t());
	boost::shared_ptr<k3d::mesh::indices_t> face_first_loops(new k3d::mesh::indices_t());
	boost::shared_ptr<k3d::mesh::counts_t> face_loop_counts(new k3d::mesh::counts_t());
	boost::shared_ptr<k3d::mesh::selection_t> face_selection(new k3d::mesh::selection_t());
	boost::shared_ptr<k3d::mesh::materials_t> face_materials(new k3d::mesh::materials_t());
	boost::shared_ptr<k3d::mesh::indices_t> loop_first_edges(new k3d::mesh::indices_t());
	boost::shared_ptr<k3d::mesh::indices_t> edge_points(new k3d::mesh::indices_t());
	boost::shared_ptr<k3d::mesh::indices_t> clockwise_edges(new k3d::mesh::indices_t());
	boost::shared_ptr<k3d::mesh::selection_t> edge_selection(new k3d::mesh::selection_t());
	boost::shared_ptr<k3d::mesh::points_t> points(new k3d::mesh::points_t());
	boost::shared_ptr<k3d::mesh::selection_t> point_selection(new k3d::mesh::selection_t());

	// Convert GTS vertices to K-3D mesh points
	points->reserve(gts_surface_vertex_number(Surface));
	point_selection->reserve(gts_surface_vertex_number(Surface));

	detail::vertex_data vertex_data(*points, *point_selection);
	gts_surface_foreach_vertex(Surface, detail::convert_vertices, &vertex_data);

	// Convert GTS faces to K-3D mesh faces
	detail::face_data face_data(vertex_data, *face_first_loops, *face_loop_counts, *face_selection, *face_materials, *loop_first_edges, *edge_points, *clockwise_edges, *edge_selection);
	gts_surface_foreach_face(Surface, detail::convert_faces, &face_data);

	// Create a single polyhedron ... is this a safe assumption to make?
	first_faces->push_back(0);
	face_counts->push_back(face_first_loops->size());
	types->push_back(k3d::mesh::polyhedra_t::POLYGONS);

	polyhedra->first_faces = first_faces;
	polyhedra->face_counts = face_counts;
	polyhedra->types = types;
	polyhedra->face_first_loops = face_first_loops;
	polyhedra->face_loop_counts = face_loop_counts;
	polyhedra->face_selection = face_selection;
	polyhedra->face_materials = face_materials;
	polyhedra->loop_first_edges = loop_first_edges;
	polyhedra->edge_points = edge_points;
	polyhedra->clockwise_edges = clockwise_edges;
	polyhedra->edge_selection = edge_selection;

	result.polyhedra = polyhedra;
	result.points = points;
	result.point_selection = point_selection;

	return result;
}
*/

} // namespace gts

} // namespace module

