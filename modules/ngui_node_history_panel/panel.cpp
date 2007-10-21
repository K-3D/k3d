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

#include <gtkmm/box.h>
#include <gtkmm/menu.h>
#include <gtkmm/scrolledwindow.h>
#include <gtkmm/treestore.h>
#include <gtkmm/treeview.h>

#include <k3dsdk/ngui/asynchronous_update.h>
#include <k3dsdk/ngui/command_arguments.h>
#include <k3dsdk/ngui/document_state.h>
#include <k3dsdk/ngui/hotkey_cell_renderer_text.h>
#include <k3dsdk/ngui/icons.h>
#include <k3dsdk/ngui/interactive.h>
#include <k3dsdk/ngui/panel.h>
#include <k3dsdk/ngui/ui_component.h>

#include <k3dsdk/application_plugin_factory.h>
#include <k3dsdk/ipipeline.h>
#include <k3dsdk/ideletable.h>
#include <k3dsdk/inode_collection.h>
#include <k3dsdk/iproperty_collection.h>
#include <k3dsdk/module.h>
#include <k3dsdk/nodes.h>
#include <k3dsdk/state_change_set.h>
#include <k3dsdk/string_cast.h>
#include <k3dsdk/xml.h>

#include <boost/assign/list_of.hpp>
#include <boost/format.hpp>

#include <set>

// Temporary hack
using namespace libk3dngui;

namespace module
{

namespace ngui
{

namespace node_history
{

namespace detail
{

/// This hack makes it easier to implement a context-menu
class tree_view :
	public Gtk::TreeView
{
	typedef Gtk::TreeView base;

	bool on_button_press_event(GdkEventButton* Event)
	{
		base::on_button_press_event(Event);
		return false;
	}
};

/////////////////////////////////////////////////////////////////////////////
// implementation

class implementation :
	public libk3dngui::asynchronous_update
{
public:
	implementation(document_state& DocumentState) :
		m_document_state(DocumentState),
		m_node(0)
	{
		m_scrolled_window.set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
		m_scrolled_window.add(m_view);

		m_model = Gtk::TreeStore::create(m_columns);

		m_view.set_model(m_model);
		m_view.set_headers_visible(false);
		m_view.set_reorderable(false);
//		m_view.signal_row_activated().connect(sigc::mem_fun(*this, &implementation::on_activate_row));
		m_view.signal_button_press_event().connect_notify(sigc::mem_fun(*this, &implementation::on_button_press_event), true);
		m_view.get_selection()->signal_changed().connect(sigc::mem_fun(*this, &implementation::on_selection_changed));

		Gtk::CellRendererText* const cell_text = new hotkey_cell_renderer_text();
		cell_text->property_editable() = true;
		cell_text->signal_edited().connect(sigc::mem_fun(*this, &implementation::on_node_name_edited));

		Gtk::TreeViewColumn* const name_column = new Gtk::TreeViewColumn;
		name_column->pack_start(*manage(cell_text), true);
		name_column->add_attribute(cell_text->property_text(), m_columns.name);

		m_view.append_column("icon", m_columns.icon);
		m_view.append_column(*manage(name_column));

		// Reset the control contents anytime the document node collection changes
		m_document_state.document().nodes().add_nodes_signal().connect(sigc::mem_fun(*this, &implementation::on_nodes_added));
		m_document_state.document().nodes().remove_nodes_signal().connect(sigc::mem_fun(*this, &implementation::on_nodes_removed));
		m_document_state.document().nodes().rename_node_signal().connect(sigc::mem_fun(*this, &implementation::on_node_renamed));

		m_document_state.view_node_history_signal().connect(sigc::mem_fun(*this, &implementation::on_view_node_history));

		schedule_update();
	}

	/// Looks-up a model row by node
	bool get_row(k3d::inode* const Object, const Gtk::TreeNodeChildren& ParentRows, Gtk::TreeIter& Row)
	{
		for(Gtk::TreeIter row = ParentRows.begin(); row != ParentRows.end(); ++row)
		{
			if(row->get_value(m_columns.node) == Object)
			{
				Row = row;
				return true;
			}

			if(get_row(Object, row->children(), Row))
				return true;
		}

		return false;
	}

	const k3d::icommand_node::result execute_command(const std::string& Command, const std::string& Arguments)
	{
		try
		{
			if(Command == "rename")
			{
				std::stringstream buffer(Arguments);
				k3d::xml::element arguments;
				buffer >> arguments;

				return_val_if_fail(arguments.name == "arguments", k3d::icommand_node::RESULT_ERROR);
				const std::string old_name = k3d::xml::element_text(arguments, "oldname");
				const std::string new_name = k3d::xml::element_text(arguments, "newname");
				return_val_if_fail(!old_name.empty(), k3d::icommand_node::RESULT_ERROR);
				return_val_if_fail(!new_name.empty(), k3d::icommand_node::RESULT_ERROR);

				k3d::inode* const node = k3d::find_node(m_document_state.document().nodes(), old_name);
				return_val_if_fail(node, k3d::icommand_node::RESULT_ERROR);

				Gtk::TreeIter row;
				return_val_if_fail(get_row(node, m_model->children(), row), k3d::icommand_node::RESULT_ERROR);

				interactive::set_text(m_view, *m_view.get_column(0), *m_view.get_column_cell_renderer(0), row, new_name);
				return k3d::icommand_node::RESULT_CONTINUE;
			}
			else if(Command == "select")
			{
				command_arguments arguments(Arguments);
				k3d::inode* const node = arguments.get_node(m_document_state.document(), "node");
				return_val_if_fail(node, k3d::icommand_node::RESULT_ERROR);

				Gtk::TreeIter row;
				return_val_if_fail(get_row(node, m_model->children(), row), k3d::icommand_node::RESULT_ERROR);

				interactive::select_row(m_view, *m_view.get_column(0), row);
				return k3d::icommand_node::RESULT_CONTINUE;
			}
			else if(Command == "context_menu")
			{
				m_document_state.popup_context_menu();
				return k3d::icommand_node::RESULT_CONTINUE;
			}
		}
		catch(std::exception& e)
		{
			k3d::log() << error << e.what() << std::endl;
			return k3d::icommand_node::RESULT_ERROR;
		}

		return k3d::icommand_node::RESULT_UNKNOWN_COMMAND;
	}

	void on_button_press_event(GdkEventButton* Event)
	{
		if(Event->button != 3)
			return;

		m_document_state.popup_context_menu();
	}

	/// Called by the signal system anytime new nodes are added to the document
	void on_nodes_added(const k3d::inode_collection::nodes_t& Objects)
	{
		schedule_update();
	}

	/// Called by the signal system anytime nodes are removed from the document
	void on_nodes_removed(const k3d::inode_collection::nodes_t& Objects)
	{
		schedule_update();
	}

	/// Called by the signal system anytime an node is renamed
	void on_node_renamed(k3d::inode* const Object)
	{
		schedule_update();
	}

	/// Called by the signal system anytime the user edits an node name in-placea
	void on_node_name_edited(const Glib::ustring& Path, const Glib::ustring& NewText)
	{
		// Get the row that was activated ...
		Gtk::TreeRow row = *m_model->get_iter(Path);

		// Look-up the actual node ...
		k3d::inode* const node = row[m_columns.node];
		return_if_fail(node);

		// If the name didn't change, we're done ...
		if(node->name() == NewText)
			return;

		// Format our command arguments as XML so there's no ambiguity about where the old name stops and the new name starts
		k3d::xml::element arguments("arguments");
		arguments.append(k3d::xml::element("oldname", node->name()));
		arguments.append(k3d::xml::element("newname", NewText));
		std::stringstream buffer;
		buffer << k3d::xml::single_line() << arguments;
		m_command_signal.emit("rename", buffer.str());

		k3d::record_state_change_set change_set(m_document_state.document(), k3d::string_cast(boost::format(_("Rename node %1%")) % NewText), K3D_CHANGE_SET_CONTEXT);
		node->set_name(NewText);
	}

	bool on_view_node_history(k3d::inode* const Object)
	{
		if(Object != m_node)
		{
			m_node = Object;

			m_node_deleted_connection.disconnect();
			m_node_properties_changed_connection.disconnect();

			schedule_update();

			if(m_node)
			{
				m_node_deleted_connection = m_node->deleted_signal().connect(sigc::mem_fun(*this, &implementation::on_node_deleted));
				k3d::iproperty_collection* const property_collection = dynamic_cast<k3d::iproperty_collection*>(m_node);
				if(property_collection)
					m_node_properties_changed_connection = property_collection->connect_properties_changed_signal(sigc::hide(sigc::mem_fun(*this, &implementation::schedule_update)));

			}

			return true;
		}

		return false;
	}

	void on_node_deleted()
	{
		on_view_node_history(0);
	}

	/// Defines a mapping of properties to their owning node
	typedef std::map<k3d::iproperty*, k3d::inode*> node_map_t;

	/// Rebuilds the contents of the control from scratch
	void on_update()
	{
		// Create a mapping of properties to nodes ...
		node_map_t node_map;
		const k3d::inode_collection::nodes_t& nodes = m_document_state.document().nodes().collection();
		for(k3d::inode_collection::nodes_t::const_iterator node = nodes.begin(); node != nodes.end(); ++node)
		{
			k3d::iproperty_collection* const property_collection = dynamic_cast<k3d::iproperty_collection*>(*node);
			if(!property_collection)
				continue;

			const k3d::iproperty_collection::properties_t properties(property_collection->properties());
			for(k3d::iproperty_collection::properties_t::const_iterator property = properties.begin(); property != properties.end(); ++property)
				node_map[*property] = *node;
		}

		// Keep track of nodes as they're added, to prevent an infinite loop ...
		std::set<k3d::inode*> visited;
		
		m_model->clear();
		
		if(m_node)
			insert_row(node_map, visited, *m_node, m_model->children());

		m_view.expand_all();
		m_view.scroll_to_point(0, 0);
	}

	/// Inserts a row into the tree representing an node, recursively adding connected nodes as children
	void insert_row(const node_map_t& ObjectMap, std::set<k3d::inode*>& Visited, k3d::inode& Object, const Gtk::TreeNodeChildren& ParentRows)
	{
		if(Visited.count(&Object))
			return;

		Visited.insert(&Object);
		
		Gtk::TreeIter i = m_model->append(ParentRows);
		Gtk::TreeRow row = *i;

		row[m_columns.node] = &Object;
		row[m_columns.icon] = quiet_load_icon(Object.factory().name(), Gtk::ICON_SIZE_MENU);
		row[m_columns.name] = Object.name();

		k3d::iproperty_collection* const property_collection = dynamic_cast<k3d::iproperty_collection*>(&Object);
		if(!property_collection)
			return;

		k3d::iproperty_collection::properties_t all_properties = property_collection->properties();
		for(k3d::iproperty_collection::properties_t::iterator property = all_properties.begin(); property != all_properties.end(); ++property)
		{
			k3d::iproperty* connection = m_document_state.document().pipeline().dependency(**property);
			if(!connection)
				continue;

			node_map_t::const_iterator node = ObjectMap.find(connection);
			if(node == ObjectMap.end())
				continue;

			insert_row(ObjectMap, Visited, *node->second, row.children());
		}
	}

	void on_selection_changed()
	{
		// Nothing selected? We're done ...
		if(!m_view.get_selection()->count_selected_rows())
			return;

		// Get the currently-selected row ...
		Gtk::TreeRow row = *m_view.get_selection()->get_selected();

		// Look-up the actual node ...
		k3d::inode* const node = row[m_columns.node];
		return_if_fail(node);

		command_arguments arguments;
		arguments.append("node", node);
		m_command_signal.emit("select", arguments);

		// Request that (somebody somewhere) show node details ...
		m_document_state.view_node_properties_signal().emit(node);
	}

	/// Stores a reference to the owning document
	document_state& m_document_state;
	/// Stores a reference to the currently-selected node (if any)
	k3d::inode* m_node;
	/// Tracks whether the currently-visible node is deleted
	sigc::connection m_node_deleted_connection;
	/// Keeps track of changes to the set of node properties
	sigc::connection m_node_properties_changed_connection;

	/// Defines the data model for the tree control
	class columns :
		public Gtk::TreeModelColumnRecord
	{
	public:
		columns()
		{
			add(node);
			add(icon);
			add(name);
		}

		Gtk::TreeModelColumn<k3d::inode*> node;
		Gtk::TreeModelColumn<Glib::RefPtr<Gdk::Pixbuf> > icon;
		Gtk::TreeModelColumn<Glib::ustring> name;
	};

	columns m_columns;
	Glib::RefPtr<Gtk::TreeStore> m_model;
	Gtk::ScrolledWindow m_scrolled_window;
	tree_view m_view;
	std::auto_ptr<Gtk::Menu> m_context_menu;

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
	public k3d::ideletable,
	public Gtk::VBox
{
	typedef Gtk::VBox base;

public:
	panel() :
		base(false, 0),
		ui_component("node_history", 0),
		m_implementation(0)
	{
	}

	~panel()
	{
		delete m_implementation;
	}

	void initialize(document_state& DocumentState, k3d::icommand_node& Parent)
	{
		ui_component::set_parent("node_history", &Parent);

		m_implementation = new detail::implementation(DocumentState);

		m_implementation->m_command_signal.connect(sigc::mem_fun(*this, &panel::record_command));

		m_implementation->m_view.signal_focus_in_event().connect(sigc::bind_return(sigc::hide(m_implementation->m_panel_grab_signal.make_slot()), false), false);
		
		pack_start(m_implementation->m_scrolled_window, Gtk::PACK_EXPAND_WIDGET);
		show_all();
	}

	const std::string panel_type()
	{
		return "node_history";
	}

	sigc::connection connect_focus_signal(const sigc::slot<void>& Slot)
	{
		return m_implementation->m_panel_grab_signal.connect(Slot);
	}

	const k3d::icommand_node::result execute_command(const std::string& Command, const std::string& Arguments)
	{
		const k3d::icommand_node::result result = m_implementation->execute_command(Command, Arguments);
		if(result != RESULT_UNKNOWN_COMMAND)
			return result;

		return ui_component::execute_command(Command, Arguments);
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::application_plugin_factory<panel> factory(
			k3d::uuid(0x680e37df, 0xe5428bb2, 0x5e159d8d, 0x283bf0b6),
			"NGUINodeHistoryPanel",
			_("Displays a hierarchical list of node inputs"),
			"NGUI Panels",
			k3d::iplugin_factory::EXPERIMENTAL,
			boost::assign::map_list_of("ngui:component-type", "panel")("ngui:panel-type", "node_history")("ngui:panel-label", "Node History"));

		return factory;
	}

private:
	detail::implementation* m_implementation;
};

} // namespace node_history

} // namespace ngui

} // namespace module

K3D_MODULE_START(Registry)
	Registry.register_factory(module::ngui::node_history::panel::get_factory());
K3D_MODULE_END

