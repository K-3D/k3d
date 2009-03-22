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

#include "tree_layout.h"


#include <boost/graph/breadth_first_search.hpp>

namespace module
{

namespace ngui
{

namespace pipeline
{

/// BFS visitor class that computes rank and item for each node in a tree
class tree_layout::coordinate_visitor :
	public boost::default_bfs_visitor
{
public:
	coordinate_visitor(k3d::graph::indices_t& VertexRank, k3d::graph::indices_t& VertexItem, std::map<k3d::uint_t, k3d::uint_t>& RankCounts) :
		vertex_rank(VertexRank),
		vertex_item(VertexItem),
		rank_counts(RankCounts)
	{
	}

	template<typename edge_t, typename graph_t>
	void tree_edge(const edge_t Edge, const graph_t Graph)
	{
		const k3d::uint_t rank = vertex_rank[boost::source(Edge, Graph)] + 1;

		vertex_rank[boost::target(Edge, Graph)] = rank;
		vertex_item[boost::target(Edge, Graph)] = rank_counts[rank]++;

//		rank_counts[rank] += 1;
	}

private:
	k3d::graph::indices_t& vertex_rank;
	k3d::graph::indices_t& vertex_item;
	std::map<k3d::uint_t, k3d::uint_t>& rank_counts;
};

tree_layout::tree_layout() :
	m_column_offset(init_owner(*this) + init_name("column_offset") + init_label("") + init_description("") + init_value(1.0)),
	m_row_offset(init_owner(*this) + init_name("row_offset") + init_label("") + init_description("") + init_value(1.0))
{
	m_column_offset.changed_signal().connect(make_reset_graph_slot());
	m_row_offset.changed_signal().connect(make_reset_graph_slot());
}

void tree_layout::on_initialize_graph(const k3d::graph::undirected& Input, k3d::graph::undirected& Output)
{
	// The input graph must be a tree ...
	const k3d::graph::indices_t* const root_array = Input.graph_data.lookup<k3d::graph::indices_t>("root");
	return_if_fail(root_array);
	return_if_fail(root_array->size() == 1);
	const k3d::uint_t root = root_array->at(0);

	return_if_fail(Input.topology);
	const k3d::graph::undirected::adjacency_list_t& input_topology = *Input.topology;
	const k3d::uint_t vertex_count = boost::num_vertices(input_topology);

	// Shallow-copy our input ...
	Output = Input;

	// Use a BFS to calculate each vertex' rank and number (its position within its rank) ...
	k3d::graph::indices_t vertex_rank(vertex_count, 0);
	k3d::graph::indices_t vertex_item(vertex_count, 0);
	std::map<k3d::uint_t, k3d::uint_t> rank_counts;
	boost::breadth_first_search(input_topology, root, visitor(coordinate_visitor(vertex_rank, vertex_item, rank_counts)));

	// Convert rank and item numbers into 2D coordinates ...
	const k3d::double_t column_offset = m_column_offset.pipeline_value();
	const k3d::double_t row_offset = m_row_offset.pipeline_value();

	k3d::graph::points_2d_t& vertex_position = Output.vertex_data.create("position", new k3d::graph::points_2d_t(vertex_count));
	for(k3d::uint_t vertex = 0; vertex != vertex_count; ++vertex)
	{
		const k3d::uint_t rank = vertex_rank[vertex];
		const k3d::uint_t item = vertex_item[vertex];
		const k3d::uint_t item_offset = rank_counts[rank] ? rank_counts[rank] - 1 : 0;

		vertex_position[vertex] = k3d::point2(rank * column_offset, (item * row_offset) - (item_offset * row_offset * 0.5));
	}
}

void tree_layout::on_update_graph(const k3d::graph::undirected& Input, k3d::graph::undirected& Output)
{
}

} // namespace pipeline

} // namespace ngui

} // namespace module

