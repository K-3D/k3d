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

#include <k3d-i18n-config.h>
#include <k3dsdk/application.h>
#include <k3dsdk/application_plugin_factory.h>
#include <k3dsdk/iapplication.h>
#include <k3dsdk/inode.h>
#include <k3dsdk/iplugin_factory_collection.h>
#include <k3dsdk/iproperty_collection.h>
#include <k3dsdk/module.h>
#include <k3dsdk/ngui/asynchronous_update.h>
#include <k3dsdk/ngui/auto_property_page.h>
#include <k3dsdk/ngui/auto_property_toolbar.h>
#include <k3dsdk/ngui/button.h>
#include <k3dsdk/ngui/custom_property_page.h>
#include <k3dsdk/ngui/document_state.h>
#include <k3dsdk/ngui/panel.h>
#include <k3dsdk/ngui/uri.h>
#include <k3dsdk/ngui/widget_manip.h>
#include <k3dsdk/plugins.h>

#include <gtkmm/box.h>
#include <gtkmm/label.h>
#include <gtkmm/scrolledwindow.h>
#include <gtkmm/stock.h>

#include <boost/assign/list_of.hpp>
#include <boost/scoped_ptr.hpp>

using namespace libk3dngui;

namespace module
{

namespace ngui
{

namespace node_properties
{

namespace detail
{

/////////////////////////////////////////////////////////////////////////////
// implementation

class implementation :
	public asynchronous_update
{
public:
	implementation(document_state& DocumentState, k3d::icommand_node& Parent) :
		m_document_state(DocumentState),
		m_parent(Parent),
		m_auto_toolbar(DocumentState, Parent, "toolbar"),
		m_auto_properties(DocumentState, Parent)
	{
		// Setup the panel label (always visible) ...
		m_label.set_alignment(Gtk::ALIGN_LEFT);
		m_label.set_padding(5, 5);

		// Setup the auto-generated page (usually visible, unless a custom page is available) ...
		Gtk::ScrolledWindow* const scrolled_window = new Gtk::ScrolledWindow();
		scrolled_window->set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
		scrolled_window->add(m_auto_properties.get_widget());
		scrolled_window->signal_button_press_event().connect(sigc::bind_return(sigc::hide(m_panel_grab_signal.make_slot()), false), false);

		m_auto_page.pack_start(m_auto_toolbar.get_widget(), Gtk::PACK_SHRINK);
		m_auto_page.pack_start(*Gtk::manage(scrolled_window), Gtk::PACK_EXPAND_WIDGET);

		// Setup the main top-level widget ...
		m_main_widget.pack_start(m_label, Gtk::PACK_SHRINK);
		m_main_widget.pack_start(m_auto_page, Gtk::PACK_EXPAND_WIDGET);

		m_document_state.view_node_properties_signal().connect(sigc::mem_fun(*this, &implementation::on_view_node_properties));
		m_document_state.document().close_signal().connect(sigc::mem_fun(*this, &implementation::on_document_closed));

		// Initial update ...
		m_nodes = m_document_state.selected_nodes();
		if(m_nodes.size() > 1)
			m_nodes.resize(1);
		update_connections();
		schedule_update();
	}

	bool on_view_node_properties(k3d::inode* const Node)
	{
		m_nodes = k3d::nodes_t(1, Node);
		update_connections();
		
		schedule_update();

		return false;
	}

	void on_document_closed()
	{
		block_updates();
	}

	void update_connections()
	{
		std::for_each(m_node_connections.begin(), m_node_connections.end(), std::mem_fun_ref(&sigc::connection::disconnect));
		m_node_connections.clear();

		for(k3d::nodes_t::const_iterator node = m_nodes.begin(); node != m_nodes.end(); ++node)
		{
			m_node_connections.push_back((**node).deleted_signal().connect(sigc::bind(sigc::mem_fun(*this, &implementation::on_node_deleted), *node)));
			m_node_connections.push_back((**node).name_changed_signal().connect(sigc::mem_fun(*this, &implementation::update_label)));
			if(k3d::iproperty_collection* const property_collection = dynamic_cast<k3d::iproperty_collection*>(*node))
				m_node_connections.push_back(property_collection->connect_properties_changed_signal(sigc::hide(sigc::mem_fun(*this, &implementation::on_node_properties_changed))));
		}
	}

	void on_node_properties_changed()
	{
		schedule_update();
	}

	void on_node_deleted(k3d::inode* Node)
	{
		m_nodes.erase(std::remove(m_nodes.begin(), m_nodes.end(), Node), m_nodes.end());
		update_connections();

		schedule_update();
	}

	void update_label()
	{
		switch(m_nodes.size())
		{
		case 0:
			m_label.set_text("");
			break;
		case 1:
			m_label.set_text(m_nodes[0]->name());
			break;
		default:
			m_label.set_text(_("Multiple Nodes"));
			break;
		}
	}

	void on_update()
	{
		// Cache the set of available custom property page plugins ...
		static std::map<k3d::string_t, k3d::iplugin_factory*> custom_pages;
		if(custom_pages.empty())
		{
			const k3d::iplugin_factory_collection::factories_t& factories = k3d::application().plugins();
			for(k3d::iplugin_factory_collection::factories_t::const_iterator factory = factories.begin(); factory != factories.end(); ++factory)
			{
				k3d::iplugin_factory::metadata_t metadata = (**factory).metadata();

				if(metadata["ngui:component-type"] != "property-page")
					continue;

				const k3d::string_t plugin_type = metadata["ngui:plugin-type"];
				if(plugin_type.empty())
				{
					k3d::log() << error << "Property page plugin without ngui:plugin-type metadata will be ignored" << std::endl;
					continue;
				}

				custom_pages[plugin_type] = *factory;
			}
		}

		update_label();

		m_main_widget.hide();

		if(m_nodes.size() == 1 && custom_pages.count(m_nodes[0]->factory().name()))
		{
			m_custom_page.reset(k3d::plugin::create<k3d::ngui::custom_property_page::control>(*custom_pages[m_nodes[0]->factory().name()]));
			if(m_custom_page)
			{
				m_auto_page.hide();
				m_main_widget.pack_start(m_custom_page->get_widget(m_document_state, m_parent, *m_nodes[0]), Gtk::PACK_EXPAND_WIDGET);
				m_main_widget.show();
				return;
			}
		}

		m_custom_page.reset();

		m_auto_toolbar.set_object(m_nodes.size() == 1 ? m_nodes[0] : 0);
		m_auto_properties.set_properties(m_nodes.begin(), m_nodes.end());
		m_auto_page.show();

		m_main_widget.show();

/*
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
*/
	}

/*
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
*/

	/// Stores a reference to the owning document
	document_state& m_document_state;
	k3d::icommand_node& m_parent;
	/// Stores the current set of nodes to be displayed (if any)
	k3d::nodes_t m_nodes;
	/// Stores the current set of connections to node signals (if any)
	std::vector<sigc::connection> m_node_connections;

	/// Contains the other widgets
	Gtk::VBox m_main_widget;
	/// Displays the current node name
	Gtk::Label m_label;

	/// Contains the standard auto-generated UI
	Gtk::VBox m_auto_page;
	/// Provides the standard auto-generated toolbar
	k3d::ngui::auto_property_toolbar::control m_auto_toolbar;
	/// Provides the standard auto-generated property controls
	k3d::ngui::auto_property_page::control m_auto_properties;

	/// Stores the (optional) custom UI
	boost::scoped_ptr<k3d::ngui::custom_property_page::control> m_custom_page;

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

		pack_start(m_implementation->m_main_widget, Gtk::PACK_EXPAND_WIDGET);
		show_all();
	}

	const k3d::string_t panel_type()
	{
		return get_factory().name();
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
			boost::assign::map_list_of("ngui:component-type", "panel")("ngui:panel-label", "Node Properties"));

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

