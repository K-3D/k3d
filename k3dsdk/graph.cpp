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

namespace detail
{

static void print(std::ostream& Stream, const boost::shared_ptr<const graph::adjacency_list>& Topology)
{
	if(!Topology)
		return;

	Stream << "  vertices: (" << boost::num_vertices(*Topology) << ")\n";

	Stream << "  edges: (" << boost::num_edges(*Topology) << ")\n";
	for(std::pair<graph::edge_iterator, graph::edge_iterator> edges = boost::edges(*Topology); edges.first != edges.second; ++edges.first)
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

static void print(std::ostream& Stream, const std::string& Name, const boost::shared_ptr<array>& Array)
{
	Stream << "  " << Name << ": ";
	if(Array)
	{
		Stream << "(" << Array->size() << ") ";
		if(print<graph::bools>(Stream, *Array))
		{
		}
		else if(print<graph::indices>(Stream, *Array))
		{
		}
		else if(print<graph::doubles>(Stream, *Array))
		{
		}
		else if(print<graph::ints>(Stream, *Array))
		{
		}
		else if(print<graph::strings>(Stream, *Array))
		{
		}
		else if(print<graph::points>(Stream, *Array))
		{
		}
		else if(print<graph::vectors>(Stream, *Array))
		{
		}
		else if(print<graph::nodes>(Stream, *Array))
		{
		}
		else
		{
            		Stream << "unsupported type [" << k3d::demangle(typeid(*Array)) << "]" << std::endl;
		}
	}

	Stream << "\n";
}

static void print(std::ostream& Stream, const k3d::graph::named_arrays& Arrays)
{
    for(k3d::graph::named_arrays::const_iterator array = Arrays.begin(); array != Arrays.end(); ++array)
    	print(Stream, array->first, array->second);
}

} // namespace detail

////////////////////////////////////////////////////////////////////////////////////
// graph

graph::graph()
{
}

/** \todo Print the topology */
std::ostream& operator<<(std::ostream& Stream, const graph& RHS)
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

} // namespace k3d

