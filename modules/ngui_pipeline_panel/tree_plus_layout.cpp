// K-3D
// Copyright (c) 1995-2007, Timothy M. Shead
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
// License along with this program; if not, read to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

/** \file
	\author Timothy M. Shead
*/

#include "graph_operations.h"
#include "tree_plus_layout.h"

namespace module
{

namespace ngui
{

namespace pipeline
{

namespace detail
{

void tree_plus_layout(
	const k3d::graph::topology_t& Topology,
	const std::vector<size_t>& Vertices,
	k3d::graph::bools_t& VertexVisible,
	const k3d::graph::bools_t& VertexExpanded,
	k3d::graph::points_t& VertexPosition,
	const size_t Column,
	const double ColumnOffset,
	const double RowOffset
	)
{
	std::vector<size_t> new_vertices;

	for(size_t i = 0; i != Vertices.size(); ++i)
	{
		const size_t vertex = Vertices[i];

		VertexVisible[vertex] = true;
		VertexPosition[vertex] = k3d::point2(Column * ColumnOffset, i * RowOffset);

		std::pair<k3d::graph::in_edge_iterator_t, k3d::graph::in_edge_iterator_t> in_edges = boost::in_edges(vertex, Topology);
		for(k3d::graph::in_edge_iterator_t edge = in_edges.first; edge != in_edges.second; ++edge)
		{
			const size_t edge_vertex = boost::source(*edge, Topology);

			if(VertexVisible[edge_vertex])
				continue;

			new_vertices.push_back(edge_vertex);
		}

		std::pair<k3d::graph::out_edge_iterator_t, k3d::graph::out_edge_iterator_t> out_edges = boost::out_edges(vertex, Topology);
		for(k3d::graph::out_edge_iterator_t edge = out_edges.first; edge != out_edges.second; ++edge)
		{
			const size_t edge_vertex = boost::target(*edge, Topology);

			if(VertexVisible[edge_vertex])
				continue;

			new_vertices.push_back(edge_vertex);
		}
	}

	if(!new_vertices.empty())
		tree_plus_layout(Topology, new_vertices, VertexVisible, VertexExpanded, VertexPosition, Column + 1, ColumnOffset, RowOffset);
}


} // namespace detail

tree_plus_layout::tree_plus_layout() :
	m_root(init_owner(*this) + init_name("root") + init_label(_("")) + init_description(_("")) + init_value(0)),
	m_column_offset(init_owner(*this) + init_name("column_offset") + init_label(_("")) + init_description(_("")) + init_value(1.0)),
	m_row_offset(init_owner(*this) + init_name("row_offset") + init_label(_("")) + init_description(_("")) + init_value(1.0))
{
	m_root.changed_signal().connect(make_reset_graph_slot());
	m_column_offset.changed_signal().connect(make_reset_graph_slot());
	m_row_offset.changed_signal().connect(make_reset_graph_slot());
}

void tree_plus_layout::on_initialize_graph(const k3d::graph& Input, k3d::graph& Output)
{
	Output = Input;

	return_if_fail(Output.topology);

	const k3d::graph::topology_t& topology = *Output.topology;
	const size_t vertex_count = boost::num_vertices(topology);

	k3d::graph::bools_t& vertex_visible = get_array<k3d::graph::bools_t>(Output.vertex_data, "visible", vertex_count);
	k3d::graph::bools_t& vertex_expanded = get_array<k3d::graph::bools_t>(Output.vertex_data, "expanded", vertex_count);
	k3d::graph::points_t& vertex_position = get_array<k3d::graph::points_t>(Output.vertex_data, "position", vertex_count);
	
	k3d::graph::bools_t& edge_visible = get_array<k3d::graph::bools_t>(Output.edge_data, "visible", boost::num_edges(topology));

	std::fill(vertex_visible.begin(), vertex_visible.end(), false);
	std::fill(edge_visible.begin(), edge_visible.end(), false);

	std::vector<size_t> vertices(1, m_root.pipeline_value());
	detail::tree_plus_layout(topology, vertices, vertex_visible, vertex_expanded, vertex_position, 0, m_column_offset.pipeline_value(), m_row_offset.pipeline_value());

	std::pair<k3d::graph::edge_iterator_t, k3d::graph::edge_iterator_t> edges = boost::edges(topology);
	size_t edge_index = 0;
	for(k3d::graph::edge_iterator_t edge = edges.first; edge != edges.second; ++edge, ++edge_index)
	{
		if(vertex_visible[boost::source(*edge, topology)] && vertex_visible[boost::target(*edge, topology)])
			edge_visible[edge_index] = true;
	}
}

void tree_plus_layout::on_update_graph(const k3d::graph& Input, k3d::graph& Output)
{
}

} // namespace pipeline

} // namespace ngui

} // namespace module

