#ifndef MODULES_OPENGL_PAINTERS_SDS_CACHE_H
#define MODULES_OPENGL_PAINTERS_SDS_CACHE_H

// K-3D
// Copyright (c) 1995-2005, Timothy M. Shead
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

/** \file Common SDS painter classes
 * 	\author Bart Janssens (bart.janssens@lid.kviv.be)
 */


#include <k3dsdk/gl.h>
#include <k3dsdk/iproperty.h>
#include <k3dsdk/properties.h>
#include <k3dsdk/subdivision_surface/catmull_clark.h>

#include "painter_cache.h"

namespace module
{

namespace opengl
{

namespace painters
{

/// Color with alpha channel support
typedef k3d::basic_rgba<double, k3d::color_traits<double> > color_t;

/// Common SDS cache functionality
class sds_cache : public scheduler
{
public:
	sds_cache(const k3d::mesh* const Mesh) : m_levels(2), m_cache(0), m_selection_changed(false) {}
	
	~sds_cache();
	
	/// Visit the data representing the SDS patch surface
	void visit_surface(const k3d::uint_t Level, k3d::sds::ipatch_surface_visitor& Visitor);
	
	/// Visit the data representing the SDS patch boundaries
	void visit_boundary(const k3d::mesh& Mesh, const k3d::uint_t Level, k3d::sds::ipatch_boundary_visitor& Visitor);
	
	/// Visit the data representing the patch corners
	void visit_corners(const k3d::uint_t Level, k3d::sds::ipatch_corner_visitor& Visitor);
	
	const k3d::uint_t point_count() const
	{
		return m_mesh.points->size();
	}
	const k3d::uint_t edge_count() const
	{
		return m_mesh.polyhedra->edge_points->size();
	}
	const k3d::uint_t face_count() const
	{
		return m_mesh.polyhedra->face_first_loops->size();
	}
	
protected:
	/// Scheduler implementation
	void on_schedule(k3d::inode* Painter);
	void on_schedule(k3d::hint::mesh_geometry_changed* Hint, k3d::inode* Painter);
	void on_schedule(k3d::hint::selection_changed* Hint, k3d::inode* Painter);
	void on_schedule(k3d::hint::mesh_topology_changed* Hint, k3d::inode* Painter) { on_schedule(Painter); }
	void on_execute(const k3d::mesh& Mesh, k3d::inode* Painter);
private:
	
	/// Executed when a level has changed 
	void level_changed(k3d::ihint* Hint);
	
	/// Register a level property
	void register_painter(k3d::inode* Painter);
	
	/// Remove a level property
	void remove_painter(k3d::inode* Painter);
	
	k3d::uint_t m_levels;
	/// Indices of changed points
	k3d::mesh::indices_t m_indices;
	typedef std::set<k3d::iproperty*> levels_t;
	levels_t m_level_properties;
	// store connections for safe deletion of cache
	typedef std::map<const k3d::inode*, sigc::connection> connections_t;
	connections_t m_changed_connections; // connections to changed_signals
	connections_t m_deleted_connections; // connections to deleted_signals
	k3d::sds::catmull_clark_subdivider* m_cache;
	bool m_selection_changed;
	/// The mesh at the highest level
	k3d::mesh m_mesh;
};

/// Stores SDS face data in OpenGL-compatible arrays
class face_visitor : public k3d::sds::ipatch_surface_visitor
{
public:
	face_visitor(const k3d::uint_t PointCount, const k3d::uint_t EdgeCount, const k3d::uint_t FaceCount) :
		points_array(PointCount),
		normals_array(PointCount),
		indices(EdgeCount),
		face_starts(FaceCount),
		m_point(0),
		m_edge(0),
		m_face(0)
	{
		k3d::log() << debug << "created SDS face visitor with " << PointCount << " points, " << EdgeCount << " edges and " << FaceCount << " faces" << std::endl; 
	}
	~face_visitor()
	{
		k3d::log() << debug << "SDS face visitor visited " << m_point << " points, " << m_edge << " edges and " << m_face << " faces" << std::endl;
	}
	void on_vertex(const k3d::point3& Point, const k3d::normal3& Normal)
	{
		points_array[m_point]= Point;
		normals_array[m_point++] = Normal;
	}
	void on_edge(const k3d::uint_t PointIndex)
	{
		indices[m_edge++] = PointIndex;
	}
	void on_patch(k3d::uint_t Face)
	{
		face_starts[m_face++] = m_edge;
	}
	k3d::mesh::points_t points_array;
	k3d::mesh::normals_t normals_array;
	std::vector<GLuint> indices;
	k3d::mesh::indices_t face_starts;
private:
	k3d::uint_t m_point;
	k3d::uint_t m_edge;
	k3d::uint_t m_face;
};

/// Stores SDS patch border data in OpenGL-compatible arrays
class edge_visitor : public k3d::sds::ipatch_boundary_visitor
{
public:
	edge_visitor(const k3d::uint_t EdgeCount) : edge_starts(EdgeCount) {}

	void on_point(const k3d::point3& Point)
	{
		points_array.push_back(Point);
	}
	void on_boundary(const k3d::uint_t Edge)
	{
		edge_starts[Edge] = points_array.size();
	}
	
	k3d::mesh::points_t points_array;
	k3d::mesh::indices_t edge_starts;
};

/// Stores SDS patch corner data in OpenGL-compatible arrays
class point_visitor : public k3d::sds::ipatch_corner_visitor
{
public:
	
	void on_corner(const k3d::point3& Point)
	{
		points_array.push_back(Point);
	}
	
	k3d::mesh::points_t points_array;
};

} // namespace opengl

} // namespace painters

} // namespace module

#endif // !MODULES_OPENGL_PAINTERS_SDS_CACHE_H
