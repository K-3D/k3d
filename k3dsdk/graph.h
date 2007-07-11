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

#include "array.h"
#include "vectors.h"

#include <boost/shared_ptr.hpp>
#include <map>

namespace k3d
{

////////////////////////////////////////////////////////////////////////////////
// graph

/// Encapsulates a directed graph stored as an adjacency list
class graph
{
public:
	graph();

	/// Defines storage for a generic collection of indices
	typedef typed_array<size_t> indices_t;
	/// Defines storage for a generic collection of counts
	typedef typed_array<size_t> counts_t;
	/// Defines storage for a generic collection of orders
	typedef typed_array<size_t> orders_t;
	/// Defines storage for a generic collection of booleans
	typedef typed_array<bool> bools_t;
	/// Defines storage for a generic collection of selection weights
	typedef typed_array<double> selection_t;
	/// Defines a heterogeneous collection of named, shared arrays
	typedef std::map<std::string, boost::shared_ptr<array> > named_arrays;

	/// Stores the beginning of each vertex' edge list
	boost::shared_ptr<const indices_t> vertex_first_edges;
	/// Stores the number of edges in each vertex' edge list
	boost::shared_ptr<const counts_t> vertex_edge_counts;
	/// Stores per-vertex selection state
	boost::shared_ptr<const selection_t> vertex_selection;
	/// Stores user-defined per-vertex data
	named_arrays vertex_data;

	/// Stores edges adjacent to a given vertex
	boost::shared_ptr<const indices_t> edges;
	/// Stores per-edge selection state
	boost::shared_ptr<const selection_t> edge_selection;
	/// Stores uder-defined per-edge data
	named_arrays edge_data;
};

/// Stream serialization
std::ostream& operator<<(std::ostream& Stream, const graph& RHS);

/// Performs a deep-copy from one graph to another (the new graph doesn't share any memory with the old)
void deep_copy(const graph& From, graph& To);

/// Performs sanity-checking on a graph, validating all constraints - returns true iff the graph is valid
const bool validate(graph& Graph);

} // namespace k3d

#endif // K3DSDK_NEW_MESH_H

