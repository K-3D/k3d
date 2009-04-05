// K-3D
// Copyright (c) 1995-2004, Timothy M. Shead
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

/** \author Timothy M. Shead <tshead@k-3d.com>
*/

#include "undo_utility.h"
#include <algorithm>

namespace k3d
{

namespace ngui
{

const parent_nodes_t parent_nodes(const k3d::istate_recorder::node* const Node)
{
	parent_nodes_t results;
	for(const k3d::istate_recorder::node* node = Node; node; node = node->parent)
		results.push_back(node);

	std::reverse(results.begin(), results.end());
	return results;
}

const k3d::istate_recorder::node* next_redo(k3d::istate_recorder& StateRecorder)
{
	const k3d::istate_recorder::node* const current_node = StateRecorder.current_node();

	// First, see if the current node is located anywhere within the ancestry of the
	// most-recently-created node.  If it is, head in that direction ...
	const parent_nodes_t recent_nodes = parent_nodes(StateRecorder.newest_node());
	parent_nodes_t::const_iterator i = std::find(recent_nodes.begin(), recent_nodes.end(), current_node);
	if(i != recent_nodes.end() && ++i != recent_nodes.end())
		return *i;

	// ... otherwise, if we're no longer on the path to the most-recently-created node,
	// "follow the local terrain" by following the most recent branches ...
	if(current_node)
		{
			if(current_node->children.size())
				return current_node->children.back();
		}
	else
		{
			if(StateRecorder.root_nodes().size())
				return StateRecorder.root_nodes().back();
		}

	return 0;
}

} // namespace ngui

} // namespace k3d

