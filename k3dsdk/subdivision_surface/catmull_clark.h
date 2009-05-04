#ifndef K3DSDK_SUBDIVISION_SURFACE_CATMULL_CLARK_H
#define K3DSDK_SUBDIVISION_SURFACE_CATMULL_CLARK_H

// K-3D
// Copyright (c) 1995-2008, Timothy M. Shead
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

/** \file catmull_clark.h
		\author Bart Janssens (bart.janssens@lid.kviv.be)
		\created Feb 18, 2009
*/

#include <k3dsdk/inode.h>
#include <k3dsdk/mesh.h>
#include <k3dsdk/polyhedron.h>
#include <k3dsdk/types.h>

#include <boost/ptr_container/ptr_vector.hpp>

namespace k3d
{

class point3;
class normal3;

namespace sds
{

/// Visitor used to collect information about the SDS surface
class ipatch_surface_visitor
{
public:
	/// Called at the start of each patch (corresponding to a face on the original mesh) traversal
	virtual void on_patch(const k3d::uint_t Face) = 0;
	
	/// Called for each edge that makes up a side of one of the quads on the subdivided surface
	virtual void on_edge(const k3d::uint_t PointIndex) = 0;
	
	/// Called for each vertex on the subdivided surface
	virtual void on_vertex(const k3d::point3& Point, const k3d::normal3& Normal) = 0;
	
protected:
	ipatch_surface_visitor() {}
	ipatch_surface_visitor(const ipatch_surface_visitor&) {}
	virtual ~ipatch_surface_visitor() {}
	ipatch_surface_visitor& operator=(const ipatch_surface_visitor&) { return *this; }
};

/// Visitor used to collect information about the patch boundaries of the SDS surface
class ipatch_boundary_visitor
{
public:
	/// Called at the start of each boundary, corresponding to an edge on the original mesh
	virtual void on_boundary(const k3d::uint_t Edge) = 0;
	
	/// Called for each point on the patch boundaries
	virtual void on_point(const k3d::point3& Point) = 0;
	
protected:
	ipatch_boundary_visitor() {}
	ipatch_boundary_visitor(const ipatch_boundary_visitor&) {}
	virtual ~ipatch_boundary_visitor() {}
	ipatch_boundary_visitor& operator=(const ipatch_boundary_visitor&) { return *this; }
};

/// Visitor used to collect information about patch corners of the SDS surface
class ipatch_corner_visitor
{
public:
	/// Called for each patch corner, corresponding to a vertex of the original mesh
	virtual void on_corner(const k3d::point3& Point) = 0;
	
protected:
	ipatch_corner_visitor() {}
	ipatch_corner_visitor(const ipatch_corner_visitor&) {}
	virtual ~ipatch_corner_visitor() {}
	ipatch_corner_visitor& operator=(const ipatch_corner_visitor&) { return *this; }
};

/// Stores a hierarchy of Catmull-Clark levels, and provides methods to create and update this hierarchy
class catmull_clark_subdivider
{
	typedef std::vector<k3d::mesh::points_t> points_t;
	typedef boost::ptr_vector<k3d::polyhedron::primitive> polyhedra_t;
public:
	/// Create an empty Catmull-Clark mesh hierarchy with the given number of levels.
	catmull_clark_subdivider(const k3d::uint_t Levels = 2);
	
	~catmull_clark_subdivider();
	
	/// Set the number of SDS levels (rebuilds the cache)
	void set_levels(const k3d::uint_t Levels);
	
	/// Creates the topology of the hierarchy, with the final level being in Output
	/**
	 * Note: the Node is passed in order to enable pipeline profiling
	 */
	void create_mesh(const k3d::mesh::points_t& InputPoints, const k3d::polyhedron::const_primitive& InputPolyhedron, const k3d::mesh::selection_t& InputFaceSelection, k3d::inode* Node = 0);
	
	/// Updates the point coordinates throughout the hierarchy, with the final level in Output
	/**
	 * Note: the Node is passed in order to enable pipeline profiling
	 */
	void update_mesh(const k3d::mesh::points_t& InputPoints, const k3d::polyhedron::const_primitive& InputPolyhedron, const k3d::attribute_arrays& InputVertexData, const k3d::mesh::selection_t& InputFaceSelection, k3d::inode* Node = 0);
	
	/// Stores the subdivided mesh in the provided structures
	void copy_output(k3d::mesh::points_t& Points, k3d::polyhedron::primitive& Polyhedron, k3d::attribute_arrays& VertexData);
	
	/// Visit the data representing the SDS patch surface
	void visit_surface(const k3d::uint_t Level, ipatch_surface_visitor& Visitor);
	
	/// Visit the data representing the SDS patch boundaries, using the given source mesh (needed for topology info)
	void visit_boundary(const k3d::polyhedron::const_primitive Polyhedron, const k3d::uint_t Level, ipatch_boundary_visitor& Visitor);
	
	/// Visit the data representing the patch corners
	void visit_corners(const k3d::uint_t Level, ipatch_corner_visitor& Visitor);
	
private:
	class implementation;
	implementation* m_implementation;
};

} // namespace sds

} // namespace k3d

#endif // !K3DSDK_SUBDIVISION_SURFACE_CATMULL_CLARK_H
