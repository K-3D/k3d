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
#include <k3dsdk/application_plugin_factory.h>
#include <k3dsdk/classes.h>
#include <k3dsdk/command_tree.h>
#include <k3dsdk/gl.h>
#include <k3dsdk/idocument.h>
#include <k3dsdk/idocument_plugin_factory.h>
#include <k3dsdk/inode_collection.h>
#include <k3dsdk/ipipeline.h>
#include <k3dsdk/iuser_interface.h>
#include <k3dsdk/module.h>
#include <k3dsdk/ngui/asynchronous_update.h>
#include <k3dsdk/ngui/button.h>
#include <k3dsdk/ngui/document_state.h>
#include <k3dsdk/ngui/panel.h>
#include <k3dsdk/ngui/toolbar.h>
#include <k3dsdk/ngui/ui_component.h>
#include <k3dsdk/ngui/undo_utility.h>
#include <k3dsdk/nodes.h>
#include <k3dsdk/state_change_set.h>
#include <k3dsdk/utility.h>
#include <k3dsdk/utility_gl.h>

#include <gtkmm/box.h>
#include <gtkmm/menu.h>
#include <gtkmm/scrolledwindow.h>
#include <gtkmm/stock.h>
#include <gtkmm/treestore.h>
#include <gtkmm/treeview.h>

#include <boost/assign/list_of.hpp>

// Temporary hack
using namespace libk3dngui;

namespace module
{

namespace ngui
{

namespace undo_tree
{

namespace detail
{

/////////////////////////////////////////////////////////////////////////////
// implementation

/** \todo This use of the pimpl-idiom is obsolete and doesn't make much sense in the context of a plugin */
class implementation :
	public Gtk::VBox,
	public asynchronous_update
{
	typedef Gtk::VBox base;

public:
	implementation(document_state& DocumentState, k3d::icommand_node& Parent) :
		base(false, 0),
		m_document_state(DocumentState),
		m_parent(Parent),
		m_undo_button(0),
		m_redo_button(0)
	{
		toolbar::control* const toolbar = new toolbar::control(m_parent, "toolbar");

		m_undo_button = new button::control(*toolbar, "undo");
		m_redo_button = new button::control(*toolbar, "redo");

		m_undo_button->add(*Gtk::manage(new Gtk::Image(Gtk::Stock::UNDO, Gtk::ICON_SIZE_SMALL_TOOLBAR)));
		m_redo_button->add(*Gtk::manage(new Gtk::Image(Gtk::Stock::REDO, Gtk::ICON_SIZE_SMALL_TOOLBAR)));

		m_undo_button->signal_clicked().connect(sigc::mem_fun(*this, &implementation::on_undo));
		m_redo_button->signal_clicked().connect(sigc::mem_fun(*this, &implementation::on_redo));

		toolbar->row(0).pack_start(*Gtk::manage(m_undo_button), Gtk::PACK_SHRINK);
		toolbar->row(0).pack_start(*Gtk::manage(m_redo_button), Gtk::PACK_SHRINK);

		Gtk::ScrolledWindow* const scrolled_window = new Gtk::ScrolledWindow();
		scrolled_window->set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
		scrolled_window->add(m_view);

		pack_start(*Gtk::manage(toolbar), Gtk::PACK_SHRINK);
		pack_start(*Gtk::manage(scrolled_window), Gtk::PACK_EXPAND_WIDGET);

		m_model = Gtk::TreeStore::create(m_columns);

		m_view.set_model(m_model);
		m_view.set_headers_visible(false);
		m_view.set_reorderable(false);
		m_view.signal_row_activated().connect(sigc::mem_fun(*this, &implementation::on_select_row));

		Gtk::CellRendererText* const cell_text = new Gtk::CellRendererText();

		Gtk::TreeViewColumn* const column = new Gtk::TreeViewColumn;
		column->pack_start(m_columns.saved, false);
		column->pack_start(m_columns.current, false);
		column->pack_start(*manage(cell_text), true);
		column->add_attribute(cell_text->property_text(), m_columns.label);
		column->add_attribute(cell_text->property_strikethrough(), m_columns.strikethrough);

		m_view.append_column(*manage(column));

		m_document_state.document().state_recorder().connect_node_added_signal(sigc::mem_fun(*this, &implementation::on_node_added));
		m_document_state.document().state_recorder().connect_current_node_changed_signal(sigc::mem_fun(*this, &implementation::on_current_node_changed));
		m_document_state.document().state_recorder().connect_last_saved_node_changed_signal(sigc::mem_fun(*this, &implementation::on_last_saved_node_changed));

		schedule_update();
	}

	/// Called by the signal system when the user clicks the undo button
	void on_undo()
	{
		const k3d::istate_recorder::node* current_node = m_document_state.document().state_recorder().current_node();
		return_if_fail(current_node);

		current_node->change_set->undo();
		m_document_state.document().state_recorder().set_current_node(current_node->parent);
		k3d::gl::redraw_all(m_document_state.document(), k3d::gl::irender_viewport::ASYNCHRONOUS);
	}

	/// Called by the signal system when the user clicks the redo button
	void on_redo()
	{
		const k3d::istate_recorder::node* const redo_node = next_redo(m_document_state.document().state_recorder());
		return_if_fail(redo_node);

		redo_node->change_set->redo();
		m_document_state.document().state_recorder().set_current_node(redo_node);
		k3d::gl::redraw_all(m_document_state.document(), k3d::gl::irender_viewport::ASYNCHRONOUS);
	}

	/// Called by the signal system anytime a node is added to the hierarchical undo tree
	void on_node_added(const k3d::istate_recorder::node* const NewNode)
	{
		add_node(NewNode, 0, m_model->children());
	}

	/// Called by the signal system anytime the current node in the hierarchical undo tree changes
	void on_current_node_changed()
	{
		k3d::istate_recorder& state_recorder = m_document_state.document().state_recorder();
		const k3d::istate_recorder::node* const current_node = state_recorder.current_node();
		const k3d::istate_recorder::node* const redo_node = next_redo(state_recorder);
		const parent_nodes_t active_nodes = parent_nodes(current_node);

		set_current_node(current_node, redo_node, active_nodes, m_model->children());

		update_buttons();
	}

	/// Updates the state of the undo/redo buttons to match the current node in the hierarchical undo tree
	void update_buttons()
	{
		k3d::istate_recorder& state_recorder = m_document_state.document().state_recorder();
		const k3d::istate_recorder::node* const current_node = state_recorder.current_node();
		const k3d::istate_recorder::node* const redo_node = next_redo(state_recorder);

		// Setup the undo button ...
		if(current_node)
		{
			m_undo_button->set_sensitive(true);

			const std::string label = k3d::string_cast(boost::format(_("Undo %1%")) % current_node->label);
			m_undo_button->set_tip(label);
//				m_undo_button->set_label(label);
		}
		else
		{
			m_undo_button->set_sensitive(false);

			const std::string label = _("Can't Undo");
			m_undo_button->set_tip(label);
//				m_undo_button->set_label(label);
		}

		// Setup the redo menu item ...
		if(redo_node)
		{
			m_redo_button->set_sensitive(true);

			const std::string label = k3d::string_cast(boost::format(_("Redo %1%")) % redo_node->label);
			m_redo_button->set_tip(label);
//				m_redo_button->set_label(label);
		}
		else
		{
			m_redo_button->set_sensitive(false);

			const std::string label = _("Can't Redo");
			m_redo_button->set_tip(label);
//				m_redo_button->set_label(label);
		}
	}

	/// Called by the signal system anytime the last saved node in the hierarchical undo tree changes
	void on_last_saved_node_changed()
	{
		k3d::istate_recorder& state_recorder = m_document_state.document().state_recorder();
		const k3d::istate_recorder::node* const last_saved_node = state_recorder.last_saved_node();

		set_last_saved_node(last_saved_node, m_model->children());
	}

	void on_update()
	{
		update_contents();
		update_buttons();
	}

	/// Rebuilds the contents of the control from scratch
	void update_contents()
	{
		k3d::istate_recorder& state_recorder = m_document_state.document().state_recorder();
		const k3d::istate_recorder::node* const current_node = state_recorder.current_node();
		const k3d::istate_recorder::node* const redo_node = next_redo(state_recorder);
		const k3d::istate_recorder::node* const saved_node = state_recorder.last_saved_node();
		const parent_nodes_t active_nodes = parent_nodes(current_node);

		m_model->clear();

		const k3d::istate_recorder::nodes_t root_nodes = state_recorder.root_nodes();
		for(k3d::istate_recorder::nodes_t::const_iterator node = root_nodes.begin(); node != root_nodes.end(); ++node)
			insert_node(current_node, redo_node, saved_node, *node, active_nodes, m_model->children());
	}

	/// Used to efficiently insert a single new undo hierarchy node into the treeview model
	bool add_node(const k3d::istate_recorder::node* const NewNode, const k3d::istate_recorder::node* const ParentNode, const Gtk::TreeNodeChildren& ParentRows)
	{
		if(NewNode->parent == ParentNode)
		{
			Gtk::TreeRow row = *m_model->append(ParentRows);
			row[m_columns.node] = NewNode;
			row[m_columns.label] = NewNode->label;
			row[m_columns.current] = Glib::RefPtr<Gdk::Pixbuf>();
			row[m_columns.saved] = Glib::RefPtr<Gdk::Pixbuf>();
			row[m_columns.strikethrough] = false;

			return true;
		}

		for(Gtk::TreeIter row = ParentRows.begin(); row != ParentRows.end(); ++row)
		{
			if(add_node(NewNode, (*row)[m_columns.node], row->children()))
				return true;
		}

		return false;
	}

	/// Used to efficiently update the treeview model when the undo hierarchy current node has changed
	void set_current_node(const k3d::istate_recorder::node* const CurrentNode, const k3d::istate_recorder::node* const RedoNode, const parent_nodes_t& ActiveNodes, const Gtk::TreeNodeChildren& ParentRows)
	{
		for(Gtk::TreeIter i = ParentRows.begin(); i != ParentRows.end(); ++i)
		{
			Gtk::TreeRow row = *i;

			if(row[m_columns.node] == CurrentNode)
			{
				row[m_columns.current] = m_view.render_icon(Gtk::Stock::UNDO, Gtk::ICON_SIZE_SMALL_TOOLBAR);

				m_view.scroll_to_row(Gtk::TreePath(i), 0.5);
				m_view.expand_to_path(Gtk::TreePath(i));
				m_view.expand_row(Gtk::TreePath(i), false);
			}
			else if(row[m_columns.node] == RedoNode)
			{
				row[m_columns.current] = m_view.render_icon(Gtk::Stock::REDO, Gtk::ICON_SIZE_SMALL_TOOLBAR);
			}
			else
			{
				row[m_columns.current] = Glib::RefPtr<Gdk::Pixbuf>();
			}

			row[m_columns.strikethrough] = std::find(ActiveNodes.begin(), ActiveNodes.end(), row[m_columns.node]) == ActiveNodes.end();

			set_current_node(CurrentNode, RedoNode, ActiveNodes, row.children());
		}
	}

	/// Used to efficiently update the treeview model when the undo hierarchy last saved node has changed
	void set_last_saved_node(const k3d::istate_recorder::node* const LastSavedNode, const Gtk::TreeNodeChildren& ParentRows)
	{
		for(Gtk::TreeIter i = ParentRows.begin(); i != ParentRows.end(); ++i)
		{
			Gtk::TreeRow row = *i;

			if(row[m_columns.node] == LastSavedNode)
			{
				row[m_columns.saved] = m_view.render_icon(Gtk::Stock::SAVE, Gtk::ICON_SIZE_SMALL_TOOLBAR);
			}
			else
			{
				row[m_columns.saved] = Glib::RefPtr<Gdk::Pixbuf>();
			}

			set_last_saved_node(LastSavedNode, row.children());
		}
	}

	/// Used to recursively create the treeview model from scratch
	void insert_node(const k3d::istate_recorder::node* const CurrentNode, const k3d::istate_recorder::node* const RedoNode, const k3d::istate_recorder::node* const SavedNode, k3d::istate_recorder::node* const Node, const parent_nodes_t& ActiveNodes, const Gtk::TreeNodeChildren& ParentRows)
	{
		Gtk::TreeIter i = m_model->append(ParentRows);
		Gtk::TreeRow row = *i;
		row[m_columns.node] = Node;
		row[m_columns.label] = Node->label;

		if(Node == CurrentNode)
		{
			row[m_columns.current] = m_view.render_icon(Gtk::Stock::UNDO, Gtk::ICON_SIZE_SMALL_TOOLBAR);
		}
		else if(Node == RedoNode)
		{
			row[m_columns.current] = m_view.render_icon(Gtk::Stock::REDO, Gtk::ICON_SIZE_SMALL_TOOLBAR);
		}
		else
		{
			row[m_columns.current] = Glib::RefPtr<Gdk::Pixbuf>();
		}

		if(Node == SavedNode)
		{
			row[m_columns.saved] = m_view.render_icon(Gtk::Stock::SAVE, Gtk::ICON_SIZE_SMALL_TOOLBAR);
		}
		else
		{
			row[m_columns.saved] = Glib::RefPtr<Gdk::Pixbuf>();
		}
		row[m_columns.strikethrough] = std::find(ActiveNodes.begin(), ActiveNodes.end(), Node) == ActiveNodes.end();

		for(k3d::istate_recorder::nodes_t::const_iterator node = Node->children.begin(); node != Node->children.end(); ++node)
			insert_node(CurrentNode, RedoNode, SavedNode, *node, ActiveNodes, row.children());

		if(Node == CurrentNode)
		{
			m_view.scroll_to_row(Gtk::TreePath(i), 0.5);
			m_view.expand_to_path(Gtk::TreePath(i));
			m_view.expand_row(Gtk::TreePath(i), false);
		}
	}

	/// Called when the user selects (double-clicks) a row in the treeview control
	void on_select_row(const Gtk::TreePath& Path, Gtk::TreeViewColumn* Column)
	{
		Gtk::TreeRow row = *m_model->get_iter(Path);
		const k3d::istate_recorder::node* const selected_node = row[m_columns.node];

		// Get the current node parents ...
		const parent_nodes_t current_parents = parent_nodes(m_document_state.document().state_recorder().current_node());

		// Get the selected node parents ...
		const parent_nodes_t selected_parents = parent_nodes(selected_node);

		// Remove common nodes from both lists to produce the list of undos and the list of redos to be performed to change the state
		parent_nodes_t undos;
		for(unsigned long i = 0; i != current_parents.size(); ++i)
		{
			if(i < selected_parents.size() && current_parents[i] == selected_parents[i])
				continue;

			undos.push_back(current_parents[i]);
		}
		std::reverse(undos.begin(), undos.end());

		parent_nodes_t redos;
		for(unsigned long i = 0; i != selected_parents.size(); ++i)
		{
			if(i < current_parents.size() && current_parents[i] == selected_parents[i])
				continue;

			redos.push_back(selected_parents[i]);
		}

		// Change from the current to the selected state ...
		m_document_state.document().state_recorder().set_current_node(selected_node);
		for(parent_nodes_t::const_iterator undo = undos.begin(); undo != undos.end(); ++undo)
			(*undo)->change_set->undo();

		for(parent_nodes_t::const_iterator redo = redos.begin(); redo != redos.end(); ++redo)
			(*redo)->change_set->redo();

		k3d::gl::redraw_all(m_document_state.document(), k3d::gl::irender_viewport::ASYNCHRONOUS);
	}

	/// Stores a reference to the owning document
	document_state& m_document_state;
	k3d::icommand_node& m_parent;

	/// Declares columns for the TreeView data model
	class columns :
		public Gtk::TreeModelColumnRecord
	{
	public:
		columns()
		{
			add(node);
			add(label);
			add(current);
			add(saved);
			add(strikethrough);
		}

		Gtk::TreeModelColumn<const k3d::istate_recorder::node*> node;
		Gtk::TreeModelColumn<Glib::ustring> label;
		Gtk::TreeModelColumn<Glib::RefPtr<Gdk::Pixbuf> > current;
		Gtk::TreeModelColumn<Glib::RefPtr<Gdk::Pixbuf> > saved;
		Gtk::TreeModelColumn<bool> strikethrough;
	};

	/// Defines columns for the TreeView data model
	columns m_columns;
	/// Provides storage for the TreeView
	Glib::RefPtr<Gtk::TreeStore> m_model;

	button::control* m_undo_button;
	button::control* m_redo_button;
	Gtk::TreeView m_view;

	sigc::signal<void, const std::string&, const std::string&> m_command_signal;

	/// Signal that will be emitted whenever this control should grab the panel focus
	sigc::signal<void> m_panel_grab_signal;
};

} // namespace detail

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// panel

class panel :
	public Gtk::VBox,
	public libk3dngui::panel::control,
	public ui_component
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

	void initialize(document_state& DocumentState, k3d::icommand_node& Parent)
	{
		k3d::command_tree().add(*this, "undo_tree", &Parent);

		m_implementation = new detail::implementation(DocumentState, *this);

		m_implementation->m_command_signal.connect(sigc::mem_fun(*this, &panel::record_command));
		
		m_implementation->m_undo_button->signal_focus_in_event().connect(sigc::bind_return(sigc::hide(m_implementation->m_panel_grab_signal.make_slot()), false), false);
		m_implementation->m_redo_button->signal_focus_in_event().connect(sigc::bind_return(sigc::hide(m_implementation->m_panel_grab_signal.make_slot()), false), false);
		m_implementation->m_view.signal_focus_in_event().connect(sigc::bind_return(sigc::hide(m_implementation->m_panel_grab_signal.make_slot()), false), false);
		
		pack_start(*m_implementation, Gtk::PACK_EXPAND_WIDGET);
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
			k3d::uuid(0x0242c7b7, 0x234e7664, 0x3a848fa7, 0x91465b42),
			"NGUIUndoTreePanel",
			_("Provides a panel for displaying the undo tree"),
			"NGUI Panel",
			k3d::iplugin_factory::EXPERIMENTAL,
			boost::assign::map_list_of("ngui:component-type", "panel")("ngui:panel-label", "Undo Tree"));

		return factory;
	}

private:
	detail::implementation* m_implementation;
};

} // namespace undo_tree

} // namespace ngui

} // namespace module

K3D_MODULE_START(Registry)
	Registry.register_factory(module::ngui::undo_tree::panel::get_factory());
K3D_MODULE_END

