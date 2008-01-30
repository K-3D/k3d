#ifndef NGUI_SELECTION_TOOL_H
#define NGUI_SELECTION_TOOL_H

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

namespace libk3dngui
{

class document_state;
	
/////////////////////////////////////////////////////////////////////////////
// selection_tool

/// tool implementation that provides geometry selection and viewport navigation
class selection_tool :
	public tool
{
	typedef tool base;

public:
	selection_tool(document_state& DocumentState, const std::string& Name);
	~selection_tool();

	const k3d::icommand_node::result execute_command(const std::string& Command, const std::string& Arguments);
	
	/// True if the selection is converted when switching modes
	bool convert_selection();
	
	/// True if the selection is kept when switching modes
	bool keep_selection();
	
	/// True if backfacing components are to be picked
	bool pick_backfacing();
	
	/// True if backfacing components are to be paint-selected
	bool paint_backfacing();
	
	/// True if backfacing components are to be rubber-band selected
	bool rubber_band_backfacing();

protected:
	virtual k3d::iproperty_collection* get_property_collection();
	virtual viewport_input_model& get_input_model();

	class implementation;
	implementation* const m_implementation;
};

} // namespace libk3dngui

#endif // NGUI_SELECTION_TOOL_H


