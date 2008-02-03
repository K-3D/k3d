// K-3D
// Copyright (c) 1995-2006, Timothy M. Shead
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

#include "basic_viewport_input_model.h"
#include "document_state.h"
#include "navigation_input_model.h"
#include "selection_input_model.h"
#include "selection_tool.h"

#include <k3dsdk/data.h>
#include <k3d-i18n-config.h>
#include <k3dsdk/property_collection.h>


namespace libk3dngui
{

typedef k3d_data(bool, immutable_name, change_signal, no_undo, local_storage, no_constraint, writable_property, no_serialization) bool_property_t;

namespace detail
{

/// Toggles the value of the given boolean property
void toggle_property(bool_property_t& Property)
{
	if (Property.internal_value())
		Property.set_value(false);
	else
		Property.set_value(true);
}

}

/////////////////////////////////////////////////////////////////////////////
// selection_tool::implementation

class selection_tool::implementation :
	public k3d::property_collection
{
public:
	implementation(document_state& DocumentState) :
		m_document_state(DocumentState),
		m_navigation_model(DocumentState),
		m_selection_model(DocumentState),
		m_extended_mode(init_owner(*this) + init_name("extended_mode") + init_label(_("Extended Selection Mode")) + init_description(_("Extended mode adds newly clicked geometry instead of replacing selection")) + init_value(false)),
		m_extended_component_mode(init_owner(*this) + init_name("extended_component_mode") + init_label(_("Extended Component Selection Mode")) + init_description(_("Extended mode adds newly clicked components instead of replacing selection")) + init_value(false)),
		m_paint_mode(init_owner(*this) + init_name("component_paint_mode") + init_label(_("Component Paint Mode")) + init_description(_("Use mouse pointer to paint-select geometry components")) + init_value(true)),
		m_double_click_mode(init_owner(*this) + init_name("double_click_mode") + init_label(_("Double Click Mode")) + init_description(_("Double click switches back to object selection mode when clicking on nothing")) + init_value(true)),
		m_convert_selection(init_owner(*this) + init_name("convert_selection") + init_label(_("Convert Selection")) + init_description(_("Convert selection when switching between modes")) + init_value(true)),
		m_keep_selection(init_owner(*this) + init_name("keep_selection") + init_label(_("Keep Selection")) + init_description(_("Keep selection from the old modes when switching between modes")) + init_value(false)),
		m_pick_backfacing(init_owner(*this) + init_name("pick_backfacing") + init_label(_("Pick Backfacing")) + init_description(_("Select backfacing components when picking (clicking)")) + init_value(false)),
		m_paint_backfacing(init_owner(*this) + init_name("paint_backfacing") + init_label(_("Paint Backfacing")) + init_description(_("Select backfacing components when painting a selection")) + init_value(false)),
		m_rubber_band_backfacing(init_owner(*this) + init_name("rubber_band_backfacing") + init_label(_("Rubber Band Backfacing")) + init_description(_("Select backfacing components when rubber band selecting")) + init_value(true))
	{
		m_input_model.connect_lbutton_down(sigc::mem_fun(m_selection_model, &selection_input_model::on_button_down));
		m_input_model.connect_lbutton_click(sigc::mem_fun(m_selection_model, &selection_input_model::on_button_click));
		m_input_model.connect_lbutton_double_click(sigc::mem_fun(m_selection_model, &selection_input_model::on_button_double_click));
		m_input_model.connect_lbutton_start_drag(sigc::mem_fun(m_selection_model, &selection_input_model::on_button_start_drag));
		m_input_model.connect_lbutton_drag(sigc::mem_fun(m_selection_model, &selection_input_model::on_button_drag));
		m_input_model.connect_lbutton_end_drag(sigc::mem_fun(m_selection_model, &selection_input_model::on_button_end_drag));

		m_input_model.connect_mbutton_click(sigc::mem_fun(m_navigation_model, &navigation_input_model::on_button1_click));
		m_input_model.connect_mbutton_start_drag(sigc::mem_fun(m_navigation_model, &navigation_input_model::on_button1_start_drag));
		m_input_model.connect_mbutton_drag(sigc::mem_fun(m_navigation_model, &navigation_input_model::on_button1_drag));
		m_input_model.connect_mbutton_end_drag(sigc::mem_fun(m_navigation_model, &navigation_input_model::on_button1_end_drag));
		m_input_model.connect_rbutton_click(sigc::mem_fun(m_navigation_model, &navigation_input_model::on_button2_click));
		m_input_model.connect_rbutton_start_drag(sigc::mem_fun(m_navigation_model, &navigation_input_model::on_button2_start_drag));
		m_input_model.connect_rbutton_drag(sigc::mem_fun(m_navigation_model, &navigation_input_model::on_button2_drag));
		m_input_model.connect_rbutton_end_drag(sigc::mem_fun(m_navigation_model, &navigation_input_model::on_button2_end_drag));
		m_input_model.connect_scroll(sigc::mem_fun(m_navigation_model, &navigation_input_model::on_scroll));
		
		m_selection_model.set_extended_mode(m_extended_mode.internal_value());
		m_selection_model.set_extended_component_mode(m_extended_component_mode.internal_value());
		m_selection_model.set_paint_mode(m_paint_mode.internal_value());
		m_selection_model.set_double_click_mode(m_double_click_mode.internal_value());

		m_extended_mode.changed_signal().connect(sigc::mem_fun(*this, &implementation::on_extended_mode_changed));
		m_extended_component_mode.changed_signal().connect(sigc::mem_fun(*this, &implementation::on_extended_component_mode_changed));
		m_paint_mode.changed_signal().connect(sigc::mem_fun(*this, &implementation::on_paint_mode_changed));
		m_double_click_mode.changed_signal().connect(sigc::mem_fun(*this, &implementation::on_double_click_mode_changed));
		m_pick_backfacing.changed_signal().connect(sigc::mem_fun(*this, &implementation::on_pick_backfacing_changed));
		m_paint_backfacing.changed_signal().connect(sigc::mem_fun(*this, &implementation::on_paint_backfacing_changed));
		m_rubber_band_backfacing.changed_signal().connect(sigc::mem_fun(*this, &implementation::on_rubber_band_backfacing_changed));
	}

	k3d::idocument& document()
	{
		return m_document_state.document();
	}

	void on_extended_mode_changed(k3d::iunknown*)
	{
		m_selection_model.set_extended_mode(m_extended_mode.internal_value());
	}

	void on_extended_component_mode_changed(k3d::iunknown*)
	{
		m_selection_model.set_extended_component_mode(m_extended_component_mode.internal_value());
	}

	void on_paint_mode_changed(k3d::iunknown*)
	{
		m_selection_model.set_paint_mode(m_paint_mode.internal_value());
	}

	void on_double_click_mode_changed(k3d::iunknown*)
	{
		m_selection_model.set_double_click_mode(m_double_click_mode.internal_value());
	}
	
	void on_pick_backfacing_changed(k3d::iunknown*)
	{
		m_selection_model.set_pick_backfacing(m_pick_backfacing.internal_value());
	}
	
	void on_paint_backfacing_changed(k3d::iunknown*)
	{
		m_selection_model.set_paint_backfacing(m_paint_backfacing.internal_value());
	}
	
	void on_rubber_band_backfacing_changed(k3d::iunknown*)
	{
		m_selection_model.set_rubber_band_backfacing(m_rubber_band_backfacing.internal_value());
	}

	document_state& m_document_state;
	/// Provides interactive navigation behavior
	navigation_input_model m_navigation_model;
	/// Provides interactive selection behavior
	selection_input_model m_selection_model;
	/// Dispatches incoming user input events
	basic_viewport_input_model m_input_model;

	/// Stores extended selection toggle
	bool_property_t m_extended_mode;
	/// Stores extended component selection toggle
	bool_property_t m_extended_component_mode;
	/// Store box selection / paint mode toggle
	bool_property_t m_paint_mode;
	/// Enables double-click actions
	bool_property_t m_double_click_mode;
	/// Convert selection when switching selection modes
	bool_property_t m_convert_selection;
	/// Keep selection when switching selection modes
	bool_property_t m_keep_selection;
	/// Pick backfacing elements
	bool_property_t m_pick_backfacing;
	/// Paint-select backfacing elements
	bool_property_t m_paint_backfacing;
	/// Rubber-band select backfacing elements
	bool_property_t m_rubber_band_backfacing;
};

/////////////////////////////////////////////////////////////////////////////
// selection_tool

selection_tool::selection_tool(document_state& DocumentState, const std::string& Name) :
	base(DocumentState, Name),
	m_implementation(new implementation(DocumentState))
{
	m_implementation->m_navigation_model.connect_command_signal(sigc::mem_fun(*this, &selection_tool::record_command));
	m_implementation->m_selection_model.connect_command_signal(sigc::mem_fun(*this, &selection_tool::record_command));
}

selection_tool::~selection_tool()
{
	delete m_implementation;
}

const k3d::icommand_node::result selection_tool::execute_command(const std::string& Command, const std::string& Arguments)
{
	k3d::icommand_node::result result = m_implementation->m_selection_model.execute_command(Command, Arguments);
	if(result != RESULT_UNKNOWN_COMMAND)
		return result;

	result = m_implementation->m_navigation_model.execute_command(Command, Arguments);
	if(result != RESULT_UNKNOWN_COMMAND)
		return result;
	
	// Allow access to selection options
	if (Command == "toggle_pick_backfacing")
	{
		detail::toggle_property(m_implementation->m_pick_backfacing);
		return RESULT_CONTINUE;
	}
	if (Command == "toggle_paint_backfacing")
	{
		detail::toggle_property(m_implementation->m_paint_backfacing);
		return RESULT_CONTINUE;
	}
	if (Command == "toggle_rubber_band_backfacing")
	{
		detail::toggle_property(m_implementation->m_rubber_band_backfacing);
		return RESULT_CONTINUE;
	}
	
	return RESULT_UNKNOWN_COMMAND;
}

bool selection_tool::convert_selection()
{
	return m_implementation->m_convert_selection.internal_value();
}

bool selection_tool::keep_selection()
{
	return m_implementation->m_keep_selection.internal_value();
}

bool selection_tool::pick_backfacing()
{
	return m_implementation->m_pick_backfacing.internal_value();
}

bool selection_tool::paint_backfacing()
{
	return m_implementation->m_paint_backfacing.internal_value();
}

bool selection_tool::rubber_band_backfacing()
{
	return m_implementation->m_rubber_band_backfacing.internal_value();
}

k3d::iproperty_collection* selection_tool::get_property_collection()
{
	return m_implementation;
}

viewport_input_model& selection_tool::get_input_model()
{
	return m_implementation->m_input_model;
}

} // namespace libk3dngui

