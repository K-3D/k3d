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

#include <boost/python.hpp>
using namespace boost::python;

namespace k3d
{

namespace python
{

icommand_node::icommand_node() :
	base()
{
}

icommand_node::icommand_node(k3d::icommand_node* CommandNode) :
	base(CommandNode)
{
}

const std::string icommand_node::name()
{
	return k3d::command_tree().name(wrapped());
}

const list icommand_node::children()
{
	list results;
	
	const k3d::icommand_tree::nodes_t children = k3d::command_tree().children(&wrapped());
	for(k3d::icommand_tree::nodes_t::const_iterator child = children.begin(); child != children.end(); ++child)
		results.append(icommand_node(*child));
	
	return results;
}

icommand_node icommand_node::get_child(const std::string& Name)
{
	k3d::icommand_tree::nodes_t children = k3d::command_tree().children(&wrapped());
	for(k3d::icommand_tree::nodes_t::iterator child = children.begin(); child != children.end(); ++child)
	{
		if(Name == k3d::command_tree().name(**child))
			return icommand_node(*child);
	}

	throw std::runtime_error("No child named " + Name);
}

void icommand_node::execute_command(const std::string& Command, const std::string& Arguments)
{
	switch(wrapped().execute_command(Command, Arguments))
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

void export_icommand_node()
{
	class_<icommand_node>("icommand_node")
		.def("command_node_name", &icommand_node::name)
		.def("children", &icommand_node::children)
		.def("get_child", &icommand_node::get_child)
		.def("execute_command", &icommand_node::execute_command);
}

} // namespace python

} // namespace k3d

