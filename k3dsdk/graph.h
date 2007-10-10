#ifndef K3DSDK_GRAPH_H
#define K3DSDK_GRAPH_H

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

#include "named_arrays.h"
#include "point2.h"
#include "typed_array.h"
#include "vector2.h"

#include <boost/graph/adjacency_list.hpp>

namespace k3d
{

class inode;

////////////////////////////////////////////////////////////////////////////////
// graph

/// Encapsulates a directed graph stored as an adjacency list
class graph
{
public:
	graph();

	struct vertex
	{
	};

	struct edge
	{
		edge() : index(0) {}
		uint_t index;
	};

	/// Defines storage for a generic graph topology
	typedef boost::adjacency_list<boost::listS, boost::vecS, boost::undirectedS, vertex, edge> adjacency_list_t;
	typedef boost::graph_traits<adjacency_list_t>::vertex_descriptor vertex_descriptor_t;
	typedef boost::graph_traits<adjacency_list_t>::edge_descriptor edge_descriptor_t;
	typedef boost::graph_traits<adjacency_list_t>::vertex_iterator vertex_iterator_t;
	typedef boost::graph_traits<adjacency_list_t>::edge_iterator edge_iterator_t;
	typedef boost::graph_traits<adjacency_list_t>::out_edge_iterator out_edge_iterator_t;
	typedef boost::graph_traits<adjacency_list_t>::in_edge_iterator in_edge_iterator_t;

	/// Defines storage for a generic collection of boolean values
	typedef typed_array<bool_t> bools_t;
	/// Defines storage for a generic collection of integer values
	typedef typed_array<int32_t> ints_t;
	/// Defines storage for a generic collection of index values
	typedef typed_array<uint_t> indices_t;
	/// Defines storage for a generic collection of floating-point values
	typedef typed_array<double_t> doubles_t;
	/// Defines storage for a generic collection of string values
	typedef typed_array<string_t> strings_t;
	/// Defines storage for a generic collection of two-dimensional points
	typedef typed_array<point2> points_t;
	/// Defines storage for a generic collection of two-dimensional vectors
	typedef typed_array<vector2> vectors_t;
	/// Defines storage for a generic collection of inode objects
	typedef typed_array<inode*> nodes_t;
	/// Defines a heterogeneous collection of named, shared arrays
	typedef k3d::named_arrays named_arrays_t;

	/// Stores the graph topology
	boost::shared_ptr<const adjacency_list_t> topology;
	/// Stores user-defined per-graph data
	named_arrays_t graph_data;
	/// Stores user-defined per-vertex data
	named_arrays_t vertex_data;
	/// Stores uder-defined per-edge data
	named_arrays_t edge_data;
};

/// Stream serialization
std::ostream& operator<<(std::ostream& Stream, const graph& RHS);

} // namespace k3d

#endif // K3DSDK_GRAPH_H

