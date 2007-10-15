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

#include "inode.h"
#include "iplugin_factory.h"
#include "node_name_map.h"
#include "result.h"
#include "string_cast.h"
#include "uuid.h"

namespace k3d
{

///////////////////////////////////////////////////////////////////////////////////
// node_name_map::implementation

class node_name_map::implementation
{
public:
	typedef std::vector<inode*> nodes_t;
	nodes_t m_nodes;
};

///////////////////////////////////////////////////////////////////////////////////
// node_name_map
	
node_name_map::node_name_map() :
	m_implementation(new implementation())
{
}

node_name_map::~node_name_map()
{
	delete m_implementation;
}

void node_name_map::add_nodes(const inode_collection::nodes_t& Nodes)
{
	m_implementation->m_nodes.insert(m_implementation->m_nodes.end(), Nodes.begin(), Nodes.end());
}

const std::string node_name_map::name(inode& Node)
{
	const uuid node_factory_id = Node.factory().persistent_factory_id();
	size_t node_number = 1;

	const size_t begin = 0;
	const size_t end = begin + m_implementation->m_nodes.size();
	for(size_t i = begin; i != end; ++i)
	{
		if(m_implementation->m_nodes[i] == &Node)
		{
			std::ostringstream buffer;
			buffer << node_factory_id << " " << node_number;
			return buffer.str();
		}

		if(m_implementation->m_nodes[i]->factory().persistent_factory_id() != node_factory_id)
			continue;

		++node_number;
	}

	log() << error << k3d_file_reference << "internal error: can't find node [" << Node.name() << "]" << std::endl;
	return std::string();
}

inode* node_name_map::node(const std::string& Name)
{
	uuid lookup_node_factory_id = uuid::null();
	size_t lookup_node_number = 0;

	std::istringstream buffer(Name);
	buffer >> lookup_node_factory_id >> lookup_node_number;
	return_val_if_fail(lookup_node_factory_id != uuid::null(), 0);
	return_val_if_fail(lookup_node_number != 0, 0);

	size_t node_number = 1;
	const size_t begin = 0;
	const size_t end = begin + m_implementation->m_nodes.size();
	for(size_t i = begin; i != end; ++i)
	{
		if(m_implementation->m_nodes[i]->factory().persistent_factory_id() != lookup_node_factory_id)
			continue;

		if(node_number == lookup_node_number)
			return m_implementation->m_nodes[i];

		++node_number;
	}

	log() << error << k3d_file_reference << "can't find node [" << lookup_node_factory_id << "] number [" << lookup_node_number << "]" << std::endl;
	return 0;
}

} // namespace k3d

