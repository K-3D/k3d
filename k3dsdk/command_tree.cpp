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

#include "icommand_tree.h"
#include "result.h"
#include "string_cast.h"

#include <map>
#include <vector>

namespace k3d
{

namespace detail
{

/////////////////////////////////////////////////////////////////////////////
// command_tree

/// Provides an implementation of icommand_tree
class command_tree :
	public icommand_tree
{
public:
	void add(icommand_node& Node, const string_t& Name, icommand_node* const Parent)
	{
		m_original_name_map[&Node] = Name;
		m_parent_map[&Node] = Parent;
		
		nodes_t& nodes = m_name_tree[Parent][Name];
		nodes.push_back(&Node);

		size_t count = 0;
		for(nodes_t::reverse_iterator node = nodes.rbegin(); node != nodes.rend(); ++node, ++count)
		{
			if(count)
				m_name_map[*node] = Name + string_cast(count);
			else
				m_name_map[*node] = Name;
		}

		m_changed_signal.emit();
	}

	void remove(icommand_node& Node)
	{
		return_if_fail(m_original_name_map.count(&Node));
		return_if_fail(m_name_map.count(&Node));
		return_if_fail(m_parent_map.count(&Node));

		const string_t original_name = m_original_name_map[&Node];
		icommand_node* const parent = m_parent_map[&Node];

		m_original_name_map.erase(&Node);
		m_name_map.erase(&Node);
		m_parent_map.erase(&Node);

		nodes_t& nodes = m_name_tree[parent][original_name];
		nodes.erase(std::remove(nodes.begin(), nodes.end(), &Node), nodes.end());
			
		size_t count = 0;
		for(nodes_t::reverse_iterator node = nodes.rbegin(); node != nodes.rend(); ++node, ++count)
		{
			if(count)
				m_name_map[*node] = original_name + string_cast(count);
			else
				m_name_map[*node] = original_name;
		}

		for(parent_map_t::iterator child = m_parent_map.begin(); child != m_parent_map.end(); ++child)
		{
			if(child->second == &Node)
				child->second = parent;
		}
		
		m_changed_signal.emit();
	}

	const string_t name(icommand_node& Node)
	{
		name_map_t::iterator node = m_name_map.find(&Node);
		return_val_if_fail(node != m_name_map.end(), string_t());

		return node->second;
	}
	
	icommand_node* parent(icommand_node& Node)
	{
		parent_map_t::iterator node = m_parent_map.find(&Node);
		return_val_if_fail(node != m_parent_map.end(), 0);

		return node->second;
	}

	const nodes_t children(icommand_node* const Node)
	{
		nodes_t results;

		for(parent_map_t::iterator child = m_parent_map.begin(); child != m_parent_map.end(); ++child)
		{
			if(child->second == Node)
				results.push_back(child->first);
		}

		return results;
	}

	sigc::connection connect_changed_signal(const sigc::slot<void>& Slot)
	{
		return m_changed_signal.connect(Slot);
	}
	
	icommand_tree::command_signal_t& command_signal()
	{
		return m_command_signal;
	}

private:
	/// This bizarre fellow helps us generate unique names
	std::map<icommand_node*, std::map<string_t, nodes_t> > m_name_tree;
	
	typedef std::map<icommand_node*, string_t> name_map_t;
	/// Stores unmodified command node names
	name_map_t m_original_name_map;
	/// Stores modified (for uniqueness) command node names
	name_map_t m_name_map;
	
	/// Stores command-node parents
	typedef std::map<icommand_node*, icommand_node*> parent_map_t;
	parent_map_t m_parent_map;

	/// Signal emitted whenever the tree changes
	sigc::signal<void> m_changed_signal;

	/// Signal used to distribute commands to recorders (for macro recording, tutorial recording, blackbox recorder, etc)
	command_signal_t m_command_signal;
};

} // namespace detail

icommand_tree& command_tree()
{
	static detail::command_tree instance;
	return instance;
}

} // namespace k3d

