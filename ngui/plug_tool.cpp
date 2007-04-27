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

#include <gdkmm/cursor.h>
#include <gtkmm/widget.h>

#include "document_state.h"
#include "icons.h"
#include "keyboard.h"
#include "null_input_model.h"
#include "plug_tool.h"
#include "utility.h"

namespace libk3dngui
{

/////////////////////////////////////////////////////////////////////////////
// plug_tool::implementation

struct plug_tool::implementation
{
	implementation(document_state& DocumentState) :
		m_document_state(DocumentState)
	{
	}

	/// Stores a reference to the owning document
	document_state& m_document_state;
	/// Stores the current set of selected properties (used to make interactive connections)
	k3d::iproperty_collection::properties_t m_selected_properties;
	/// Dispatches user input events
	null_input_model m_input_model;
};

/////////////////////////////////////////////////////////////////////////////
// plug_tool

/** \todo Figure out why connections don't work */
plug_tool::plug_tool(document_state& DocumentState, const std::string& Name) :
	base(DocumentState, Name),
	m_implementation(new implementation(DocumentState))
{
}

plug_tool::~plug_tool()
{
	delete m_implementation;
}

bool plug_tool::connection_started()
{
	return !m_implementation->m_selected_properties.empty();
}

bool plug_tool::allow_connection(k3d::iproperty& Property)
{
	if(!connection_started())
		return false;

	for(k3d::iproperty_collection::properties_t::const_iterator property = m_implementation->m_selected_properties.begin(); property != m_implementation->m_selected_properties.end(); ++property)
	{
		if((*property) == &Property)
			return false;

		if((*property)->property_type() != Property.property_type())
			return false;
	}

	return true;
}

void plug_tool::set_selected_properties(const k3d::iproperty_collection::properties_t& Properties)
{
	m_implementation->m_selected_properties = Properties;
}

const k3d::iproperty_collection::properties_t& plug_tool::get_selected_properties()
{
	return m_implementation->m_selected_properties;
}

void plug_tool::on_activate()
{
	m_implementation->m_selected_properties.clear();
	m_implementation->m_document_state.set_cursor_signal().emit(load_icon("connect_property_cursor", Gtk::ICON_SIZE_BUTTON));
}

void plug_tool::on_deactivate()
{
	m_implementation->m_selected_properties.clear();
	m_implementation->m_document_state.clear_cursor_signal().emit();
}

iuser_input_model& plug_tool::get_input_model()
{
	return m_implementation->m_input_model;
}

} // namespace libk3dngui


