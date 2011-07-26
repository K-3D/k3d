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

#include <boost/python.hpp>

#include <k3dsdk/python/idocument_python.h>
#include <k3dsdk/python/iunknown_python.h>
#include <k3dsdk/python/node_python.h>

#include <k3dsdk/node.h>


namespace k3d
{

namespace python
{

class node
{
public:
	static boost::python::list wrap_nodes(const std::vector<inode*>& Nodes)
	{
		boost::python::list python_nodes;

		for(uint_t i = 0; i != Nodes.size(); ++i)
			python_nodes.append(wrap_unknown(*Nodes[i]));

		return python_nodes;
	}

	static boost::python::list lookup_all(idocument_wrapper& Document)
	{
		return wrap_nodes(k3d::node::lookup(Document.wrapped()));
	}

	static boost::python::list lookup_by_uuid(idocument_wrapper& Document, const uuid& ID)
	{
		return wrap_nodes(k3d::node::lookup(Document.wrapped(), ID));
	}

	static boost::python::list lookup_by_name(idocument_wrapper& Document, const string_t& Name)
	{
		return wrap_nodes(k3d::node::lookup(Document.wrapped(), Name));
	}

	static boost::python::list lookup_by_metadata(idocument_wrapper& Document, const string_t& MetadataName, const string_t& MetadataValue)
	{
		return wrap_nodes(k3d::node::lookup(Document.wrapped(), MetadataName, MetadataValue));
	}

	static boost::python::object lookup_one_by_name(idocument_wrapper& Document, const string_t& Name)
	{
		return wrap_unknown(k3d::node::lookup_one(Document.wrapped(), Name));
	}

	static void show_one(idocument_wrapper& Document, iunknown_wrapper& Node)
	{
		k3d::node::show(Document.wrapped(), Node.wrapped<inode>());
	}

	static void hide_one(idocument_wrapper& Document, iunknown_wrapper& Node)
	{
		k3d::node::hide(Document.wrapped(), Node.wrapped<inode>());
	}
};

void define_namespace_node()
{
	boost::python::scope outer = boost::python::class_<node>("node", boost::python::no_init)
		.def("lookup", node::lookup_all,
			"Returns the set of all nodes in the given document.")
		.def("lookup", node::lookup_by_uuid,
			"Returns the set of nodes in a document that match a specific factory ID.")
		.def("lookup", node::lookup_by_name,
			"Returns the set of nodes in a document that match the given node name.")
		.def("lookup", node::lookup_by_metadata,
			"Returns the set of nodes in a document that match the given metadata name and value.")
		.staticmethod("lookup")

		.def("lookup_one", node::lookup_one_by_name,
			"Returns the one-and-only-one node in a document that matches the given node name, or None.")
		.staticmethod("lookup_one")

		.def("show", node::show_one,
			"Makes a node visible by adding it to every render engine in the document.")
		.staticmethod("show")

		.def("hide", node::hide_one,
			"Hides a node by removing it from every render engine in the document.")
		.staticmethod("hide")
		;
}

} // namespace python

} // namespace k3d

