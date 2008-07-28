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

#include <gtkmm/widget.h>

#include "document_state.h"
#include "tool.h"
#include "viewport.h"

#include <k3dsdk/command_tree.h>
#include <k3dsdk/idocument.h>
#include <k3dsdk/result.h>
#include <k3dsdk/utility_gl.h>

namespace libk3dngui
{

/////////////////////////////////////////////////////////////////////////////
// tool

tool::tool() :
	m_document_state(0)
{
}

tool::tool(document_state& DocumentState, const k3d::string_t& Name) :
	m_document_state(&DocumentState)
{
	k3d::command_tree().add(*this, Name, dynamic_cast<k3d::icommand_node*>(&DocumentState.document()));
}

tool::~tool()
{
}

void tool::initialize(document_state& DocumentState)
{
	// This should never be called more-than-once ...
	assert(!m_document_state);

	m_document_state = &DocumentState;
	on_initialize(DocumentState);
}

void tool::activate()
{
	on_activate();
}

void tool::deactivate()
{
	on_deactivate();
}

void tool::document_selection_changed()
{
	on_document_selection_changed();
}

void tool::redraw(viewport::control& Viewport)
{
	on_redraw(Viewport);
}

void tool::select(viewport::control& Viewport)
{
	on_select(Viewport);
}

k3d::iproperty_collection* tool::properties()
{
	return get_property_collection();
}

viewport_input_model& tool::input_model()
{
	return get_input_model();
}

void tool::redraw_all()
{
	k3d::gl::redraw_all(m_document_state->document(), k3d::gl::irender_viewport::ASYNCHRONOUS);
}

void tool::record_command(const k3d::string_t& Command, const k3d::string_t& Arguments)
{
	return_if_fail(Command.size());
	k3d::command_tree().command_signal().emit(*this, k3d::icommand_node::COMMAND_INTERACTIVE, Command, Arguments);
}

void tool::on_initialize(document_state&)
{
}

void tool::on_activate()
{
}

void tool::on_deactivate()
{
}

void tool::on_document_selection_changed()
{
}

void tool::on_redraw(viewport::control& Viewport)
{
}

void tool::on_select(viewport::control& Viewport)
{
}

k3d::iproperty_collection* tool::get_property_collection()
{
	return 0;
}


} // namespace libk3dngui

