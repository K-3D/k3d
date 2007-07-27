#ifndef NGUI_PARENT_TOOL_H
#define NGUI_PARENT_TOOL_H

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
		\author Tim Shead (tshead@k-3d.com)
*/

#include "tool.h"

namespace libk3dngui
{

/////////////////////////////////////////////////////////////////////////////
// parent_tool

/// Tool implementation that provides interactive node parenting
class parent_tool :
	public tool
{
	typedef tool base;

public:
	parent_tool(document_state& DocumentState, const std::string& Name);
	~parent_tool();

	const k3d::icommand_node::result execute_command(const std::string& Command, const std::string& Arguments);

private:
	virtual void on_deactivate();
	virtual viewport_input_model& get_input_model();

	struct implementation;
	implementation* const m_implementation;
};

} // namespace libk3dngui

#endif // NGUI_PARENT_TOOL_H

