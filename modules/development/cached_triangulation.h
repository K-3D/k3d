#ifndef CACHED_TRIANGULATION_H_
#define CACHED_TRIANGULATION_H_

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

/** \file Cached triangulation for use in mesh painters.
 * 	\author Bart Janssens (bart.janssens@lid.kviv.be)
 */

#include <k3dsdk/hints.h>
#include <k3dsdk/painter_cache.h>
#include <k3dsdk/triangulator.h>

namespace module
{

namespace development
{

class cached_triangulation :
	public k3d::hint::hint_processor,
	public k3d::triangulator,
	public k3d::scheduler
{
public:
	/// Links a single index to a list of indices
	typedef std::vector<k3d::mesh::indices_t> index_vectors_t;

	k3d::mesh::points_t& points()
	{
		return m_points;
	}
	
	k3d::mesh::indices_t& indices()
	{
		return m_indices;
	}
	
	k3d::mesh::indices_t& face_starts()
	{
		return m_face_starts;
	}
	
	index_vectors_t& point_links()
	{
		return m_point_links;
	}
	
	index_vectors_t& face_points()
	{
		return m_face_points;
	}
	
private:

	void on_geometry_changed(const k3d::mesh& Mesh, k3d::iunknown* Hint)
	{
		if (m_affected_indices.empty())
		{
			k3d::hint::mesh_geometry_changed_t* geohint = dynamic_cast<k3d::hint::mesh_geometry_changed_t*>(Hint);
			m_affected_indices = geohint->changed_points;
		}
	}

	void on_topology_changed(const k3d::mesh& Mesh, k3d::iunknown* Hint)
	{
		m_indices.clear();
		m_points.clear();
		m_face_starts.clear();
		m_point_links.clear();
	}

	void on_schedule(const k3d::mesh& Mesh, k3d::iunknown* Hint)
	{
		k3d::hint::hint_processor::process(Mesh, Hint);
	}
	
	void on_execute(const k3d::mesh& Mesh);
	
	virtual void on_add_vertex(const k3d::point3& Coordinates, k3d::uint_t Vertices[4], k3d::double_t Weights[4], k3d::uint_t& NewVertex)
	{
		k3d::log() << debug << "New vertex in triangulated painter: " << Coordinates << ", " << NewVertex << std::endl;
	}
	
	virtual void on_add_triangle(const k3d::uint_t Point1, const k3d::uint_t Point2, const k3d::uint_t Point3);
	
	virtual void on_begin();
	
	boost::shared_ptr<const k3d::mesh::points_t> m_input_points;
	
	// mapping between mesh point index and triangulation point index (valid for the face being processed only!)
	typedef std::map<k3d::uint_t, k3d::uint_t> point_map_t;
	point_map_t m_point_map;
	
	// points used by the triangles
	k3d::mesh::points_t m_points;
	
	// indices into m_points for the triangle corners
	k3d::mesh::indices_t m_indices;
	
	// First triangle corner (index into m_indices) for each original face
	k3d::mesh::indices_t m_face_starts;
	
	// Link between original point indices and the indices of its copies
	index_vectors_t m_point_links;
	
	// Link between a face and its corners in the triangle representation
	index_vectors_t m_face_points;
	
	k3d::mesh::indices_t m_affected_indices;
};

}

}

#endif /*CACHED_TRIANGULATION_H_*/
