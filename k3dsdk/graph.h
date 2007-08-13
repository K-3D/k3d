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
#include "typed_array.h"
#include "vectors.h"

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
		size_t index;
	};

	/// Defines storage for a generic graph topology
	typedef boost::adjacency_list<boost::listS, boost::vecS, boost::undirectedS, vertex, edge> adjacency_list;
	typedef boost::graph_traits<adjacency_list>::vertex_descriptor vertex_descriptor;
	typedef boost::graph_traits<adjacency_list>::edge_descriptor edge_descriptor;
	typedef boost::graph_traits<adjacency_list>::vertex_iterator vertex_iterator;
	typedef boost::graph_traits<adjacency_list>::edge_iterator edge_iterator;
	typedef boost::graph_traits<adjacency_list>::out_edge_iterator out_edge_iterator;
	typedef boost::graph_traits<adjacency_list>::in_edge_iterator in_edge_iterator;

	/// Defines storage for a generic collection of boolean values
	typedef typed_array<bool> bools;
	/// Defines storage for a generic collection of integer values
	typedef typed_array<int> ints;
	/// Defines storage for a generic collection of index values
	typedef typed_array<size_t> indices;
	/// Defines storage for a generic collection of floating-point values
	typedef typed_array<double> doubles;
	/// Defines storage for a generic collection of string values
	typedef typed_array<std::string> strings;
	/// Defines storage for a generic collection of two-dimensional points
	typedef typed_array<point2> points;
	/// Defines storage for a generic collection of two-dimensional vectors
	typedef typed_array<vector2> vectors;
	/// Defines storage for a generic collection of inode objects
	typedef typed_array<inode*> nodes;
	/// Defines a heterogeneous collection of named, shared arrays
	typedef k3d::named_arrays named_arrays;

	/// Stores the graph topology
	boost::shared_ptr<const adjacency_list> topology;
	/// Stores user-defined per-graph data
	named_arrays graph_data;
	/// Stores user-defined per-vertex data
	named_arrays vertex_data;
	/// Stores uder-defined per-edge data
	named_arrays edge_data;
};

/// Stream serialization
std::ostream& operator<<(std::ostream& Stream, const graph& RHS);

} // namespace k3d

#endif // K3DSDK_GRAPH_H

