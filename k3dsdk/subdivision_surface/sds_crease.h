#ifndef K3DSDK_SUBDIVISION_SURFACE_SDS_CREASE_H
#define K3DSDK_SUBDIVISION_SURFACE_SDS_CREASE_H

// K-3D SDS creases
// Copyright (c) 2004-2005, Bart Janssens
//
// Contact: bart.janssens@lid.kviv.be
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
		\brief Variable sharpness creases for SDS meshes.
		\author Bart Janssens <bart.janssens@lid.kviv.be>
*/

#include <k3dsdk/legacy_mesh.h>

#include <map>
#include <set>

namespace k3d
{

namespace sds
{

/// Updates the given polyhedron in-place to simulate variable-sharpness creases
void crease(k3d::legacy::mesh& Mesh, k3d::legacy::polyhedron& Polyhedron);

/// Split selected faces parallel
void split_faces_parallel(const k3d::legacy::mesh& Input, k3d::legacy::mesh& Output, const double Factor);

/// Return the sharpness of an edge
double get_sharpness(const k3d::legacy::split_edge& Edge);

/// Return the centroid of the face Edge belongs to
k3d::point3 get_centroid(const k3d::legacy::split_edge& Edge);

/// point data structure with extra information about neigbouring points
class point;

/// provide information on how to connect points
class link;

/// Provide methods to split faces and edges
class splitter
{
	/// Keeps information about a point:
	// [0] - position
	// [1] - centroid of the face this edge belongs to
	// [2] - 'near' extra point
	// [3] - 'far' extra point
	// [4] - extra point in the face, near this point
	typedef boost::array<point*, 5> point_info;
	/// Keeps original information about an edge:
	// [0] - clockwise
	// [1] - counter-clockwise
	// [2] - companion
	typedef boost::array<k3d::legacy::split_edge*, 3> edge_info;
	/// Combine edge and point info:
	typedef std::pair<point_info, edge_info> point_edge_info;
	/// Keeps information about edges:
	typedef std::map<k3d::legacy::split_edge*, point_edge_info> edges_t;

	/// Map between the first edge of a face and its face
	typedef std::map<k3d::legacy::split_edge*, k3d::legacy::face*> faces_t;

public:
	/// Create a new splitter
	splitter(k3d::legacy::polyhedron& Polyhedron, k3d::legacy::mesh::points_t& Points, bool Use_selection = false);
	~splitter();

	/// Convert sharpness to factor
	static double sharpness_to_factor(const double sharpness)
	{
		if (sharpness <= 0.0)
			return 0.0;

		if (sharpness < 1.0)
			return 0.5 - 0.1*sharpness;

		return 0.4/sharpness;
	}

	/// Split an edge, adding a point at fraction f near the edge vertex.
	void split_edge_add_point(k3d::legacy::split_edge& Edge, const double factor, point* Start = 0, point* End = 0);

	/// Create the creases
	void make_creases();

	/// Split a face parallel
	void split_face_parallel(k3d::legacy::split_edge& Edge, double factor);

	/// Link 2 selected points that belong to the same face
	void link_points();

private:
	/// Split an edge, adding a point at fraction f near the edge vertex. Return a pointer to the newly created point, or 0 if the function failed.
	point* split_edge(k3d::legacy::split_edge& Edge, const double factor, point* Start = 0, point* End = 0);

	/// Link 2 points that belong to the same face
	void link_points(point* A, point* B, k3d::imaterial* const Material);

	/// Retrieve an iterator to point_edge_info of an edge
	edges_t::iterator& find_info(k3d::legacy::split_edge& Edge);

	/// Get the edge at Index
	k3d::legacy::split_edge* get_edge(k3d::legacy::split_edge& Edge, int Index);

	/// Get the point at Index
	point* get_point(k3d::legacy::split_edge& Edge, int Index);

	/// Find the counter-clockwise edge on a face
	k3d::legacy::split_edge* counter_clockwise(k3d::legacy::split_edge& Edge);

	/// Find the clockwise edge
	k3d::legacy::split_edge* clockwise(k3d::legacy::split_edge& Edge);

	/// Find the companion of an edge
	k3d::legacy::split_edge* companion(k3d::legacy::split_edge& Edge);

	/// Find the start vertex of an edge
	point* start(k3d::legacy::split_edge& Edge);

	/// Find the end vertex of an edge
	point* end(k3d::legacy::split_edge& Edge);

	/// Find the face centroid for an edge
	k3d::legacy::point* centroid(k3d::legacy::split_edge& Edge);

	/// Find the near split-point of an edge
	point* near_point(k3d::legacy::split_edge& Edge);

	/// Find the far split-point of an edge
	point* far_point(k3d::legacy::split_edge& Edge);

	/// Find the face vertex near an edge
	point* face_vertex(k3d::legacy::split_edge& Edge);

	/// set the point
	void set_point(k3d::legacy::split_edge& Edge, point& Point, int Index);

	/// set the near split point
	void set_near(k3d::legacy::split_edge& Edge, point& Point);

	/// set the far split point
	void set_far(k3d::legacy::split_edge& Edge, point& Point);

	/// Set the face vertex
	void set_face_vertex(k3d::legacy::split_edge& Edge, point& Point);

	/// Split an edge near its own vertex
	void split_near(k3d::legacy::split_edge& edge, const double factor);

	/// Split an edge near its far end
	void split_far(k3d::legacy::split_edge& edge, const double factor);

	/// Find the face that contains Edge
	k3d::legacy::face* find_face(k3d::legacy::split_edge& Edge);

	/// Find out if any of the edges in Edges are the start of an edge.
	bool is_face(k3d::legacy::split_edge& Edge);
	/// add a new face
	void add_face(k3d::legacy::split_edge& Edge, k3d::imaterial* const Material);
	/// add a new point
	point* add_point(k3d::legacy::point* Point);
	/// add a new t_point
	point* add_t_point(k3d::legacy::point* Point, k3d::legacy::split_edge* To, k3d::legacy::split_edge* From, bool dummy_edge = false);

	edges_t m_edges;
	k3d::legacy::polyhedron::faces_t m_face_vector;
	faces_t m_all_faces;
	std::vector<point*> m_added_points;
	// find operations cache the iterator to the edge info here, so subsequent searches for the same edge don't take O(log(n)) operations each.
	std::pair<k3d::legacy::split_edge*, edges_t::iterator> m_cached_edge;
	std::vector<k3d::legacy::point*> m_centroids;

	k3d::legacy::polyhedron& m_Polyhedron;
	k3d::legacy::mesh::points_t& m_Points;
};

} // namespace sds

} // namespace k3d

#endif // !K3DSDK_SUBDIVISION_SURFACE_SDS_CREASE_H


