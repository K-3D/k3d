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

/** \file Declares structures to add extra "convenience" data to a mesh
	\author Bart Janssens (bart.janssens@lid.kviv.be)
*/

#include "mesh_topology_data.h"

namespace module
{

namespace development
{

companions::companions(const k3d::mesh& Mesh) : base(Mesh)
{
	const k3d::mesh::indices_t& edge_points = *Mesh.polyhedra->edge_points;
	const k3d::mesh::indices_t& clockwise_edges = *Mesh.polyhedra->clockwise_edges;
	std::map<std::pair<size_t, size_t>, size_t > edge_end_points; // maps edge start and endpoint to edge number
	k3d::uint_t edge_count = edge_points.size();
	for (k3d::uint_t edge = 0; edge != edge_count; ++edge)
	{
		edge_end_points[std::make_pair(edge_points[edge], edge_points[clockwise_edges[edge]])] = edge;
	}
	for (k3d::uint_t edge = 0; edge != edge_count; ++edge)
	{
		std::map<std::pair<size_t, size_t>, size_t >::iterator companion = edge_end_points.find(std::make_pair(edge_points[clockwise_edges[edge]], edge_points[edge]));
		if (companion != edge_end_points.end())
		{
			m_data.push_back(companion->second);
		}
		else
		{
			m_data.push_back(edge);
		}
	}
}

valencies::valencies(const k3d::mesh& Mesh) : base(Mesh)
{
	// Default to 0 for all points
	m_data.resize(Mesh.points->size(), 0);
	
	// Add 1 for each edge that starts at a point
	const k3d::mesh::indices_t& edge_points = *Mesh.polyhedra->edge_points;
	k3d::uint_t edge_count = edge_points.size();
	for (k3d::uint_t edge = 0; edge != edge_count; ++edge)
	{
		++m_data[edge_points[edge]];
	}
}

} // namespace development

} // namespace module
