#ifndef NGUI_TOOL_H
#define NGUI_TOOL_H

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

#include <k3dsdk/command_node.h>
#include <k3dsdk/vectors.h>

#include <gdk/gdkevents.h>

namespace k3d { class inode; }
namespace k3d { class iproperty_collection; }
namespace k3d { class iviewport; }

namespace libk3dngui
{

class document_state;
class viewport_input_model;
namespace viewport { class control; }

/////////////////////////////////////////////////////////////////////////////
// tool

/// Abstract base class for Tools - objects that generate state changes based on user interaction with viewports
class tool :
	public k3d::command_node
{
	typedef k3d::command_node base;

public:
	virtual ~tool();

	/// Set the document for this tool - this is ugly, but application plugins don't take ctor arguments
	void initialize(document_state& DocumentState);
	/// Returns a unique string identifying the tool type
	virtual const k3d::string_t tool_type() = 0;
	/// Called to activate the tool
	void activate();
	/// Called to deactivate the tool
	void deactivate();
	/// Called when document selection changed
	void document_selection_changed();
	/// Called to give the tool a chance to draw in the viewport
	void redraw(viewport::control& Viewport);
	/// Called to give the tool a chance to draw in the viewport
	void select(viewport::control& Viewport);
	/// Returns tool's property collection
	k3d::iproperty_collection* properties();
	/// Returns the tool's user input model
	viewport_input_model& input_model();

	/// Convenience function that schedules a screen update for all viewports
	void redraw_all();
	/// Records an interactive tutorial event
	void record_command(const k3d::string_t& Command, const k3d::string_t& Arguments = "");

protected:
	tool();
	tool(document_state& DocumentState, const k3d::string_t& Name);

private:
	/// Called when the tool is initialized
	virtual void on_initialize(document_state& DocumentState);

	/// Called when the tool is activated - override in derived classes to implement custom behavior
	virtual void on_activate();
	/// Called when the tool is deactivated - override in derived classes to implement custom behavior
	virtual void on_deactivate();
	/// Called when the document selection changed - override in derived classes to implement custom behavior
	virtual void on_document_selection_changed();
	/// Called to give the tool a chance to draw in the viewport
	virtual void on_redraw(viewport::control& Viewport);
	/// Called to give the tool a chance to draw in the viewport
	virtual void on_select(viewport::control& Viewport);
	/// Asks for tool's property collection
	virtual k3d::iproperty_collection* get_property_collection();
	/// Returns the tool's user input model
	virtual viewport_input_model& get_input_model() = 0;

	/// Stores a reference to the owning document
	document_state* m_document_state;
};

} // namespace libk3dngui

#endif // NGUI_TOOL_H

