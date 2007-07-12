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

#include "angle_axis_control.h"
#include "aqsis_layer_chooser.h"
#include "asynchronous_update.h"
#include "bitmap_preview.h"
#include "bounding_box.h"
#include "button.h"
#include "check_button.h"
#include "collapsible_frame.h"
#include "color_chooser.h"
#include "combo_box.h"
#include "document_state.h"
#include "entry.h"
#include "enumeration_chooser.h"
#include "file_chooser_dialog.h"
#include "icons.h"
#include "messages.h"
#include "node_chooser.h"
#include "path_chooser.h"
#include "point_control.h"
#include "property_button.h"
#include "property_label.h"
#include "render.h"
#include "scale.h"
#include "script_button.h"
#include "selection_button.h"
#include "spin_button.h"
#include "tool.h"
#include "tool_properties.h"
#include "toolbar.h"
#include "ui_component.h"
#include "utility.h"
#include "widget_manip.h"

#include <k3dsdk/command_tree.h>
#include <k3d-i18n-config.h>
#include <k3dsdk/ianimation_render_engine.h>
#include <k3dsdk/iaqsis.h>
#include <k3dsdk/icamera.h>
#include <k3dsdk/icamera_animation_render_engine.h>
#include <k3dsdk/icamera_preview_render_engine.h>
#include <k3dsdk/icamera_still_render_engine.h>
#include <k3dsdk/ideletable.h>
#include <k3dsdk/idocument.h>
#include <k3dsdk/ienumeration_property.h>
#include <k3dsdk/ilist_property.h>
#include <k3dsdk/imeasurement_property.h>
#include <k3dsdk/inode.h>
#include <k3dsdk/iplugin_factory.h>
#include <k3dsdk/ipreview_render_engine.h>
#include <k3dsdk/iproperty_group_collection.h>
#include <k3dsdk/iscript_property.h>
#include <k3dsdk/iselectable.h>
#include <k3dsdk/istill_render_engine.h>
#include <k3dsdk/iuser_property.h>
#include <k3dsdk/mesh_selection.h>
#include <k3dsdk/options.h>
#include <k3dsdk/property.h>
#include <k3dsdk/types_ri.h>
#include <k3dsdk/state_change_set.h>
#include <k3dsdk/string_cast.h>
#include <k3dsdk/string_modifiers.h>
#include <k3dsdk/system.h>
#include <k3dsdk/utility.h>

// Not strictly required to compile, but this #include ensures that we have a std::typeinfo for k3d::mesh that matches the SDK (i.e. we don't break the ODR)
#include <k3dsdk/bitmap.h>
#include <k3dsdk/legacy_mesh.h>

#include <gtkmm/arrow.h>
#include <gtkmm/box.h>
#include <gtkmm/frame.h>
#include <gtkmm/image.h>
#include <gtkmm/label.h>
#include <gtkmm/scrolledwindow.h>
#include <gtkmm/stock.h>
#include <gtkmm/table.h>

#include <boost/format.hpp>

namespace libk3dngui
{

namespace tool_properties
{

/////////////////////////////////////////////////////////////////////////////
// control::implementation

class control::implementation :
	public asynchronous_update
{
public:
	implementation(document_state& DocumentState, k3d::icommand_node& Parent) :
		m_document_state(DocumentState),
		m_parent(Parent)
	{
		m_label.set_alignment(Gtk::ALIGN_LEFT);
		m_label.set_padding(5, 5);

		m_scrolled_window.set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
		m_scrolled_window.add(m_vbox);

		m_document_state.view_tool_properties_signal().connect(sigc::mem_fun(*this, &implementation::on_view_tool_properties));
		m_document_state.active_tool_changed_signal().connect(sigc::hide(sigc::mem_fun(*this, &implementation::schedule_update)));
		schedule_update();
	}

	bool on_view_tool_properties(k3d::iproperty_collection* const Object)
	{
		tool& active_tool = m_document_state.active_tool();
		k3d::iproperty_collection* const property_collection = dynamic_cast<k3d::iproperty_collection*>(&active_tool);
		if(property_collection)
		{
			m_tool_properties_changed_connection = property_collection->connect_properties_changed_signal(sigc::hide(sigc::mem_fun(*this, &implementation::schedule_update)));

			return true;
		}

		return false;
	}

	void update_label(const std::string& Name)
	{
		m_label.set_text(k3d::string_cast(boost::format(_("%1% Properties")) % Name));
	}

	void reset()
	{
		Glib::ListHandle<Widget*> children = m_vbox.get_children();
		std::for_each(children.begin(), children.end(), k3d::delete_object());
	}

	void on_update()
	{
		tool& active_tool = m_document_state.active_tool();

		update_label(k3d::command_tree().name(active_tool));
		reset();

		// Create a toolbar ...
		toolbar::control* const toolbar_control = new toolbar::control(m_parent, "toolbar");
		m_vbox.pack_start(*Gtk::manage(toolbar_control), Gtk::PACK_SHRINK);

		k3d::istate_recorder* const state_recorder = 0;

		// Get the object properties, grouped together ...
		k3d::iproperty_collection* const property_collection = active_tool.properties();
		k3d::iproperty_group_collection::groups_t property_groups;
		if(property_collection)
		{
			k3d::iproperty_collection::properties_t all_properties = property_collection->properties();
			k3d::iproperty_group_collection::groups_t groups;

			k3d::iproperty_group_collection* const property_group_collection = dynamic_cast<k3d::iproperty_group_collection*>(&active_tool);
			if(property_group_collection)
			{
				groups = property_group_collection->property_groups();
				for(k3d::iproperty_group_collection::groups_t::const_iterator group = groups.begin(); group != groups.end(); ++group)
				{
					for(k3d::iproperty_collection::properties_t::const_iterator property = group->properties.begin(); property != group->properties.end(); ++property)
						all_properties.erase(std::remove(all_properties.begin(), all_properties.end(), *property), all_properties.end());
				}
			}

			property_groups.insert(property_groups.end(), k3d::iproperty_group_collection::group("tool name", all_properties));
			property_groups.insert(property_groups.end(), groups.begin(), groups.end());
		}

		// For each property group ...
		for(k3d::iproperty_group_collection::groups_t::const_iterator property_group = property_groups.begin(); property_group != property_groups.end(); ++property_group)
		{
			if(property_group->properties.empty())
				continue;

			collapsible_frame::control* const frame = new collapsible_frame::control(property_group->name, m_collapsible_frame_group);
			m_vbox.pack_start(*Gtk::manage(frame), Gtk::PACK_SHRINK);

			Gtk::Table* const table = new Gtk::Table(property_group->properties.size(), 5, false);
			frame->add(*Gtk::manage(table));

			// Store entries for focus chain within table
			std::list<Gtk::Widget*> entry_list;

			const unsigned long prop_button_begin = 1;
			const unsigned long prop_button_end = 2;
			const unsigned long prop_label_begin = 2;
			const unsigned long prop_label_end = 3;
			const unsigned long prop_control_begin = 3;
			const unsigned long prop_control_end = 4;

			// For each property within the group ...
			unsigned int row = 0;
			for(unsigned int i = 0; i != property_group->properties.size(); ++i, ++row)
			{
				k3d::iproperty& property = *property_group->properties[i];

				const std::string property_name = property.property_name();
				const std::type_info& property_type = property.property_type();

				// Provide a property button for the property ...
				table->attach(*Gtk::manage(
					new property_button::control(m_parent, property_name + "_property", property_widget::proxy(m_document_state,property))),
					prop_button_begin, prop_button_end, row, row + 1, Gtk::SHRINK, Gtk::SHRINK);

				// Provide a label for the property ...
				table->attach(*Gtk::manage(
					new property_label::control(m_parent, property_name + "_label", property_widget::proxy(m_document_state, property))),
					prop_label_begin, prop_label_end, row, row + 1, Gtk::FILL | Gtk::SHRINK, Gtk::FILL | Gtk::SHRINK);

				// Boolean properties ...
				if(property_type == typeid(bool))
				{
					check_button::control* const control = new check_button::control(m_parent, property_name, check_button::proxy(property, state_recorder, property_name));
					table->attach(*Gtk::manage(control), prop_control_begin, prop_control_end, row, row + 1, Gtk::FILL | Gtk::SHRINK, Gtk::FILL | Gtk::SHRINK);

					entry_list.push_back(control);
				}
				// Scalar properties ...
				else if(property_type == typeid(double) || property_type == typeid(float) || property_type == typeid(long) || property_type == typeid(unsigned long) || property_type == typeid(int) || property_type == typeid(unsigned int))
				{
					spin_button::control* const control = new spin_button::control(m_parent, property_name, spin_button::proxy(property, state_recorder, property_name));
					k3d::imeasurement_property* const measurement_property = dynamic_cast<k3d::imeasurement_property*>(&property);
					if(measurement_property)
					{
						control->set_step_increment(measurement_property->property_step_increment());
						control->set_units(measurement_property->property_units());
					}
					table->attach(*Gtk::manage(control), prop_control_begin, prop_control_end, row, row + 1, Gtk::FILL | Gtk::SHRINK, Gtk::FILL | Gtk::SHRINK);

					entry_list.push_back(control);
				}
				// Color properties ...
				else if(property_type == typeid(k3d::color) || property_type == typeid(k3d::ri::color))
				{
					color_chooser::control* const control = new color_chooser::control(m_parent, property_name, color_chooser::proxy(property, state_recorder, property_name));
					table->attach(*Gtk::manage(control), prop_control_begin, prop_control_end, row, row + 1, Gtk::FILL | Gtk::SHRINK, Gtk::FILL | Gtk::SHRINK);

					entry_list.push_back(control);
				}
				// String properties ...
				else if(property_type == typeid(std::string) || property_type == typeid(k3d::ri::string))
				{
					if(dynamic_cast<k3d::ienumeration_property*>(&property))
					{
						enumeration_chooser::control* const control = new enumeration_chooser::control(m_parent, property_name, enumeration_chooser::proxy(property, state_recorder, property_name));
						table->attach(*Gtk::manage(control), prop_control_begin, prop_control_end, row, row + 1, Gtk::FILL | Gtk::SHRINK, Gtk::FILL | Gtk::SHRINK);

						entry_list.push_back(control);
					}
					else if(dynamic_cast<k3d::iscript_property*>(&property))
					{
						script_button::control* const control = new script_button::control(m_parent, property_name, script_button::proxy(property, state_recorder, property_name));
						table->attach(*Gtk::manage(control), prop_control_begin, prop_control_end, row, row + 1, Gtk::FILL | Gtk::SHRINK, Gtk::FILL | Gtk::SHRINK);

						entry_list.push_back(control);
					}
					else if(k3d::ilist_property<std::string>* const list_property = dynamic_cast<k3d::ilist_property<std::string>*>(&property))
					{
						combo_box::control* const control = new combo_box::control(m_parent, property_name, combo_box::proxy(property, state_recorder, property_name));
						control->set_values(list_property->property_values());
						table->attach(*Gtk::manage(control), prop_control_begin, prop_control_end, row, row + 1, Gtk::FILL | Gtk::SHRINK, Gtk::FILL | Gtk::SHRINK);

						entry_list.push_back(control);
					}
					else
					{
						entry::control* const control = new entry::control(m_parent, property_name, entry::proxy(property, state_recorder, property_name));
						table->attach(*Gtk::manage(control), prop_control_begin, prop_control_end, row, row + 1, Gtk::FILL | Gtk::SHRINK, Gtk::FILL | Gtk::SHRINK);

						entry_list.push_back(control);
					}
				}
				// k3d::aqsis::ilayer_connection* properties
				else if(k3d::aqsis::ilayer_connection_property* const layer_connection_property = dynamic_cast<k3d::aqsis::ilayer_connection_property*>(&property))
				{
					aqsis_layer_chooser::control* const control = new aqsis_layer_chooser::control(m_document_state, *layer_connection_property, m_parent, property_name, state_recorder);
					table->attach(*Gtk::manage(control), prop_control_begin, prop_control_end, row, row + 1, Gtk::FILL | Gtk::SHRINK, Gtk::FILL | Gtk::SHRINK);

					entry_list.push_back(control);
				}
				// k3d::inode* properties ...
				else if(property_type == typeid(k3d::inode*))
				{
					node_chooser::control* const control = new node_chooser::control(m_parent, property_name, node_chooser::proxy(m_document_state, property, state_recorder, property_name), node_chooser::filter(property));
					table->attach(*Gtk::manage(control), prop_control_begin, prop_control_end, row, row + 1, Gtk::FILL | Gtk::SHRINK, Gtk::FILL | Gtk::SHRINK);

					entry_list.push_back(control);
				}
/*
				// Bitmap properties ...
				else if(property_type == typeid(k3d::bitmap*))
				{
					bitmap_preview::control* const control = new bitmap_preview::control(m_parent, property_name, bitmap_preview::proxy(property));
					table->attach(*Gtk::manage(control), prop_control_begin, prop_control_end, row, row + 1, Gtk::FILL | Gtk::SHRINK, Gtk::FILL | Gtk::SHRINK);

					entry_list.push_back(control);
				}
*/
				// Filesystem-path properties ...
				else if(property_type == typeid(k3d::filesystem::path))
				{
					path_chooser::control* const control = new path_chooser::control(m_parent, property_name, path_chooser::proxy(property, state_recorder, property_name));
					table->attach(*Gtk::manage(control), prop_control_begin, prop_control_end, row, row + 1, Gtk::FILL | Gtk::SHRINK, Gtk::FILL | Gtk::SHRINK);

					entry_list.push_back(control);
				}
				// k3d::bounding_box3 properties ...
				else if(property_type == typeid(k3d::bounding_box3))
				{
					bounding_box::control* const control = new bounding_box::control(m_parent, property_name, bounding_box::proxy(property));
					table->attach(*Gtk::manage(control), prop_control_begin, prop_control_end, row, row + 1, Gtk::FILL | Gtk::SHRINK, Gtk::FILL | Gtk::SHRINK);

					entry_list.push_back(control);
				}
				// k3d::point3 properties ...
				else if(property_type == typeid(k3d::vector3) || property_type == typeid(k3d::point3) || property_type == typeid(k3d::ri::point))
				{
					point::control* const control = new point::control(m_parent, property_name, point::proxy(property));
					table->attach(*Gtk::manage(control), prop_control_begin, prop_control_end, row, row + 1, Gtk::FILL | Gtk::SHRINK, Gtk::FILL | Gtk::SHRINK);

					entry_list.push_back(control);
				}
				// k3d::angle_axis properties ...
				else if(property_type == typeid(k3d::angle_axis))
				{
					angle_axis::control* const control = new angle_axis::control(m_parent, property_name, angle_axis::proxy(property, state_recorder, property_name));
					table->attach(*Gtk::manage(control), prop_control_begin, prop_control_end, row, row + 1, Gtk::FILL | Gtk::SHRINK, Gtk::FILL | Gtk::SHRINK);

					entry_list.push_back(control);
				}
				// Transformation properties ...
				else if(property_type == typeid(k3d::matrix4) || property_type == typeid(k3d::ri::matrix))
				{
				}
				// Mesh properties ...
				else if(property_type == typeid(k3d::legacy::mesh*))
				{
				}
				// Mesh Selection properties ...
				else if(property_type == typeid(k3d::mesh_selection))
				{
					selection_button::control* const control = new selection_button::control(m_parent, property_name, selection_button::proxy(property, state_recorder, property_name));
					table->attach(*Gtk::manage(control), prop_control_begin, prop_control_end, row, row + 1, Gtk::FILL | Gtk::SHRINK, Gtk::FILL | Gtk::SHRINK);
				}
				else
				{
					k3d::log() << warning << k3d_file_reference << "unknown property type: " << property_type.name() << " name: " << property_name << std::endl;
				}
			}

			// Set focus chain
			table->set_focus_chain(entry_list);
		}

		m_vbox.show_all();
	}

	/// Stores a reference to the owning document
	document_state& m_document_state;
	k3d::icommand_node& m_parent;
	/// Keeps track of object name changes
	//sigc::connection m_object_name_change_connection;
	/// Keeps track of changes to the set of object properties
	sigc::connection m_tool_properties_changed_connection;
	/// Displays the current object name
	Gtk::Label m_label;
	/// Contains the set of object properties
	Gtk::ScrolledWindow m_scrolled_window;
	/// Parent widget for the rest of the implementation
	Gtk::VBox m_vbox;
	/// Groups collapsible frames together
	collapsible_frame::group m_collapsible_frame_group;

	sigc::signal<void, const std::string&, const std::string&> m_command_signal;

	/// Signal that will be emitted whenever this control should grab the panel focus
	sigc::signal<void> m_panel_grab_signal;
};

/////////////////////////////////////////////////////////////////////////////
// control

control::control(document_state& DocumentState, k3d::icommand_node& Parent) :
	ui_component("tool_properties", &Parent),
	m_implementation(new implementation(DocumentState, *this))
{
	m_implementation->m_command_signal.connect(sigc::mem_fun(*this, &control::record_command));

	m_implementation->m_scrolled_window.signal_button_press_event().connect(sigc::bind_return(sigc::hide(m_implementation->m_panel_grab_signal.make_slot()), false), false);
	
	pack_start(m_implementation->m_label, Gtk::PACK_SHRINK);
	pack_start(m_implementation->m_scrolled_window, Gtk::PACK_EXPAND_WIDGET);
	show_all();
}

control::~control()
{
	delete m_implementation;
}

void control::initialize(document_state& DocumentState, k3d::icommand_node& Parent)
{
	assert_not_implemented();
}

const std::string control::panel_type()
{
	return "tool_properties";
}

sigc::connection control::connect_focus_signal(const sigc::slot<void>& Slot)
{
	return m_implementation->m_panel_grab_signal.connect(Slot);
}

} // namespace tool_properties

} // namespace libk3dngui

