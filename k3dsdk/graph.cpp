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

#include <iterator>

namespace k3d
{

namespace detail
{

template<typename pointer_type>
static void print(std::ostream& Stream, const std::string& Label, const pointer_type& Pointer)
{
	if(Pointer)
	{
		Stream << Label << " (" << Pointer->size() <<  "): ";
		std::copy(Pointer->begin(), Pointer->end(), std::ostream_iterator<typename pointer_type::element_type::value_type>(Stream, " "));
		Stream << "\n";
	}
}

static void print(std::ostream& Stream, const std::string& Label, const k3d::graph::named_arrays& Arrays)
{
    for(k3d::graph::named_arrays::const_iterator array_iterator = Arrays.begin(); array_iterator != Arrays.end(); ++array_iterator)
    {
        Stream << Label << " " << array_iterator->first << " (" << array_iterator->second->size() << "): ";
        if(typed_array<double>* const array = dynamic_cast<typed_array<double>*>(array_iterator->second.get()))
            std::copy(array->begin(), array->end(), std::ostream_iterator<double>(Stream, " "));
        else if(typed_array<k3d::color>* const array = dynamic_cast<typed_array<k3d::color>*>(array_iterator->second.get()))
            std::copy(array->begin(), array->end(), std::ostream_iterator<k3d::color>(Stream, " "));
        else
            Stream << "<unknown type>" << std::endl;
        Stream << "\n";
    }
}

} // namespace detail

////////////////////////////////////////////////////////////////////////////////////
// graph

graph::graph()
{
}

/** \todo Print materials */
std::ostream& operator<<(std::ostream& Stream, const graph& RHS)
{
	Stream << "graph:" << std::endl;

	detail::print(Stream, "  vertex first edges", RHS.vertex_first_edges);
	detail::print(Stream, "  vertex edge counts", RHS.vertex_edge_counts);
	detail::print(Stream, "  vertex selection", RHS.vertex_selection);
	detail::print(Stream, "  vertex data", RHS.vertex_data);

	detail::print(Stream, "  edges", RHS.edges);
	detail::print(Stream, "  edge_selection", RHS.edge_selection);
	detail::print(Stream, "  edge data", RHS.edge_data);

	return Stream;
}

////////////////////////////////////////////////////////////////////////////////
// deep_copy

void deep_copy(const graph& From, graph& To)
{
	To = From;
	assert_not_implemented(); // Need to ensure that all storage is unique
}


////////////////////////////////////////////////////////////////////////////////
// validate

const bool validate(graph& Graph)
{
	if(!Graph.vertex_first_edges && !Graph.vertex_edge_counts && !Graph.vertex_selection && !Graph.edges && !Graph.edge_selection)
		return true;

	return_val_if_fail(Graph.vertex_first_edges, false);
	return_val_if_fail(Graph.vertex_edge_counts, false);
	return_val_if_fail(Graph.vertex_selection, false);
	return_val_if_fail(Graph.edges, false);
	return_val_if_fail(Graph.edge_selection, false);

	return true;
}

} // namespace k3d

