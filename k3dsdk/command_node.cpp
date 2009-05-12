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

/** \file
	\author Tim Shead (tshead@k-3d.com)
*/

#include "command_node.h"
#include "command_tree.h"
#include "idocument.h"
#include "inode.h"
#include "result.h"

#include <algorithm>
#include <functional>
#include <iostream>

namespace k3d
{

namespace detail
{

class filepath
{
public:
	filepath(const string_t& Path) :
		m_path(Path)
	{
	}

	class iterator
	{
	public:
		string_t operator*() const
		{
			return m_path.substr(0, m_path.find("/"));
		}

		iterator& operator++()
		{
			m_path.erase(0, m_path.find("/"));
			m_path.erase(0, 1);
			return *this;
		}

		bool operator!=(const iterator& RHS)
		{
			return m_path != RHS.m_path;
		}

	private:
		friend class filepath;

		iterator()
		{
		}

		iterator(const string_t& Path) :
			m_path(Path)
		{
		}

		string_t m_path;
	};

	iterator begin() const
	{
		return iterator(m_path);
	}

	iterator end() const
	{
		return iterator();
	}

private:
	const string_t m_path;
};

class same_name :
	public std::unary_function<k3d::icommand_node*, bool>
{
public:
	same_name(const string_t& Name) :
		m_name(Name)
	{
	}

	bool operator()(k3d::icommand_node* Node) const
	{
		return k3d::command_tree().name(*Node) == m_name;
	}

private:
	const string_t m_name;
};

} // namespace detail

command_node::~command_node()
{
	command_tree().remove(*this);
}

icommand_node::result command_node::execute_command(const string_t& Command, const string_t& Arguments)
{
	log() << error << "Command node ["
		<< k3d::command_tree().name(*this)
		<< "] unhandled command: ["
		<< Command << "] [" << Arguments << "]" << std::endl;

	return RESULT_UNKNOWN_COMMAND;
}

const string_t command_node::path(icommand_node& Node)
{
	string_t result;
	for(icommand_node* node = &Node; node; node = command_tree().parent(*node))
		result = "/" + command_tree().name(*node) + result;

	return result;
}

icommand_node* command_node::lookup(const string_t& Path)
{
	// Sanity checks ...
	return_val_if_fail(Path.size(), 0);
	return_val_if_fail(Path[0] == '/', 0);

	// At this point, we have an absolute path, so start with the root node, and get rid of the leading slash
	detail::filepath path(Path.substr(1, string_t::npos));
	icommand_node* result = 0;

	// Loop through remaining path components
	for(detail::filepath::iterator subpath = path.begin(); subpath != path.end(); ++subpath)
	{
		const icommand_tree::nodes_t children = command_tree().children(result);
		const icommand_tree::nodes_t::const_iterator child = std::find_if(children.begin(), children.end(), detail::same_name(*subpath));
		if(child != children.end())
			result = *child;
		else
			return 0; // We return quietly here, so objects can use get_command_node() to test for the existence of a node at runtime
	}

	return result;
}

icommand_node* command_node::lookup(icommand_node* const Parent, const string_t& Child)
{
	// Sanity checks ...
	return_val_if_fail(Child.size(), 0);

	const icommand_tree::nodes_t children = command_tree().children(Parent);
	const icommand_tree::nodes_t::const_iterator child = std::find_if(children.begin(), children.end(), detail::same_name(Child));
	if(child != children.end())
		return *child;
		
	return 0; // We return quietly here, so objects can use get_command_node() to test for the existence of a node at runtime
}

bool command_node::is_descendant(icommand_node* const Parent, icommand_node* const Descendant)
{
	// Look for the parent object by travelling up-the-chain
	for(icommand_node* node = Descendant; node; node = command_tree().parent(*node))
	{
		if(node == Parent)
			return true;
	}

	return false;
}

/*
implementation::implementation(const string_t& Name, icommand_node* const Parent)
{
	k3d::command_tree().add(*this, Name, Parent);
}

void implementation::set_parent(const string_t& Name, icommand_node* const Parent)
{
	k3d::command_tree().remove(*this);
	k3d::command_tree().add(*this, Name, Parent);
}
*/

} // namespace k3d

