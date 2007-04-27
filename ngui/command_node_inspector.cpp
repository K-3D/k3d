// K-3D
// Copyright (c) 1995-2005, Timothy M. Shead
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
		\brief Implements the command_node_inspector class
		\author Tim Shead (tshead@k-3d.com)
*/

#include "application_window.h"
#include "asynchronous_update.h"
#include "button.h"
#include "command_node_inspector.h"
#include "panel_frame.h"
#include "widget_manip.h"

#include <k3dsdk/command_tree.h>
#include <k3dsdk/i18n.h>
#include <k3dsdk/icommand_node.h>
#include <k3dsdk/icommand_tree.h>
#include <k3dsdk/iproperty.h>
#include <k3dsdk/iproperty_collection.h>

#include <glibmm/main.h>
#include <gtkmm/buttonbox.h>
#include <gtkmm/scrolledwindow.h>
#include <gtkmm/stock.h>
#include <gtkmm/treestore.h>
#include <gtkmm/treeview.h>

namespace libk3dngui
{

/////////////////////////////////////////////////////////////////////////////
// command_node_inspector

/// Provides an interactive UI for working with the k3d::icommand_tree hierarchy of k3d::icommand_node objects
class command_node_inspector :
	public application_window,
	public asynchronous_update
{
	typedef application_window base;

public:
	command_node_inspector() :
		base("command_node_inspector", 0)
	{
		m_node_store = Gtk::TreeStore::create(m_node_columns);

		set_title(_("Command Node Inspector"));
		set_role("command_node_inspector");
		set_position(Gtk::WIN_POS_CENTER);
		set_border_width(10);
		resize(250, 350);

		Gtk::TreeView* const tree = new Gtk::TreeView(m_node_store);
		tree->set_headers_visible(true);
		tree->append_column(_("Node"), m_node_columns.m_node_name);
		tree->signal_row_activated().connect(sigc::mem_fun(*this, &command_node_inspector::on_select_row));

		Gtk::ScrolledWindow* const scrolled_window = new Gtk::ScrolledWindow();
		scrolled_window->set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
		scrolled_window->add(*manage(tree));

		Gtk::HButtonBox* const box2 = new Gtk::HButtonBox(Gtk::BUTTONBOX_END);
		box2->pack_start(*Gtk::manage(
			new button::control(*this, "close", Gtk::Stock::CLOSE)
			<< disable_recording()
			<< connect_button(sigc::mem_fun(*this, &command_node_inspector::close))));

		Gtk::VBox* const box1 = new Gtk::VBox(false, 10);
		box1->pack_start(*manage(scrolled_window), Gtk::PACK_EXPAND_WIDGET);
		box1->pack_start(*manage(box2), Gtk::PACK_SHRINK);

		add(*manage(box1));

		k3d::command_tree().connect_changed_signal(sigc::mem_fun(*this, &command_node_inspector::on_command_tree_changed));
		schedule_update();
		show_all();
	}

private:
	/// Called by the signal system when the command tree is modified
	void on_command_tree_changed()
	{
		schedule_update();
	}

	struct sort_by_name
	{
		bool operator()(k3d::icommand_node* LHS, k3d::icommand_node* RHS)
		{
			return k3d::command_tree().name(*LHS) < k3d::command_tree().name(*RHS);
		}
	};

	void on_update()
	{
		m_node_store->clear();
		
		k3d::icommand_tree::nodes_t children = k3d::command_tree().children(0);
		std::sort(children.begin(), children.end(), sort_by_name());

		for(k3d::icommand_tree::nodes_t::iterator child = children.begin(); child != children.end(); ++child)
			insert_node(**child, m_node_store->children());
	}

	void insert_node(k3d::icommand_node& Node, const Gtk::TreeNodeChildren& Parent)
	{
		std::string node_name = k3d::command_tree().name(Node);
		if(panel_frame::control* const frame = dynamic_cast<panel_frame::control*>(&Node))
			node_name += " [" + frame->mounted_panel_type() + "]";
		
		Gtk::TreeRow node_row = *m_node_store->append(Parent);
		node_row[m_node_columns.m_node_name] = node_name;
		node_row[m_node_columns.m_node] = &Node;

		k3d::icommand_tree::nodes_t children = k3d::command_tree().children(&Node);
		std::sort(children.begin(), children.end(), sort_by_name());

		for(k3d::icommand_tree::nodes_t::iterator child = children.begin(); child != children.end(); ++child)
			insert_node(**child, node_row.children());
	}

	void on_select_row(const Gtk::TreePath& Path, Gtk::TreeViewColumn* Column)
	{
		// Get the node that was activated ...
		Gtk::TreeRow row = *m_node_store->get_iter(Path);
		k3d::icommand_node* const node = row[m_node_columns.m_node];
		return_if_fail(node);

		// Make this a recordable event ...
		k3d::command_tree().command_signal().emit(*node, k3d::icommand_node::COMMAND_INTERACTIVE, "highlight", "");

		// Highlight the node ...
		node->execute_command("highlight", "");
	}

	class columns :
		public Gtk::TreeModelColumnRecord
	{
	public:
		columns()
		{
			add(m_node_name);
			add(m_node);
		}

		Gtk::TreeModelColumn<Glib::ustring> m_node_name;
		Gtk::TreeModelColumn<k3d::icommand_node*> m_node;
	};

	columns m_node_columns;
	Glib::RefPtr<Gtk::TreeStore> m_node_store;
};

void create_command_node_inspector()
{
	new command_node_inspector();
}

} // namespace libk3dngui

