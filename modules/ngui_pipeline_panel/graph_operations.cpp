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

#include <k3dsdk/ngui/document_state.h>

#include <k3dsdk/graph.h>
#include <k3dsdk/ipipeline.h>

#include <boost/assign/list_of.hpp>

using namespace libk3dngui;

namespace module
{

namespace ngui
{

namespace pipeline
{

namespace detail
{


} // namespace detail

void create_graph(document_state& DocumentState, k3d::graph& Graph)
{
	const k3d::nodes_t nodes = DocumentState.document().nodes().collection();

	boost::shared_ptr<k3d::graph::topology_t> topology(new k3d::graph::topology_t());

	boost::shared_ptr<nodes_t> vertex_node(new nodes_t());
	
	boost::shared_ptr<k3d::graph::indices_t> edge_type(new k3d::graph::indices_t());

	// Insert nodes ...
	std::map<k3d::inode*, size_t> node_map;
	for(k3d::nodes_t::const_iterator node = nodes.begin(); node != nodes.end(); ++node)
	{
		size_t vertex_descriptor = boost::add_vertex(*topology);
		node_map[*node] = vertex_descriptor;
		vertex_node->push_back(*node);
	}

	// Insert edges ...
	for(k3d::nodes_t::const_iterator node = nodes.begin(); node != nodes.end(); ++node)
	{
		if(k3d::iproperty_collection* const property_collection = dynamic_cast<k3d::iproperty_collection*>(*node))
		{
			const k3d::iproperty_collection::properties_t properties = property_collection->properties();
			for(k3d::iproperty_collection::properties_t::const_iterator property = properties.begin(); property != properties.end(); ++property)
			{
				if(typeid(k3d::inode*) == (*property)->property_type())
				{
					if(k3d::inode* const referenced_node = boost::any_cast<k3d::inode*>((*property)->property_value()))
					{
						boost::add_edge(node_map[referenced_node], node_map[*node], *topology);
						edge_type->push_back(BEHAVIOR_EDGE);
					}
				}
			}
		}
	}

	const k3d::ipipeline::dependencies_t dependencies = DocumentState.document().pipeline().dependencies();
	for(k3d::ipipeline::dependencies_t::const_iterator dependency = dependencies.begin(); dependency != dependencies.end(); ++dependency)
	{
		if(dependency->first && dependency->first->property_node() && dependency->second && dependency->second->property_node())
		{
			boost::add_edge(node_map[dependency->second->property_node()], node_map[dependency->first->property_node()], *topology);
			edge_type->push_back(DATA_EDGE);
		}
	}

	Graph.topology = topology;
	Graph.vertex_data["node"] = vertex_node;
	Graph.edge_data["type"] = edge_type;
}

void tree_plus_layout(
	const k3d::graph::topology_t& Topology,
	const std::vector<size_t>& Vertices,
	k3d::graph::bools_t& VertexVisible,
	const k3d::graph::bools_t& VertexExpanded,
	k3d::graph::ints_t& VertexColumn,
	k3d::graph::ints_t& VertexRow,
	size_t Column
	)
{
	std::vector<size_t> new_vertices;

	size_t row = 0;
	for(size_t i = 0; i != Vertices.size(); ++i)
	{
		const size_t vertex = Vertices[i];

//		if(!VertexExpanded[vertex])
//			continue;

		std::pair<k3d::graph::in_edge_iterator_t, k3d::graph::in_edge_iterator_t> in_edges = boost::in_edges(vertex, Topology);
		for(k3d::graph::in_edge_iterator_t edge = in_edges.first; edge != in_edges.second; ++edge)
		{
			const size_t edge_vertex = boost::source(*edge, Topology);

			if(VertexVisible[edge_vertex])
				continue;

			VertexVisible[edge_vertex] = true;
			VertexColumn[edge_vertex] = Column;
			VertexRow[edge_vertex] = row++;

			new_vertices.push_back(edge_vertex);
		}

		std::pair<k3d::graph::out_edge_iterator_t, k3d::graph::out_edge_iterator_t> out_edges = boost::out_edges(vertex, Topology);
		for(k3d::graph::out_edge_iterator_t edge = out_edges.first; edge != out_edges.second; ++edge)
		{
			const size_t edge_vertex = boost::target(*edge, Topology);

			if(VertexVisible[edge_vertex])
				continue;

			VertexVisible[edge_vertex] = true;
			VertexColumn[edge_vertex] = Column;
			VertexRow[edge_vertex] = row++;

			new_vertices.push_back(edge_vertex);
		}
	}

	if(!new_vertices.empty())
		tree_plus_layout(Topology, new_vertices, VertexVisible, VertexExpanded, VertexColumn, VertexRow, Column + 1);
}

void tree_plus_layout(k3d::graph& Graph, k3d::graph::vertex_descriptor_t Root)
{
	return_if_fail(Graph.topology);

	const k3d::graph::topology_t& topology = *Graph.topology;

	k3d::graph::bools_t& vertex_visible = get_array<k3d::graph::bools_t>(Graph.vertex_data, "visible", boost::num_vertices(topology));
	k3d::graph::bools_t& vertex_expanded = get_array<k3d::graph::bools_t>(Graph.vertex_data, "expanded", boost::num_vertices(topology));
	k3d::graph::ints_t& vertex_column = get_array<k3d::graph::ints_t>(Graph.vertex_data, "column", boost::num_vertices(topology));
	k3d::graph::ints_t& vertex_row = get_array<k3d::graph::ints_t>(Graph.vertex_data, "row", boost::num_vertices(topology));
	
	k3d::graph::bools_t& edge_visible = get_array<k3d::graph::bools_t>(Graph.edge_data, "visible", boost::num_edges(topology));

	std::fill(vertex_visible.begin(), vertex_visible.end(), false);
	std::fill(edge_visible.begin(), edge_visible.end(), false);

	vertex_visible[Root] = true;
	vertex_column[Root] = 0;
	vertex_row[Root] = 0;

	tree_plus_layout(topology, std::vector<size_t>(1, Root), vertex_visible, vertex_expanded, vertex_column, vertex_row, 1);

	std::pair<k3d::graph::edge_iterator_t, k3d::graph::edge_iterator_t> edges = boost::edges(topology);
	size_t edge_index = 0;
	for(k3d::graph::edge_iterator_t edge = edges.first; edge != edges.second; ++edge, ++edge_index)
	{
		if(vertex_visible[boost::source(*edge, topology)] && vertex_visible[boost::target(*edge, topology)])
			edge_visible[edge_index] = true;
	}
}

} // namespace pipeline

} // namespace ngui

} // namespace module

