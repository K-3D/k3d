#ifndef ICOMMAND_NODE_SIMPLE_H_
#define ICOMMAND_NODE_SIMPLE_H_

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
// License along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

/** \file
		\brief Declares icommand_node_simple, an abstract interface for objects that can execute a command that takes no arguments
		\author Bart Janssens (bart.janssens@lid.kviv.be)
*/

#include "icommand_node.h"

namespace k3d
{

/// Abstract interface for objects that can execute a command that takes no arguments
class icommand_node_simple :
	public virtual iunknown
{
public:
	
	/// Executes a command, returning the command result
	virtual const icommand_node::result execute_command() = 0;

protected:
	icommand_node_simple() {}
	icommand_node_simple(const icommand_node_simple&) {}
	icommand_node_simple& operator=(const icommand_node_simple&) { return *this; }
	virtual ~icommand_node_simple() {}
};

} // namespace k3d

#endif /*ICOMMAND_NODE_SIMPLE_H_*/
