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
// License along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

#include "color.h"
#include "graph.h"
#include "type_registry.h"

#include <boost/graph/adjacency_list_io.hpp>

#include <iterator>

namespace k3d
{

namespace graph
{

namespace detail
{

static void print(std::ostream& Stream, const pipeline_data<undirected::adjacency_list_t>& Topology)
{
	if(!Topology)
		return;

	Stream << "  vertices: (" << boost::num_vertices(*Topology) << ")\n";

	Stream << "  edges: (" << boost::num_edges(*Topology) << ")\n";
	for(std::pair<undirected::edge_iterator_t, undirected::edge_iterator_t> edges = boost::edges(*Topology); edges.first != edges.second; ++edges.first)
		Stream << "    index: " << (*Topology)[*edges.first].index << " edge: " << *edges.first << "\n";
}

template<typename array_t>
static bool print(std::ostream& Stream, const array& Array)
{
	if(const array_t* const concrete_array = dynamic_cast<const array_t*>(&Array))
	{
		std::copy(concrete_array->begin(), concrete_array->end(), std::ostream_iterator<typename array_t::value_type>(Stream, " "));
		return true;
	}

	return false;
}

static void print(std::ostream& Stream, const std::string& Name, const pipeline_data<array>& Array)
{
	Stream << "  " << Name << ": ";
	if(Array)
	{
		Stream << "(" << Array->size() << ") ";
		if(print<graph::bools_t>(Stream, *Array))
		{
		}
		else if(print<graph::indices_t>(Stream, *Array))
		{
		}
		else if(print<graph::doubles_t>(Stream, *Array))
		{
		}
		else if(print<graph::integers_t>(Stream, *Array))
		{
		}
		else if(print<graph::strings_t>(Stream, *Array))
		{
		}
		else if(print<graph::points_2d_t>(Stream, *Array))
		{
		}
		else if(print<graph::vectors_2d_t>(Stream, *Array))
		{
		}
		else if(print<graph::nodes_t>(Stream, *Array))
		{
		}
		else
		{
            		Stream << "unsupported type [" << k3d::demangle(typeid(*Array)) << "]" << std::endl;
		}
	}

	Stream << "\n";
}

static void print(std::ostream& Stream, const k3d::graph::attribute_arrays_t& Arrays)
{
	for(k3d::graph::attribute_arrays_t::const_iterator array = Arrays.begin(); array != Arrays.end(); ++array)
		print(Stream, array->first, array->second);
}

} // namespace detail

////////////////////////////////////////////////////////////////////////////////////
// undirected

undirected::undirected()
{
}

/** \todo Print the topology */
std::ostream& operator<<(std::ostream& Stream, const undirected& RHS)
{
	Stream << "topology:" << "\n";
	detail::print(Stream, RHS.topology);

	Stream << "graph data:" << "\n";
	detail::print(Stream, RHS.graph_data);

	Stream << "vertex data:" << "\n";
	detail::print(Stream, RHS.vertex_data);
	
	Stream << "edge data:" << "\n";
	detail::print(Stream, RHS.edge_data);

	return Stream;
}

} // namespace graph

} // namespace k3d

