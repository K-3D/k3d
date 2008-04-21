#ifndef NGUI_SNAP_TOOL_H
#define NGUI_SNAP_TOOL_H

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
		\author Romain Behar (romainbehar@yahoo.com)
*/

#include "tool.h"

#include <k3dsdk/property_collection.h>

namespace libk3dngui
{

/////////////////////////////////////////////////////////////////////////////
// snap_tool

/// Implementation of tool that implements standard geometry move behavior
class snap_tool :
	public tool
{
	typedef tool base;

public:
	snap_tool(document_state& DocumentState, const std::string& Name);
	~snap_tool();

	const k3d::string_t tool_type() { return "snap_tool"; }

	const k3d::icommand_node::result execute_command(const std::string& Command, const std::string& Arguments);

private:
	void on_activate();
	void on_deactivate();
	void on_document_selection_changed();
	void on_redraw(viewport::control& Viewport);
	void on_select(viewport::control& Viewport);
	k3d::iproperty_collection* get_property_collection();
	viewport_input_model& get_input_model();

	struct implementation;
	implementation* const m_implementation;
};

} // namespace libk3dngui

#endif // NGUI_SNAP_TOOL_H


