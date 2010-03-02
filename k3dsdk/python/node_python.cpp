// K-3D
// Copyright (c) 1995-2008, Timothy M. Shead
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

#include <k3dsdk/python/idocument_python.h>
#include <k3dsdk/python/iunknown_python.h>
#include <k3dsdk/python/node_python.h>

#include <k3dsdk/node.h>

#include <boost/python.hpp>
using namespace boost::python;

namespace k3d
{

namespace python
{

class node
{
public:
	static list wrap_nodes(const std::vector<inode*>& Nodes)
	{
		list python_nodes;

		for(uint_t i = 0; i != Nodes.size(); ++i)
			python_nodes.append(wrap_unknown(*Nodes[i]));

		return python_nodes;
	}

	static list lookup_all(idocument_wrapper& Document)
	{
		return wrap_nodes(k3d::node::lookup(Document.wrapped()));
	}

	static list lookup_by_uuid(idocument_wrapper& Document, const uuid& ID)
	{
		return wrap_nodes(k3d::node::lookup(Document.wrapped(), ID));
	}

	static list lookup_by_name(idocument_wrapper& Document, const string_t& Name)
	{
		return wrap_nodes(k3d::node::lookup(Document.wrapped(), Name));
	}

	static list lookup_by_metadata(idocument_wrapper& Document, const string_t& MetadataName, const string_t& MetadataValue)
	{
		return wrap_nodes(k3d::node::lookup(Document.wrapped(), MetadataName, MetadataValue));
	}
};

void define_namespace_node()
{
	scope outer = class_<node>("node", no_init)
		.def("lookup", node::lookup_all,
			"Returns the set of all nodes in the given document.")
		.def("lookup", node::lookup_by_uuid,
			"Returns the set of nodes in a document that match a specific factory ID.")
		.def("lookup", node::lookup_by_name,
			"Returns the set of nodes in a document that match the given node name.")
		.def("lookup", node::lookup_by_metadata,
			"Returns the set of nodes in a document that match the given metadata name and value.")
		.staticmethod("lookup");
}

} // namespace python

} // namespace k3d

