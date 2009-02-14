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
#include "iunknown_python.h"
#include "utility_python.h"

#include <k3dsdk/command_tree.h>
#include <k3dsdk/types.h>

#include <boost/python.hpp>
using namespace boost::python;

namespace k3d
{

namespace python
{

static const string_t name(iunknown_wrapper& Self)
{
	return k3d::command_tree().name(dynamic_cast<k3d::icommand_node&>(Self.wrapped()));
}

static const list children(iunknown_wrapper& Self)
{
	list results;
	
	const k3d::icommand_tree::nodes_t children = k3d::command_tree().children(&dynamic_cast<k3d::icommand_node&>(Self.wrapped()));
	for(k3d::icommand_tree::nodes_t::const_iterator child = children.begin(); child != children.end(); ++child)
		results.append(wrap_unknown(*child));
	
	return results;
}

static object get_child(iunknown_wrapper& Self, const string_t& Name)
{
	k3d::icommand_tree::nodes_t children = k3d::command_tree().children(&dynamic_cast<k3d::icommand_node&>(Self.wrapped()));
	for(k3d::icommand_tree::nodes_t::iterator child = children.begin(); child != children.end(); ++child)
	{
		if(Name == k3d::command_tree().name(**child))
			return wrap_unknown(*child);
	}

	throw std::runtime_error("No child named " + Name);
}

static void execute_command(iunknown_wrapper& Self, const string_t& Command, const string_t& Arguments)
{
	switch(dynamic_cast<k3d::icommand_node&>(Self.wrapped()).execute_command(Command, Arguments))
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

void define_methods_icommand_node(k3d::iunknown& Interface, boost::python::object& Instance)
{
	if(!dynamic_cast<icommand_node*>(&Interface))
		return;

	utility::add_method(utility::make_function(&name, "Returns the name of this command node as a string."), "command_node_name", Instance);
	utility::add_method(utility::make_function(&children, "Returns this command node's children (if any) as a list of icommand_node objects."), "children", Instance);
	utility::add_method(utility::make_function(&get_child, "Given a string name, returns the given child."), "get_child", Instance);
	utility::add_method(utility::make_function(&execute_command, "Executes a command."), "execute_command", Instance);
}

} // namespace python

} // namespace k3d

