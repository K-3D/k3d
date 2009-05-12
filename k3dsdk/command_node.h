#ifndef K3DSDK_COMMAND_NODE_H
#define K3DSDK_COMMAND_NODE_H

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

#include "icommand_node.h"

namespace k3d
{

/// Provides a default implementation of icommand_node
class command_node :
	public icommand_node
{
public:
	virtual ~command_node();

	result execute_command(const string_t& Command, const string_t& Arguments);

	/// Returns the location of a command node within the command tree as a filesystem-like absolute path
	static const string_t path(icommand_node& Node);
	/// Returns a command node based on its absolute path (could return NULL)
	static icommand_node* lookup(const string_t& Path);
	/// Returns a command node relative to its parent (could return NULL)
	static icommand_node* lookup(icommand_node* const Parent, const string_t& Child);
	/// Returns true iff Descendant is rooted at (or *is*) Parent
	static bool_t is_descendant(icommand_node* const Parent, icommand_node* const Descendant);
};

} // namespace k3d

#endif // !K3DSDK_COMMAND_NODE_H

