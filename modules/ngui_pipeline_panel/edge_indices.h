#ifndef EDGE_INDICES_H
#define EDGE_INDICES_H

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

/** \file
	\author Timothy M. Shead (tshead@k-3d.com)
*/

#include "graph_modifier.h"

namespace k3d { class idocument; }

namespace module
{

namespace ngui
{

namespace pipeline
{

/// graph_modifier implementation that computes the index for every edge in a graph
class edge_indices :
	public graph_modifier
{
public:
	edge_indices();

private:
	void on_initialize_graph(const k3d::graph& Input, k3d::graph& Output);
	void on_update_graph(const k3d::graph& Input, k3d::graph& Output);
};

} // namespace pipeline

} // namespace ngui

} // namespace module

#endif // !EDGE_INDICES

