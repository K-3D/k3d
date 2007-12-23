// K-3D
// Copyright (c) 1995-2006, Timothy M. Shead
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
		\author Romain Behar (romainbehar@yahoo.com)
		\author Timothy M. Shead (tshead@k-3d.com)
*/

#include "helpers.h"

#include <k3dsdk/basic_math.h>
#include <k3dsdk/document_plugin_factory.h>
#include <k3d-i18n-config.h>
#include <k3dsdk/node.h>
#include <k3dsdk/persistent.h>
#include <k3dsdk/measurement.h>
#include <k3dsdk/legacy_mesh_modifier.h>
#include <k3dsdk/mesh_selection_sink.h>
#include <k3dsdk/selection.h>
#include <k3dsdk/utility.h>

#include <set>

namespace libk3dpolyhedra
{

namespace detail
{

/// Caches new geometry for better interactive performance
class new_point
{
public:
	new_point(const k3d::point3& OldPosition, const k3d::normal3& Direction, const k3d::vector3& InsetDirection, const double Ratio, k3d::legacy::point& Point) :
		old_position(OldPosition),
		direction(k3d::to_vector(Direction)),
		inset_direction(InsetDirection),
		ratio(Ratio),
		point(&Point)
	{
	}

	void update(const double Distance, const double Inset)
	{
		point->position = old_position + ratio * (Distance * direction + Inset * inset_direction);
	}

private:
	k3d::point3 old_position;
	k3d::vector3 direction;
	k3d::vector3 inset_direction;
	double ratio;
	k3d::legacy::point* point;
};

/// Caches new geometry for better interactive performance
typedef std::vector<new_point> new_points_t;

/// Connects two vertices belonging to the same face
k3d::legacy::face* segment_connect_vertices(k3d::imaterial* Material, k3d::legacy::split_edge* Edge1, k3d::legacy::split_edge* Edge2)
{
	// Make new edge loops
	k3d::legacy::split_edge* new_edge1 = new k3d::legacy::split_edge(Edge1->vertex, Edge2);
	k3d::legacy::split_edge* new_edge2 = new k3d::legacy::split_edge(Edge2->vertex, Edge1);

	k3d::legacy::split_edge* anti_edge1 = k3d::legacy::face_anticlockwise(Edge1);
	k3d::legacy::split_edge* anti_edge2 = k3d::legacy::face_anticlockwise(Edge2);

	anti_edge1->face_clockwise = new_edge1;
	anti_edge2->face_clockwise = new_edge2;

	k3d::legacy::join_edges(*new_edge1, *new_edge2);

	// Create second face
	k3d::legacy::face* new_face = new k3d::legacy::face(Edge1, Material);

	return new_face;
}

/// Extrude a face
struct indexed_face_t
{
	// Make sure faces are always processed in the same order (for procedural modeling)
	const unsigned long index;
	k3d::legacy::face* face;

	indexed_face_t(const unsigned long Index, k3d::legacy::face* Face) :
		index(Index), face(Face)
	{
	}
};
typedef std::map<k3d::legacy::split_edge*, indexed_face_t> edge_face_map_t;
typedef std::vector<k3d::legacy::split_edge*> face_t;
typedef std::map<k3d::legacy::point*, k3d::normal3> vertex_normals_t;
void extrude_face(face_t& Face, k3d::legacy::face::holes_t& FaceHoles, k3d::imaterial* Material, const unsigned long Segments, k3d::normal3& Direction, vertex_normals_t& VertexNormals, const bool Inside, const bool BackFace, edge_face_map_t& InteriorEdges, k3d::legacy::mesh& Mesh, k3d::legacy::polyhedron& Polyhedron, new_points_t& NewPoints, k3d::legacy::polyhedron::faces_t& BackFaces, k3d::legacy::polyhedron::faces_t& SideFaces)
{
	// Do nothing on zero-length direction
	if(!Direction.length2())
		return;

	// Create a new point for each original, save old and new points
	typedef std::map<k3d::legacy::point*, k3d::legacy::point*> point_map_t;
	point_map_t point_map;

	typedef std::vector<k3d::legacy::point*> points_t;
	points_t old_points;
	points_t new_points;
	std::vector<k3d::normal3> point_normals;

	// Map extruded edges to their companions
	typedef std::map<k3d::legacy::split_edge*, k3d::legacy::split_edge*> edge_map_t;
	edge_map_t edge_map;

	for(face_t::iterator edge = Face.begin(); edge != Face.end(); ++edge)
	{
		// New point
		k3d::legacy::point* old_point = (*edge)->vertex;
		old_points.push_back(old_point);
		k3d::legacy::point* const point = new k3d::legacy::point(*old_point);
		new_points.push_back(point);

		point_map.insert(std::make_pair(old_point, point));

		// Map edge
		edge_map.insert(std::make_pair(*edge, (*edge)->companion));

		// Store point normal
		if(VertexNormals.size())
		{
			vertex_normals_t::iterator normal = VertexNormals.find(old_point);
			assert_warning(normal != VertexNormals.end());
			point_normals.push_back(normal->second);
		}
	}

	// Save new points
	Mesh.points.insert(Mesh.points.end(), new_points.begin(), new_points.end());

	// Offset points
	points_t& source_points = Inside ? old_points : new_points;
	points_t& target_points = Inside ? new_points : old_points;

	typedef std::vector<k3d::normal3> normals_t;
	normals_t normals;
	typedef std::vector<k3d::vector3> vectors_t;
	vectors_t insets;

	const unsigned long edge_count = old_points.size();
	for(unsigned long i = 0; i != edge_count; ++i)
	{
		const k3d::vector3 e1 = source_points[i]->position - source_points[(i+edge_count-1)%edge_count]->position;
		const k3d::vector3 e2 = source_points[(i+1)%edge_count]->position - source_points[i]->position;
		const k3d::vector3 e3 = k3d::normalize(e1 ^ Direction);
		const k3d::vector3 e4 = k3d::normalize(e2 ^ Direction);

		k3d::normal3 point_normal = point_normals.size() ? point_normals[i] : Direction;
		k3d::vector3 inset_direction = (-1 / cos(acos(e3 * e4) / 2)) * k3d::normalize(e3 + e4);

		if(!Inside)
		{
			point_normal *= -1;
			inset_direction *= -1;
		}

		if(Segments > 1)
		{
			normals.push_back(point_normal);
			insets.push_back(inset_direction);
		}

		NewPoints.push_back(new_point(source_points[i]->position, point_normal, inset_direction, 1.0, *target_points[i]));
	}

	// Create extrusion faces
	typedef std::vector<k3d::legacy::split_edge*> edges_t;
	edges_t side_edges;
	edges_t bottom_edges;
	for(face_t::iterator edge = Face.begin(); edge != Face.end(); ++edge)
	{
		k3d::legacy::split_edge* next_edge = ((edge + 1) != Face.end()) ? *(edge + 1) : *Face.begin();

		k3d::legacy::split_edge* start = new k3d::legacy::split_edge((*edge)->vertex);
		k3d::legacy::split_edge* side1 = new k3d::legacy::split_edge(next_edge->vertex);
		k3d::legacy::split_edge* top = new k3d::legacy::split_edge(point_map[next_edge->vertex]);
		k3d::legacy::split_edge* side2 = new k3d::legacy::split_edge(point_map[(*edge)->vertex]);

		start->face_clockwise = side1;
		side1->face_clockwise = top;
		top->face_clockwise = side2;
		side2->face_clockwise = start;

		// Save bottom edge
		if(BackFace)
			bottom_edges.push_back(start);

		if((*edge)->companion)
			k3d::legacy::join_edges(*edge_map[*edge], *start);
		k3d::legacy::join_edges(**edge, *top);

		side_edges.push_back(side2);
		side_edges.push_back(side1);

		// Make sure new face doesn't start at side1 or side2 for segments creation (see below)
		k3d::legacy::face* const new_face = new k3d::legacy::face(start, Material);
		new_face->selection_weight = 1.0;

		Polyhedron.faces.push_back(new_face);
		SideFaces.push_back(new_face);
	}

	// Join side faces
	const unsigned long sides = side_edges.size();
	for(unsigned long n = 0; n < sides / 2; ++n)
	{
		k3d::legacy::split_edge* edge1 = side_edges[n*2 + 1];
		k3d::legacy::split_edge* edge2 = side_edges[(n*2 + 2) % sides];

		if(edge1 && edge2)
			k3d::legacy::join_edges(*edge1, *edge2);
	}

	// Extrude face
	for(face_t::iterator edge = Face.begin(); edge != Face.end(); ++edge)
	{
		(*edge)->vertex = point_map[(*edge)->vertex];

		if(!InteriorEdges.size())
			continue;

		// Interior edge with border vertex (for region option)
		k3d::legacy::split_edge* next_edge = ((edge + 1) != Face.end()) ? *(edge + 1) : *Face.begin();
		k3d::legacy::split_edge* interior_edge = (*edge)->face_clockwise;
		while(interior_edge != next_edge)
		{
			interior_edge->vertex = point_map[interior_edge->vertex];

			// Remove edge since it's not fully interior
			InteriorEdges.erase(interior_edge);

			if(!interior_edge->companion)
				break;

			interior_edge = interior_edge->companion->face_clockwise;
		}
	}

	// Create back face (only if every bottom edge has no companion)
	if(BackFace)
	{
		bool free = true;
		for(edges_t::iterator edge = bottom_edges.begin(); edge != bottom_edges.end(); ++edge)
		{
			if((*edge)->companion)
			{
				free = false;
				break;
			}
		}

		if(free)
		{
			edges_t back_edges;
			for(edges_t::reverse_iterator edge = bottom_edges.rbegin(); edge != bottom_edges.rend(); ++edge)
			{
				k3d::legacy::split_edge* new_edge = new k3d::legacy::split_edge((*edge)->face_clockwise->vertex);
				back_edges.push_back(new_edge);

				k3d::legacy::join_edges(*new_edge, **edge);
			}

			k3d::legacy::loop_edges(back_edges.begin(), back_edges.end());

			// Create and save back face
			k3d::legacy::face* const new_face = new k3d::legacy::face(back_edges.front(), Material);
			new_face->selection_weight = true;

			Polyhedron.faces.push_back(new_face);
			BackFaces.push_back(new_face);

			// Duplicate holes
			const k3d::vector3 null_vector(0, 0, 0);
			const k3d::normal3 null_normal(0, 0, 0);
			for(k3d::legacy::face::holes_t::iterator hole = FaceHoles.begin(); hole != FaceHoles.end(); ++hole)
			{
				// Copy original hole's edges
				edges_t hole_edges;
				for(k3d::legacy::split_edge* edge = *hole; edge; edge = edge->face_clockwise)
				{
					hole_edges.push_back(edge);

					if(edge->face_clockwise == (*hole))
						break;
				}

				// Duplicate them into an 'inversed' hole
				edges_t new_edges;
				for(edges_t::reverse_iterator edge = hole_edges.rbegin(); edge != hole_edges.rend(); ++edge)
				{
					k3d::legacy::point* vertex = (*edge)->face_clockwise->vertex;

					k3d::legacy::point* new_vertex = new k3d::legacy::point(vertex->position);
					Mesh.points.push_back(new_vertex);
					NewPoints.push_back(new_point(vertex->position, null_normal, null_vector, 1.0, *new_vertex));

					k3d::legacy::split_edge* new_edge = new k3d::legacy::split_edge(new_vertex);
					new_edges.push_back(new_edge);
				}

				k3d::legacy::loop_edges(new_edges.begin(), new_edges.end());

				new_face->holes.push_back(new_edges.front());

				// Link the two holes
				k3d::legacy::split_edge* first_edge = 0;
				k3d::legacy::split_edge* previous_edge = 0;

				edges_t::reverse_iterator original_edge_i = hole_edges.rbegin();
				edges_t::iterator new_edge_i = new_edges.begin();
				for(; original_edge_i != hole_edges.rend(); ++original_edge_i, ++new_edge_i)
				{
					k3d::legacy::split_edge* original_edge = *original_edge_i;
					k3d::legacy::split_edge* new_edge = *new_edge_i;

					// First edge : side edge
					k3d::legacy::split_edge* hole_edge1 = new k3d::legacy::split_edge(original_edge->vertex);

					// Edge 2 : original_edge's companion
					k3d::legacy::split_edge* hole_edge2 = new k3d::legacy::split_edge(original_edge->face_clockwise->vertex);
					if(original_edge->companion)
						k3d::legacy::join_edges(*original_edge->companion, *hole_edge2);
					else
						k3d::legacy::join_edges(*original_edge, *hole_edge2);

					// Edge 3 : other side edge
					k3d::legacy::split_edge* hole_edge3 = new k3d::legacy::split_edge(new_edge->vertex);

					// Edge 4 : new_edge's companion
					k3d::legacy::split_edge* hole_edge4 = new k3d::legacy::split_edge(new_edge->face_clockwise->vertex);
					if(new_edge->companion)
						k3d::legacy::join_edges(*new_edge->companion, *hole_edge4);
					else
						k3d::legacy::join_edges(*new_edge, *hole_edge4);

					// Set side edge companions
					if(!previous_edge)
					{
						first_edge = hole_edge3;
					}
					else
					{
						k3d::legacy::join_edges(*previous_edge, *hole_edge3);
					}

					previous_edge = hole_edge1;

					// Loop edges (reversed)
					hole_edge1->face_clockwise = hole_edge4;
					hole_edge2->face_clockwise = hole_edge1;
					hole_edge3->face_clockwise = hole_edge2;
					hole_edge4->face_clockwise = hole_edge3;

					k3d::legacy::face* hole_face = new k3d::legacy::face(hole_edge1, Material);
					hole_face->selection_weight = 1.0;

					Polyhedron.faces.push_back(hole_face);
					SideFaces.push_back(hole_face);
				}

				k3d::legacy::join_edges(*previous_edge, *first_edge);
			}
		}
	}

	// Create segments
	if(Segments > 1)
		{
			const double segment_number = static_cast<double>(Segments);
			k3d::point3 null_direction(0, 0, 0);

			// Save all side1 edges from extrusion faces
			edges_t side1_edges;
			const unsigned long sides = side_edges.size();
			for(unsigned long n = 0; n < sides / 2; ++n)
			{
				side1_edges.push_back(side_edges[n*2 + 1]);
			}

			const unsigned long size = side1_edges.size();
			for(double segment = segment_number - 1; segment >= 1; --segment)
			{
				const double ratio = segment / segment_number;

				// Split all side edges
				unsigned long n = 1;
				for(edges_t::iterator edge = side1_edges.begin(); edge != side1_edges.end(); ++edge, ++n)
				{
					k3d::legacy::point* split_point = helpers::append_edge(*edge);

					Mesh.points.push_back(split_point);
					NewPoints.push_back(new_point((*edge)->vertex->position, normals[n % size], insets[n % size], ratio, *split_point));
				}

				// Create additional faces
				for(edges_t::iterator edge = side1_edges.begin(); edge != side1_edges.end(); ++edge)
				{
					k3d::legacy::split_edge* edge1 = (*edge)->face_clockwise;
					k3d::legacy::split_edge* edge2 = edge1->face_clockwise->face_clockwise->face_clockwise;
					k3d::legacy::face* new_face = segment_connect_vertices(Material, edge1, edge2);
					new_face->selection_weight = 1.0;

					Polyhedron.faces.push_back(new_face);
					SideFaces.push_back(new_face);
				}
			}
		}
}

// Returns edge with smallest index in edge_face_map_t set
k3d::legacy::split_edge* smallest_index_edge(edge_face_map_t& EdgeFaceMap)
{
	return_val_if_fail(EdgeFaceMap.size(), 0);

	edge_face_map_t::iterator edge_face = EdgeFaceMap.begin();
	k3d::legacy::split_edge* min_edge = edge_face->first;
	unsigned long min_index = edge_face->second.index;

	while(++edge_face != EdgeFaceMap.end())
	{
		if(edge_face->second.index < min_index)
		{
			min_index = edge_face->second.index;
			min_edge = edge_face->first;
		}
	}

	return min_edge;
}
// Returns face corresponding to smallest index in edge_face_map_t set
k3d::legacy::face* smallest_index_face(edge_face_map_t& EdgeFaceMap)
{
	return_val_if_fail(EdgeFaceMap.size(), 0);

	edge_face_map_t::iterator edge_face = EdgeFaceMap.begin();
	k3d::legacy::face* min_face = edge_face->second.face;
	unsigned long min_index = edge_face->second.index;

	while(++edge_face != EdgeFaceMap.end())
	{
		if(edge_face->second.index < min_index)
		{
			min_index = edge_face->second.index;
			min_face = edge_face->second.face;
		}
	}

	return min_face;
}

typedef std::set<k3d::legacy::face*> face_set_t;
void get_connected_faces(k3d::legacy::face* Face, edge_face_map_t& EdgeFaceMap, face_set_t& RegionFaces, edge_face_map_t& BorderEdges, edge_face_map_t& InteriorEdges)
{
	RegionFaces.insert(Face);

	typedef std::list<k3d::legacy::face*> faces_t;
	faces_t connected_faces;

	k3d::legacy::split_edge* edge = Face->first_edge;
	do
	{
		if(!edge->companion)
		{
			BorderEdges.insert(std::make_pair(edge, indexed_face_t(BorderEdges.size(), Face)));
		}
		else
		{
			edge_face_map_t::iterator edge_face_companion = EdgeFaceMap.find(edge->companion);
			if(edge_face_companion != EdgeFaceMap.end())
			{
				connected_faces.push_back(edge_face_companion->second.face);
				InteriorEdges.insert(std::make_pair(edge, edge_face_companion->second));
			}
			else
			{
				BorderEdges.insert(std::make_pair(edge, indexed_face_t(BorderEdges.size(), edge_face_companion->second.face)));
			}
		}

		edge = edge->face_clockwise;
	}
	while(edge != Face->first_edge);

	for(faces_t::iterator face = connected_faces.begin(); face != connected_faces.end(); ++face)
	{
		if(RegionFaces.find(*face) == RegionFaces.end())
			get_connected_faces(*face, EdgeFaceMap, RegionFaces, BorderEdges, InteriorEdges);
	}
}

// Indexed point to process them in order for procedural modeling
struct indexed_point_t
{
	indexed_point_t(k3d::legacy::point* Point, unsigned long Index) :
		point(Point), index(Index)
	{
	}

	k3d::legacy::point* point;
	unsigned long index;

	indexed_point_t& operator = (const indexed_point_t& IP)
	{
		point = IP.point;
		index = IP.index;

		return *this;
	}
};

bool operator < (const indexed_point_t& Left, const indexed_point_t& Right)
{
	return Left.index < Right.index;
}

void extrude_regions(k3d::legacy::polyhedron::faces_t& Faces, const unsigned long Segments, const bool Inside, const bool GroupNormals, const bool BackFace, k3d::legacy::mesh& Mesh, k3d::legacy::polyhedron& Polyhedron, new_points_t& NewPoints, k3d::legacy::polyhedron::faces_t& BackFaces, k3d::legacy::polyhedron::faces_t& SideFaces)
{
	// Build an edge to face map for selected faces
	edge_face_map_t edge_face_map;
	unsigned long index = 0;
	for(k3d::legacy::polyhedron::faces_t::iterator face = Faces.begin(); face != Faces.end(); ++face)
	{
		k3d::legacy::split_edge* edge = (*face)->first_edge;
		do
		{
			edge_face_map.insert(std::make_pair(edge, indexed_face_t(index++, *face)));

			// Next edge
			edge = edge->face_clockwise;
		}
		while(edge != (*face)->first_edge);
	}

	while(edge_face_map.size())
	{
		unsigned long previous_edge_number = edge_face_map.size();

		// Get connected edges as two sets : border and interior edges
		face_set_t region_faces;
		edge_face_map_t border_edges;
		edge_face_map_t interior_edges;
		get_connected_faces(smallest_index_face(edge_face_map), edge_face_map, region_faces, border_edges, interior_edges);

		// Remove edges from original set
		if(border_edges.size() + interior_edges.size() == edge_face_map.size())
			edge_face_map.clear();
		else
		{
			for(edge_face_map_t::iterator edge = border_edges.begin(); edge != border_edges.end(); ++edge)
				edge_face_map.erase(edge->first);

			for(edge_face_map_t::iterator edge = interior_edges.begin(); edge != interior_edges.end(); ++edge)
				edge_face_map.erase(edge->first);
		}

		// Compute either average region normal or vertex normals, collect holes and get a material
		k3d::normal3 region_normal(0, 0, 0);
		vertex_normals_t vertex_normals;
		k3d::legacy::face::holes_t region_holes;
		k3d::imaterial* material = 0;
		for(face_set_t::iterator face = region_faces.begin(); face != region_faces.end(); ++face)
		{
			region_holes.insert(region_holes.end(), (*face)->holes.begin(), (*face)->holes.end());
			material = (*face)->material;

			k3d::normal3 face_normal = k3d::normalize(k3d::legacy::normal(**face));
			region_normal += face_normal;

			if(!GroupNormals)
			{
				// Vertex normals (within the region)
				k3d::legacy::split_edge* edge = (*face)->first_edge;
				do
				{
					// Vertex normal
					vertex_normals_t::iterator normal = vertex_normals.find(edge->vertex);
					if(normal == vertex_normals.end())
					{
						vertex_normals.insert(std::make_pair(edge->vertex, face_normal));
					}
					else
					{
						normal->second += face_normal;
					}

					// Next edge
					edge = edge->face_clockwise;
				}
				while(edge != (*face)->first_edge);
			}
		}

		// Average region normal and normalize vertex normals
		region_normal /= static_cast<double>(region_faces.size());
		for(vertex_normals_t::iterator normal = vertex_normals.begin(); normal != vertex_normals.end(); ++normal)
			normal->second = k3d::normalize(normal->second);

		// Extrude each independant region
		return_if_fail(border_edges.size());

		while(border_edges.size())
		{
			const unsigned long previous_border_edge_number = border_edges.size();

			// Find a region border from first border edge
			face_t region;
			k3d::legacy::split_edge* start_edge = smallest_index_edge(border_edges);
			edge_face_map_t::iterator face_edge = border_edges.find(start_edge);

			k3d::legacy::split_edge* first_edge = face_edge->first;
			k3d::legacy::split_edge* next_edge;
			do
			{
				return_if_fail(face_edge != border_edges.end());

				// Add next border edge to region, save its face
				region.push_back(face_edge->first);

				next_edge = face_edge->first->face_clockwise;
				while(border_edges.find(next_edge) == border_edges.end())
					{
						next_edge = next_edge->companion->face_clockwise;

						return_if_fail(next_edge != face_edge->first->face_clockwise);
					}

				// Find next border edge
				face_edge = border_edges.find(next_edge);
			}
			while(next_edge != first_edge);

			// Remove region edges
			for(face_t::iterator edge = region.begin(); edge != region.end(); ++edge)
			{
				border_edges.erase(*edge);
			}

			// Sanity checks
			return_if_fail(region.size());
			return_if_fail(border_edges.size() < previous_border_edge_number);

			// Extrude region border
			extrude_face(region, region_holes, material, Segments, region_normal, vertex_normals, Inside, BackFace, interior_edges, Mesh, Polyhedron, NewPoints, BackFaces, SideFaces);
		}

		// Move face holes
		for(face_set_t::iterator face = region_faces.begin(); face != region_faces.end(); ++face)
		{
			if(!(*face)->holes.size())
				continue;

			k3d::vector3 inset_direction(0, 0, 0);
			for(k3d::legacy::face::holes_t::iterator hole = (*face)->holes.begin(); hole != (*face)->holes.end(); ++hole)
			{
				k3d::legacy::split_edge* edge = *hole;
				do
				{
					NewPoints.push_back(new_point(edge->vertex->position, (Inside ? 1.0 : -1.0) * region_normal, inset_direction, 1.0, *(edge->vertex)));

					edge = edge->face_clockwise;
				}
				while(edge != *hole);
			}
		}

		// Move interior points only when extruding inside (in edge index order for procedural modeling)
		if(Inside)
		{
			// List all interior points
			typedef std::vector<indexed_point_t> points_t;
			points_t interior_points;

			std::set<k3d::legacy::point*> point_set;

			for(edge_face_map_t::iterator interior_edge = interior_edges.begin(); interior_edge != interior_edges.end(); ++interior_edge)
			{
				k3d::legacy::point* point = interior_edge->first->vertex;
				if(point_set.find(point) == point_set.end())
				{
					interior_points.push_back(indexed_point_t(point, interior_edge->second.index));
					point_set.insert(point);
				}
			}

			std::sort(interior_points.begin(), interior_points.end());

			// Move interior points along region normal
			k3d::vector3 inset_direction(0, 0, 0);
			for(points_t::iterator point_i = interior_points.begin(); point_i != interior_points.end(); ++point_i)
			{
				k3d::legacy::point* point = point_i->point;
				NewPoints.push_back(new_point(point->position, (Inside ? 1.0 : -1.0) * region_normal, inset_direction, 1.0, *point));
			}
		}

		// Sanity check
		return_if_fail(edge_face_map.size() < previous_edge_number);
	}
}

struct extrude_faces
{
	extrude_faces(const unsigned long Segments, const bool Inside, const bool BackFace, k3d::legacy::mesh& Mesh, k3d::legacy::polyhedron& Polyhedron, new_points_t& NewPoints, k3d::legacy::polyhedron::faces_t& BackFaces, k3d::legacy::polyhedron::faces_t& SideFaces) :
		segments(Segments), inside(Inside), back_face(BackFace), mesh(Mesh), polyhedron(Polyhedron), new_points(NewPoints), back_faces(BackFaces), side_faces(SideFaces)
	{
	}

	void operator()(k3d::legacy::face* Face)
	{
		// Calculate face normal
		k3d::normal3 face_normal = k3d::legacy::normal(*Face);
		if(0.0 == face_normal.length2())
			return;

		face_t face_edges;
		k3d::legacy::split_edge* edge = Face->first_edge;
		do
		{
			face_edges.push_back(edge);
			edge = edge->face_clockwise;
		}
		while(edge != Face->first_edge);

		face_normal = k3d::normalize(face_normal);
		edge_face_map_t dummy_map;
		vertex_normals_t dummy_normals;
		extrude_face(face_edges, Face->holes, Face->material, segments, face_normal, dummy_normals, inside, back_face, dummy_map, mesh, polyhedron, new_points, back_faces, side_faces);

		// Move holes with extrusion
		if(Face->holes.size())
		{
			k3d::vector3 inset_direction(0, 0, 0);
			for(k3d::legacy::face::holes_t::iterator hole = Face->holes.begin(); hole != Face->holes.end(); ++hole)
			{
				k3d::legacy::split_edge* edge = *hole;
				do
				{
					new_points.push_back(new_point(edge->vertex->position, (inside ? 1.0 : -1.0) * face_normal, inset_direction, 1.0, *edge->vertex));

					edge = edge->face_clockwise;
				}
				while(edge != *hole);
			}
		}
	}

	const unsigned long segments;
	const bool inside;
	const bool back_face;
	k3d::legacy::mesh& mesh;
	k3d::legacy::polyhedron& polyhedron;
	new_points_t& new_points;
	k3d::legacy::polyhedron::faces_t& back_faces;
	k3d::legacy::polyhedron::faces_t& side_faces;
};

} // namespace detail

/////////////////////////////////////////////////////////////////////////////
// extrude_faces

class extrude_faces :
	public k3d::mesh_selection_sink<k3d::legacy::mesh_modifier<k3d::persistent<k3d::node> > >
{
	typedef k3d::mesh_selection_sink<k3d::legacy::mesh_modifier<k3d::persistent<k3d::node> > > base;

public:
	extrude_faces(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_distance(init_owner(*this) + init_name("distance") + init_label(_("Distance")) + init_description(_("Distance between original and new faces")) + init_value(1.0) + init_step_increment(0.1) + init_units(typeid(k3d::measurement::distance))),
		m_segments(init_owner(*this) + init_name("segments") + init_label(_("Segments")) + init_description(_("Segment number between original and new faces")) + init_value(1) + init_constraint(constraint::minimum(1)) + init_step_increment(1) + init_units(typeid(k3d::measurement::scalar))),
		m_inset(init_owner(*this) + init_name("inset") + init_label(_("Inset")) + init_description(_("Inset value for the new faces")) + init_value(0.0) + init_step_increment(0.02) + init_units(typeid(k3d::measurement::distance))),
		m_direction(init_owner(*this) + init_name("direction") + init_label(_("Direction")) + init_description(_("Extrusion direction : inside / outside")) + init_value(INSIDE) + init_enumeration(direction_values())),
		m_region(init_owner(*this) + init_name("region") + init_label(_("Region")) + init_description(_("Extrude neighbour faces as a single one (named a region)")) + init_value(true)),
		m_group_normals(init_owner(*this) + init_name("group_normals") + init_label(_("Group normals")) + init_description(_("For region extrusion use the average normal; else each face is extrude along its own normal")) + init_value(false)),
		m_back_face(init_owner(*this) + init_name("back_face") + init_label(_("Back face")) + init_description(_("Add back face when possible")) + init_value(true)),
		m_select_side_faces(init_owner(*this) + init_name("select_side_faces") + init_label(_("Select side faces")) + init_description(_("Select side faces on output")) + init_value(false))
	{
		m_mesh_selection.changed_signal().connect(make_reset_mesh_slot());
		m_input_mesh.changed_signal().connect(sigc::mem_fun(*this, &extrude_faces::reset_mesh));

		m_distance.changed_signal().connect(make_update_mesh_slot());
		m_segments.changed_signal().connect(make_reset_mesh_slot());
		m_inset.changed_signal().connect(make_update_mesh_slot());
		m_direction.changed_signal().connect(make_reset_mesh_slot());
		m_region.changed_signal().connect(make_reset_mesh_slot());
		m_group_normals.changed_signal().connect(make_reset_mesh_slot());
		m_back_face.changed_signal().connect(make_reset_mesh_slot());
		m_select_side_faces.changed_signal().connect(make_update_mesh_slot());
	}

	/** \todo Improve the implementation so we don't have to do this */
	k3d::iunknown* on_rewrite_hint(iunknown* const Hint)
	{
		// Force updates to re-allocate our mesh, for simplicity
		return 0;
	}
	
	

	void on_initialize_mesh(const k3d::legacy::mesh& InputMesh, k3d::legacy::mesh& Mesh)
	{
		k3d::legacy::deep_copy(InputMesh, Mesh);
		k3d::merge_selection(m_mesh_selection.pipeline_value(), Mesh);

		// Clear data caches
		m_new_points.clear();
		m_selected_faces.clear();
		m_side_faces.clear();

		const unsigned long segments = m_segments.pipeline_value();
		const direction_t direction = m_direction.pipeline_value();
		const bool region = m_region.pipeline_value();
		const bool group_normals = m_group_normals.pipeline_value();
		const bool back_face = m_back_face.pipeline_value();

		// For each polyhedron ...
		k3d::legacy::polyhedron::faces_t back_faces;
		for(k3d::legacy::mesh::polyhedra_t::iterator polyhedron = Mesh.polyhedra.begin(); polyhedron != Mesh.polyhedra.end(); ++polyhedron)
		{
			// Get the set of all selected faces ...
			k3d::copy_if((*polyhedron)->faces.begin(), (*polyhedron)->faces.end(), std::inserter(m_selected_faces, m_selected_faces.end()), k3d::is_selected());

			if(m_selected_faces.empty())
				m_selected_faces = (*polyhedron)->faces;

			if(region)
			{
				// Extrude neighbouring faces as a single one
				detail::extrude_regions(m_selected_faces, segments, (direction == INSIDE), group_normals, back_face, Mesh, **polyhedron, m_new_points, back_faces, m_side_faces);
			}
			else
			{
				// Extrude each face
				std::for_each(m_selected_faces.begin(), m_selected_faces.end(), detail::extrude_faces(segments, (direction == INSIDE), back_face, Mesh, **polyhedron, m_new_points, back_faces, m_side_faces));
			}

			assert_warning(is_valid(**polyhedron));
		}

		m_selected_faces.insert(m_selected_faces.end(), back_faces.begin(), back_faces.end());
		std::for_each(m_selected_faces.begin(), m_selected_faces.end(), k3d::selection::set_weight(1.0));
	}

	void on_update_mesh(const k3d::legacy::mesh& InputMesh, k3d::legacy::mesh& Mesh)
	{
		const double distance = m_distance.pipeline_value();
		const double inset = m_inset.pipeline_value();

		for(detail::new_points_t::iterator new_point = m_new_points.begin(); new_point != m_new_points.end(); ++new_point)
			new_point->update(distance, inset);

		// Update new items selection
		const bool select_side_faces = m_select_side_faces.pipeline_value();
		const double side_weight = select_side_faces ? 1.0 : 0.0;
		for(k3d::legacy::polyhedron::faces_t::iterator face = m_side_faces.begin(); face != m_side_faces.end(); ++face)
			(*face)->selection_weight = side_weight;
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<extrude_faces,
			k3d::interface_list<k3d::imesh_source,
			k3d::interface_list<k3d::imesh_sink > > > factory(
				k3d::uuid(0xdbd23c4e, 0x7119478c, 0x9ab2ee83, 0x5b56d527),
				"ExtrudeFaces",
				_("Extrudes selected faces along their normals"),
				"Polygons",
				k3d::iplugin_factory::STABLE);

		return factory;
	}

private:

	// Ignore hint, so input geometry changes also result in a complete initialization
	virtual void reset_mesh(iunknown* const Hint)
	{
		m_output_mesh.reset(0, 0);
	}
	
	/// Direction type
	typedef enum
	{
		INSIDE,
		OUTSIDE
	} direction_t;

	friend std::ostream& operator << (std::ostream& Stream, const direction_t& Value)
	{
		switch(Value)
		{
			case INSIDE:
				Stream << "inside";
				break;
			case OUTSIDE:
				Stream << "outside";
				break;
		}
		return Stream;
	}

	friend std::istream& operator >> (std::istream& Stream, direction_t& Value)
	{
		std::string text;
		Stream >> text;

		if(text == "inside")
			Value = INSIDE;
		else if(text == "outside")
			Value = OUTSIDE;
		else
			k3d::log() << error << k3d_file_reference << ": unknown enumeration [" << text << "]" << std::endl;

		return Stream;
	}

	static const k3d::ienumeration_property::enumeration_values_t& direction_values()
	{
		static k3d::ienumeration_property::enumeration_values_t values;
		if(values.empty())
		{
			values.push_back(k3d::ienumeration_property::enumeration_value_t("Inside", "inside", "Bevels inward from the original polygons"));
			values.push_back(k3d::ienumeration_property::enumeration_value_t("Outside", "outside", "Bevels outward from the original polygons"));
		}

		return values;
	}

	k3d_data(double, immutable_name, change_signal, with_undo, local_storage, no_constraint, measurement_property, with_serialization) m_distance;
	k3d_data(k3d::int32_t, immutable_name, change_signal, with_undo, local_storage, with_constraint, measurement_property, with_serialization) m_segments;
	k3d_data(double, immutable_name, change_signal, with_undo, local_storage, no_constraint, measurement_property, with_serialization) m_inset;
	k3d_data(direction_t, immutable_name, change_signal, with_undo, local_storage, no_constraint, enumeration_property, with_serialization) m_direction;
	k3d_data(bool, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_region;
	k3d_data(bool, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_group_normals;
	k3d_data(bool, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_back_face;
	k3d_data(bool, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_select_side_faces;

	/// Caches new geometry for better interactive performance
	detail::new_points_t m_new_points;
	k3d::legacy::polyhedron::faces_t m_selected_faces;
	k3d::legacy::polyhedron::faces_t m_side_faces;
};

/////////////////////////////////////////////////////////////////////////////
// extrude_faces_factory

k3d::iplugin_factory& extrude_faces_factory()
{
	return extrude_faces::get_factory();
}

} // namespace libk3dpolyhedra

