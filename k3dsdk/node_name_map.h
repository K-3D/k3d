#ifndef K3DSDK_NODE_NAME_MAP_H
#define K3DSDK_NODE_NAME_MAP_H

// K-3D
// Copyright (c) 1995-2006, Timothy M. Shead
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
		\author Tim Shead (tshead@k-3d.com)
*/

#include "inode_collection.h"
#include "inode_name_map.h"

namespace k3d
{

/// Concrete implementation of inode_name_map that generates repeatable unique node names for use by e.g: tutorial recording and playback
class node_name_map :
	public inode_name_map
{
public:
	node_name_map();
	~node_name_map();

	/// Adds nodes to the collection
	void add_nodes(const inode_collection::nodes_t& Nodes);
	
	virtual const std::string name(inode& Node);
	virtual inode* node(const std::string& Name);

private:
	class implementation;
	implementation* const m_implementation;
};

} // namespace k3d

#endif // !K3DSDK_NODE_NAME_MAP_H

