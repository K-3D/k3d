#ifndef K3DSDK_ICOMMAND_NODE_H
#define K3DSDK_ICOMMAND_NODE_H

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

#include "iunknown.h"
#include "types.h"

namespace k3d
{

/// Abstract interface for objects that can accept commands from external entities
class icommand_node :
	public virtual iunknown
{
public:
	/// Enumerates the types of commands that can be executed by a node
	typedef enum
	{
		COMMAND_INTERACTIVE,
		COMMAND_MACRO,
	} type;

	/// Enumerates the possible return values from execute_command
	typedef enum
	{
		/// The command was executed without error, further execution should be halted
		RESULT_STOP,
		/// The command was executed without error, continue executing normally
		RESULT_CONTINUE,
		/// The command failed to execute
		RESULT_ERROR,
		/// The command was not recognized
		RESULT_UNKNOWN_COMMAND
		
	} result;
	
	/// Executes a command, returning the command result
	virtual result execute_command(const string_t& Command, const string_t& Arguments) = 0;

protected:
	icommand_node() {}
	icommand_node(const icommand_node& Other) : iunknown(Other) {}
	icommand_node& operator=(const icommand_node&) { return *this; }
	virtual ~icommand_node() {}
};

} // namespace k3d

#endif // !K3DSDK_ICOMMAND_NODE_H

