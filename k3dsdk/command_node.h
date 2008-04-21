#ifndef K3DSDK_COMMAND_NODE_H
#define K3DSDK_COMMAND_NODE_H

// K-3D
// Copyright (c) 1995-2005, Timothy M. Shead
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
		\brief Declares command_node, which provides a default implementation of icommand_node
		\author Tim Shead (tshead@k-3d.com)
*/

#include "icommand_node.h"
#include <string>

namespace k3d
{

namespace command_node
{

/////////////////////////////////////////////////////////////////////////////
// path

/// Returns the location of a command node within the command tree as a filesystem-like absolute path
const std::string path(icommand_node& Node);

/////////////////////////////////////////////////////////////////////////////
// lookup

/// Returns a command node based on its absolute path (could return NULL)
icommand_node* lookup(const std::string& Path);
/// Returns a command node relative to its parent (could return NULL)
icommand_node* lookup(icommand_node* const Parent, const std::string& Child);

/////////////////////////////////////////////////////////////////////////////
// is_descendant

/// Returns true iff Descendant is rooted at (or *is*) Parent
bool is_descendant(icommand_node* const Parent, icommand_node* const Descendant);

/////////////////////////////////////////////////////////////////////////////
// implementation

/// Provides an implementation of icommand_node
class implementation :
	public icommand_node
{
public:
	implementation();
	implementation(const std::string& Name, icommand_node* Parent);
	virtual ~implementation();

	void set_parent(const std::string& Name, icommand_node* Parent);

	const result execute_command(const std::string& Command, const std::string& Arguments);
};

} // namespace command_node

} // namespace k3d

#endif // K3DSDK_COMMAND_NODE_H

