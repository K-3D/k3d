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

#include <boost/assign/list_of.hpp>
#include <boost/graph/circle_layout.hpp>
#include <boost/graph/connected_components.hpp>
#include <boost/graph/fruchterman_reingold.hpp>
#include <boost/graph/gursoy_atun_layout.hpp>
#include <boost/graph/random_layout.hpp>
#include <boost/random/mersenne_twister.hpp>

using namespace libk3dngui;

namespace module
{

namespace ngui_pipeline
{

namespace detail
{

class position_map
{
public:
	struct point_proxy
	{
		double x;
		double y;
	};

	position_map(k3d::typed_array<k3d::point2>& Storage) :
		storage(Storage)
	{
	}

	point_proxy& operator[](const size_t Index)
	{
		return reinterpret_cast<point_proxy&>(storage[Index]);
	}

	k3d::typed_array<k3d::point2>& storage;
};

template<typename storage_t>
class property_map
{
public:
	typedef typename storage_t::size_type key_type;
	typedef typename storage_t::value_type value_type;
	typedef typename storage_t::reference reference;
	typedef boost::lvalue_property_map_tag category;

	property_map(storage_t& Storage) :
		storage(Storage)
	{
	}

	storage_t& storage;
};

template<typename property_map_t>
void put(property_map_t& Map, typename property_map_t::key_type Key, typename property_map_t::value_type Value)
{
	Map.storage[Key] = Value;
}

} // namespace detail

void create_graph(document_state& DocumentState, k3d::graph& Graph)
{
	const k3d::nodes_t nodes = DocumentState.document().nodes().collection();

	boost::shared_ptr<k3d::graph::topology_t> topology(new k3d::graph::topology_t());
	boost::shared_ptr<k3d::graph::strings_t> vertex_label(new k3d::graph::strings_t());
	boost::shared_ptr<k3d::graph::indices_t> edge_type(new k3d::graph::indices_t());

	// Insert nodes ...
	std::map<k3d::inode*, size_t> node_map;
	for(k3d::nodes_t::const_iterator node = nodes.begin(); node != nodes.end(); ++node)
	{
		size_t vertex_descriptor = boost::add_vertex(*topology);
		node_map[*node] = vertex_descriptor;
		vertex_label->push_back((*node)->name());
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
//								stream << " [style=dotted,label=\"" << escaped_string((*property)->property_name()) << "\"]\n";
					}
				}
			}
		}
	}

	const k3d::idag::dependencies_t dependencies = DocumentState.document().dag().dependencies();
	for(k3d::idag::dependencies_t::const_iterator dependency = dependencies.begin(); dependency != dependencies.end(); ++dependency)
	{
		if(dependency->first && dependency->first->property_node() && dependency->second && dependency->second->property_node())
		{
			boost::add_edge(node_map[dependency->second->property_node()], node_map[dependency->first->property_node()], *topology);
			edge_type->push_back(DATA_EDGE);
//					stream << " [headlabel=\"" << escaped_string(dependency->first->property_name()) << "\" taillabel=\"" << escaped_string(dependency->second->property_name()) << "\"]\n";
		}
	}

	Graph.topology = topology;
	Graph.vertex_data["label"] = vertex_label;
	Graph.edge_data["type"] = edge_type;
}

void circular_layout(k3d::graph& Graph)
{
	return_if_fail(Graph.topology);

	const k3d::graph::topology_t& topology = *Graph.topology;
	k3d::graph::points_t* vertex_position = 0;
	if(Graph.vertex_data.count("position"))
		vertex_position = dynamic_cast<k3d::graph::points_t*>(Graph.vertex_data["position"].get());
	
	if(!vertex_position)
	{
		vertex_position = new k3d::graph::points_t(boost::num_vertices(topology));
		Graph.vertex_data["position"].reset(vertex_position);
	}

	detail::position_map position_map(*vertex_position);
	boost::circle_graph_layout(topology, position_map, 0.5);

}

void random_layout(k3d::graph& Graph)
{
	return_if_fail(Graph.topology);

	const k3d::graph::topology_t& topology = *Graph.topology;
	k3d::graph::points_t* vertex_position = 0;
	if(Graph.vertex_data.count("position"))
		vertex_position = dynamic_cast<k3d::graph::points_t*>(Graph.vertex_data["position"].get());
	
	if(!vertex_position)
	{
		vertex_position = new k3d::graph::points_t(boost::num_vertices(topology));
		Graph.vertex_data["position"].reset(vertex_position);
	}

	detail::position_map position_map(*vertex_position);
	boost::mt19937 rng;
	boost::random_graph_layout(topology, position_map, 0.0, 1.0, 0.0, 1.0, rng);
}

/*
void icicle_layout(k3d::graph& Graph)
{
	return_if_fail(Graph.topology);

	const k3d::graph::topology_t& topology = *Graph.topology;
	boost::shared_ptr<k3d::graph::points_t> vertex_position(new k3d::graph::points_t(boost::num_vertices(topology)));

	// Begin with a flat collection of vertices ...
	typedef std::vector<size_t> icicle_t;
	typedef std::map<size_t, icicle_t> icicles_t;
	icicles_t icicles;

	const size_t vertex_begin = 0;
	const size_t vertex_end = boost::num_vertices(topology);
	for(size_t vertex = vertex_begin; vertex != vertex_end; ++vertex)
		icicles.insert(std::make_pair(vertex, std::vector<size_t>()));

	// Merge vertices into vertical "icicles" based on connectivity ...
	for(icicles_t::iterator icicle = icicles.begin(); icicle != icicles.end(); )
	{
		if(boost::out_degree(icicle->first, topology))
		{
			typedef boost::graph_traits<k3d::graph::topology_t>::out_edge_iterator iter_t;
			std::pair<iter_t, iter_t> out_edges = boost::out_edges(icicle->first, topology);

			const size_t target = boost::target(*out_edges.first, topology);
			icicles[target].push_back(icicle->first);
			icicles[target].insert(icicles[target].end(), icicle->second.begin(), icicle->second.end());
			icicles.erase(icicle++);
		}
		else
		{
			++icicle;
		}
	}

	// Assign coordinates to each vertex ...
	const double x_offset = 0.2;
	const double y_offset = 0.06;

	double x = 0;
	for(icicles_t::iterator icicle = icicles.begin(); icicle != icicles.end(); ++icicle)
	{
		double y = 0;

		for(icicle_t::reverse_iterator vertex = icicle->second.rbegin(); vertex != icicle->second.rend(); ++vertex)
		{
			(*vertex_position)[*vertex][0] = x;
			(*vertex_position)[*vertex][1] = y;

			y += y_offset;
		}

		(*vertex_position)[icicle->first][0] = x;
		(*vertex_position)[icicle->first][1] = y;

		x += x_offset;
	}

	Graph.vertex_data["position"] = vertex_position;
}
*/

void force_directed_layout(k3d::graph& Graph)
{
	return_if_fail(Graph.topology);

	const k3d::graph::topology_t& topology = *Graph.topology;
	k3d::graph::points_t* vertex_position = 0;

	if(Graph.vertex_data.count("position"))
		vertex_position = dynamic_cast<k3d::graph::points_t*>(Graph.vertex_data["position"].get());
	
	if(!vertex_position)
	{
		vertex_position = new k3d::graph::points_t(boost::num_vertices(topology));
		Graph.vertex_data["position"].reset(vertex_position);
	}

	const size_t vertex_begin = 0;
	const size_t vertex_end = boost::num_vertices(topology);

	// Handle repulsion forces ...
	for(size_t vertex = vertex_begin; vertex != vertex_end; ++vertex)
	{
		for(size_t other = vertex_begin; other != vertex_end; ++other)
		{
			if(vertex != other)
			{
				const k3d::vector2 difference = (*vertex_position)[vertex] - (*vertex_position)[other];
				(*vertex_position)[vertex] += k3d::normalize(difference) * (0.001 / difference.length2());
			}
		}
	}

	// Handle attraction forces ...
	typedef boost::graph_traits<k3d::graph::topology_t>::edge_iterator iter_t;
	for(std::pair<iter_t, iter_t> edges = boost::edges(topology); edges.first != edges.second; ++edges.first)
	{
		const size_t source = boost::source(*edges.first, topology);
		const size_t target = boost::target(*edges.first, topology);

		if(source != target)
		{
			const k3d::vector2 difference = (*vertex_position)[target] - (*vertex_position)[source];
			const k3d::vector2 force = k3d::normalize(difference) * (0.01 * difference.length2());
			
			(*vertex_position)[source] += force;
			(*vertex_position)[target] -= force;
		}
	}
}

} // namespace ngui_pipeline

} // namespace module

