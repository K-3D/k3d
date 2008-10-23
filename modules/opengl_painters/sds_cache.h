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


#include <k3dsdk/iproperty.h>
#include <k3dsdk/properties.h>
#include <k3dsdk/subdivision_surface/k3d_sds_binding.h>

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
	sds_cache(const k3d::mesh* const Mesh) : m_levels(2), m_cache(0), m_mesh(Mesh), m_selection_changed(false) {}
	
	~sds_cache();
	
	/// The explorable subdivided geometry
	k3d::sds::k3d_sds_cache& cache()
	{
		if (!m_cache)
		{
			k3d::log() << error << "No valid SDS cache!" << std::endl;
			m_cache = new k3d::sds::k3d_sds_cache();
		}
		return *m_cache;
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
	k3d::mesh::indices_t m_indices;
	typedef std::set<k3d::iproperty*> levels_t;
	levels_t m_level_properties;
	// store connections for safe deletion of cache
	typedef std::map<const k3d::inode*, sigc::connection> connections_t;
	connections_t m_changed_connections; // connections to changed_signals
	connections_t m_deleted_connections; // connections to deleted_signals
	k3d::sds::k3d_sds_cache* m_cache;
	const k3d::mesh* const m_mesh;
	bool m_selection_changed;
};

/// Stores SDS face data in OpenGL-compatible arrays
class face_visitor : public k3d::sds::sds_visitor
{
public:
	virtual void on_point(const k3d::sds::position_t& Point, const k3d::sds::position_t& Normal = k3d::sds::position_t(0,0,1))
	{
		points_array.push_back(Point);
		normals_array.push_back(Normal);
	}
	virtual void on_subfacet(const k3d::uint_t Point1, const k3d::uint_t Point2, const k3d::uint_t Point3, const k3d::uint_t Point4)
	{
		indices.push_back(Point1);
		indices.push_back(Point2);
		indices.push_back(Point3);
		indices.push_back(Point4);
	}
	virtual void on_face(k3d::uint_t Face)
	{
		face_starts.push_back(indices.size());
	}
	std::vector<k3d::sds::position_t> points_array;
	std::vector<k3d::sds::position_t> normals_array;
	std::vector<GLuint> indices;
	k3d::mesh::indices_t face_starts;
};

/// Stores SDS patch border data in OpenGL-compatible arrays
class edge_visitor : public k3d::sds::sds_visitor
{
public:
	edge_visitor(const k3d::mesh::indices_t& ClockwiseEdges,
			const k3d::mesh::indices_t& LoopFirstEdges,
			const k3d::mesh::indices_t& FaceFirstLoops) :
		edge_starts(ClockwiseEdges.size()),
		m_clockwise_edges(ClockwiseEdges),
		m_loop_first_edges(LoopFirstEdges),
		m_face_first_loops(FaceFirstLoops)
	{}
	virtual void on_point(const k3d::sds::position_t& Point, const k3d::sds::position_t& Normal = k3d::sds::position_t(0,0,1))
	{
		points_array.push_back(Point);
	}
	virtual void on_face(k3d::uint_t Face)
	{
		m_edge = m_loop_first_edges[m_face_first_loops[Face]];
	}
	virtual void on_edge()
	{
		edge_starts[m_edge] = points_array.size();
		m_edge = m_clockwise_edges[m_edge];
	}
	
	std::vector<k3d::sds::position_t> points_array;
	k3d::mesh::indices_t edge_starts;
private:
	const k3d::mesh::indices_t& m_clockwise_edges;
	const k3d::mesh::indices_t& m_loop_first_edges;
	const k3d::mesh::indices_t& m_face_first_loops;
	k3d::uint_t m_edge;
};

/// Stores SDS patch corner data in OpenGL-compatible arrays
class point_visitor : public k3d::sds::sds_visitor
{
public:
	point_visitor(const k3d::mesh::indices_t& ClockwiseEdges,
			const k3d::mesh::indices_t& EdgePoints,
			const k3d::mesh::indices_t& LoopFirstEdges,
			const k3d::mesh::indices_t& FaceFirstLoops,
			const k3d::uint_t PointCount) :
		m_clockwise_edges(ClockwiseEdges),
		m_edge_points(EdgePoints),
		m_loop_first_edges(LoopFirstEdges),
		m_face_first_loops(FaceFirstLoops),
		points_array(PointCount)
	{}
	virtual void on_point(const k3d::sds::position_t& Point, const k3d::sds::position_t& Normal = k3d::sds::position_t(0,0,1))
	{
		points_array[m_edge_points[m_edge]] = Point;
	}
	virtual void on_face(k3d::uint_t Face)
	{
		m_edge = m_loop_first_edges[m_face_first_loops[Face]];
	}
	virtual void on_edge()
	{
		m_edge = m_clockwise_edges[m_edge];
	}
	
	std::vector<k3d::sds::position_t> points_array;
private:
	const k3d::mesh::indices_t& m_clockwise_edges;
	const k3d::mesh::indices_t& m_edge_points;
	const k3d::mesh::indices_t& m_loop_first_edges;
	const k3d::mesh::indices_t& m_face_first_loops;
	k3d::uint_t m_edge;
};

} // namespace opengl

} // namespace painters

} // namespace module

#endif // !MODULES_OPENGL_PAINTERS_SDS_CACHE_H
