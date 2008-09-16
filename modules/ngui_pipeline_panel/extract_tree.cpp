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

#include "extract_tree.h"

#include <k3dsdk/attribute_array_copier.h>

#include <boost/graph/breadth_first_search.hpp>

namespace module
{

namespace ngui
{

namespace pipeline
{

namespace detail
{

/// BFS visitor class that marks vertices and edges for extraction from a graph
class extract_tree_visitor :
	public boost::default_bfs_visitor
{
public:
	extract_tree_visitor(k3d::graph::bools_t& ExtractVertex, k3d::graph::bools_t& ExtractEdge) :
		extract_vertex(ExtractVertex),
		extract_edge(ExtractEdge)
	{
	}

	template<typename vertex_t, typename graph_t>
	void discover_vertex(const vertex_t Vertex, const graph_t& Graph)
	{
		extract_vertex[Vertex] = true;
	}

	template<typename edge_t, typename graph_t>
	void tree_edge(const edge_t Edge, const graph_t Graph)
	{
		extract_edge[Graph[Edge].index] = true;
	}

private:
	k3d::graph::bools_t& extract_vertex;
	k3d::graph::bools_t& extract_edge;
};

} // namespace detail

extract_tree::extract_tree() :
	m_root(init_owner(*this) + init_name("root") + init_label("") + init_description("") + init_value(0))
{
	m_root.changed_signal().connect(make_reset_graph_slot());
}

void extract_tree::on_initialize_graph(const k3d::graph& Input, k3d::graph& Output)
{
	return_if_fail(Input.topology);
	const k3d::graph::adjacency_list_t& input_topology = *Input.topology;
	const k3d::uint_t vertex_count = boost::num_vertices(input_topology);
	const k3d::uint_t edge_count = boost::num_edges(input_topology);

	// Use a simple BFS to determine which vertices and edges should appear in the output tree ...
	k3d::graph::bools_t extract_vertex(vertex_count, false);
	k3d::graph::bools_t extract_edge(edge_count, false);
	detail::extract_tree_visitor extract_tree_visitor(extract_vertex, extract_edge);
	boost::breadth_first_search(input_topology, m_root.pipeline_value(), visitor(extract_tree_visitor));

	// Create a new graph, only copying those vertices and edges that were marked for extraction ...
	k3d::graph::adjacency_list_t& output_topology = Output.topology.create();

	Output.graph_data = Input.graph_data;
	Output.vertex_data = Input.vertex_data.clone_types();
	Output.edge_data = Input.edge_data.clone_types();

	k3d::attribute_array_copier vertex_data_copier(Input.vertex_data, Output.vertex_data);
	k3d::attribute_array_copier edge_data_copier(Input.edge_data, Output.edge_data);

	std::map<k3d::graph::vertex_descriptor_t, k3d::graph::vertex_descriptor_t> vertex_map;
	for(k3d::graph::vertex_descriptor_t vertex = 0; vertex != vertex_count; ++vertex)
	{
		if(extract_vertex[vertex])
		{
			vertex_map[vertex] = boost::add_vertex(output_topology);
			vertex_data_copier.push_back(vertex);
		}
	}

	for(std::pair<k3d::graph::edge_iterator_t, k3d::graph::edge_iterator_t> edges = boost::edges(input_topology); edges.first != edges.second; ++edges.first)
	{
		const k3d::graph::edge_descriptor_t edge = *edges.first;
		const k3d::uint_t edge_index = input_topology[edge].index;

		if(extract_edge[edge_index])
		{
			boost::add_edge(vertex_map[boost::source(edge, input_topology)], vertex_map[boost::target(edge, input_topology)], output_topology);
			edge_data_copier.push_back(edge_index);
		}
	}

	// Mark the new graph as a tree by specifying the index of the root vertex
	k3d::graph::indices_t& root = Output.graph_data.create<k3d::graph::indices_t>("root");
	root.push_back(vertex_map[m_root.pipeline_value()]);
}

void extract_tree::on_update_graph(const k3d::graph& Input, k3d::graph& Output)
{
}

} // namespace pipeline

} // namespace ngui

} // namespace module

