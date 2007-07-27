#ifndef NGUI_KNIFE_TOOL_H
#define NGUI_KNIFE_TOOL_H

// K-3D
// Copyright (c) 2005-2006, Romain Behar
//
// Contact: romainbehar@yahoo.com
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
		\author Romain Behar (romainbehar@yahoo.com)
*/

#include "tool.h"

namespace libk3dngui
{

/////////////////////////////////////////////////////////////////////////////
// knife_tool

class knife_tool :
	public tool
{
	typedef tool base;

public:
	knife_tool(document_state& DocumentState, const std::string& Name);
	~knife_tool();

	const k3d::icommand_node::result execute_command(const std::string& Command, const std::string& Arguments);

private:
	void on_activate();
	void on_deactivate();
	void on_redraw(viewport::control& Viewport);
	virtual viewport_input_model& get_input_model();

	struct implementation;
	implementation* const m_implementation;
};

} // namespace libk3dngui

#endif // NGUI_KNIFE_TOOL_H

