#ifndef SDS_CACHE_H_
#define SDS_CACHE_H_

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
#include <k3dsdk/painter_cache.h>
#include <k3dsdk/subdivision_surface/k3d_sds_binding.h>

namespace module
{

namespace development
{

/// Color with alpha channel support
typedef k3d::basic_rgba<double, k3d::color_traits<double> > color_t;

/// Common SDS cache functionality
class sds_cache : public k3d::scheduler
{
public:
	sds_cache() : levels(2), update(true) {}
	
	~sds_cache();
	
	/// Notify the cache that one of the registered painters changed level
	void level_changed();
	
	/// Register a level property
	void register_property(k3d::iproperty* LevelProperty);
	
	/// Remove a level property
	void remove_property(k3d::iproperty* LevelProperty);
	
	int levels;
	k3d::mesh::indices_t indices;

	bool update;
	
	/// The explorable subdivided geometry
	k3d::sds::k3d_sds_cache cache;
protected:
	/// Scheduler implementation
	virtual void on_execute(const k3d::mesh& Mesh);
private:
	typedef std::set<k3d::iproperty*> levels_t;
	levels_t m_levels;
	// store connections for safe deletion of cache
	std::vector<sigc::connection> m_connections;
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

} // namespace development

} // namespace module

#endif /*SDS_CACHE_H_*/
