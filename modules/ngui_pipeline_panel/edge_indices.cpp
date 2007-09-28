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

#include "edge_indices.h"

#include <k3dsdk/shared_pointer.h>

namespace module
{

namespace ngui
{

namespace pipeline
{

edge_indices::edge_indices()
{
}

void edge_indices::on_initialize_graph(const k3d::graph& Input, k3d::graph& Output)
{
	Output = Input;

	k3d::graph::adjacency_list& topology = *k3d::make_unique(Output.topology);

	k3d::uint_t index = 0;
	for(std::pair<k3d::graph::edge_iterator, k3d::graph::edge_iterator> edges = boost::edges(topology); edges.first != edges.second; ++index, ++edges.first)
		topology[*edges.first].index = index;
}

void edge_indices::on_update_graph(const k3d::graph& Input, k3d::graph& Output)
{
}

} // namespace pipeline

} // namespace ngui

} // namespace module

