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
	\author Timothy M. Shead (tshead@k-3d.com)
*/

#include "icommand_node_python.h"

#include <k3dsdk/command_tree.h>
#include <k3dsdk/types.h>

#include <boost/python.hpp>
using namespace boost::python;

namespace k3d
{

namespace python
{

static const string_t name(icommand_node_wrapper& Self)
{
	return k3d::command_tree().name(Self.wrapped());
}

static const list children(icommand_node_wrapper& Self)
{
	list results;
	
	const k3d::icommand_tree::nodes_t children = k3d::command_tree().children(&Self.wrapped());
	for(k3d::icommand_tree::nodes_t::const_iterator child = children.begin(); child != children.end(); ++child)
		results.append(wrap(*child));
	
	return results;
}

static object get_child(icommand_node_wrapper& Self, const string_t& Name)
{
	k3d::icommand_tree::nodes_t children = k3d::command_tree().children(&Self.wrapped());
	for(k3d::icommand_tree::nodes_t::iterator child = children.begin(); child != children.end(); ++child)
	{
		if(Name == k3d::command_tree().name(**child))
			return wrap(*child);
	}

	throw std::runtime_error("No child named " + Name);
}

static void execute_command(icommand_node_wrapper& Self, const string_t& Command, const string_t& Arguments)
{
	switch(Self.wrapped().execute_command(Command, Arguments))
	{
		case k3d::icommand_node::RESULT_STOP:
			throw std::runtime_error("Stop executing");
		case k3d::icommand_node::RESULT_CONTINUE:
			return;
		case k3d::icommand_node::RESULT_ERROR:
			throw std::runtime_error("Error executing command");
		case k3d::icommand_node::RESULT_UNKNOWN_COMMAND:
			throw std::runtime_error("Unknown command");
	}

	throw std::runtime_error("Unknown command result");
}

void define_class_icommand_node()
{
	class_<icommand_node_wrapper>("icommand_node",
		"Abstract interface implemented by object that can execute arbitrary commands.\n\n"
		"Use L{dynamic_cast} to convert an icommand_node object to some other interface type.", no_init)
		.def("command_node_name", &name,
			"Returns the name of this command node as a string.")
		.def("children", &children,
			"Returns this command node's children (if any) as a list of icommand_node objects.")
		.def("get_child", &get_child,
			"Given a string name, returns the given child.")
		.def("execute_command", &execute_command,
			"Executes a command.");
}

} // namespace python

} // namespace k3d

