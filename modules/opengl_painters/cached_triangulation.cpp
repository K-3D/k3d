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

#include "cached_triangulation.h"

namespace module
{

namespace opengl
{

namespace painters
{

void cached_triangulation::on_execute(const k3d::mesh& Mesh, k3d::inode* Painter)
{
	m_input_points = Mesh.points;
	if (m_points.empty())
	{
		m_progress = 0;
		m_point_links.resize(Mesh.points->size());
		m_points.resize(Mesh.polyhedra->edge_points->size());
		k3d::triangulator::process(Mesh);
	}
	else
	{
		if (m_affected_indices.empty())
		{
			for (k3d::uint_t index = 0; index != m_point_links.size(); ++index)
			{
				k3d::mesh::indices_t& linked_points = m_point_links[index];
				for (k3d::uint_t i = 0; i != linked_points.size(); ++i)
				{
					m_points[linked_points[i]] = m_input_points->at(index);
				} 
			}
		}
		else
		{
			for (k3d::uint_t index = 0; index != m_affected_indices.size(); ++index)
			{
				k3d::mesh::indices_t& linked_points = m_point_links[m_affected_indices[index]];
				for (k3d::uint_t i = 0; i != linked_points.size(); ++i)
				{
					m_points[linked_points[i]] = m_input_points->at(m_affected_indices[index]);
				} 
			}
		}
	}
	m_affected_indices.clear();
}

void cached_triangulation::start_face(const k3d::uint_t Face)
{
	m_point_map.clear();
	m_face_starts.push_back(m_indices.size());
	m_face_points.push_back(k3d::mesh::indices_t());
}

void cached_triangulation::add_vertex(const k3d::point3& Coordinates, k3d::uint_t Vertices[4], k3d::uint_t Edges[4], k3d::double_t Weights[4], k3d::uint_t& NewVertex)
{
	NewVertex = m_points.size();
	k3d::log() << debug << "added new vertex: " << NewVertex << std::endl;
	m_point_map.insert(std::make_pair(m_points.size(), m_points.size()));
	m_face_points.back().push_back(m_points.size());
	m_points.push_back(Coordinates);
}

void cached_triangulation::add_triangle(k3d::uint_t Vertices[3], k3d::uint_t Edges[3])
{
	typedef std::pair<point_map_t::iterator, bool> result_t;
	// Create point copies for this face, if they don't exist already
	result_t r1 = m_point_map.insert(std::make_pair(Vertices[0], m_progress));
	if (r1.second)
	{
		m_point_links[Vertices[0]].push_back(m_progress);
		m_face_points.back().push_back(m_progress);
		m_points[m_progress] = m_input_points->at(Vertices[0]);
		++m_progress;
	}
	result_t r2 = m_point_map.insert(std::make_pair(Vertices[1], m_progress));
	if (r2.second)
	{
		m_point_links[Vertices[1]].push_back(m_progress);
		m_face_points.back().push_back(m_progress);
		m_points[m_progress] = m_input_points->at(Vertices[1]);
		++m_progress;
	}
	result_t r3 = m_point_map.insert(std::make_pair(Vertices[2], m_progress));
	if (r3.second)
	{
		m_point_links[Vertices[2]].push_back(m_progress);
		m_face_points.back().push_back(m_progress);
		m_points[m_progress] = m_input_points->at(Vertices[2]);
		++m_progress;
	}
		
	// Store corner indices
	m_indices.push_back(r1.first->second);
	m_indices.push_back(r2.first->second);
	m_indices.push_back(r3.first->second);
}

} // namespace opengl

} // namespace painters

} // namespace module
