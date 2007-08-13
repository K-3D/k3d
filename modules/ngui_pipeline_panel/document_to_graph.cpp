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

#include "document_to_graph.h"

#include <k3dsdk/ipipeline.h>

namespace module
{

namespace ngui
{

namespace pipeline
{

document_to_graph::document_to_graph(k3d::idocument& Document) :
	m_document(Document)
{
}

void document_to_graph::on_initialize_graph(k3d::graph& Output)
{
	const k3d::nodes_t nodes = m_document.nodes().collection();

	boost::shared_ptr<k3d::graph::adjacency_list> topology(new k3d::graph::adjacency_list());
	boost::shared_ptr<k3d::graph::nodes> vertex_node(new k3d::graph::nodes());
	boost::shared_ptr<k3d::graph::indices> edge_type(new k3d::graph::indices());

	// Insert nodes ...
	std::map<k3d::inode*, k3d::graph::vertex_descriptor> node_map;
	for(k3d::nodes_t::const_iterator node = nodes.begin(); node != nodes.end(); ++node)
	{
		node_map[*node] = boost::add_vertex(*topology);
		vertex_node->push_back(*node);
	}

	// Insert edges ...
	for(k3d::nodes_t::const_iterator node = nodes.begin(); node != nodes.end(); ++node)
	{
		if(k3d::iproperty_collection* const property_collection = dynamic_cast<k3d::iproperty_collection*>(*node))
		{
			const k3d::iproperty_collection::properties_t properties = property_collection->properties();
			for(k3d::iproperty_collection::properties_t::const_iterator property = properties.begin(); property != properties.end(); ++property)
			{
				if(typeid(k3d::inode*) == (*property)->property_type())
				{
					if(k3d::inode* const referenced_node = boost::any_cast<k3d::inode*>((*property)->property_value()))
					{
						boost::add_edge(node_map[referenced_node], node_map[*node], *topology).first;
						edge_type->push_back(BEHAVIOR_EDGE);
					}
				}
			}
		}
	}

	const k3d::ipipeline::dependencies_t dependencies = m_document.pipeline().dependencies();
	for(k3d::ipipeline::dependencies_t::const_iterator dependency = dependencies.begin(); dependency != dependencies.end(); ++dependency)
	{
		if(dependency->first && dependency->first->property_node() && dependency->second && dependency->second->property_node())
		{
			boost::add_edge(node_map[dependency->second->property_node()], node_map[dependency->first->property_node()], *topology).first;
			edge_type->push_back(DATA_EDGE);
		}
	}

	Output.topology = topology;
	Output.vertex_data["node"] = vertex_node;
	Output.edge_data["type"] = edge_type;
}

void document_to_graph::on_update_graph(k3d::graph& Output)
{
}

} // namespace pipeline

} // namespace ngui

} // namespace module

