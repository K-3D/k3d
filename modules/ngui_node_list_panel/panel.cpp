// K-3D
// Copyright (c) 1995-2009, Timothy M. Shead
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

#include <gtkmm/box.h>
#include <gtkmm/liststore.h>
#include <gtkmm/menu.h>
#include <gtkmm/scrolledwindow.h>
#include <gtkmm/treeview.h>

#include <k3d-i18n-config.h>
#include <k3dsdk/application_plugin_factory.h>
#include <k3dsdk/classes.h>
#include <k3dsdk/gl.h>
#include <k3dsdk/idocument.h>
#include <k3dsdk/idocument_plugin_factory.h>
#include <k3dsdk/inode_collection.h>
#include <k3dsdk/inode_name_map.h>
#include <k3dsdk/ipipeline.h>
#include <k3dsdk/iselectable.h>
#include <k3dsdk/iuser_interface.h>
#include <k3dsdk/module.h>
#include <k3dsdk/ngui/asynchronous_update.h>
#include <k3dsdk/ngui/document_state.h>
#include <k3dsdk/ngui/hotkey_cell_renderer_text.h>
#include <k3dsdk/ngui/icons.h>
#include <k3dsdk/ngui/interactive.h>
#include <k3dsdk/ngui/keyboard.h>
#include <k3dsdk/ngui/panel.h>
#include <k3dsdk/ngui/selection.h>
#include <k3dsdk/ngui/utility.h>
#include <k3dsdk/nodes.h>
#include <k3dsdk/state_change_set.h>
#include <k3dsdk/utility.h>

#include <boost/assign/list_of.hpp>

#include <set>

namespace module
{

namespace ngui
{

namespace node_list
{

/*
namespace detail
{

/////////////////////////////////////////////////////////////////////////////
// implementation

class implementation :
	public k3d::ngui::asynchronous_update
{
public:
	implementation(document_state& DocumentState) :
		m_document_state(DocumentState),
		m_filter_policy(new all_nodes_filter_policy(DocumentState.document())),
		m_layout_policy(new sort_by_label_layout_policy()),
		m_document_selection_change_mutex(false),
		m_node_list_selection_change_mutex(false)
	{
		// Connect signals
		m_view.signal_button_press_event().connect(sigc::mem_fun(*this, &implementation::on_button_press_event), false);

		// Update node selection according to document's
		m_document_state.document_selection_change_signal().connect(sigc::mem_fun(*this, &implementation::on_document_selection_changed));

		schedule_update();
	}

	/// Looks-up a node for a given row
	k3d::inode* const get_node(const Gtk::TreeIter& Row)
	{
		return (*Row)[m_columns.node];
	}

	bool on_button_press_event(GdkEventButton* Event)
	{
		if(3 == Event->button)
		{
			Gtk::TreeViewColumn* column = 0;
			int cell_x = 0;
			int cell_y = 0;
			m_view.get_path_at_pos(static_cast<int>(Event->x), static_cast<int>(Event->y), m_context_menu_path, column, cell_x, cell_y);
		}

		return false;
	}

	/// This mutex blocks node list update triggered by document selection update (true when updating document selection from new node list selection)
	bool m_document_selection_change_mutex;
	/// This mutex blocks document selection update triggered by node list update (true when updating node list selection from new document selection)
	bool m_node_list_selection_change_mutex;


};

} // namespace detail
*/

/////////////////////////////////////////////////////////////////////////////
// panel

class panel :
	public k3d::ngui::panel::control,
	public k3d::iunknown,
	public Gtk::VBox
{
	typedef Gtk::VBox base;

public:
	panel() :
		base(false, 0),
		m_document_state(0)
	{
		m_model = Gtk::ListStore::create(m_columns);

		m_view.set_model(m_model);
		m_view.set_headers_visible(false);
		m_view.set_reorderable(false);

		Gtk::CellRendererText* const cell_text = new k3d::ngui::hotkey_cell_renderer_text();
		cell_text->property_editable() = true;
		cell_text->signal_edited().connect(sigc::mem_fun(*this, &panel::on_node_name_edited));

		Gtk::TreeViewColumn* const name_column = new Gtk::TreeViewColumn;
		name_column->pack_start(*manage(cell_text), true);
		name_column->add_attribute(cell_text->property_text(), m_columns.name);

		m_view.append_column("icon", m_columns.icon);
		m_view.append_column(*manage(name_column));
		m_view.get_selection()->set_mode(Gtk::SELECTION_MULTIPLE);

		m_scrolled_window.set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
		m_scrolled_window.add(m_view);

		pack_start(m_scrolled_window, Gtk::PACK_EXPAND_WIDGET);

		// Set ourselves up so we can grab the (NGUI) panel focus ...
		m_view.signal_focus_in_event().connect(sigc::bind_return(sigc::hide(m_panel_grab_signal.make_slot()), false), false);

		show_all();
	}

	~panel()
	{
	}

	void initialize(k3d::ngui::document_state& DocumentState)
	{
		m_document_state = &DocumentState;

		// Reset the control contents anytime the document node collection changes
		m_document_state->document().nodes().add_nodes_signal().connect(sigc::mem_fun(*this, &panel::on_nodes_added));
		m_document_state->document().nodes().remove_nodes_signal().connect(sigc::mem_fun(*this, &panel::on_nodes_removed));
		m_document_state->document().nodes().rename_node_signal().connect(sigc::mem_fun(*this, &panel::on_node_renamed));

		m_view.get_selection()->set_select_function(sigc::mem_fun(*this, &panel::on_select_row));
		m_view.get_selection()->signal_changed().connect(sigc::mem_fun(*this, &panel::on_selection_changed));

		on_nodes_added(m_document_state->document().nodes().collection());
	}

	const k3d::string_t panel_type()
	{
		return get_factory().name();
	}

	sigc::connection connect_focus_signal(const sigc::slot<void>& Slot)
	{
		return m_panel_grab_signal.connect(Slot);
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::application_plugin_factory<panel> factory(
			k3d::uuid(0xdae07bf6, 0xa64cc64e, 0xce15e798, 0x16f8eb43),
			"NGUINodeListPanel",
			_("Displays the document nodes as a flat list"),
			"NGUI Panel",
			k3d::iplugin_factory::EXPERIMENTAL,
			boost::assign::map_list_of("ngui:component-type", "panel")("ngui:panel-label", "Node List"));

		return factory;
	}
	
private:
	/// Looks-up a model row based on a node pointer
	k3d::bool_t get_row(k3d::inode* const Node, Gtk::TreeIter& Row)
	{
		Gtk::TreeNodeChildren rows = m_model->children();
		for(Gtk::TreeIter row = rows.begin(); row != rows.end(); ++row)
		{
			if(row->get_value(m_columns.node) == Node)
			{
				Row = row;
				return true;
			}
		}

		return false;
	}

	/// Looks-up a node based on a model row path
	k3d::inode* const get_node(const Gtk::TreePath& Path)
	{
		if(Path.empty())
			return 0;

		Gtk::TreeRow row = *m_model->get_iter(Path);
		return row[m_columns.node];
	}

	/// Called by the signal system anytime new nodes are added to the document
	void on_nodes_added(const k3d::inode_collection::nodes_t& Nodes)
	{
		for(k3d::inode_collection::nodes_t::const_iterator node = Nodes.begin(); node != Nodes.end(); ++node)
		{
			Gtk::TreeRow new_row = *m_model->append();
			new_row[m_columns.name] = (*node)->name();
			new_row[m_columns.icon] = k3d::ngui::quiet_load_icon((*node)->factory().name(), Gtk::ICON_SIZE_MENU);
			new_row[m_columns.node] = *node;
		}

		m_model->set_sort_column(2, Gtk::SORT_ASCENDING);
	}

	/// Called by the signal system anytime nodes are removed from the document
	void on_nodes_removed(const k3d::inode_collection::nodes_t& Nodes)
	{
		Gtk::TreeIter row;
		for(k3d::inode_collection::nodes_t::const_iterator node = Nodes.begin(); node != Nodes.end(); ++node)
		{
			if(get_row(*node, row))
			{
				m_model->erase(row);
			}
		}
	}

	/// Called by the signal system anytime a node is renamed
	void on_node_renamed(k3d::inode* const Node)
	{
		Gtk::TreeIter row;
		if(get_row(Node, row))
		{
			(*row)[m_columns.name] = Node->name();
			m_model->set_sort_column(2, Gtk::SORT_ASCENDING);
		}
	}

	/// Called by the signal system anytime the user edits a node name in-place
	void on_node_name_edited(const Glib::ustring& Path, const Glib::ustring& NewText)
	{
		Gtk::TreeRow row = *m_model->get_iter(Path);
		k3d::inode* const node = row[m_columns.node];
		return_if_fail(node);

		// If the name didn't change, we're done ...
		if(node->name() == NewText)
			return;

		k3d::record_state_change_set change_set(m_document_state->document(), k3d::string_cast(boost::format(_("Rename node %1%")) % NewText), K3D_CHANGE_SET_CONTEXT);
		node->set_name(NewText);
	}

	/// Called for each row whose state is changing during interactive selection ...
	bool on_select_row(const Glib::RefPtr<Gtk::TreeModel>&, const Gtk::TreePath& Path, bool PathCurrentlySelected)
	{
		const k3d::bool_t new_selection_state = !PathCurrentlySelected;

		// We often get row selections that cancel each other out - filter them out of the selection set
		if(m_selection_paths.count(Path) && m_selection_paths[Path] != new_selection_state)
		{
			m_selection_paths.erase(Path);
			return true;
		}

		// Otherwise, store the new selection state ...
		m_selection_paths[Path] = new_selection_state;

		return true;
	}

	/// Called to signal that interactive selection is complete ...
	void on_selection_changed()
	{
		std::vector<k3d::inode*> selected_nodes;
		std::vector<k3d::inode*> deselected_nodes;
		for(selection_paths_t::const_iterator selection = m_selection_paths.begin(); selection != m_selection_paths.end(); ++selection)
		{
			if(selection->second)
				selected_nodes.push_back(get_node(selection->first));
			else
				deselected_nodes.push_back(get_node(selection->first));
		}

		if(selected_nodes.size() || deselected_nodes.size())
		{
			k3d::record_state_change_set change_set(m_document_state->document(), _("Select nodes"), K3D_CHANGE_SET_CONTEXT);
			k3d::ngui::selection::state(m_document_state->document()).select_nodes(selected_nodes);
			k3d::ngui::selection::state(m_document_state->document()).deselect_nodes(deselected_nodes);
		}

		if(selected_nodes.size() == 1)
		{
			m_document_state->view_node_properties_signal().emit(selected_nodes[0]);
			m_document_state->view_node_history_signal().emit(selected_nodes[0]);
		}

		m_selection_paths.clear();

/*
		if(!m_context_menu_path.empty())
			m_document_state.popup_context_menu();

		m_context_menu_path = Gtk::TreePath();
*/
	}

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
	Glib::RefPtr<Gtk::ListStore> m_model;
	Gtk::TreeView m_view;
	Gtk::ScrolledWindow m_scrolled_window;

	/// Stores the set of nodes being selected / deselected by the current interactive selection operation
	typedef std::map<Gtk::TreePath, bool> selection_paths_t;
	selection_paths_t m_selection_paths;
	/// Stores the row over which the context menu should be displayed (if at all)
	Gtk::TreePath m_context_menu_path;

	/// Signal to be emitted whenever this control needs to grab the panel focus
	sigc::signal<void> m_panel_grab_signal;

	k3d::ngui::document_state* m_document_state;
};

} // namespace node_list

} // namespace ngui

} // namespace module

K3D_MODULE_START(Registry)
	Registry.register_factory(module::ngui::node_list::panel::get_factory());
K3D_MODULE_END

