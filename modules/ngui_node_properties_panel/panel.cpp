// K-3D
// Copyright (c) 1995-2007, Timothy M. Shead
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

#include <k3d-i18n-config.h>
#include <k3dsdk/ngui/angle_axis_control.h>
#include <k3dsdk/ngui/asynchronous_update.h>
#include <k3dsdk/ngui/bitmap_preview.h>
#include <k3dsdk/ngui/bounding_box.h>
#include <k3dsdk/ngui/button.h>
#include <k3dsdk/ngui/check_button.h>
#include <k3dsdk/ngui/collapsible_frame.h>
#include <k3dsdk/ngui/color_chooser.h>
#include <k3dsdk/ngui/combo_box.h>
#include <k3dsdk/ngui/document_state.h>
#include <k3dsdk/ngui/entry.h>
#include <k3dsdk/ngui/enumeration_chooser.h>
#include <k3dsdk/ngui/file_chooser_dialog.h>
#include <k3dsdk/ngui/icons.h>
#include <k3dsdk/ngui/messages.h>
#include <k3dsdk/ngui/node_chooser.h>
#include <k3dsdk/ngui/node_collection_chooser.h>
#include <k3dsdk/ngui/panel.h>
#include <k3dsdk/ngui/path_chooser.h>
#include <k3dsdk/ngui/point_control.h>
#include <k3dsdk/ngui/properties.h>
#include <k3dsdk/ngui/property_button.h>
#include <k3dsdk/ngui/property_label.h>
#include <k3dsdk/ngui/render.h>
#include <k3dsdk/ngui/scale_control.h>
#include <k3dsdk/ngui/script_button.h>
#include <k3dsdk/ngui/selection_button.h>
#include <k3dsdk/ngui/spin_button.h>
#include <k3dsdk/ngui/toggle_button.h>
#include <k3dsdk/ngui/toolbar.h>
#include <k3dsdk/ngui/ui_component.h>
#include <k3dsdk/ngui/uri.h>
#include <k3dsdk/ngui/utility.h>
#include <k3dsdk/ngui/widget_manip.h>

#include <k3dsdk/application_plugin_factory.h>
#include <k3dsdk/irender_animation.h>
#include <k3dsdk/icamera.h>
#include <k3dsdk/idocument.h>
#include <k3dsdk/ienumeration_property.h>
#include <k3dsdk/ikeyframer.h>
#include <k3dsdk/ilist_property.h>
#include <k3dsdk/imeasurement_property.h>
#include <k3dsdk/imesh_sink.h>
#include <k3dsdk/imesh_source.h>
#include <k3dsdk/imesh_storage.h>
#include <k3dsdk/inode.h>
#include <k3dsdk/inode_collection_property.h>
#include <k3dsdk/iplugin_factory.h>
#include <k3dsdk/iproperty_group_collection.h>
#include <k3dsdk/irender_camera_animation.h>
#include <k3dsdk/irender_camera_frame.h>
#include <k3dsdk/irender_camera_preview.h>
#include <k3dsdk/irender_frame.h>
#include <k3dsdk/irender_preview.h>
#include <k3dsdk/iscript_property.h>
#include <k3dsdk/iselectable.h>
#include <k3dsdk/iuser_property.h>
#include <k3dsdk/mesh.h>
#include <k3dsdk/mesh_selection.h>
#include <k3dsdk/module.h>
#include <k3dsdk/options.h>
#include <k3dsdk/state_change_set.h>
#include <k3dsdk/string_cast.h>
#include <k3dsdk/string_modifiers.h>
#include <k3dsdk/system.h>
#include <k3dsdk/time_source.h>
#include <k3dsdk/type_registry.h>
#include <k3dsdk/types_ri.h>
#include <k3dsdk/user_properties.h>
#include <k3dsdk/utility.h>

// Not strictly required to compile, but this #include ensures that we have a std::typeinfo for k3d::legacy::mesh that matches the SDK (i.e. we don't break the ODR)
#include <k3dsdk/legacy_mesh.h>

#include <gtkmm/arrow.h>
#include <gtkmm/box.h>
#include <gtkmm/frame.h>
#include <gtkmm/image.h>
#include <gtkmm/label.h>
#include <gtkmm/scrolledwindow.h>
#include <gtkmm/stock.h>
#include <gtkmm/table.h>

#include <boost/any.hpp>
#include <boost/assign/list_of.hpp>
#include <boost/format.hpp>

// Temporary hack
using namespace libk3dngui;

namespace module
{

namespace ngui
{

namespace node_properties
{

namespace detail
{

class bypass_property_proxy :
	public toggle_button::imodel
{
public:
	bypass_property_proxy(document_state& DocumentState, k3d::iproperty& InputProperty, k3d::iproperty& OutputProperty) :
		m_document_state(DocumentState),
		m_input_property(InputProperty),
		m_output_property(OutputProperty)
	{
	}

	const Glib::ustring label()
	{
		return _("Bypass modifier");
	}

	const k3d::bool_t value()
	{
		// true == bypassed, false == normal
		return m_document_state.document().pipeline().dependency(m_output_property) == &m_input_property;
	}

	void set_value(const k3d::bool_t Value)
	{
		// true == bypassed, false == normal
		k3d::ipipeline::dependencies_t dependencies;
		dependencies.insert(std::make_pair(&m_output_property, Value ? &m_input_property : static_cast<k3d::iproperty*>(0)));
		m_document_state.document().pipeline().set_dependencies(dependencies);

		m_changed_signal.emit();
	}

	sigc::connection connect_changed_signal(const sigc::slot<void>& Slot)
	{
		return m_changed_signal.connect(Slot);
	}

private:
	bypass_property_proxy(const bypass_property_proxy&);
	bypass_property_proxy& operator=(const bypass_property_proxy&);

	document_state& m_document_state;
	k3d::iproperty& m_input_property;
	k3d::iproperty& m_output_property;

	sigc::signal<void> m_changed_signal;
};

/////////////////////////////////////////////////////////////////////////////
// implementation

class implementation :
	public asynchronous_update
{
public:
	implementation(document_state& DocumentState, k3d::icommand_node& Parent) :
		m_document_state(DocumentState),
		m_node(0),
		m_parent(Parent),
		m_help_button(m_parent, "onlin_help", Gtk::Stock::HELP)
	{
		m_label.set_alignment(Gtk::ALIGN_LEFT);
		m_label.set_padding(5, 5);

		&m_help_button << connect_button(sigc::mem_fun(*this, &implementation::on_online_help));

		m_scrolled_window.set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
		m_scrolled_window.add(m_vbox);

		// If only one node is selected, show its properties
		const k3d::nodes_t nodes = m_document_state.selected_nodes();
		if(1 == nodes.size())
			on_view_node_properties(nodes.front());

		m_document_state.document().close_signal().connect(sigc::mem_fun(*this, &implementation::on_document_closed));
		m_document_state.view_node_properties_signal().connect(sigc::mem_fun(*this, &implementation::on_view_node_properties));

		schedule_update();
	}

	void on_document_closed()
	{
		block_updates();
	}

	bool on_view_node_properties(k3d::inode* const Node)
	{
		if(Node != m_node)
		{
			m_node = Node;

			m_node_deleted_connection.disconnect();
			m_node_name_change_connection.disconnect();
			m_node_properties_changed_connection.disconnect();

			schedule_update();

			if(m_node)
			{
				m_node_deleted_connection = m_node->deleted_signal().connect(sigc::mem_fun(*this, &implementation::on_node_deleted));
				m_node_name_change_connection = m_node->name_changed_signal().connect(sigc::mem_fun(*this, &implementation::update_label));
				k3d::iproperty_collection* const property_collection = dynamic_cast<k3d::iproperty_collection*>(m_node);
				if(property_collection)
					m_node_properties_changed_connection = property_collection->connect_properties_changed_signal(sigc::hide(sigc::mem_fun(*this, &implementation::on_node_properties_changed)));

			}

			return true;
		}

		return false;
	}

	void on_node_properties_changed()
	{
		m_vbox.hide();
		schedule_update();
	}

	void on_node_deleted()
	{
		on_view_node_properties(0);
	}

	void update_label()
	{
		if(m_node)
		{
			m_label.set_text(m_node->name());
			m_help_button.set_sensitive(true);
		}
		else
		{
			m_label.set_text("");
			m_help_button.set_sensitive(false);
		}
	}

	void reset()
	{
		Glib::ListHandle<Gtk::Widget*> children = m_vbox.get_children();
		std::for_each(children.begin(), children.end(), k3d::delete_object());
	}

	void on_update()
	{
		update_label();

		reset();

		// Create a toolbar ...
		toolbar::control* const toolbar_control = new toolbar::control(m_parent, "toolbar");
		m_vbox.pack_start(*manage(toolbar_control), Gtk::PACK_SHRINK);

		k3d::istate_recorder* const state_recorder = &m_document_state.document().state_recorder();

		// Add controls for cameras and camera render engines ...
		if(dynamic_cast<k3d::icamera*>(m_node) || dynamic_cast<k3d::irender_camera_preview*>(m_node))
		{
			button::control* const control =
				new button::control(m_parent, "render_camera_preview", *Gtk::manage(new Gtk::Image(load_icon("render_preview", Gtk::ICON_SIZE_BUTTON))))
					<< connect_button(sigc::mem_fun(*this, &implementation::on_render_camera_preview))
					<< set_tooltip(_("Render Preview"));

			toolbar_control->row(0).pack_start(*Gtk::manage(control), Gtk::PACK_SHRINK);
		}

		if(dynamic_cast<k3d::icamera*>(m_node) || dynamic_cast<k3d::irender_camera_frame*>(m_node))
		{
			button::control* const control =
				new button::control(m_parent, "render_camera_frame", *Gtk::manage(new Gtk::Image(load_icon("render_frame", Gtk::ICON_SIZE_BUTTON))))
					<< connect_button(sigc::mem_fun(*this, &implementation::on_render_camera_frame))
					<< set_tooltip(_("Render Frame"));

			toolbar_control->row(0).pack_start(*Gtk::manage(control), Gtk::PACK_SHRINK);
		}

		if(dynamic_cast<k3d::icamera*>(m_node) || dynamic_cast<k3d::irender_camera_animation*>(m_node))
		{
			button::control* const control =
				new button::control(m_parent, "render_camera_animation", *Gtk::manage(new Gtk::Image(load_icon("render_animation", Gtk::ICON_SIZE_BUTTON))))
					<< connect_button(sigc::mem_fun(*this, &implementation::on_render_camera_animation))
					<< set_tooltip(_("Render Animation"));

			toolbar_control->row(0).pack_start(*Gtk::manage(control), Gtk::PACK_SHRINK);
		}

		// Add controls for render engines
		if(dynamic_cast<k3d::irender_preview*>(m_node))
		{
			button::control* const control =
				new button::control(m_parent, "render_preview", *Gtk::manage(new Gtk::Image(load_icon("render_preview", Gtk::ICON_SIZE_BUTTON))))
					<< connect_button(sigc::mem_fun(*this, &implementation::on_render_preview))
					<< set_tooltip(_("Render Preview"));

			toolbar_control->row(0).pack_start(*Gtk::manage(control), Gtk::PACK_SHRINK);
		}

		if(dynamic_cast<k3d::irender_frame*>(m_node))
		{
			button::control* const control =
				new button::control(m_parent, "render_frame", *Gtk::manage(new Gtk::Image(load_icon("render_frame", Gtk::ICON_SIZE_BUTTON))))
					<< connect_button(sigc::mem_fun(*this, &implementation::on_render_frame))
					<< set_tooltip(_("Render Frame"));

			toolbar_control->row(0).pack_start(*Gtk::manage(control), Gtk::PACK_SHRINK);
		}

		if(dynamic_cast<k3d::irender_animation*>(m_node))
		{
			button::control* const control =
				new button::control(m_parent, "render_animation", *Gtk::manage(new Gtk::Image(load_icon("render_animation", Gtk::ICON_SIZE_BUTTON))))
					<< connect_button(sigc::mem_fun(*this, &implementation::on_render_animation))
					<< set_tooltip(_("Render Animation"));

			toolbar_control->row(0).pack_start(*Gtk::manage(control), Gtk::PACK_SHRINK);
		}

		// Add a "reset" button for nodes that implement k3d::imesh_storage (FrozenMesh, external file readers, etc) ...
		if(dynamic_cast<k3d::imesh_storage*>(m_node))
		{
			button::control* const control =
				new button::control(m_parent, "reset_mesh", _("Reset Mesh"))
					<< connect_button(sigc::mem_fun(*this, &implementation::on_reset_mesh))
					<< set_tooltip(_("Reset / Reload Mesh"));

			toolbar_control->row(0).pack_start(*Gtk::manage(control), Gtk::PACK_SHRINK);
		}

		// Add a "disable" button for mesh modifiers ...
		if(k3d::imesh_source* const mesh_source = dynamic_cast<k3d::imesh_source*>(m_node))
		{
			if(k3d::imesh_sink* const mesh_sink = dynamic_cast<k3d::imesh_sink*>(m_node))
			{
				toggle_button::control* const control =
					new toggle_button::control(
						m_parent,
						"disable_mesh_modifier",
						new detail::bypass_property_proxy(m_document_state, mesh_sink->mesh_sink_input(), mesh_source->mesh_source_output()),
						state_recorder,
						_("Disable"))
						<< set_tooltip(_("Disable / bypass mesh modifier"));

				toolbar_control->row(0).pack_start(*Gtk::manage(control), Gtk::PACK_SHRINK);
			}
		}

		// Get the node properties, grouped together ...
		k3d::iproperty_collection* const property_collection = dynamic_cast<k3d::iproperty_collection*>(m_node);
		k3d::iproperty_group_collection::groups_t property_groups;
		if(property_collection)
		{
			k3d::iproperty_collection::properties_t all_properties = property_collection->properties();
			k3d::iproperty_group_collection::groups_t groups;

			k3d::iproperty_group_collection* const property_group_collection = dynamic_cast<k3d::iproperty_group_collection*>(m_node);
			if(property_group_collection)
			{
				groups = property_group_collection->property_groups();
				for(k3d::iproperty_group_collection::groups_t::const_iterator group = groups.begin(); group != groups.end(); ++group)
				{
					for(k3d::iproperty_collection::properties_t::const_iterator property = group->properties.begin(); property != group->properties.end(); ++property)
						all_properties.erase(std::remove(all_properties.begin(), all_properties.end(), *property), all_properties.end());
				}
			}

			property_groups.insert(property_groups.end(), k3d::iproperty_group_collection::group(m_node->factory().name(), all_properties));
			property_groups.insert(property_groups.end(), groups.begin(), groups.end());
		}
		
		// Used to determine if we need to add ikeyframer buttons
		k3d::ikeyframer* keyframer = dynamic_cast<k3d::ikeyframer*>(m_node);
		k3d::iproperty* last_time_property;
		
		// First add a manual keyframe button
		if (keyframer)
		{
			button::control* const control =
						new button::control(m_parent, "set_keyframe_button", "Set Keyframe")
						<< connect_button(sigc::bind(sigc::mem_fun(*this, &implementation::on_manual_keyframe), keyframer))
						<< set_tooltip("Manually set keyframe");
						
			toolbar_control->row(0).pack_start(*Gtk::manage(control), Gtk::PACK_SHRINK);
		}

		// For each property group ...
		for(k3d::iproperty_group_collection::groups_t::const_iterator property_group = property_groups.begin(); property_group != property_groups.end(); ++property_group)
		{
			if(property_group->properties.empty())
				continue;

			collapsible_frame::control* const frame = new collapsible_frame::control(property_group->name, m_collapsible_frame_group);
			m_vbox.pack_start(*manage(frame), Gtk::PACK_SHRINK);

			Gtk::Table* const table = new Gtk::Table(property_group->properties.size(), 5, false);
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
				if(property_type == typeid(k3d::bool_t))
				{
					check_button::control* const control = new check_button::control(m_parent, property_name, check_button::proxy(property, state_recorder, property_name));
					table->attach(*manage(control), prop_control_begin, prop_control_end, row, row + 1, Gtk::FILL | Gtk::SHRINK, Gtk::FILL | Gtk::SHRINK);

					entry_list.push_back(control);
				}
				// Scalar properties ...
				else if(property_type == typeid(k3d::int32_t) || property_type == typeid(k3d::uint32_t) || property_type == typeid(k3d::float_t) || property_type == typeid(k3d::double_t))
				{
					spin_button::control* const control = new spin_button::control(m_parent, property_name, spin_button::model(property), state_recorder);
					table->attach(*manage(control), prop_control_begin, prop_control_end, row, row + 1, Gtk::FILL | Gtk::SHRINK, Gtk::FILL | Gtk::SHRINK);
					entry_list.push_back(control);
				}
				// Color properties ...
				else if(property_type == typeid(k3d::color))
				{
					color_chooser::control* const control = new color_chooser::control(m_parent, property_name, color_chooser::proxy(property, state_recorder, property_name));
					table->attach(*manage(control), prop_control_begin, prop_control_end, row, row + 1, Gtk::FILL | Gtk::SHRINK, Gtk::FILL | Gtk::SHRINK);

					entry_list.push_back(control);
				}
				// String properties ...
				else if(property_type == typeid(std::string))
				{
					if(dynamic_cast<k3d::ienumeration_property*>(&property))
					{
						enumeration_chooser::control* const control = new enumeration_chooser::control(m_parent, property_name, enumeration_chooser::model(property), state_recorder);
						table->attach(*manage(control), prop_control_begin, prop_control_end, row, row + 1, Gtk::FILL | Gtk::SHRINK, Gtk::FILL | Gtk::SHRINK);

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
				// k3d::inode* properties ...
				else if(property_type == typeid(k3d::inode*))
				{
					node_chooser::control* const control = new node_chooser::control(m_parent, property_name, node_chooser::proxy(m_document_state, property, state_recorder, property_name), node_chooser::filter(property));
					table->attach(*manage(control), prop_control_begin, prop_control_end, row, row + 1, Gtk::FILL | Gtk::SHRINK, Gtk::FILL | Gtk::SHRINK);

					entry_list.push_back(control);
				}
				else if(property_type == typeid(k3d::inode_collection_property::nodes_t))
				{
					node_collection_chooser::control* const control = new node_collection_chooser::control(m_parent, property_name, node_collection_chooser::model(property), state_recorder);
					table->attach(*manage(control), prop_control_begin, prop_control_end, row, row + 1, Gtk::FILL | Gtk::SHRINK, Gtk::FILL | Gtk::SHRINK);
					entry_list.push_back(control);
				}
				// Bitmap properties ...
				else if(property_type == k3d::type_id_k3d_bitmap_ptr())
				{
					bitmap_preview::control* const control = new bitmap_preview::control(m_parent, property_name, bitmap_preview::proxy(property));
					table->attach(*manage(control), prop_control_begin, prop_control_end, row, row + 1, Gtk::FILL | Gtk::SHRINK, Gtk::FILL | Gtk::SHRINK);

					entry_list.push_back(control);
				}
				// Filesystem-path properties ...
				else if(property_type == typeid(k3d::filesystem::path))
				{
					path_chooser::control* const control = new path_chooser::control(m_parent, property_name, path_chooser::proxy(property, state_recorder, property_name));
					table->attach(*manage(control), prop_control_begin, prop_control_end, row, row + 1, Gtk::FILL | Gtk::SHRINK, Gtk::FILL | Gtk::SHRINK);

					entry_list.push_back(control);
				}
				// k3d::bounding_box3 properties ...
				else if(property_type == typeid(k3d::bounding_box3))
				{
					bounding_box::control* const control = new bounding_box::control(m_parent, property_name, bounding_box::proxy(property));
					table->attach(*manage(control), prop_control_begin, prop_control_end, row, row + 1, Gtk::FILL | Gtk::SHRINK, Gtk::FILL | Gtk::SHRINK);

					entry_list.push_back(control);
				}
				// k3d::point3 properties ...
				else if(property_type == typeid(k3d::point3) || property_type == typeid(k3d::vector3) || property_type == typeid(k3d::normal3))
				{
					point::control* const control = new point::control(m_parent, property_name, point::proxy(property));
					table->attach(*manage(control), prop_control_begin, prop_control_end, row, row + 1, Gtk::FILL | Gtk::SHRINK, Gtk::FILL | Gtk::SHRINK);

					entry_list.push_back(control);
				}
				// k3d::angle_axis properties ...
				else if(property_type == typeid(k3d::angle_axis))
				{
					angle_axis::control* const control = new angle_axis::control(m_parent, property_name, angle_axis::proxy(property, state_recorder, property_name));
					table->attach(*manage(control), prop_control_begin, prop_control_end, row, row + 1, Gtk::FILL | Gtk::SHRINK, Gtk::FILL | Gtk::SHRINK);

					entry_list.push_back(control);
				}
				// Transformation properties ...
				else if(property_type == typeid(k3d::matrix4))
				{
				}
				// Mesh properties ...
				else if(property_type == typeid(k3d::legacy::mesh*))
				{
				}
				else if(property_type == typeid(k3d::mesh*))
				{
				}
				// HPoint properties ...
				else if(property_type == typeid(k3d::point4))
				{
				}
				// Mesh Selection properties ...
				else if(property_type == typeid(k3d::mesh_selection))
				{
					selection_button::control* const control = new selection_button::control(m_parent, property_name, selection_button::proxy(property, state_recorder, property_name));
					table->attach(*manage(control), prop_control_begin, prop_control_end, row, row + 1, Gtk::FILL | Gtk::SHRINK, Gtk::FILL | Gtk::SHRINK);
				}
				else
				{
					k3d::log() << warning << k3d_file_reference << "unknown property type: " << property_type.name() << " name: " << property_name << std::endl;
				}

				// Provide a "delete" button for user properties ...
				if(dynamic_cast<k3d::iuser_property*>(&property))
				{
					button::control* const control =
						new button::control(m_parent, property_name + "_delete", *Gtk::manage(new Gtk::Image(Gtk::Stock::DELETE, Gtk::ICON_SIZE_BUTTON)))
						<< connect_button(sigc::bind(sigc::bind(sigc::mem_fun(*this, &implementation::on_delete_user_property), &property), property_collection))
						<< set_tooltip(_("Delete user property (no undo)"));

					table->attach(*manage(control), prop_delete_begin, prop_delete_end, row, row + 1, Gtk::SHRINK, Gtk::SHRINK);
				}
				
				// If we have a keyframer, add a delete button to each keyframe group
				if (keyframer && property_name.find("key_time_", 0) != std::string::npos)
				{
					last_time_property = &property;
				}
				if (keyframer && property_name.find("key_value_", 0) != std::string::npos)
				{
					if (!last_time_property)
					{
						k3d::log() << warning << "No time property registered for " << property_name << std::endl;
					}
					else
					{
						++row;
						std::string keynumber = property_name.substr(10, property_name.size() - 9);
						button::control* const control =
						new button::control(m_parent, "delete_key_" + keynumber, "Delete Key " + keynumber)
							<< connect_button(sigc::bind(sigc::bind(sigc::mem_fun(*this, &implementation::on_key_delete), last_time_property), keyframer))
							<< set_tooltip("Delete Key " + keynumber);
							
						table->attach(*manage(control), prop_label_begin, prop_label_end, row, row + 1, Gtk::SHRINK, Gtk::SHRINK);
						
						button::control* const zoomcontrol =
						new button::control(m_parent, "zoom_key_" + keynumber, "Zoom to Key " + keynumber)
							<< connect_button(sigc::bind(sigc::bind(sigc::mem_fun(*this, &implementation::on_key_zoom), last_time_property), keyframer))
							<< set_tooltip("Sets the time to the time associated with " + keynumber);
							
						table->attach(*manage(zoomcontrol), prop_control_begin, prop_control_end, row, row + 1, Gtk::SHRINK, Gtk::SHRINK);
						
						last_time_property = 0;
					}
				}
			}

			// Add controls for managing user properties ...
			if(property_collection)
			{
				button::control* const control =
					new button::control(m_parent, "add_user_property", *Gtk::manage(new Gtk::Image(Gtk::Stock::ADD, Gtk::ICON_SIZE_BUTTON)))
						<< connect_button(sigc::mem_fun(*this, &implementation::on_add_user_property))
						<< set_tooltip(_("Add a user property to this node"));

				table->attach(*Gtk::manage(control), prop_delete_begin, prop_delete_end, row, row + 1, Gtk::SHRINK, Gtk::SHRINK);
			}

			// Set focus chain
			table->set_focus_chain(entry_list);
		}

		m_vbox.show_all();
	}

	void on_online_help()
	{
		if(m_node)
		{
			k3d::ngui::uri::open("http://www.k-3d.org/wiki/" + m_node->factory().name());
		}
	}

	void on_render_camera_preview()
	{
		k3d::icamera* camera = dynamic_cast<k3d::icamera*>(m_node);
		if(!camera)
			camera = pick_camera(m_document_state);
		if(!camera)
			return;

		k3d::irender_camera_preview* render_engine = dynamic_cast<k3d::irender_camera_preview*>(m_node);
		if(!render_engine)
			render_engine = pick_camera_preview_render_engine(m_document_state);
		if(!render_engine)
			return;

		render(*camera, *render_engine);
	}

	void on_render_camera_frame()
	{
		k3d::icamera* camera = dynamic_cast<k3d::icamera*>(m_node);
		if(!camera)
			camera = pick_camera(m_document_state);
		if(!camera)
			return;

		k3d::irender_camera_frame* render_engine = dynamic_cast<k3d::irender_camera_frame*>(m_node);
		if(!render_engine)
			render_engine = pick_camera_still_render_engine(m_document_state);
		if(!render_engine)
			return;

		render(*camera, *render_engine);
	}

	void on_render_camera_animation()
	{
		k3d::icamera* camera = dynamic_cast<k3d::icamera*>(m_node);
		if(!camera)
			camera = pick_camera(m_document_state);
		if(!camera)
			return;

		k3d::irender_camera_animation* render_engine = dynamic_cast<k3d::irender_camera_animation*>(m_node);
		if(!render_engine)
			render_engine = pick_camera_animation_render_engine(m_document_state);
		if(!render_engine)
			return;

		render(m_document_state, *camera, *render_engine);
	}

	void on_render_preview()
	{
		k3d::irender_preview* render_engine = dynamic_cast<k3d::irender_preview*>(m_node);
		return_if_fail(render_engine);

		render(*render_engine);
	}

	void on_render_frame()
	{
		k3d::irender_frame* render_engine = dynamic_cast<k3d::irender_frame*>(m_node);
		return_if_fail(render_engine);

		render(*render_engine);
	}

	void on_render_animation()
	{
		k3d::irender_animation* render_engine = dynamic_cast<k3d::irender_animation*>(m_node);
		return_if_fail(render_engine);

		render(m_document_state, *render_engine);
	}

	void on_reset_mesh()
	{
		k3d::imesh_storage* const mesh_storage = dynamic_cast<k3d::imesh_storage*>(m_node);
		return_if_fail(mesh_storage);

		mesh_storage->reset_mesh(0);
	}

	void on_delete_user_property(k3d::iproperty_collection* Collection, k3d::iproperty* Property)
	{
		return_if_fail(Collection);
		return_if_fail(Property);
		return_if_fail(dynamic_cast<k3d::iuser_property*>(Property));

		k3d::record_state_change_set change_set(m_document_state.document(), "Delete user property", K3D_CHANGE_SET_CONTEXT);

		if(m_document_state.document().state_recorder().current_change_set())
			m_document_state.document().state_recorder().current_change_set()->record_old_state(new k3d::user::property_container(*Collection));

		Collection->unregister_property(*Property);
		if(k3d::ipersistent* const persistent = dynamic_cast<k3d::ipersistent*>(Property))
		{
			if(k3d::ipersistent_container* const persistent_container = dynamic_cast<k3d::ipersistent_container*>(Collection))
				persistent_container->disable_serialization(*persistent);
		}

		undoable_delete(Property, m_document_state.document());

		if(m_document_state.document().state_recorder().current_change_set())
			m_document_state.document().state_recorder().current_change_set()->record_new_state(new k3d::user::property_container(*Collection));
	}

	void on_add_user_property()
	{
		return_if_fail(m_node);
		k3d::ngui::property::create(*m_node, m_parent);
	}
	
	void on_manual_keyframe(k3d::ikeyframer* Keyframer)
	{
		Keyframer->keyframe();
	}
	
	void on_key_delete(k3d::ikeyframer* Keyframer, k3d::iproperty* TimeProperty)
	{
		Keyframer->delete_key(TimeProperty);
	}
	
	void on_key_zoom(k3d::ikeyframer* Keyframer, k3d::iproperty* TimeProperty)
	{
		k3d::iwritable_property* timeprop = dynamic_cast<k3d::iwritable_property*>(k3d::get_time(m_document_state.document()));
		if (timeprop)
			timeprop->property_set_value(TimeProperty->property_internal_value());
	}

	/// Stores a reference to the owning document
	document_state& m_document_state;
	/// Stores a reference to the currently-selected node (if any)
	k3d::inode* m_node;
	k3d::icommand_node& m_parent;
	/// Tracks whether the currently-visible node is deleted
	sigc::connection m_node_deleted_connection;
	/// Keeps track of node name changes
	sigc::connection m_node_name_change_connection;
	/// Keeps track of changes to the set of node properties
	sigc::connection m_node_properties_changed_connection;
	/// Displays the current node name
	Gtk::Label m_label;
	/// Online help button
	button::control m_help_button;
	/// Contains the set of node properties
	Gtk::ScrolledWindow m_scrolled_window;
	/// Parent widget for the rest of the implementation
	Gtk::VBox m_vbox;
	/// Groups collapsible frames together
	collapsible_frame::group m_collapsible_frame_group;

	sigc::signal<void, const std::string&, const std::string&> m_command_signal;

	/// Signal that will be emitted whenever this control should grab the panel focus
	sigc::signal<void> m_panel_grab_signal;
};

} // namespace detail

/////////////////////////////////////////////////////////////////////////////
// panel

class panel :
	public libk3dngui::panel::control,
	public libk3dngui::ui_component,
	public Gtk::VBox
{
	typedef Gtk::VBox base;

public:
	panel() :
		ui_component("node_properties", 0),
		m_implementation(0)
	{
	}

	~panel()
	{
		delete m_implementation;
	}

	void initialize(document_state& DocumentState, k3d::icommand_node& Parent)
	{
		ui_component::set_parent("node_properties", &Parent);

		m_implementation = new detail::implementation(DocumentState, *this);

		m_implementation->m_command_signal.connect(sigc::mem_fun(*this, &panel::record_command));
		m_implementation->m_scrolled_window.signal_button_press_event().connect(sigc::bind_return(sigc::hide(m_implementation->m_panel_grab_signal.make_slot()), false), false);

		Gtk::HBox* const hbox = new Gtk::HBox();
		hbox->pack_start(m_implementation->m_label, Gtk::PACK_EXPAND_WIDGET);
		hbox->pack_start(m_implementation->m_help_button, Gtk::PACK_SHRINK);

		pack_start(*Gtk::manage(hbox), Gtk::PACK_SHRINK);
		pack_start(m_implementation->m_scrolled_window, Gtk::PACK_EXPAND_WIDGET);
		show_all();
	}

	const std::string panel_type()
	{
		return "node_properties";
	}

	sigc::connection connect_focus_signal(const sigc::slot<void>& Slot)
	{
		return m_implementation->m_panel_grab_signal.connect(Slot);
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::application_plugin_factory<panel> factory(
			k3d::uuid(0x159a2e07, 0x4b92028d, 0x9a998884, 0x4cf8bba5),
			"NGUINodePropertiesPanel",
			_("Displays properties for one node"),
			"NGUI Panels",
			k3d::iplugin_factory::EXPERIMENTAL,
			boost::assign::map_list_of("ngui:component-type", "panel")("ngui:panel-type", "node_properties")("ngui:panel-label", "Node Properties"));

		return factory;
	}

private:
	detail::implementation* m_implementation;
};

} // namespace node_properties

} // namespace ngui

} // namespace module

K3D_MODULE_START(Registry)
	Registry.register_factory(module::ngui::node_properties::panel::get_factory());
K3D_MODULE_END

