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

#include "angle_axis.h"
#include "auto_property_page.h"
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
#include "node_chooser.h"
#include "node_collection_chooser.h"
#include "path_chooser.h"
#include "point3.h"
#include "property_button.h"
#include "property_label.h"
#include "script_button.h"
#include "selection_button.h"
#include "spin_button.h"
#include "widget_manip.h"

#include <k3d-i18n-config.h>
#include <k3dsdk/istate_recorder.h>
#include <k3dsdk/iuser_property.h>
#include <k3dsdk/state_change_set.h>
#include <k3dsdk/type_registry.h>
#include <k3dsdk/user_properties.h>
#include <k3dsdk/utility.h>

// Not strictly required to compile, but this #include ensures that we have a std::typeinfo for legacy::mesh that matches the SDK (i.e. we don't break the ODR)
#include <k3dsdk/legacy_mesh.h>
#include <k3dsdk/mesh.h>

#include <gtkmm/box.h>
#include <gtkmm/image.h>
#include <gtkmm/stock.h>
#include <gtkmm/table.h>

using namespace libk3dngui;

namespace k3d
{

namespace ngui
{

namespace auto_property_page
{

///////////////////////////////////////////////////////////////////////////
// control::implementation

class control::implementation
{
public:
	implementation(document_state& DocumentState, icommand_node& Parent) :
		m_document_state(DocumentState),
		m_parent(Parent)
	{
	}

	void set_properties(const objects_t& Objects)
	{
		iproperty_group_collection::groups_t final_groups;

		if(Objects.size() == 1) // As a special case when displaying a single object ...
		{
			// Get the node properties, grouped together ...
			if(iproperty_collection* const property_collection = dynamic_cast<iproperty_collection*>(Objects[0]))
			{
				iproperty_group_collection::group builtin_properties(_("Builtin Properties"));
				iproperty_group_collection::group user_properties(_("User Properties"));

				const iproperty_collection::properties_t properties = property_collection->properties();
				for(iproperty_collection::properties_t::const_iterator property = properties.begin(); property != properties.end(); ++property)
				{
					if(dynamic_cast<iuser_property*>(*property))
						user_properties.properties.push_back(*property);
					else
						builtin_properties.properties.push_back(*property);
				}

				if(iproperty_group_collection* const property_group_collection = dynamic_cast<iproperty_group_collection*>(Objects[0]))
				{
					const iproperty_group_collection::groups_t groups = property_group_collection->property_groups();
					for(iproperty_group_collection::groups_t::const_iterator group = groups.begin(); group != groups.end(); ++group)
					{
						for(iproperty_collection::properties_t::const_iterator property = group->properties.begin(); property != group->properties.end(); ++property)
						{
							builtin_properties.properties.erase(
								std::remove(builtin_properties.properties.begin(), builtin_properties.properties.end(), *property),
								builtin_properties.properties.end());
						}

						if(!group->properties.empty())
							final_groups.push_back(*group);
					}
				}

				if(!builtin_properties.properties.empty())
					final_groups.insert(final_groups.begin(), builtin_properties);
				if(!user_properties.properties.empty())
					final_groups.push_back(user_properties);
			}
		}
		else // Display multiple objects ...
		{
			// For each object ...
			for(objects_t::const_iterator object = Objects.begin(); object != Objects.end(); ++object)
			{
				// Create a property group ...
				iproperty_group_collection::group group(_("Unknown object type"));

				// Assign the group a name based on the node name or the factory name ...
				if(inode* const node = dynamic_cast<inode*>(*object))
				{
					group.name = node->name();

					if(group.name.empty())
						group.name = node->factory().name();
				}

				if(iproperty_collection* const property_collection = dynamic_cast<iproperty_collection*>(*object))
					group.properties = property_collection->properties();

				final_groups.push_back(group);
			}
		}

		set_properties(final_groups);
	}

	void set_properties(const iproperty_group_collection::groups_t& PropertyGroups)
	{
		// Delete existing controls ...
		Glib::ListHandle<Gtk::Widget*> children = m_vbox.get_children();
		std::for_each(children.begin(), children.end(), delete_object());

		istate_recorder* const state_recorder = &m_document_state.document().state_recorder();

		for(iproperty_group_collection::groups_t::const_iterator group = PropertyGroups.begin(); group != PropertyGroups.end(); ++group)
		{
			collapsible_frame::control* const frame = new collapsible_frame::control(group->name, m_collapsible_frame_group);
			m_vbox.pack_start(*manage(frame), Gtk::PACK_SHRINK);

			Gtk::Table* const table = new Gtk::Table(group->properties.size(), 5, false);
			frame->add(*manage(table));

			// Store entries for focus chain within table
			std::list<Gtk::Widget*> entry_list;

			const unsigned long prop_delete_begin = 0;
			const unsigned long prop_delete_end = 1;
			const unsigned long prop_button_begin = 1;
			const unsigned long prop_button_end = 2;
			const unsigned long prop_label_begin = 2;
			const unsigned long prop_label_end = 3;
			const unsigned long prop_control_begin = 3;
			const unsigned long prop_control_end = 4;

			// For each property within the group ...
			unsigned int row = 0;
			for(unsigned int i = 0; i != group->properties.size(); ++i, ++row)
			{
				iproperty& property = *group->properties[i];

				const string_t property_name = property.property_name();
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
				if(property_type == typeid(bool_t))
				{
					check_button::control* const control = new check_button::control(m_parent, property_name, check_button::proxy(property, state_recorder, property_name));
					table->attach(*manage(control), prop_control_begin, prop_control_end, row, row + 1, Gtk::FILL | Gtk::SHRINK, Gtk::FILL | Gtk::SHRINK);

					entry_list.push_back(control);
				}
				// Scalar properties ...
				else if(property_type == typeid(int32_t) || property_type == typeid(uint32_t) || property_type == typeid(float_t) || property_type == typeid(double_t))
				{
					spin_button::control* const control = new spin_button::control(m_parent, property_name, spin_button::model(property), state_recorder);
					table->attach(*manage(control), prop_control_begin, prop_control_end, row, row + 1, Gtk::FILL | Gtk::SHRINK, Gtk::FILL | Gtk::SHRINK);
					entry_list.push_back(control);
				}
				// Color properties ...
				else if(property_type == typeid(color))
				{
					color_chooser::control* const control = new color_chooser::control(m_parent, property_name, color_chooser::proxy(property, state_recorder, property_name));
					table->attach(*manage(control), prop_control_begin, prop_control_end, row, row + 1, Gtk::FILL | Gtk::SHRINK, Gtk::FILL | Gtk::SHRINK);

					entry_list.push_back(control);
				}
				// String properties ...
				else if(property_type == typeid(string_t))
				{
					if(dynamic_cast<ienumeration_property*>(&property))
					{
						enumeration_chooser::control* const control = new enumeration_chooser::control(m_parent, property_name, enumeration_chooser::model(property), state_recorder);
						table->attach(*manage(control), prop_control_begin, prop_control_end, row, row + 1, Gtk::FILL | Gtk::SHRINK, Gtk::FILL | Gtk::SHRINK);

						entry_list.push_back(control);
					}
					else if(dynamic_cast<iscript_property*>(&property))
					{
						script_button::control* const control = new script_button::control(m_parent, property_name, script_button::proxy(property, state_recorder, property_name));
						table->attach(*Gtk::manage(control), prop_control_begin, prop_control_end, row, row + 1, Gtk::FILL | Gtk::SHRINK, Gtk::FILL | Gtk::SHRINK);

						entry_list.push_back(control);
					}
					else if(ilist_property<string_t>* const list_property = dynamic_cast<ilist_property<string_t>*>(&property))
					{
						combo_box::control* const control = new combo_box::control(m_parent, property_name, combo_box::proxy(property, state_recorder, property_name));
						control->set_values(list_property->property_values());
						table->attach(*manage(control), prop_control_begin, prop_control_end, row, row + 1, Gtk::FILL | Gtk::SHRINK, Gtk::FILL | Gtk::SHRINK);

						entry_list.push_back(control);
					}
					else
					{
						entry::control* const control = new entry::control(m_parent, property_name, entry::proxy(property, state_recorder, property_name));
						table->attach(*manage(control), prop_control_begin, prop_control_end, row, row + 1, Gtk::FILL | Gtk::SHRINK, Gtk::FILL | Gtk::SHRINK);

						entry_list.push_back(control);
					}
				}
				// inode* properties ...
				else if(property_type == typeid(inode*))
				{
					node_chooser::control* const control = new node_chooser::control(m_parent, property_name, node_chooser::proxy(m_document_state, property, state_recorder, property_name), node_chooser::filter(property));
					table->attach(*manage(control), prop_control_begin, prop_control_end, row, row + 1, Gtk::FILL | Gtk::SHRINK, Gtk::FILL | Gtk::SHRINK);

					entry_list.push_back(control);
				}
				else if(property_type == typeid(inode_collection_property::nodes_t))
				{
					node_collection_chooser::control* const control = new node_collection_chooser::control(m_parent, property_name, node_collection_chooser::model(property), state_recorder);
					table->attach(*manage(control), prop_control_begin, prop_control_end, row, row + 1, Gtk::FILL | Gtk::SHRINK, Gtk::FILL | Gtk::SHRINK);
					entry_list.push_back(control);
				}
				// Bitmap properties ...
				else if(property_type == type_id_k3d_bitmap_ptr())
				{
					bitmap_preview::control* const control = new bitmap_preview::control(m_parent, property_name, bitmap_preview::proxy(property));
					table->attach(*manage(control), prop_control_begin, prop_control_end, row, row + 1, Gtk::FILL | Gtk::SHRINK, Gtk::FILL | Gtk::SHRINK);

					entry_list.push_back(control);
				}
				// Filesystem-path properties ...
				else if(property_type == typeid(filesystem::path))
				{
					path_chooser::control* const control = new path_chooser::control(m_parent, property_name, path_chooser::proxy(property, state_recorder, property_name));
					table->attach(*manage(control), prop_control_begin, prop_control_end, row, row + 1, Gtk::FILL | Gtk::SHRINK, Gtk::FILL | Gtk::SHRINK);

					entry_list.push_back(control);
				}
				// bounding_box3 properties ...
				else if(property_type == typeid(bounding_box3))
				{
					bounding_box::control* const control = new bounding_box::control(m_parent, property_name, bounding_box::proxy(property));
					table->attach(*manage(control), prop_control_begin, prop_control_end, row, row + 1, Gtk::FILL | Gtk::SHRINK, Gtk::FILL | Gtk::SHRINK);

					entry_list.push_back(control);
				}
				// point3 properties ...
				else if(property_type == typeid(point3) || property_type == typeid(vector3) || property_type == typeid(normal3))
				{
					point::control* const control = new point::control(m_parent, property_name, point::proxy(property));
					table->attach(*manage(control), prop_control_begin, prop_control_end, row, row + 1, Gtk::FILL | Gtk::SHRINK, Gtk::FILL | Gtk::SHRINK);

					entry_list.push_back(control);
				}
				// angle_axis properties ...
				else if(property_type == typeid(angle_axis))
				{
					libk3dngui::angle_axis::control* const control = new libk3dngui::angle_axis::control(m_parent, property_name, libk3dngui::angle_axis::proxy(property, state_recorder, property_name));
					table->attach(*manage(control), prop_control_begin, prop_control_end, row, row + 1, Gtk::FILL | Gtk::SHRINK, Gtk::FILL | Gtk::SHRINK);

					entry_list.push_back(control);
				}
				// Transformation properties ...
				else if(property_type == typeid(matrix4))
				{
				}
				// Mesh properties ...
				else if(property_type == typeid(legacy::mesh*))
				{
				}
				else if(property_type == typeid(mesh*))
				{
				}
				// HPoint properties ...
				else if(property_type == typeid(point4))
				{
				}
				// Pipeline-profiler records ...
				else if(property_type == typeid(std::map<inode*, std::map<string_t, double_t> >))
				{
				}
				// Mesh Selection properties ...
				else if(property_type == typeid(mesh_selection))
				{
					selection_button::control* const control = new selection_button::control(m_parent, property_name, selection_button::proxy(property, state_recorder, property_name));
					table->attach(*manage(control), prop_control_begin, prop_control_end, row, row + 1, Gtk::FILL | Gtk::SHRINK, Gtk::FILL | Gtk::SHRINK);
				}
				else
				{
					log() << warning << k3d_file_reference << "unknown property type: " << property_type.name() << " name: " << property_name << std::endl;
				}

				// Provide a "delete" button for user properties ...
				if(dynamic_cast<iuser_property*>(&property))
				{
					iproperty_collection* const property_collection = dynamic_cast<iproperty_collection*>(property.property_node());

					button::control* const control =
						new button::control(m_parent, property_name + "_delete", *Gtk::manage(new Gtk::Image(Gtk::Stock::DELETE, Gtk::ICON_SIZE_BUTTON)))
						<< connect_button(sigc::bind(sigc::bind(sigc::mem_fun(*this, &implementation::on_delete_user_property), &property), property_collection))
						<< set_tooltip(_("Delete user property (no undo)"));

					table->attach(*manage(control), prop_delete_begin, prop_delete_end, row, row + 1, Gtk::SHRINK, Gtk::SHRINK);
				}
			}

			// Set focus chain ...
			table->set_focus_chain(entry_list);
		}

		// Display everything ...
		m_vbox.show_all();
	}

	void on_delete_user_property(iproperty_collection* Collection, iproperty* Property)
	{
		return_if_fail(Collection);
		return_if_fail(Property);
		return_if_fail(dynamic_cast<iuser_property*>(Property));

		record_state_change_set change_set(m_document_state.document(), "Delete user property", K3D_CHANGE_SET_CONTEXT);

		if(m_document_state.document().state_recorder().current_change_set())
			m_document_state.document().state_recorder().current_change_set()->record_old_state(new user::property_container(*Collection));

		Collection->unregister_property(*Property);
		if(ipersistent* const persistent = dynamic_cast<ipersistent*>(Property))
		{
			if(ipersistent_container* const persistent_container = dynamic_cast<ipersistent_container*>(Collection))
				persistent_container->disable_serialization(*persistent);
		}

		undoable_delete(Property, m_document_state.document());

		if(m_document_state.document().state_recorder().current_change_set())
			m_document_state.document().state_recorder().current_change_set()->record_new_state(new user::property_container(*Collection));
	}

	document_state& m_document_state;
	icommand_node& m_parent;
	/// Stores the collection of property controls
	Gtk::VBox m_vbox;
	/// Groups collapsible frames together
	collapsible_frame::group m_collapsible_frame_group;
};

///////////////////////////////////////////////////////////////////////////
// control

control::control(document_state& DocumentState, icommand_node& Parent) :
	m_implementation(new implementation(DocumentState, Parent))
{
}

control::~control()
{
	delete m_implementation;
}

void control::set_properties(iunknown* Object)
{
	m_implementation->set_properties(objects_t(1, Object));
}

void control::set_properties(const objects_t& Objects)
{
	m_implementation->set_properties(Objects);
}

void control::set_properties(const iproperty_group_collection::groups_t& PropertyGroups)
{
	m_implementation->set_properties(PropertyGroups);
}

Gtk::Widget& control::get_widget()
{
	return m_implementation->m_vbox;
}

} // namespace auto_property_page

} // namespace ngui

} // namespace k3d

