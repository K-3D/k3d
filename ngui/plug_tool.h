#ifndef NGUI_PLUG_TOOL_H
#define NGUI_PLUG_TOOL_H

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
// plug_tool

/// Implementation of tool that implements standard geometry plug behavior
class plug_tool :
	public tool
{
	typedef tool base;

public:
	plug_tool(document_state& DocumentState, const std::string& Name);
	~plug_tool();

	/// Sets the current property selection (used for interactive connections between properties)
	void set_selected_properties(const k3d::iproperty_collection::properties_t& Properties);
	/// Gets the current property selection (used for interactive connections between properties)
	const k3d::iproperty_collection::properties_t& get_selected_properties();

	/// Returns true iff a connection operation is in-progress
	bool connection_started();
	/// Returns true iff a connection could be completed with the given property
	bool allow_connection(k3d::iproperty& Property);

private:
	virtual void on_activate();
	virtual void on_deactivate();
	virtual iuser_input_model& get_input_model();

	struct implementation;
	implementation* const m_implementation;
};

} // namespace libk3dngui

#endif // NGUI_PLUG_TOOL_H


