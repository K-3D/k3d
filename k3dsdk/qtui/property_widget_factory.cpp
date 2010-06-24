// K-3D
// Copyright (c) 1995-2010, Timothy M. Shead
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

#include <k3dsdk/imetadata.h>
#include <k3dsdk/log.h>
#include <k3dsdk/metadata.h>
#include <k3dsdk/metadata_keys.h>
#include <k3dsdk/plugin.h>
#include <k3dsdk/qtui/property_widget.h>
#include <k3dsdk/qtui/property_widget_factory.h>
#include <k3dsdk/type_registry.h>

#include <QWidget>

namespace k3d
{

namespace qtui
{

/*
class control::implementation
{
public:
	implementation(document_state& DocumentState) :
		m_document_state(DocumentState)
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


		// Display everything ...
		m_vbox.show_all();
	}

	document_state& m_document_state;
};
*/

QWidget* property_widget_factory::create(idocument& Document, iproperty& Property)
{
	static const plugin::factory::collection_t widget_factories = plugin::factory::lookup("qtui:component-type", "property-widget");

	istate_recorder* const state_recorder = &Document.state_recorder();
	const string_t property_name = Property.property_name();
	const std::type_info& property_type = Property.property_type();
	
	// Keep track of the control that's created (if any) ...
	QWidget* result = 0;

	// Look for custom property controls that match our property role ...
	if(!result)
	{
		// Look for an explicit property role ...
		if(imetadata* const property_metadata = dynamic_cast<imetadata*>(&Property))
		{
			const string_t property_widget_role = property_metadata->get_metadata()[k3d::metadata::key::role()];
			if(property_widget_role.size())
			{
				// Now see if we can find a custom property widget that matches the role ...
				for(plugin::factory::collection_t::const_iterator factory = widget_factories.begin(); factory != widget_factories.end(); ++factory)
				{
					if((**factory).metadata()["qtui:property-role"] != property_widget_role)
						continue;

					if(property_widget* const widget = plugin::create<property_widget>(**factory))
					{
						if(result = dynamic_cast<QWidget*>(widget))
						{
							widget->initialize(Document, Property);
						}
						else
						{
							k3d::log() << error << "property widget must derive from QWidget" << std::endl;
						}
					}
					else
					{
						k3d::log() << error << "error creating property widget" << std::endl;
					}

					break;
				}
			}
		}
	}

	// Look for custom property controls that match our property type ...
	if(!result)
	{
		// Get the C++ property type ...
		const string_t property_widget_type = k3d::type_string(property_type);
		if(property_widget_type.size())
		{
			// Now see if we can find a custom property control that matches the type ...
			for(plugin::factory::collection_t::const_iterator factory = widget_factories.begin(); factory != widget_factories.end(); ++factory)
			{
				if((**factory).metadata()["qtui:property-type"] != property_widget_type)
					continue;

				if(property_widget* const widget = plugin::create<property_widget>(**factory))
				{
					if(result = dynamic_cast<QWidget*>(widget))
					{
						widget->initialize(Document, Property);
					}
					else
					{
						k3d::log() << error << "property widget must derive from QWidget" << std::endl;
					}
				}
				else
				{
					k3d::log() << error << "error creating property widget" << std::endl;
				}

				break;
			}
		}
	}

/*
	// Otherwise, provide our own hard-wired controls based on property type ...
	if(!result)
	{
		// Boolean properties ...
		if(property_type == typeid(bool_t))
		{
			control = new check_button::control(check_button::proxy(property, state_recorder, property_name));
		}
		// Scalar properties ...
		else if(property_type == typeid(int32_t) || property_type == typeid(uint32_t) || property_type == typeid(float_t) || property_type == typeid(double_t))
		{
			control = new spin_button::control(spin_button::model(property), state_recorder);
		}
		// Color properties ...
		else if(property_type == typeid(color))
		{
			control = new color_chooser::control(color_chooser::proxy(property, state_recorder, property_name));
		}
		// String properties ...
		else if(property_type == typeid(string_t))
		{
			if(dynamic_cast<ienumeration_property*>(&property))
			{
				control = new enumeration_chooser::control(enumeration_chooser::model(property), state_recorder);
			}
			else if(dynamic_cast<iscript_property*>(&property))
			{
				control = new script_button::control(script_button::model(property), state_recorder, property_name);
			}
			else if(ilist_property<string_t>* const list_property = dynamic_cast<ilist_property<string_t>*>(&property))
			{
				combo_box::control* const combo_box = new combo_box::control(combo_box::proxy(property, state_recorder, property_name));
				combo_box->set_values(list_property->property_values());
				control = combo_box;
			}
			else
			{
				if(imetadata* const metadata = dynamic_cast<imetadata*>(&property))
				{
					imetadata::metadata_t property_metadata = metadata->get_metadata();
					if(property_metadata["k3d:property-type"] == "k3d:multi-line-text")
					{
						control = new text::control(text::model(property), state_recorder);
					}
				}

				if(!control)
				{
					control = new entry::control(entry::model(property), state_recorder);
				}
			}
		}
		// inode* properties ...
		else if(property_type == typeid(inode*))
		{
			control = new node_chooser::control(node_chooser::proxy(m_document_state, property, state_recorder, property_name), node_chooser::filter(property));
		}
		else if(property_type == typeid(inode_collection_property::nodes_t))
		{
			control = new node_collection_chooser::control(node_collection_chooser::model(property), state_recorder);
		}
		// Bitmap properties ...
		else if(property_type == type_id_k3d_bitmap_ptr())
		{
			control = new bitmap_preview::control(bitmap_preview::proxy(property));
		}
		// Filesystem-path properties ...
		else if(property_type == typeid(filesystem::path))
		{
			control = new path_chooser::control(path_chooser::proxy(property, state_recorder, property_name));
		}
		// bounding_box3 properties ...
		else if(property_type == typeid(bounding_box3))
		{
			control = new bounding_box::control(bounding_box::proxy(property));
		}
		// point3 properties ...
		else if(property_type == typeid(point3) || property_type == typeid(vector3) || property_type == typeid(normal3))
		{
			control = new point::control(point::proxy(property));
		}
		// angle_axis properties ...
		else if(property_type == typeid(k3d::angle_axis))
		{
			control = new k3d::ngui::angle_axis::control(k3d::ngui::angle_axis::proxy(property, state_recorder, property_name));
		}
		// Transformation properties ...
		else if(property_type == typeid(matrix4))
		{
		}
		// Mesh properties ...
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
*/
	if(!result)
	{
		log() << warning << "Couldn't create widget for property [" << property_name << "] with type [" << k3d::type_string(property_type) << "]" << std::endl;
	}

	return result;
}

} // namespace qtui

} // namespace k3d

