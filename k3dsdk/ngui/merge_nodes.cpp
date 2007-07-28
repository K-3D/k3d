// K-3D
// Copyright (c) 2005-2006, Romain Behar
//
// Contact: romainbehar@yahoo.com
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
		\author Romain Behar (romainbehar@yahoo.com)
		\author Tim Shead (tshead@k-3d.com)
*/

#include "file_chooser_dialog.h"

#include <k3dsdk/data.h>
#include <k3d-i18n-config.h>
#include <k3dsdk/ideletable.h>
#include <k3dsdk/idocument.h>
#include <k3dsdk/idocument_plugin_factory.h>
#include <k3dsdk/inode.h>
#include <k3dsdk/inode_collection.h>
#include <k3dsdk/ipersistent.h>
#include <k3dsdk/options.h>
#include <k3dsdk/persistent_lookup.h>
#include <k3dsdk/plugins.h>
#include <k3dsdk/result.h>
#include <k3dsdk/serialization.h>
#include <k3dsdk/xml.h>

#include <k3dsdk/fstream.h>

#include <gtkmm/dialog.h>
#include <gtkmm/entry.h>
#include <gtkmm/label.h>
#include <gtkmm/liststore.h>
#include <gtkmm/scrolledwindow.h>
#include <gtkmm/stock.h>
#include <gtkmm/table.h>
#include <gtkmm/treeview.h>

namespace libk3dngui
{

namespace merge_nodes
{

namespace detail
{

// Merge nodes UI
struct node_check_t
{
	node_check_t(const std::string& Name) :
		name(Name)
	{
	}

	std::string name;
	bool check;
};

typedef std::vector<node_check_t> document_nodes_t;

class merge_nodes_dialog :
	public Gtk::Dialog
{
	typedef Gtk::Dialog base;

public:
	merge_nodes_dialog(document_nodes_t& DocumentNodes, std::string& NodePrefix) :
		base("Choose Nodes to Merge:", true),
		m_document_nodes(DocumentNodes),
		m_node_prefix(NodePrefix),
		m_property_table(1, 4, false),
		m_check_all_button(_("Check All")),
		m_clear_selection_button(_("Clear Selection")),
		m_prefix_label(_("Node prefix :"))
	{
		m_model = Gtk::ListStore::create(m_nodes);
		for(document_nodes_t::const_iterator node = DocumentNodes.begin(); node != DocumentNodes.end(); ++node)
		{
			Gtk::TreeRow row = *(m_model->append());
			return_if_fail(row);

			row[m_nodes.name] = node->name;
			row[m_nodes.check] = false;
		}

		m_treeview.set_model(m_model);
		m_treeview.append_column_editable(_("Merge?"), m_nodes.check);
		m_treeview.append_column_editable(_("Node"), m_nodes.name);

		m_scrolled_window.add(m_treeview);
		m_scrolled_window.set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);

		// Setup dialog
		set_default_size(500, 350);
		set_border_width(5);

		get_vbox()->pack_start(m_scrolled_window, Gtk::PACK_EXPAND_WIDGET);

		m_property_table.attach(m_check_all_button, 0, 1, 0, 1, Gtk::EXPAND, Gtk::SHRINK);
		m_property_table.attach(m_clear_selection_button, 1, 2, 0, 1, Gtk::EXPAND, Gtk::SHRINK);
		m_property_table.attach(m_prefix_label, 2, 3, 0, 1, Gtk::EXPAND, Gtk::SHRINK);
		m_property_table.attach(m_prefix_entry, 3, 4, 0, 1, Gtk::EXPAND, Gtk::SHRINK);

		get_vbox()->pack_start(m_property_table, Gtk::PACK_SHRINK);

		add_button(Gtk::Stock::OK, Gtk::RESPONSE_OK);
		add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);

		set_position(Gtk::WIN_POS_CENTER);
		show_all();

		m_check_all_button.signal_clicked().connect(sigc::mem_fun(*this, &merge_nodes_dialog::on_check_all));
		m_clear_selection_button.signal_clicked().connect(sigc::mem_fun(*this, &merge_nodes_dialog::on_clear_selection));
		m_prefix_entry.signal_key_release_event().connect(sigc::mem_fun(*this, &merge_nodes_dialog::on_update_prefix));
	}

private:
	void on_response(int ID)
	{
		if(Gtk::RESPONSE_OK == ID)
		{
			Gtk::TreeNodeChildren rows = m_model->children();
			document_nodes_t::iterator node = m_document_nodes.begin();
			for(Gtk::TreeNodeChildren::const_iterator row = rows.begin(); row != rows.end(); ++row)
			{
				return_if_fail(node != m_document_nodes.end());

				node->check = (*row)[m_nodes.check];
				++node;
			}
		}
	}

	void on_check_all()
	{
		Gtk::TreeNodeChildren rows = m_model->children();
		for(Gtk::TreeNodeChildren::const_iterator row = rows.begin(); row != rows.end(); ++row)
			(*row)[m_nodes.check] = true;
	}

	void on_clear_selection()
	{
		Gtk::TreeNodeChildren rows = m_model->children();
		for(Gtk::TreeNodeChildren::const_iterator row = rows.begin(); row != rows.end(); ++row)
			(*row)[m_nodes.check] = false;
	}

	bool on_update_prefix(GdkEventKey* Event)
	{
		m_node_prefix = m_prefix_entry.get_text();

		return true;
	}

	document_nodes_t& m_document_nodes;
	std::string& m_node_prefix;

	Glib::RefPtr<Gtk::ListStore> m_model;
	Gtk::TreeView m_treeview;
	Gtk::ScrolledWindow m_scrolled_window;

	Gtk::Table m_property_table;
	Gtk::Button m_check_all_button;
	Gtk::Button m_clear_selection_button;
	Gtk::Label m_prefix_label;
	Gtk::Entry m_prefix_entry;

	class columns_t :
		public Gtk::TreeModelColumnRecord
	{
	public:
		columns_t()
		{
			add(name);
			add(check);
		}

		Gtk::TreeModelColumn<Glib::ustring> name;
		Gtk::TreeModelColumn<bool> check;
	};
	columns_t m_nodes;
};

} // namespace detail

void merge_nodes(k3d::idocument& Document)
{
	k3d::filesystem::path document_path;

	{
		file_chooser_dialog dialog(_("Merge K-3D Document:"), k3d::options::path::documents(), Gtk::FILE_CHOOSER_ACTION_OPEN);
		dialog.add_pattern_filter(_("K-3D Document (*.k3d)"), "*.k3d");
		dialog.add_all_files_filter();

		if(!dialog.get_file_path(document_path))
			return;
	}

	// Check for a K-3D document
	k3d::xml::element xml("k3dml");
	try
	{
		k3d::filesystem::ifstream stream(document_path);
		k3d::xml::hide_progress progress;
		parse(xml, stream, document_path.native_utf8_string().raw(), progress);
	}
	catch(std::exception& e)
	{
		k3d::log() << error << e.what() << std::endl;
		return;
	}

	return_if_fail(xml.name == "k3dml");

	// Synchronize older documents
	k3d::upgrade_document(xml);

	// Setup context
	const k3d::filesystem::path root_path = document_path.branch_path();
	k3d::persistent_lookup persistent_lookup;
	k3d::ipersistent::load_context context(root_path, persistent_lookup);

	// Get document node list
	k3d::xml::element* const xml_document = k3d::xml::find_element(xml, "document");
	return_if_fail(xml_document);

	k3d::xml::element* xml_nodes = k3d::xml::find_element(*xml_document, "nodes");
	if(!xml_nodes)
		xml_nodes = k3d::xml::find_element(*xml_document, "objects");

	return_if_fail(xml_nodes);

	if(xml_nodes->name != "nodes")
		k3d::log() << warning << "Loading data from obsolete <" << xml_nodes->name << "> tag ... re-save document to eliminate this warning" << std::endl;

	detail::document_nodes_t document_nodes;
	for(k3d::xml::element::elements_t::iterator xml_node = xml_nodes->children.begin(); xml_node != xml_nodes->children.end(); ++xml_node)
	{
		if(xml_node->name != "node" && xml_node->name != "object")
			continue;

		if(k3d::xml::attribute_value<bool>(*xml_node, "do_not_load", false))
			continue;

		const std::string name = k3d::xml::attribute_text(*xml_node, "name");
		document_nodes.push_back(detail::node_check_t(name));
	}

	// Run node list UI
	std::string node_prefix = "";
	{
		detail::merge_nodes_dialog dialog(document_nodes, node_prefix);
		if(Gtk::RESPONSE_OK != dialog.run())
			return;
	}

	// Load checked nodes
	k3d::inode_collection::nodes_t nodes;
	std::vector<k3d::ipersistent*> persistent_nodes;
	std::vector<k3d::xml::element*> node_storage;

	detail::document_nodes_t::const_iterator user_node = document_nodes.begin();
	for(k3d::xml::element::elements_t::iterator xml_node = xml_nodes->children.begin(); xml_node != xml_nodes->children.end(); ++xml_node, ++user_node)
	{
		return_if_fail(user_node != document_nodes.end());

		if(xml_node->name != "node" && xml_node->name != "object")
			continue;

		if(k3d::xml::attribute_value<bool>(*xml_node, "do_not_load", false))
			continue;

		if(xml_node->name != "node")
			k3d::log() << warning << "Loading data from obsolete <" << xml_node->name << "> tag ... re-save document to eliminate this warning" << std::endl;

		const std::string name = attribute_text(*xml_node, "name");
		return_if_fail(name == user_node->name);

		// Skip unchecked nodes
		if(!user_node->check)
			continue;

		const k3d::uuid class_id = k3d::xml::attribute_value<k3d::uuid>(*xml_node, "class", k3d::uuid::null());
		if(class_id == k3d::uuid::null())
		{
			k3d::log() << error << "node [" << name << "] with unspecified class ID will not be loaded" << std::endl;
			continue;
		}

		const k3d::ipersistent_lookup::id_type node_id = k3d::xml::attribute_value<k3d::ipersistent_lookup::id_type>(*xml_node, "id", 0);
		if(node_id == 0)
		{
			k3d::log() << error << "node [" << name << "] with unspecified ID will not be loaded" << std::endl;
			continue;
		}

		k3d::iplugin_factory* const plugin_factory = k3d::plugin(class_id);
		if(!plugin_factory)
		{
			k3d::log() << error << "node [" << name << "] with unknown class ID [" << class_id << "] will not be loaded" << std::endl;
			continue;
		}

		k3d::idocument_plugin_factory* const document_plugin_factory = dynamic_cast<k3d::idocument_plugin_factory*>(plugin_factory);
		if(!document_plugin_factory)
		{
			k3d::log() << error << "Non-document plugin [" << name << "] will not be loaded" << std::endl;
			continue;
		}

		k3d::inode* const node = document_plugin_factory->create_plugin(*plugin_factory, Document);
		if(!node)
		{
			k3d::log() << error << "Error creating node [" << name << "] instance" << std::endl;
			continue;
		}

		k3d::ipersistent* const persistent = dynamic_cast<k3d::ipersistent*>(node);
		if(!persistent)
		{
			k3d::log() << error << "node [" << name << "] does not support persistence" << std::endl;

			delete dynamic_cast<k3d::ideletable*>(node);
			continue;
		}

		k3d::undoable_new(dynamic_cast<k3d::ideletable*>(node), Document);

		nodes.push_back(node);
		persistent_nodes.push_back(persistent);
		node_storage.push_back(&(*xml_node));

		persistent_lookup.insert_lookup(node_id, node);
	}

	Document.nodes().add_nodes(nodes);

	for(unsigned long i = 0; i != persistent_nodes.size(); ++i)
	{
		persistent_nodes[i]->load(*node_storage[i], context);
		if(node_prefix.size())
		{
			k3d::inode* node = dynamic_cast<k3d::inode*>(persistent_nodes[i]);
			if(node)
				node->set_name(node_prefix + node->name());
		}
	}

	k3d::load_pipeline(Document, *xml_document, context);
}

} // namespace merge_nodes

} // namespace libk3dngui

