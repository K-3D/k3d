// K-3D
// Copyright (c) 2005-2006, Romain Behar
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
*/

#include <k3dsdk/document_plugin_factory.h>
#include <k3d-i18n-config.h>
#include <k3dsdk/measurement.h>
#include <k3dsdk/legacy_mesh_modifier.h>
#include <k3dsdk/mesh_selection_sink.h>
#include <k3dsdk/node.h>
#include <k3dsdk/persistent.h>
#include <k3dsdk/utility.h>
#include <k3dsdk/vectors.h>

#include "helpers.h"

namespace libk3dmesh
{

namespace detail
{

/// Seth Teller's algorithm to find the shortest path between two lines
double point_plane_distance(const k3d::point3& a, const k3d::point4& P)
{
	return (a[0] * P[0] + a[1] * P[1] + a[2] * P[2] + P[3]);
}

// Intersect line (a, b) and plane
k3d::point3 intersect_line_plane(const k3d::point3& a, const k3d::point3& b, const k3d::point4& Plane)
{
	double Mdota = point_plane_distance(a, Plane);
	double Mdotb = point_plane_distance(b, Plane);

	double denom = Mdota - Mdotb;
	if(std::abs(denom / (std::abs(Mdota) + std::abs(Mdotb))) < 1.0e-6)
	{
		k3d::log() << "int_line_plane(): no intersection?" << std::endl;
		return k3d::point3(0, 0, 0);
	}

	double t = Mdota / denom;

	// Lerp
	return (1 - t) * a + t * b;
}

k3d::point4 plane_from_two_vectors_and_point(const k3d::vector3& u, const k3d::vector3 v, const k3d::point3& p)
{
	k3d::point4 plane(k3d::to_point(k3d::normalize(u ^ v)), 1);

	// Plane contains p
	plane[3] = -(plane[0] * p[0] + plane[1] * p[1] + plane[2] * p[2]);

	return plane;
}

// Computes closest point on line A to line B, and on line B to line A
bool line_line_closest_points(
	const k3d::point3& a, const k3d::vector3& adir,
	const k3d::point3& b, const k3d::vector3& bdir,
	k3d::point3& a_closest, k3d::point3& b_closest)
{
	// Connecting line is perpendicular to both
	k3d::vector3 cdir = k3d::normalize(adir ^ bdir);

	// Lines are near-parallel -- all points are closest
	if(!cdir.length2())
	{
		a_closest = a;
		b_closest = b;

		return false;
	}

	// Form plane containing line A, parallel to cdir
	k3d::point4 ac = plane_from_two_vectors_and_point(cdir, adir, a);
	// Form plane containing line B, parallel to cdir
	k3d::point4 bc = plane_from_two_vectors_and_point(cdir, bdir, b);

	// Closest point on A is line A ^ bc
	a_closest = intersect_line_plane(a, a + adir, bc);
	// Closest point on B is line B ^ ac
	b_closest = intersect_line_plane(b, b + bdir, ac);

	return true;
}

} // namespace detail

namespace detail
{

typedef std::map<k3d::legacy::split_edge*, unsigned long> edge_set_t;
typedef std::map<k3d::legacy::split_edge*, k3d::legacy::face*> edge_face_map_t;

/// Functor that gets selected edges
struct get_edges
{
	get_edges(edge_set_t& Edges, edge_face_map_t& EdgeFaceMap) :
		edges(Edges), edge_face_map(EdgeFaceMap), index(0)
	{
	}

	void operator()(k3d::legacy::split_edge* Edge, k3d::legacy::face* Face)
	{
		// Can't fillet border edges
		if(!Edge->companion)
			return;

		if(Edge->selection_weight || Edge->companion->selection_weight)
		{
			// Save edge
			edges.insert(std::make_pair(Edge, index++));
			edge_face_map.insert(std::make_pair(Edge, Face));
		}
	}

	edge_set_t& edges;
	edge_face_map_t& edge_face_map;

	// Index edges to always process them in the same order (procedural modeling)
	unsigned long index;
};

/// Function template in the spirit of std::for_each that applies a functor to every edge in a polyhedron
template<typename T>
void for_each_edge_face(k3d::legacy::polyhedron& Polyhedron, T Functor)
{
	for(k3d::legacy::polyhedron::faces_t::const_iterator face = Polyhedron.faces.begin(); face != Polyhedron.faces.end(); ++face)
	{
		k3d::legacy::split_edge* edge = (*face)->first_edge;
		do
		{
			Functor(edge, *face);

			edge = edge->face_clockwise;
		}
		while(edge != (*face)->first_edge);

		// Face holes
		for(k3d::legacy::face::holes_t::iterator hole = (*face)->holes.begin(); hole != (*face)->holes.end(); ++hole)
		{
			for(k3d::legacy::split_edge* edge = *hole; edge; edge = edge->face_clockwise)
			{
				Functor(edge, *face);

				if(edge->face_clockwise == (*hole))
					break;
			}
		}
	}
}

struct border_point
{
	border_point(k3d::legacy::point* Vertex, k3d::vector3& Direction) :
		vertex(Vertex), direction(Direction), start_position(Vertex->position)
	{
	}

	void update(const double Radius)
	{
		vertex->position = start_position + Radius * direction;
	}

	k3d::legacy::point* vertex;
	k3d::vector3 direction;
	k3d::point3 start_position;
};

typedef std::map<k3d::legacy::point*, border_point*> border_points_t;
typedef std::vector<border_point*> border_point_list_t;
typedef std::map<k3d::legacy::face*, border_point_list_t> face_points_t;
typedef std::map<k3d::legacy::point*, k3d::legacy::point*> point_map_t;
typedef std::vector<k3d::legacy::split_edge*> edges_t;

// Returns edge with smallest index in edge_set_t
k3d::legacy::split_edge* smallest_index_edge(edge_set_t& EdgeSet)
{
	return_val_if_fail(EdgeSet.size(), 0);

	edge_set_t::iterator edge = EdgeSet.begin();
	k3d::legacy::split_edge* min_edge = edge->first;
	unsigned long min_index = edge->second;

	while(++edge != EdgeSet.end())
	{
		if(edge->second < min_index)
		{
			min_index = edge->second;
			min_edge = edge->first;
		}
	}

	return min_edge;
}

/// Unwelds edges where a fillet will be inserted
void unweld_edges(edge_set_t& Edges, const edge_face_map_t& EdgeFaceMap, const double Radius, k3d::legacy::polyhedron& Polyhedron, k3d::legacy::mesh& Mesh, border_points_t& BorderPoints, point_map_t& OldPointsMap, edge_set_t& LoopEnds)
{
	// Make a copy of the edge set
	edge_set_t edges = Edges;

	// For each face, save its belonging unwelded points
	face_points_t face_points;

	unsigned long edge_set_index = Edges.size();
	while(edges.size())
	{
		// Save edge number for later sanity check
		const unsigned long previous_border_edge_number = edges.size();

		// Find a region border from first border edge
		edges_t edge_loop;
		k3d::legacy::split_edge* edge = smallest_index_edge(edges);

		k3d::legacy::split_edge* first_edge = edge;
		do
		{
			return_if_fail(edges.size());

			// Add edge to loop
			edge_loop.push_back(edge);

			k3d::legacy::split_edge* next_edge = edge->face_clockwise;
			while(edges.find(next_edge) == edges.end())
			{
				if(next_edge->companion)
					// Turn around next_edge->vertex to find next edge to fillet
					next_edge = next_edge->companion->face_clockwise;
				else
				{
					// Reached a border edge, turn back
					next_edge = edge->companion;
					//assert_warning(edges.find(next_edge) == edges.end());
					break;
				}

				return_if_fail(next_edge != edge->face_clockwise);
			}

			edge = next_edge;
		}
		while(edge != first_edge);

		// Remove region edges
		for(edges_t::iterator edge_i = edge_loop.begin(); edge_i != edge_loop.end(); ++edge_i)
		{
			edges.erase(*edge_i);
		}

		// Sanity checks
		return_if_fail(edge_loop.size());
		return_if_fail(edges.size() < previous_border_edge_number);

		// Unweld loop points and calculate their moving direction
		for(edges_t::iterator edge_i = edge_loop.begin(); edge_i != edge_loop.end(); ++edge_i)
		{
			edge = *edge_i;
			k3d::legacy::split_edge* next_loop_edge = (edge_i + 1 != edge_loop.end()) ? *(edge_i + 1) : *edge_loop.begin();
			k3d::legacy::split_edge* next_edge = edge->face_clockwise;

			if(edge->companion == next_loop_edge)
			{
				// It's a non-loop end
				LoopEnds.insert(std::make_pair(edge, ++edge_set_index));

				k3d::legacy::point* old_point = next_edge->vertex;
				k3d::legacy::point* new_point = new k3d::legacy::point(old_point->position);
				Mesh.points.push_back(new_point);
				OldPointsMap.insert(std::make_pair(new_point, old_point));

				next_edge->vertex = new_point;

				k3d::vector3 direction = k3d::normalize(k3d::to_vector(next_edge->face_clockwise->vertex->position - next_edge->vertex->position));
				border_point* b_point = new border_point(new_point, direction);
				BorderPoints.insert(std::make_pair(new_point, b_point));

				// Assign point to face
				edge_face_map_t::const_iterator edge_face = EdgeFaceMap.find(edge);
				return_if_fail(edge_face != EdgeFaceMap.end());
				face_points_t::iterator face_i = face_points.find(edge_face->second);
				if(face_i == face_points.end())
				{
					std::vector<border_point*> points;
					points.push_back(b_point);
					face_points.insert(std::make_pair(edge_face->second, points));
				}
				else
				{
					face_i->second.push_back(b_point);
				}

				// Second edge
				k3d::legacy::point* old_point2 = next_loop_edge->vertex;
				k3d::legacy::point* new_point2 = new k3d::legacy::point(old_point2->position);
				Mesh.points.push_back(new_point2);
				OldPointsMap.insert(std::make_pair(new_point2, old_point2));

				next_loop_edge->vertex = new_point2;

				k3d::vector3 direction2 = k3d::normalize(k3d::to_vector(k3d::legacy::face_anticlockwise(next_loop_edge)->vertex->position - next_loop_edge->vertex->position));
				border_point* b_point2 = new border_point(new_point2, direction2);
				BorderPoints.insert(std::make_pair(new_point2, b_point2));

				// Assign point to face
				edge_face_map_t::const_iterator edge_face2 = EdgeFaceMap.find(next_loop_edge);
				return_if_fail(edge_face2 != EdgeFaceMap.end());
				face_points_t::iterator face_i2 = face_points.find(edge_face2->second);
				if(face_i2 == face_points.end())
				{
					std::vector<border_point*> points;
					points.push_back(b_point2);
					face_points.insert(std::make_pair(edge_face2->second, points));
				}
				else
				{
					face_i2->second.push_back(b_point2);
				}
			}
			else
			{
				k3d::vector3 direction(0, 0, 0);

				k3d::legacy::point* old_point = next_edge->vertex;
				k3d::legacy::point* new_point = new k3d::legacy::point(old_point->position);
				Mesh.points.push_back(new_point);
				OldPointsMap.insert(std::make_pair(new_point, old_point));

				if(next_edge == next_loop_edge)
				{
					// Point moves previous and next edge normals (in their face's plane)
					next_edge->vertex = new_point;

					k3d::vector3 edge_direction = k3d::to_vector(edge->face_clockwise->vertex->position - edge->vertex->position);
					k3d::vector3 next_edge_direction = k3d::to_vector(next_edge->face_clockwise->vertex->position - next_edge->vertex->position);

					edge_face_map_t::const_iterator face = EdgeFaceMap.find(edge);
					return_if_fail(face != EdgeFaceMap.end());
					k3d::normal3 normal = k3d::legacy::normal(*face->second);

					edge_face_map_t::const_iterator next_face = EdgeFaceMap.find(next_edge);
					return_if_fail(next_face != EdgeFaceMap.end());
					k3d::normal3 next_normal = k3d::legacy::normal(*next_face->second);

					// Edge normal in face
					k3d::vector3 dir = k3d::normalize(normal ^ edge_direction);
					k3d::vector3 next_dir = k3d::normalize(next_normal ^ next_edge_direction);

					direction = dir + next_dir;
				}
				else
				{
					// Point moves along sum of interior edge directions
					do
					{
						next_edge->vertex = new_point;

						k3d::vector3 edge_direction = k3d::normalize(k3d::to_vector(next_edge->face_clockwise->vertex->position - next_edge->vertex->position));
						direction += edge_direction;

						next_edge = next_edge->companion->face_clockwise;
					}
					while(next_edge != next_loop_edge);

					next_edge->vertex = new_point;
				}

				// Save point
				border_point* b_point = new border_point(new_point, direction);
				BorderPoints.insert(std::make_pair(new_point, b_point));

				// Assign point to face
				edge_face_map_t::const_iterator edge_face = EdgeFaceMap.find(edge);
				return_if_fail(edge_face != EdgeFaceMap.end());
				face_points_t::iterator face_i = face_points.find(edge_face->second);
				if(face_i == face_points.end())
				{
					std::vector<border_point*> points;
					points.push_back(b_point);
					face_points.insert(std::make_pair(edge_face->second, points));
				}
				else
				{
					face_i->second.push_back(b_point);
				}
			}
		}
	}

	// Move away edges according to fillet radius
	for(face_points_t::iterator fp_i = face_points.begin(); fp_i != face_points.end(); ++fp_i)
	{
		k3d::legacy::face* face = fp_i->first;

		border_point_list_t& border_points = fp_i->second;
		for(border_point_list_t::iterator point = border_points.begin(); point != border_points.end(); ++point)
		{
			(*point)->update(Radius);
		}

		// Detect face flip
		k3d::vector3 edge1_dir1 = k3d::normalize(k3d::to_vector(face->first_edge->face_clockwise->vertex->position - face->first_edge->vertex->position));
		k3d::vector3 edge1_dir2 = k3d::normalize(k3d::to_vector(face->first_edge->face_clockwise->vertex->position - face->first_edge->vertex->position));

		if(edge1_dir1 * edge1_dir2 < 0)
		{
			// Remove edges
			for(border_point_list_t::iterator point = border_points.begin(); point != border_points.end(); ++point)
			{
				BorderPoints.erase((*point)->vertex);
				delete *point;
			}

			k3d::legacy::split_edge* edge = face->first_edge;
			do
			{
				Edges.erase(edge);

				edge = edge->face_clockwise;
			}
			while(edge != face->first_edge);

			Polyhedron.faces.erase(std::remove(Polyhedron.faces.begin(), Polyhedron.faces.end(), face), Polyhedron.faces.end());
			delete face;
		}
	}
}

/// Calculates triangle normal using summation method
k3d::vector3 summation_normal(const k3d::point3& p1, const k3d::point3& p2, const k3d::point3& p3)
{
	double x = 0;
	double y = 0;
	double z = 0;

	x += (p1[1] - p2[1]) * (p1[2] + p2[2]);
	y += (p1[2] - p2[2]) * (p1[0] + p2[0]);
	z += (p1[0] - p2[0]) * (p1[1] + p2[1]);

	x += (p2[1] - p3[1]) * (p2[2] + p3[2]);
	y += (p2[2] - p3[2]) * (p2[0] + p3[0]);
	z += (p2[0] - p3[0]) * (p2[1] + p3[1]);

	x += (p3[1] - p1[1]) * (p3[2] + p1[2]);
	y += (p3[2] - p1[2]) * (p3[0] + p1[0]);
	z += (p3[0] - p1[0]) * (p3[1] + p1[1]);

	return k3d::vector3(x, y, z);
}

typedef std::vector<k3d::point3> points_t;
k3d::point3 slerp_points(const unsigned long SegmentNumber, const k3d::point3& Vertex1, const k3d::vector3& Normal1, const k3d::point3& Vertex2, const k3d::vector3& Normal2, points_t& NewPoints)
{
	k3d::point3 intersection1;
	k3d::point3 intersection2;
	line_line_closest_points(Vertex1, Normal1, Vertex2, Normal2, intersection1, intersection2);

	// The two points should be identical but get the average
	k3d::point3 rotation_center = 0.5 * (intersection1 + intersection2);

	// Get start and end vectors for Slerp
	k3d::vector3 orientation1 = k3d::to_vector(Vertex1 - rotation_center);
	const double length1 = orientation1.length();
	return_val_if_fail(length1 > 0, rotation_center);
	orientation1 /= length1;

	k3d::vector3 orientation2 = k3d::to_vector(Vertex2 - rotation_center);
	const double length2 = orientation2.length();
	return_val_if_fail(length2 > 0, rotation_center);
	orientation2 /= length2;

	// Slerp vectors
	k3d::quaternion qrot1(0, orientation1);
	k3d::quaternion qrot2(0, orientation2);

	const double segment_number = static_cast<double>(SegmentNumber);
	for(unsigned long n = 1; n < SegmentNumber; ++n)
	{
		const double t = static_cast<double>(n) / segment_number;

		k3d::quaternion rotation = k3d::Slerp(qrot1, qrot2, t * 0.5);
		k3d::vector3 new_orientation = k3d::rotation3D(rotation) * orientation1;
		const double length_lerp = length1 + t * (length2 - length1);
		k3d::point3 new_position = rotation_center + length_lerp * new_orientation;

		NewPoints.push_back(new_position);
	}

	return rotation_center;
}

k3d::point3 lerp_points(const unsigned long SegmentNumber, const k3d::point3& Vertex1, const k3d::vector3& Normal1, const k3d::point3& Vertex2, const k3d::vector3& Normal2, points_t& NewPoints)
{
	const k3d::point3 direction = Vertex2 - Vertex1;

	const double segment_number = static_cast<double>(SegmentNumber);
	for(unsigned long n = 1; n < SegmentNumber; ++n)
	{
		const double t = static_cast<double>(n) / segment_number;

		NewPoints.push_back(Vertex1 + t * direction);
	}

	return (Vertex1 + Vertex2) / 2;
}

k3d::legacy::face* add_quad(k3d::legacy::point* P1, k3d::legacy::point* P2, k3d::legacy::point* P3, k3d::legacy::point* P4, k3d::imaterial* Material)
{
	return_val_if_fail(P1, 0);
	return_val_if_fail(P2, 0);
	return_val_if_fail(P3, 0);
	return_val_if_fail(P4, 0);

	k3d::legacy::split_edge* edge1 = new k3d::legacy::split_edge(P1);
	k3d::legacy::split_edge* edge2 = new k3d::legacy::split_edge(P2);
	k3d::legacy::split_edge* edge3 = new k3d::legacy::split_edge(P3);
	k3d::legacy::split_edge* edge4 = new k3d::legacy::split_edge(P4);

	edge1->face_clockwise = edge2;
	edge2->face_clockwise = edge3;
	edge3->face_clockwise = edge4;
	edge4->face_clockwise = edge1;

	k3d::legacy::face* new_face = new k3d::legacy::face(edge1, Material);
	assert_warning(new_face);

	return new_face;
}

k3d::legacy::face* add_triangle(k3d::legacy::point* P1, k3d::legacy::point* P2, k3d::legacy::point* P3, k3d::imaterial* Material)
{
	return_val_if_fail(P1, 0);
	return_val_if_fail(P2, 0);
	return_val_if_fail(P3, 0);

	k3d::legacy::split_edge* edge1 = new k3d::legacy::split_edge(P1);
	k3d::legacy::split_edge* edge2 = new k3d::legacy::split_edge(P2);
	k3d::legacy::split_edge* edge3 = new k3d::legacy::split_edge(P3);

	edge1->face_clockwise = edge2;
	edge2->face_clockwise = edge3;
	edge3->face_clockwise = edge1;

	k3d::legacy::face* new_face = new k3d::legacy::face(edge1, Material);
	assert_warning(new_face);

	return new_face;
}

typedef std::vector<k3d::legacy::point*> vertices_t;
struct border_t
{
	k3d::legacy::point* start;
	k3d::legacy::point* end;
	vertices_t vertices;
	edges_t edge_list;
	bool flat;
};
typedef std::vector<border_t> borders_t;

struct border_info_t
{
	border_info_t(const unsigned long Index) :
		index(Index)
	{
	}

	/// Stores middle points for lerp
	points_t flat_centers;
	/// Stores center points for slerp
	points_t rotation_centers;
	borders_t borders;

	// Index edges to always process them in the same order (procedural modeling)
	unsigned long index;
};
typedef std::map<k3d::legacy::point*, border_info_t> intersection_edges_t;

/// Fills the space created between unwelded edges using SegmentNumber segments
void fillet_edges(const edge_set_t& Edges, border_points_t& BorderPoints, const unsigned long SegmentNumber, k3d::imaterial* Material, point_map_t& OldPointsMap, edge_set_t& LoopEnds, k3d::legacy::polyhedron& Polyhedron, k3d::legacy::mesh& Mesh, intersection_edges_t& IntersectionEdges)
{
	// Make a copy of the edge set
	edge_set_t edges = Edges;

	unsigned long intersection_index = 0;
	while(edges.size())
	{
		// Get an edge and its companion
		k3d::legacy::split_edge* edge = smallest_index_edge(edges);
		edges.erase(edge);
		k3d::legacy::split_edge* companion = edge->companion;
		return_if_fail(companion);

		edges.erase(companion);

		// Compute plane normals
		const k3d::point3 edge1_vertex1 = edge->vertex->position;
		const k3d::point3 edge1_vertex2 = edge->face_clockwise->vertex->position;
		border_points_t::const_iterator vertex = BorderPoints.find(edge->vertex);
		return_if_fail(vertex != BorderPoints.end());
		const k3d::vector3 edge1_dir1 = vertex->second->direction;
		vertex = BorderPoints.find(edge->face_clockwise->vertex);
		return_if_fail(vertex != BorderPoints.end());
		const k3d::vector3 edge1_dir2 = vertex->second->direction;
		const k3d::point3 plane1_point = 0.5 * (edge1_vertex1 + edge1_dir1 + edge1_vertex2 + edge1_dir2);
		k3d::vector3 plane1_normal = summation_normal(edge1_vertex1, edge1_vertex2, plane1_point);

		const k3d::point3 edge2_vertex1 = companion->vertex->position;
		const k3d::point3 edge2_vertex2 = companion->face_clockwise->vertex->position;
		vertex = BorderPoints.find(companion->vertex);
		return_if_fail(vertex != BorderPoints.end());
		const k3d::vector3 edge2_dir1 = vertex->second->direction;
		vertex = BorderPoints.find(companion->face_clockwise->vertex);
		return_if_fail(vertex != BorderPoints.end());
		const k3d::vector3 edge2_dir2 = vertex->second->direction;
		const k3d::point3 plane2_point = 0.5 * (edge2_vertex1 + edge2_dir1 + edge2_vertex2 + edge2_dir2);
		k3d::vector3 plane2_normal = summation_normal(edge2_vertex1, edge2_vertex2, plane2_point);

		points_t points1;
		points_t points2;
		k3d::point3 rotation_center1(0, 0, 0);
		k3d::point3 rotation_center2(0, 0, 0);
		const bool flat = (k3d::normalize(plane1_normal) * k3d::normalize(plane2_normal) > 0.9);
		if(!flat)
		{
			// Slerp endpoints
			rotation_center1 = slerp_points(SegmentNumber, edge1_vertex1, plane1_normal, edge2_vertex2, plane2_normal, points1);
			rotation_center2 = slerp_points(SegmentNumber, edge1_vertex2, plane1_normal, edge2_vertex1, plane2_normal, points2);
		}
		else
		{
			// Coplanar faces : just lerp
			rotation_center1 = lerp_points(SegmentNumber, edge1_vertex1, plane1_normal, edge2_vertex2, plane2_normal, points1);
			rotation_center2 = lerp_points(SegmentNumber, edge1_vertex2, plane1_normal, edge2_vertex1, plane2_normal, points2);
		}

		return_if_fail(points1.size() == points2.size());

		// Create faces
		k3d::legacy::point* previous_point1 = edge->vertex;
		k3d::legacy::point* previous_point2 = edge->face_clockwise->vertex;

		vertices_t vertices1;
		vertices_t vertices2;
		edges_t edges1;
		edges_t edges2;

		points_t::const_iterator point1 = points1.begin();
		points_t::const_iterator point2 = points2.begin();
		for(; point1 != points1.end(); ++point1, ++point2)
		{
			// Create mesh points
			k3d::legacy::point* mesh_point1 = new k3d::legacy::point(*point1);
			vertices1.push_back(mesh_point1);
			Mesh.points.push_back(mesh_point1);

			k3d::legacy::point* mesh_point2 = new k3d::legacy::point(*point2);
			vertices2.push_back(mesh_point2);
			Mesh.points.push_back(mesh_point2);

			k3d::legacy::face* face = add_quad(previous_point1, mesh_point1, mesh_point2, previous_point2, Material);
			Polyhedron.faces.push_back(face);

			// Save border edges
			edges1.push_back(face->first_edge);
			edges2.push_back(face->first_edge->face_clockwise->face_clockwise);

			previous_point1 = mesh_point1;
			previous_point2 = mesh_point2;
		}

		k3d::legacy::face* face = add_quad(companion->vertex, previous_point2, previous_point1, companion->face_clockwise->vertex, Material);
		Polyhedron.faces.push_back(face);
		edges1.push_back(face->first_edge->face_clockwise->face_clockwise);
		edges2.push_back(face->first_edge);

		// Process loop ends
		bool first = (LoopEnds.find(edge) != LoopEnds.end());
		bool second = (LoopEnds.find(companion) != LoopEnds.end());
		if(first)
		{
			k3d::legacy::split_edge* next_edge = edge;

			// Count edges at this end
			unsigned long n = 0;
			while(next_edge->face_clockwise != companion)
			{
				next_edge = next_edge->face_clockwise->companion;

				if(!next_edge)
				{
					// 'border' point
					n = 0;
					break;
				}

				++n;

				// Infinite loop
				return_if_fail(n < 1000);
			}

			if(n == 0)
			{
			}
			else if(n == 2)
			{
				// Two edges leave from edge's end
				k3d::legacy::split_edge* edge_start = edge->face_clockwise->companion;
				k3d::legacy::split_edge* edge_end = edge_start->face_clockwise;

				k3d::legacy::split_edge* new_edge = 0;
				for(edges_t::iterator edge = edges2.begin(); edge != edges2.end(); ++edge)
				{
					new_edge = new k3d::legacy::split_edge((*edge)->face_clockwise->vertex);
					edge_start->face_clockwise = new_edge;
					edge_start = new_edge;
				}

				edge_start->face_clockwise = edge_end;
				edge_end->vertex = (*edges2.rbegin())->vertex;
			}
			else
			{
				// Find some middle edge
				next_edge = edge;
				for(unsigned long i = 0; i < n / 2 + 1; ++i)
					next_edge = next_edge->face_clockwise->companion;

				// Subdivide edge
				k3d::point3 new_position = (next_edge->vertex->position + next_edge->face_clockwise->vertex->position) / 2;
				k3d::legacy::point* new_point = helpers::append_edge(next_edge);
				new_point->position = new_position;
				Mesh.points.push_back(new_point);

				// Set border edge vertices
				next_edge->face_clockwise->face_clockwise->vertex = (*edges2.rbegin())->vertex;
				next_edge->face_clockwise->companion->vertex = (*edges2.begin())->face_clockwise->vertex;

				// Fill with triangles
				for(edges_t::reverse_iterator edge_i = edges2.rbegin(); edge_i != edges2.rend(); ++edge_i)
				{
					k3d::legacy::face* new_face = add_triangle(new_point, (*edge_i)->face_clockwise->vertex, (*edge_i)->vertex, Material);
					Polyhedron.faces.push_back(new_face);
				}
			}
		}
		if(second)
		{
			k3d::legacy::split_edge* next_edge = companion;

			// Count edges at this end
			unsigned long n = 0;
			while(next_edge->face_clockwise != edge)
			{
				next_edge = next_edge->face_clockwise->companion;

				if(!next_edge)
				{
					// 'border' point
					n = 0;
					break;
				}

				++n;

				return_if_fail(n < 1000);
			}

			if(n == 0)
			{
			}
			else if(n == 2)
			{
				k3d::legacy::split_edge* edge_start = companion->face_clockwise->companion;
				k3d::legacy::split_edge* edge_end = edge_start->face_clockwise;

				k3d::legacy::split_edge* new_edge = 0;
				for(edges_t::reverse_iterator edge = edges1.rbegin(); edge != edges1.rend(); ++edge)
				{
					new_edge = new k3d::legacy::split_edge((*edge)->face_clockwise->vertex);
					edge_start->face_clockwise = new_edge;
					edge_start = new_edge;
				}

				edge_start->face_clockwise = edge_end;
				edge_end->vertex = (*edges1.begin())->vertex;
			}
			else
			{
				// Find some middle edge
				next_edge = companion;
				for(unsigned long i = 0; i < n / 2 + 1; ++i)
					next_edge = next_edge->face_clockwise->companion;

				// Subdivide edge
				k3d::point3 new_position = (next_edge->vertex->position + next_edge->face_clockwise->vertex->position) / 2;
				k3d::legacy::point* new_point = helpers::append_edge(next_edge);
				new_point->position = new_position;
				Mesh.points.push_back(new_point);

				// Set border edge vertices
				next_edge->face_clockwise->face_clockwise->vertex = (*edges1.begin())->vertex;
				next_edge->face_clockwise->companion->vertex = (*edges1.rbegin())->face_clockwise->vertex;

				// Fill with triangles
				for(edges_t::iterator edge_i = edges1.begin(); edge_i != edges1.end(); ++edge_i)
				{
					k3d::legacy::face* new_face = add_triangle(new_point, (*edge_i)->face_clockwise->vertex, (*edge_i)->vertex, Material);
					Polyhedron.faces.push_back(new_face);
				}
			}
		}

		// Save edge lists (to fill intersections later)
		if(second)
		{
			// Insert empty structure to remember deleting original point
			point_map_t::const_iterator old_point = OldPointsMap.find(edge->vertex);
			return_if_fail(old_point != OldPointsMap.end());
			border_info_t info(++intersection_index);
			IntersectionEdges.insert(std::make_pair(old_point->second, info));
		}
		else
		{
			point_map_t::const_iterator old_point = OldPointsMap.find(edge->vertex);
			return_if_fail(old_point != OldPointsMap.end());

			border_t border;
			border.start = edge->vertex;
			border.end = companion->face_clockwise->vertex;
			border.vertices = vertices1;
			border.edge_list = edges1;
			border.flat = flat;

			intersection_edges_t::iterator edge_i = IntersectionEdges.find(old_point->second);
			if(edge_i == IntersectionEdges.end())
			{
				border_info_t info(++intersection_index);
				if(flat)
					info.flat_centers.push_back(rotation_center1);
				else
					info.rotation_centers.push_back(rotation_center1);

				info.borders.push_back(border);
				IntersectionEdges.insert(std::make_pair(old_point->second, info));
			}
			else
			{
				if(flat)
					edge_i->second.flat_centers.push_back(rotation_center1);
				else
					edge_i->second.rotation_centers.push_back(rotation_center1);

				edge_i->second.borders.push_back(border);
			}
		}

		if(first)
		{
			// Insert empty structure to remember deleting original point
			point_map_t::const_iterator old_point = OldPointsMap.find(edge->face_clockwise->vertex);
			return_if_fail(old_point != OldPointsMap.end());
			border_info_t info(++intersection_index);
			IntersectionEdges.insert(std::make_pair(old_point->second, info));
		}
		else
		{
			point_map_t::const_iterator old_point = OldPointsMap.find(edge->face_clockwise->vertex);
			return_if_fail(old_point != OldPointsMap.end());

			border_t border;
			border.start = companion->vertex;
			border.end = edge->face_clockwise->vertex;
			std::reverse(vertices2.begin(), vertices2.end());
			border.vertices = vertices2;
			std::reverse(edges2.begin(), edges2.end());
			border.edge_list = edges2;
			border.flat = flat;

			intersection_edges_t::iterator edge2_i = IntersectionEdges.find(old_point->second);
			if(edge2_i == IntersectionEdges.end())
			{
				border_info_t info(++intersection_index);
				if(flat)
					info.flat_centers.push_back(rotation_center2);
				else
					info.rotation_centers.push_back(rotation_center2);

				info.borders.push_back(border);
				IntersectionEdges.insert(std::make_pair(old_point->second, info));
			}
			else
			{
				if(flat)
					edge2_i->second.flat_centers.push_back(rotation_center2);
				else
					edge2_i->second.rotation_centers.push_back(rotation_center2);

				edge2_i->second.borders.push_back(border);
			}
		}
	}
}

k3d::point3 slerp_orientations(const k3d::point3& Point1, const k3d::point3& Point2, const k3d::point3& Center, const double t)
{
	k3d::vector3 orientation1 = k3d::to_vector(Point1 - Center);
	const double length1 = orientation1.length();
	return_val_if_fail(length1 > 0, k3d::point3(0, 0, 0));
	orientation1 /= length1;

	k3d::vector3 orientation2 = k3d::to_vector(Point2 - Center);
	const double length2 = orientation2.length();
	return_val_if_fail(length2 > 0, k3d::point3(0, 0, 0));
	orientation2 /= length2;

	k3d::quaternion qrot1(0, orientation1);
	k3d::quaternion qrot2(0, orientation2);

	k3d::quaternion rotation = k3d::Slerp(qrot1, qrot2, t * 0.5);
	k3d::vector3 new_orientation = k3d::rotation3D(rotation) * orientation1;
	const double length_lerp = length1 + t * (length2 - length1);

	return Center + length_lerp * new_orientation;
}

void fill_intersections(intersection_edges_t& IntersectionEdges, k3d::imaterial* Material, k3d::legacy::polyhedron& Polyhedron, k3d::legacy::mesh& Mesh)
{
	// Create a new map to sort by index
	typedef std::map<unsigned long, k3d::legacy::point*> indexed_points_t;
	indexed_points_t indexed_points;
	for(detail::intersection_edges_t::iterator edges = IntersectionEdges.begin(); edges != IntersectionEdges.end(); ++edges)
	{
		k3d::legacy::point* point = edges->first;
		unsigned long index = edges->second.index;

		indexed_points.insert(std::make_pair(index, point));
	}

	for(indexed_points_t::iterator point = indexed_points.begin(); point != indexed_points.end(); ++point)
	{
		intersection_edges_t::iterator intersection = IntersectionEdges.find(point->second);
		return_if_fail(intersection != IntersectionEdges.end());
		detail::points_t flat_points = intersection->second.flat_centers;
		detail::points_t rotation_points = intersection->second.rotation_centers;
		borders_t borders = intersection->second.borders;
		const unsigned long border_number = borders.size();

		// Skip non-loop ends
		if(!flat_points.size() && !rotation_points.size())
			continue;

		// Process special cases
		if(rotation_points.size() == 2 && !flat_points.size())
		{
			// Merge the two borders
			border_t& border1 = borders[0];
			border_t& border2 = borders[1];

			edges_t::iterator edge1 = border1.edge_list.begin();
			edges_t::reverse_iterator edge2 = border2.edge_list.rbegin();
			k3d::legacy::point* previous_point = 0;
			for(; edge1 + 1 != border1.edge_list.end(); ++edge1, ++edge2)
			{
				if(previous_point)
					(*edge1)->vertex = previous_point;

				const k3d::point3 mean = ((*edge1)->face_clockwise->vertex->position + (*edge2)->vertex->position) / 2;
				k3d::legacy::point* point = (*edge2)->vertex;
				point->position = mean;

				Mesh.points.erase(std::remove(Mesh.points.begin(), Mesh.points.end(), (*edge1)->face_clockwise->vertex), Mesh.points.end());
				delete (*edge1)->face_clockwise->vertex;

				(*edge1)->face_clockwise->vertex = point;

				previous_point = point;
			}
			if(previous_point)
				(*edge1)->vertex = previous_point;

			continue;
		}

		// General case
		const bool flat = (flat_points.size() && !rotation_points.size());

		// Find filling center
		k3d::point3 center_position(0, 0, 0);
		k3d::point3 rotation_center(0, 0, 0);
		if(flat)
		{
			// Get average flat center
			k3d::point3 flat_center(0, 0, 0);
			for(detail::points_t::iterator center = flat_points.begin(); center != flat_points.end(); ++center)
			{
				flat_center += *center;
			}
			flat_center /= static_cast<double>(flat_points.size());

			center_position = flat_center;
		}
		else
		{
			// Get average rotation center
			for(detail::points_t::iterator center = rotation_points.begin(); center != rotation_points.end(); ++center)
			{
				rotation_center += *center;
			}
			rotation_center /= static_cast<double>(rotation_points.size());

			center_position = rotation_center;
		}

		// Check for concave intersections
		bool convex = false;
		if(!flat)
		{
			typedef std::vector<k3d::point3> normals_t;
			normals_t normals;
			k3d::point3 average(0, 0, 0);
			for(unsigned long n = 0; n < border_number; ++n)
			{
				border_t& border = borders[n];
				if(border.flat)
					continue;

				const k3d::point3 normal = (border.start->position + border.end->position) / 2 - center_position;
				normals.push_back(normal);
				average += normal;
			}

			convex = true;
			if(normals.size() > 2)
			{
				average /= static_cast<double>(normals.size());

				// Check that every normal is on the same side of the plane defined by average normal
				for(std::vector<k3d::point3>::const_iterator normal = normals.begin(); normal != normals.end(); ++normal)
				{
					if(*normal * average < 0)
					{
						convex = false;
						break;
					}
				}
			}
		}

		// Find round part center
		if(convex)
		{
			k3d::vector3 average_direction(0, 0, 0);
			double average_length = 0;

			for(unsigned long m = 0; m < border_number; ++m)
			{
				const k3d::vector3 vector = k3d::to_vector(borders[m].start->position - rotation_center);
				average_direction += vector;
				average_length += vector.length();
			}

			average_length /= static_cast<double>(border_number);

			center_position += average_length * k3d::normalize(average_direction);
		}

		// Sort border_t structures to form a loop (bubble sort)
		for(unsigned long m = 0; m < border_number - 1; ++m)
		{
			k3d::legacy::point* search = borders[m].end;
			bool found = false;
			for(unsigned long n = m + 1; n < border_number; ++n)
			{
				if(borders[n].start == search)
				{
					found = true;
					if(n > m + 1)
						std::swap(borders[n], borders[m + 1]);

					break;
				}
			}

			assert_warning(found);
		}

		// Fill intersections on even segment number
		unsigned long segments = borders[0].vertices.size();
		if((segments + 1) % 2 == 0)
		{
			while(segments > 1)
			{
				borders_t new_borders;

				// Create next vertex row
				for(unsigned long m = 0; m < border_number; ++m)
				{
					border_t new_border;

					const k3d::point3 start = borders[m].start->position;
					const k3d::point3 end = center_position;
					const double ratio = 1 / static_cast<double>(segments - 1);

					const k3d::point3 start_pos = convex ? slerp_orientations(start, end, rotation_center, ratio) : (start + ratio * (end - start));
					k3d::legacy::point* start_point = new k3d::legacy::point(start_pos);
					Mesh.points.push_back(start_point);

					new_border.start = start_point;

					const unsigned long vertex_number = borders[m].vertices.size();
					for(unsigned long v = 1; v < vertex_number - 1; ++v)
					{
						const k3d::point3 start = borders[m].vertices[v]->position;
						const k3d::point3 end = center_position;
						k3d::point3 position = convex ? slerp_orientations(start, end, rotation_center, ratio) : (start + ratio * (end - start));

						k3d::legacy::point* new_point = new k3d::legacy::point(position);
						Mesh.points.push_back(new_point);
						new_border.vertices.push_back(new_point);
					}

					new_borders.push_back(new_border);
				}

				// Copy new_border.start to next border's new_border.end
				for(unsigned long m = 0; m < border_number; ++m)
				{
					new_borders[m].end = new_borders[(m + 1) % border_number].start;
				}

				// Link original and new row
				for(unsigned long m = 0; m < border_number; ++m)
				{
					// Row
					const unsigned long vertex_number = new_borders[m].vertices.size();

					k3d::legacy::face* face = add_quad(new_borders[m].start, new_borders[m].vertices[0], borders[m].vertices[1], borders[m].vertices[0], Material);
					return_if_fail(face);

					Polyhedron.faces.push_back(face);

					for(unsigned long v = 0; v < vertex_number - 1; ++v)
					{
						face = add_quad(new_borders[m].vertices[v], new_borders[m].vertices[v + 1], borders[m].vertices[1 + v + 1], borders[m].vertices[1 + v], Material);
						return_if_fail(face);

						Polyhedron.faces.push_back(face);
					}

					face = add_quad(new_borders[m].vertices[vertex_number - 1], new_borders[m].end, borders[m].vertices[vertex_number + 1], borders[m].vertices[vertex_number], Material);
					return_if_fail(face);

					Polyhedron.faces.push_back(face);

					// End corner
					face = add_quad(new_borders[m].end, borders[(m + 1) % border_number].vertices[0], borders[m].end, borders[m].vertices[vertex_number + 1], Material);
					return_if_fail(face);

					Polyhedron.faces.push_back(face);
				}

				// Replace old borders with new ones
				std::swap(borders, new_borders);

				// Next iteration
				segments -= 2;
			}

			if(segments == 1)
			{
				k3d::legacy::point* center_point = new k3d::legacy::point(center_position);
				Mesh.points.push_back(center_point);

				// Link faces to center
				std::vector<k3d::legacy::split_edge*> edges;
				for(unsigned long m = 0; m < border_number; ++m)
				{
					return_if_fail(borders[m].vertices.size());
					k3d::legacy::face* face = add_quad(borders[(m + 1) % border_number].vertices[0], borders[m].end, borders[m].vertices[0], center_point, Material);
					return_if_fail(face);

					Polyhedron.faces.push_back(face);
				}
			}
		}
		else	//if(segments % 2 == 0)
		{
			// Fill intersections on odd segment number
			while(segments > 2)
			{
				borders_t new_borders;

				// Create next vertex row
				for(unsigned long m = 0; m < border_number; ++m)
				{
					border_t new_border;

					const k3d::point3 start = borders[m].start->position;
					const k3d::point3 end = center_position;
					const double ratio = 1 / static_cast<double>(segments);

					const k3d::point3 start_pos = convex ? slerp_orientations(start, end, rotation_center, ratio) : (start + ratio * (end - start));
					k3d::legacy::point* start_point = new k3d::legacy::point(start_pos);
					Mesh.points.push_back(start_point);

					new_border.start = start_point;

					const unsigned long vertex_number = borders[m].vertices.size();
					for(unsigned long v = 1; v < vertex_number - 1; ++v)
					{
						const k3d::point3 start = borders[m].vertices[v]->position;
						const k3d::point3 end = center_position;
						k3d::point3 position = convex ? slerp_orientations(start, end, rotation_center, ratio) : (start + ratio * (end - start));

						k3d::legacy::point* new_point = new k3d::legacy::point(position);
						Mesh.points.push_back(new_point);
						new_border.vertices.push_back(new_point);
					}

					new_borders.push_back(new_border);
				}

				// Copy new_border.start to next border's new_border.end
				for(unsigned long m = 0; m < border_number; ++m)
				{
					new_borders[m].end = new_borders[(m + 1) % border_number].start;
				}

				// Link original and new row
				for(unsigned long m = 0; m < border_number; ++m)
				{
					// Row
					const unsigned long vertex_number = new_borders[m].vertices.size();

					k3d::legacy::face* face = add_quad(new_borders[m].start, new_borders[m].vertices[0], borders[m].vertices[1], borders[m].vertices[0], Material);
					return_if_fail(face);

					Polyhedron.faces.push_back(face);

					for(unsigned long v = 0; v < vertex_number - 1; ++v)
					{
						face = add_quad(new_borders[m].vertices[v], new_borders[m].vertices[v + 1], borders[m].vertices[1 + v + 1], borders[m].vertices[1 + v], Material);
						return_if_fail(face);

						Polyhedron.faces.push_back(face);
					}

					face = add_quad(new_borders[m].vertices[vertex_number - 1], new_borders[m].end, borders[m].vertices[vertex_number + 1], borders[m].vertices[vertex_number], Material);
					return_if_fail(face);

					Polyhedron.faces.push_back(face);

					// End corner
					face = add_quad(new_borders[m].end, borders[(m + 1) % border_number].vertices[0], borders[m].end, borders[m].vertices[vertex_number + 1], Material);
					return_if_fail(face);

					Polyhedron.faces.push_back(face);
				}

				// Replace old borders with new ones
				std::swap(borders, new_borders);

				// Next iteration
				segments -= 2;
			}

			if(segments == 2)
			{
				borders_t new_borders;

				// Create next vertex row
				for(unsigned long m = 0; m < border_number; ++m)
				{
					border_t new_border;

					const k3d::point3 start = borders[m].start->position;
					const k3d::point3 end = center_position;
					const double ratio = 1 / static_cast<double>(segments);

					const k3d::point3 start_pos = convex ? slerp_orientations(borders[m].start->position, center_position, rotation_center, ratio) : (start + ratio * (end - start));
					k3d::legacy::point* start_point = new k3d::legacy::point(start_pos);
					Mesh.points.push_back(start_point);

					new_border.start = start_point;

					new_borders.push_back(new_border);
				}

				// Copy new_border.start to next border's new_border.end
				for(unsigned long m = 0; m < border_number; ++m)
				{
					new_borders[m].end = new_borders[(m + 1) % border_number].start;
				}

				// Link original and new row
				for(unsigned long m = 0; m < border_number; ++m)
				{
					k3d::legacy::face* face = add_quad(new_borders[m].start, new_borders[m].end, borders[m].vertices[1], borders[m].vertices[0], Material);
					return_if_fail(face);

					Polyhedron.faces.push_back(face);

					// End corner
					face = add_quad(new_borders[m].end, borders[(m + 1) % border_number].vertices[0], borders[m].end, borders[m].vertices[1], Material);
					return_if_fail(face);

					Polyhedron.faces.push_back(face);
				}

				// Replace old borders with new ones
				std::swap(borders, new_borders);

				// Next iteration
				segments -= 2;
			}

			if(segments == 0)
			{
				// Link all edges
				std::vector<k3d::legacy::split_edge*> edges;
				for(unsigned long m = border_number; m > 0; --m)
				{
					k3d::legacy::point* point = borders[m - 1].start;

					k3d::legacy::split_edge* edge = new k3d::legacy::split_edge(point);
					edges.push_back(edge);
				}

				k3d::legacy::loop_edges(edges.begin(), edges.end());

				k3d::legacy::face* const face = new k3d::legacy::face(*edges.begin(), Material);
				return_if_fail(face);

				Polyhedron.faces.push_back(face);
			}
		}
	}
}

} // namespace detail

/////////////////////////////////////////////////////////////////////////////
// fillet_edges_implementation

class fillet_edges_implementation :
	public k3d::mesh_selection_sink<k3d::legacy::mesh_modifier<k3d::persistent<k3d::node> > >
{
	typedef k3d::mesh_selection_sink<k3d::legacy::mesh_modifier<k3d::persistent<k3d::node> > > base;

public:
	fillet_edges_implementation(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_segments(init_owner(*this) + init_name("segments") + init_label(_("Segments")) + init_description(_("Segment number")) + init_value(4L) + init_step_increment(1) + init_units(typeid(k3d::measurement::scalar)) + init_constraint(constraint::minimum(1L))),
		m_radius(init_owner(*this) + init_name("radius") + init_label(_("Radius")) + init_description(_("Fillet radius")) + init_value(0.3) + init_step_increment(0.1) + init_units(typeid(k3d::measurement::distance)))
	{
		m_segments.changed_signal().connect(make_reset_mesh_slot());
		m_radius.changed_signal().connect(make_reset_mesh_slot());
		m_mesh_selection.changed_signal().connect(make_reset_mesh_slot());
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
		k3d::replace_selection(m_mesh_selection.value(), Mesh);

		// Clear previous caches
		m_border_points.clear();

		const unsigned long segment_number = m_segments.value();
		const double radius = m_radius.value();

		for(k3d::legacy::mesh::polyhedra_t::iterator polyhedron = Mesh.polyhedra.begin(); polyhedron != Mesh.polyhedra.end(); ++polyhedron)
		{
			if(!(*polyhedron)->faces.size())
				continue;

			// Get selected egdes
			detail::edge_set_t selected_edges;
			detail::edge_face_map_t edge_face_map;
			detail::for_each_edge_face(**polyhedron, detail::get_edges(selected_edges, edge_face_map));

			// Unweld selected edges
			detail::point_map_t old_points_map;
			detail::edge_set_t loop_ends;
			detail::unweld_edges(selected_edges, edge_face_map, radius, **polyhedron, Mesh, m_border_points, old_points_map, loop_ends);

			// Fillet edges
			detail::intersection_edges_t intersection_edges;
			detail::fillet_edges(selected_edges, m_border_points, segment_number, (*polyhedron)->faces[0]->material, old_points_map, loop_ends, **polyhedron, Mesh, intersection_edges);

			// Fill intersections
			detail::fill_intersections(intersection_edges, (*polyhedron)->faces[0]->material, **polyhedron, Mesh);

			// Remove and delete old points
			for(detail::intersection_edges_t::iterator point = intersection_edges.begin(); point != intersection_edges.end(); ++point)
			{
				k3d::legacy::point* old_point = point->first;
				Mesh.points.erase(std::remove(Mesh.points.begin(), Mesh.points.end(), old_point), Mesh.points.end());
				delete old_point;
			}

			// Set companions
			k3d::legacy::set_companions(**polyhedron);
			assert_warning(k3d::legacy::is_valid(**polyhedron));
		}
	}

	void on_update_mesh(const k3d::legacy::mesh& MeshInput, k3d::legacy::mesh& Mesh)
	{
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<fillet_edges_implementation,
				k3d::interface_list<k3d::imesh_source,
				k3d::interface_list<k3d::imesh_sink > > > factory(
					k3d::uuid(0x29672638, 0x932544bb, 0xa6b229a9, 0xca30dfc2),
					"FilletEdges",
					"Creates rounded surfaces along selected edges",
					"Mesh",
					k3d::iplugin_factory::EXPERIMENTAL);

		return factory;
	}

private:
	detail::border_points_t m_border_points;

	k3d_data(long, immutable_name, change_signal, with_undo, local_storage, with_constraint, measurement_property, with_serialization) m_segments;
	k3d_data(double, immutable_name, change_signal, with_undo, local_storage, no_constraint, measurement_property, with_serialization) m_radius;
};

/////////////////////////////////////////////////////////////////////////////
// fillet_edges_factory

k3d::iplugin_factory& fillet_edges_factory()
{
	return fillet_edges_implementation::get_factory();
}

} // namespace libk3dmesh

