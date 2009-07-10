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

/** \file
		\author Tim Shead (tshead@k-3d.com)
		\author Romain Behar (romainbehar@yahoo.com)
*/

#include <gtkmm/box.h>
#include <gtkmm/liststore.h>
#include <gtkmm/menu.h>
#include <gtkmm/scrolledwindow.h>
#include <gtkmm/treeview.h>
#include <gtk/gtkmain.h>

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
#include <k3dsdk/ngui/selection_state.h>
#include <k3dsdk/ngui/utility.h>
#include <k3dsdk/nodes.h>
#include <k3dsdk/state_change_set.h>
#include <k3dsdk/utility.h>

#include <boost/assign/list_of.hpp>

#include <set>

// Temporary hack
using namespace k3d::ngui;

namespace module
{

namespace ngui
{

namespace node_list
{

namespace detail
{

/// Encapsulates an "edge" (a directed connection between two nodes) to be visualized by the control
class edge
{
public:
	k3d::iunknown* from;
	k3d::iunknown* to;
};

/// Encapsulates a "node" (zero-to-many document nodes) to be visualized by the control
class node
{
public:
	std::string label;

	typedef std::vector<k3d::iunknown*> nodes_t;
	nodes_t nodes;
};

/// Defines a collection of nodes and dependencies
class graph
{
public:
	~graph();

	typedef std::vector<node*> nodes_t;
	nodes_t nodes;

	typedef std::vector<edge*> edges_t;
	edges_t edges;
};

/// Abstract interface for nodes that populate graphs - these control what the user ends-up seeing
class filter_policy
{
public:
	virtual ~filter_policy() {}

	virtual void populate_graph(graph& Graph) = 0;
};

/// Concrete implementation of filter_policy that does nothing
class null_filter_policy :
	public filter_policy
{
public:
	virtual ~null_filter_policy() {}

	void populate_graph(graph& Graph);
};

/// Concrete implementation of filter_policy that displays every node in the document
class all_nodes_filter_policy :
	public filter_policy
{
public:
	all_nodes_filter_policy(k3d::idocument& Document);
	virtual ~all_nodes_filter_policy() {}

	void populate_graph(graph& Graph);

private:
	k3d::idocument& m_document;
};

/// Concrete implementation of filter_policy that displays every node that matches given class ID
class factory_id_filter_policy :
	public filter_policy
{
public:
	factory_id_filter_policy(k3d::idocument& Document, const k3d::uuid& FactoryID);
	~factory_id_filter_policy() {}

	void populate_graph(graph& Graph);

private:
	k3d::idocument& m_document;
	const k3d::uuid m_factory_id;
};

/// Abstract interface for nodes that adjust the layout of a graph
class layout_policy
{
public:
	virtual ~layout_policy() {}

	virtual void update_layout(graph& Graph) = 0;
};

/// Concrete implementation of layout_policy that does nothing
class null_layout_policy :
	public layout_policy
{
public:
	virtual ~null_layout_policy() {}

	void update_layout(graph& Graph);
};

/// Concrete implementation of layout_policy that sorts nodes alphabetically by label
class sort_by_label_layout_policy :
	public layout_policy
{
public:
	~sort_by_label_layout_policy() {}

	void update_layout(graph& Graph);
};

/// Concrete implementation of layout_policy that sorts nodes based on the type of nodes they front for
class sort_by_type_layout_policy :
	public layout_policy
{
public:
	~sort_by_type_layout_policy() {}

	void update_layout(graph& Graph);
};

struct sort_by_label
{
	bool operator()(const node* const LHS, const node* const RHS)
	{
		return LHS->label < RHS->label;
	}
};

struct sort_by_type
{
	bool operator()(const node* const LHS, const node* const RHS)
	{
		if(LHS->nodes.size() != RHS->nodes.size())
			return LHS->nodes.size() < RHS->nodes.size();

		for(unsigned int i = 0; i != LHS->nodes.size(); ++i)
			return typeid(*LHS->nodes[i]).before(typeid(*RHS->nodes[i]));

		return true;
	}
};

struct sort_by_name
{
	bool operator()(k3d::iplugin_factory* const LHS, k3d::iplugin_factory* const RHS)
	{
		return LHS->name() < RHS->name();
	}
};

/////////////////////////////////////////////////////////////////////////////
// graph

graph::~graph()
{
	std::for_each(nodes.begin(), nodes.end(), k3d::delete_object());
	std::for_each(edges.begin(), edges.end(), k3d::delete_object());
}

/////////////////////////////////////////////////////////////////////////////
// null_filter_policy

void null_filter_policy::populate_graph(graph& Graph)
{
}

/////////////////////////////////////////////////////////////////////////////
// all_nodes_filter_policy

all_nodes_filter_policy::all_nodes_filter_policy(k3d::idocument& Document) :
	m_document(Document)
{
}

void all_nodes_filter_policy::populate_graph(graph& Graph)
{
	for(k3d::inode_collection::nodes_t::const_iterator n = m_document.nodes().collection().begin(); n != m_document.nodes().collection().end(); ++n)
	{
		node* const new_node = new node;
		new_node->label = (*n)->name();
		new_node->nodes.push_back(*n);

		Graph.nodes.push_back(new_node);
	}
}

/////////////////////////////////////////////////////////////////////////////
// factory_id_filter_policy

factory_id_filter_policy::factory_id_filter_policy(k3d::idocument& Document, const k3d::uuid& FactoryID) :
	m_document(Document),
	m_factory_id(FactoryID)
{
}

void factory_id_filter_policy::populate_graph(graph& Graph)
{
	for(k3d::inode_collection::nodes_t::const_iterator n = m_document.nodes().collection().begin(); n != m_document.nodes().collection().end(); ++n)
	{
		if((*n)->factory().factory_id() != m_factory_id)
			continue;

		node* const new_node = new node;
		new_node->label = (*n)->name();
		new_node->nodes.push_back(*n);

		Graph.nodes.push_back(new_node);
	}
}

/////////////////////////////////////////////////////////////////////////////
// null_layout_policy

void null_layout_policy::update_layout(graph& Graph)
{
}

/////////////////////////////////////////////////////////////////////////////
// sort_by_label_layout_policy

void sort_by_label_layout_policy::update_layout(graph& Graph)
{
	std::sort(Graph.nodes.begin(), Graph.nodes.end(), detail::sort_by_label());
}

/////////////////////////////////////////////////////////////////////////////
// sort_by_type_layout_policy

void sort_by_type_layout_policy::update_layout(graph& Graph)
{
	std::sort(Graph.nodes.begin(), Graph.nodes.end(), detail::sort_by_type());
}

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
		m_scrolled_window.set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
		m_scrolled_window.add(m_view);

		m_model = Gtk::ListStore::create(m_columns);

		m_view.set_model(m_model);
		m_view.set_headers_visible(false);
		m_view.set_reorderable(false);

		Gtk::CellRendererText* const cell_text = new hotkey_cell_renderer_text();
		cell_text->property_editable() = true;
		cell_text->signal_edited().connect(sigc::mem_fun(*this, &implementation::on_node_name_edited));

		Gtk::TreeViewColumn* const name_column = new Gtk::TreeViewColumn;
		name_column->pack_start(*manage(cell_text), true);
		name_column->add_attribute(cell_text->property_text(), m_columns.name);

		m_view.append_column("icon", m_columns.icon);
		m_view.append_column(*manage(name_column));

		// Allow the user to select more than one node using Shift or Control key
		m_view.get_selection()->set_mode(Gtk::SELECTION_MULTIPLE);

		// Connect signals
		m_view.signal_button_press_event().connect(sigc::mem_fun(*this, &implementation::on_button_press_event), false);
		m_view.get_selection()->set_select_function(sigc::mem_fun(*this, &implementation::on_select_row));
		m_view.get_selection()->signal_changed().connect(sigc::mem_fun(*this, &implementation::on_selection_changed));

		// Update node selection according to document's
		m_document_state.document_selection_change_signal().connect(sigc::mem_fun(*this, &implementation::on_document_selection_changed));

		// Reset the control contents anytime the document node collection changes
		m_document_state.document().nodes().add_nodes_signal().connect(sigc::mem_fun(*this, &implementation::on_nodes_added));
		m_document_state.document().nodes().remove_nodes_signal().connect(sigc::mem_fun(*this, &implementation::on_nodes_removed));
		m_document_state.document().nodes().rename_node_signal().connect(sigc::mem_fun(*this, &implementation::on_node_renamed));

		schedule_update();
	}

	void set_filter_policy(filter_policy* const Policy)
	{
		return_if_fail(Policy);

		m_filter_policy.reset(Policy);
		schedule_update();
	}

	void set_layout_policy(layout_policy* const Policy)
	{
		return_if_fail(Policy);

		m_layout_policy.reset(Policy);
		schedule_update();
	}

	Gtk::ScrolledWindow m_scrolled_window;

	/// Called by the signal system anytime new nodes are added to the document
	void on_nodes_added(const k3d::inode_collection::nodes_t& Nodes)
	{
		for(k3d::inode_collection::nodes_t::const_iterator node = Nodes.begin(); node != Nodes.end(); ++node)
		{
			// Bubble sort node
			Gtk::TreeNodeChildren rows = m_model->children();
			Gtk::TreeIter row = rows.begin();
			for(; row != rows.end(); ++row)
			{
				/** \todo Use current sort_*_layout_policy, once it's implemented */
				if((*node)->name() <= row->get_value(m_columns.node)->name())
					break;
			}

			Gtk::TreeRow new_row;
			if(rows.end() == row)
				new_row = *m_model->append();
			else
				new_row = *m_model->insert(row);

			new_row[m_columns.name] = (*node)->name();
			new_row[m_columns.icon] = quiet_load_icon((*node)->factory().name(), Gtk::ICON_SIZE_MENU);
			new_row[m_columns.node] = *node;
		}

		/** \todo Add nodes to m_current_graph ? */
	}

	/// Called by the signal system anytime nodes are removed from the document
	void on_nodes_removed(const k3d::inode_collection::nodes_t& Nodes)
	{
		// Selection doesn't need update (fixes a crash: nested state changes when deleting more than one selected nodes)
		m_node_list_selection_change_mutex = true;

		for(k3d::inode_collection::nodes_t::const_iterator node = Nodes.begin(); node != Nodes.end(); ++node)
		{
			Gtk::TreeIter row;
			return_if_fail(get_row(*node, row));

			m_model->erase(row);
		}

		/** \todo Remove nodes from m_current_graph ? */

		m_node_list_selection_change_mutex = false;
	}

	/// Called by the signal system anytime a node is renamed
	void on_node_renamed(k3d::inode* const Node)
	{
		// Rename row
		Gtk::TreeIter row;
		return_if_fail(get_row(Node, row));

		(*row)[m_columns.name] = Node->name();

		// Put it at the right place in the list
		Gtk::TreeNodeChildren rows = m_model->children();
		Gtk::TreeIter new_row = rows.begin();
		for(; new_row != rows.end(); ++new_row)
		{
			if(new_row == row)
				continue;

			/** \todo Use current sort_*_layout_policy, once it's implemented */
			if(Node->name() <= new_row->get_value(m_columns.node)->name())
				break;
		}

		m_model->move(row, new_row);
	}

	/// Looks-up a model row based on a node pointer
	bool get_row(k3d::inode* const Node, Gtk::TreeIter& Row)
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

	/// Looks-up a node for a given row
	k3d::inode* const get_node(const Gtk::TreeIter& Row)
	{
		return (*Row)[m_columns.node];
	}

	/// Looks-up a node based on a model row path
	k3d::inode* const get_node(const Gtk::TreePath& Path)
	{
		if(Path.empty())
			return 0;

		Gtk::TreeRow row = *m_model->get_iter(Path);
		return row[m_columns.node];
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

		k3d::record_state_change_set change_set(m_document_state.document(), k3d::string_cast(boost::format(_("Rename node %1%")) % NewText), K3D_CHANGE_SET_CONTEXT);
		node->set_name(NewText);
	}

	/// Updates the contents of the control
	void on_update()
	{
		m_node_list_selection_change_mutex = true;

		m_model->clear();

		m_current_graph.reset(new graph());
		m_filter_policy->populate_graph(*m_current_graph);
		m_layout_policy->update_layout(*m_current_graph);

		// Keep track of roughly where we were in the list ...
//		const double position = ScrolledWindow("scrolledwindow").VerticalAdjustment().Value();

		for(graph::nodes_t::const_iterator n = m_current_graph->nodes.begin(); n != m_current_graph->nodes.end(); ++n)
		{
			Gtk::TreeRow row = *m_model->append();
			row[m_columns.name] = (*n)->label;

			if(1 == (*n)->nodes.size())
			{
				k3d::inode* const node = dynamic_cast<k3d::inode*>((*n)->nodes.front());
				if(node)
					row[m_columns.icon] = quiet_load_icon(node->factory().name(), Gtk::ICON_SIZE_MENU);

				row[m_columns.node] = node;
			}
		}

		m_node_list_selection_change_mutex = false;

		// Set document selection
		on_document_selection_changed();

		// Try to restore our original position (give or take) ...
//		ScrolledWindow("scrolledwindow").VerticalAdjustment().SetValue(position);
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

	bool on_select_row(const Glib::RefPtr<Gtk::TreeModel>& model, const Gtk::TreePath& path, bool path_currently_selected)
	{
		const bool new_selection_state = !path_currently_selected;

		// We often get row selections that cancel each other out - filter them out of the selection set
		if(m_selection_paths.count(path) && m_selection_paths[path] != new_selection_state)
		{
			m_selection_paths.erase(path);
			return true;
		}

		// Otherwise, store the new selection state ...
		m_selection_paths[path] = new_selection_state;

		return true;
	}

	void on_selection_changed()
	{
		// Synchronize document node selection states with the view selection ...
		k3d::selection::records deselected_records;
		k3d::selection::records selected_records;
		for(selection_paths_t::const_iterator selection = m_selection_paths.begin(); selection != m_selection_paths.end(); ++selection)
		{
			if(selection->second)
				selected_records.push_back(k3d::selection::make_record(get_node(selection->first)));
			else
				deselected_records.push_back(k3d::selection::make_record(get_node(selection->first)));
		}

		if(selected_records.size() || deselected_records.size())
		{
			m_document_selection_change_mutex = true;

			k3d::record_state_change_set change_set(m_document_state.document(), _("Select nodes"), K3D_CHANGE_SET_CONTEXT);

			m_document_state.set_selection_mode(selection::NODES);
			selection::state(m_document_state.document()).deselect(deselected_records);
			selection::state(m_document_state.document()).select(selected_records);

			m_document_selection_change_mutex = false;
		}

		// If a single node was selected, display its properties / history
		if(m_selection_paths.size() == 1 && m_selection_paths.begin()->second)
		{
			m_document_state.view_node_properties_signal().emit(get_node(m_selection_paths.begin()->first));
			m_document_state.view_node_history_signal().emit(get_node(m_selection_paths.begin()->first));
		}

		// Record tutorial data ...
		k3d::xml::element selection("selection");
		std::vector<Gtk::TreePath> current_selection = m_view.get_selection()->get_selected_rows();
		for(unsigned int i = 0; i != current_selection.size(); ++i)
		{
			if(k3d::inode* const node = get_node(current_selection[i]))
				selection.children.push_back(k3d::xml::element("node", m_document_state.document().unique_node_names().name(*node)));
		}

		if(!m_context_menu_path.empty())
			m_document_state.popup_context_menu();

		m_selection_paths.clear();

		m_context_menu_path = Gtk::TreePath();
	}

	/// Updates widget's selection according to new document selection
	void on_document_selection_changed()
	{
/*
		if(m_document_selection_change_mutex)
			return;

		m_node_list_selection_change_mutex = true;

		m_view.get_selection()->unselect_all();

		for(Gtk::TreeIter row_iterator = m_view.get_model()->children().begin(); row_iterator != m_view.get_model()->children().end(); ++row_iterator)
		{
			Gtk::TreeRow row = *row_iterator;

			if(k3d::inode* node = row[m_columns.node])
			{
				if(m_document_state.is_selected(node))
					m_view.get_selection()->select(row_iterator);
			}
		}

		m_node_list_selection_change_mutex = false;
*/
	}

	/// Stores a reference to the owning document
	document_state& m_document_state;
	/// Stores the current graph being visualized
	std::auto_ptr<graph> m_current_graph;
	/// Stores a policy that controls what's visible in the current graph
	std::auto_ptr<filter_policy> m_filter_policy;
	/// Stores a policy that handles layout of the current graph
	std::auto_ptr<layout_policy> m_layout_policy;

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

	/// This mutex blocks node list update triggered by document selection update (true when updating document selection from new node list selection)
	bool m_document_selection_change_mutex;
	/// This mutex blocks document selection update triggered by node list update (true when updating node list selection from new document selection)
	bool m_node_list_selection_change_mutex;

	/// Stores the set of nodes being selected / deselected by the current interactive selection operation
	typedef std::map<Gtk::TreePath, bool> selection_paths_t;
	selection_paths_t m_selection_paths;
	/// Stores the row over which the context menu should be displayed (if at all)
	Gtk::TreePath m_context_menu_path;

	/// Signal that will be emitted whenever this control should grab the panel focus
	sigc::signal<void> m_panel_grab_signal;
};

} // namespace detail

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
		m_implementation(0)
	{
	}

	~panel()
	{
		delete m_implementation;
	}

	void initialize(document_state& DocumentState)
	{
		m_implementation = new detail::implementation(DocumentState);

		m_implementation->m_view.signal_focus_in_event().connect(sigc::bind_return(sigc::hide(m_implementation->m_panel_grab_signal.make_slot()), false), false);

		pack_start(m_implementation->m_scrolled_window, Gtk::PACK_EXPAND_WIDGET);
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

	void set_filter_policy(detail::filter_policy* const Policy)
	{
		m_implementation->set_filter_policy(Policy);
	}

	void set_layout_policy(detail::layout_policy* const Policy)
	{
		m_implementation->set_layout_policy(Policy);
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
	detail::implementation* m_implementation;
};

} // namespace node_list

} // namespace ngui

} // namespace module

K3D_MODULE_START(Registry)
	Registry.register_factory(module::ngui::node_list::panel::get_factory());
K3D_MODULE_END

