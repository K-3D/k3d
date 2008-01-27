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
*/

#include <gtkmm/button.h>
#include <gtkmm/buttonbox.h>
#include <gtkmm/label.h>
#include <gtkmm/liststore.h>
#include <gtkmm/scrolledwindow.h>
#include <gtkmm/treeview.h>

#include "application_window.h"
#include "asynchronous_update.h"
#include "button.h"
#include "icons.h"
#include "node_collection_chooser.h"
#include "widget_manip.h"

#include <k3d-i18n-config.h>
#include <k3dsdk/idocument.h>
#include <k3dsdk/inode.h>
#include <k3dsdk/inode_collection.h>
#include <k3dsdk/inode_collection_property.h>
#include <k3dsdk/iplugin_factory.h>
#include <k3dsdk/iproperty.h>
#include <k3dsdk/istate_recorder.h>
#include <k3dsdk/properties.h>
#include <k3dsdk/result.h>
#include <k3dsdk/state_change_set.h>
#include <k3dsdk/types.h>
#include <k3dsdk/string_cast.h>

#include <boost/shared_ptr.hpp>

namespace libk3dngui
{

namespace node_collection_chooser
{

/////////////////////////////////////////////////////////////////////////////
// list_window

class list_window :
	public application_window,
	public asynchronous_update
{
public:
	list_window(k3d::icommand_node* const Parent, const k3d::string_t& Name, const boost::shared_ptr<imodel>& Model, k3d::istate_recorder* StateRecorder) :
		application_window(Name, Parent),
		m_model(Model),
		m_state_recorder(StateRecorder),
		m_block_update(false),
		m_block_toggle(false)
	{
		set_title(m_model->label());
		set_role("node_collection_chooser");
		resize(400, 400);
		set_position(Gtk::WIN_POS_CENTER);

		m_model->connect_changed_signal(sigc::mem_fun(*this, &list_window::on_model_changed));

		Gtk::ScrolledWindow* const scrolled_window = new Gtk::ScrolledWindow();
		scrolled_window->set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
		scrolled_window->add(m_view);

		m_view_model = Gtk::ListStore::create(m_columns);
		m_view_model->signal_row_changed().connect(sigc::mem_fun(*this, &list_window::on_selection_toggled));

		m_view.set_model(m_view_model);
		m_view.set_headers_visible(true);
		m_view.set_reorderable(false);
		m_view.get_selection()->set_mode(Gtk::SELECTION_EXTENDED);

		m_view.append_column("", m_columns.icon);
		m_view.append_column(_("Name"), m_columns.name);
		m_view.append_column_editable(_("Selected"), m_columns.selected);

		Gtk::HButtonBox* const hbox = new Gtk::HButtonBox(Gtk::BUTTONBOX_END);
		hbox->pack_start(*Gtk::manage(
			new button::control(*this, "select_all", _("Select All"))
				<< connect_button(sigc::mem_fun(*this, &list_window::on_select_all))
				), Gtk::PACK_SHRINK);
		hbox->pack_start(*Gtk::manage(
			new button::control(*this, "deselect_all", _("Deselect All"))
				<< connect_button(sigc::mem_fun(*this, &list_window::on_deselect_all))
				), Gtk::PACK_SHRINK);
		hbox->pack_start(*Gtk::manage(
			new button::control(*this, "toggle_selected", _("Toggle Selected"))
				<< connect_button(sigc::mem_fun(*this, &list_window::on_toggle_selected))
				), Gtk::PACK_SHRINK);

		Gtk::VBox* const vbox = new Gtk::VBox();
		vbox->pack_start(*Gtk::manage(hbox), Gtk::PACK_SHRINK);
		vbox->pack_start(*Gtk::manage(scrolled_window), Gtk::PACK_EXPAND_WIDGET);

		add(*Gtk::manage(vbox));

		schedule_update();
	}

private:
	/// Called to refresh the contents of the list view ...
	void on_update()
	{
		const k3d::inode_collection_property::nodes_t available_nodes = m_model->available_nodes();
		const k3d::inode_collection_property::nodes_t selected_nodes = m_model->selected_nodes();

		m_block_toggle = true;

		m_view_model->clear();

		for(k3d::inode_collection_property::nodes_t::const_iterator node = available_nodes.begin(); node != available_nodes.end(); ++node)
		{
			Gtk::TreeRow row = *m_view_model->append();
			row[m_columns.node] = (*node);
			row[m_columns.icon] = quiet_load_icon((**node).factory().name(), Gtk::ICON_SIZE_MENU);
			row[m_columns.name] = (**node).name();
			row[m_columns.selected] = std::count(selected_nodes.begin(), selected_nodes.end(), *node);
		}

		m_block_toggle = false;
	}

	/// Called whenever a checkbox changes state ...
	void on_selection_toggled(const Gtk::TreeModel::Path& path, const Gtk::TreeModel::iterator& iterator)
	{
		if(m_block_toggle)
			return;

		if(m_state_recorder)
			m_state_recorder->start_recording(k3d::create_state_change_set(K3D_CHANGE_SET_CONTEXT), K3D_CHANGE_SET_CONTEXT);

		m_block_update = true;

		Gtk::TreeRow row = *iterator;
		k3d::inode* const node = row[m_columns.node];
		const k3d::bool_t selected = row[m_columns.selected];

		k3d::inode_collection_property::nodes_t selected_nodes = m_model->selected_nodes();
		if(selected)
		{
			selected_nodes.push_back(node);
		}
		else
		{
			selected_nodes.erase(std::remove(selected_nodes.begin(), selected_nodes.end(), node), selected_nodes.end());
		}
		m_model->set_selected_nodes(selected_nodes);

		m_block_update = false;

		if(m_state_recorder)
			m_state_recorder->commit_change_set(m_state_recorder->stop_recording(K3D_CHANGE_SET_CONTEXT), k3d::string_cast(boost::format(_("Change %1%")) % m_model->label()), K3D_CHANGE_SET_CONTEXT);
	}

	/// Called when the user presses the "toggle selected" button
	void on_toggle_selected()
	{
		if(m_state_recorder)
			m_state_recorder->start_recording(k3d::create_state_change_set(K3D_CHANGE_SET_CONTEXT), K3D_CHANGE_SET_CONTEXT);

		k3d::inode_collection_property::nodes_t selected_nodes = m_model->selected_nodes();
		const std::vector<Gtk::TreePath> selected_rows = m_view.get_selection()->get_selected_rows();
		for(int i = 0; i != selected_rows.size(); ++i)
		{
			Gtk::TreeRow row = *m_view_model->get_iter(selected_rows[i]);
			k3d::inode* const node = row[m_columns.node];
			const k3d::bool_t selected = !row[m_columns.selected];

			if(selected)
			{
				selected_nodes.push_back(node);
			}
			else
			{
				selected_nodes.erase(std::remove(selected_nodes.begin(), selected_nodes.end(), node), selected_nodes.end());
			}
		}
		m_model->set_selected_nodes(selected_nodes);

		if(m_state_recorder)
			m_state_recorder->commit_change_set(m_state_recorder->stop_recording(K3D_CHANGE_SET_CONTEXT), k3d::string_cast(boost::format(_("Toggle Selected %1%")) % m_model->label()), K3D_CHANGE_SET_CONTEXT);
	}

	/// Called whenever the underlying data model changes ...
	void on_model_changed()
	{
		if(m_block_update)
			return;

		schedule_update();
	}

	void on_select_all()
	{
		m_view.get_selection()->select_all();
	}

	void on_deselect_all()
	{
		m_view.get_selection()->unselect_all();
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
			add(selected);
		}

		Gtk::TreeModelColumn<k3d::inode*> node;
		Gtk::TreeModelColumn<Glib::RefPtr<Gdk::Pixbuf> > icon;
		Gtk::TreeModelColumn<Glib::ustring> name;
		Gtk::TreeModelColumn<k3d::bool_t> selected;
	};

	const boost::shared_ptr<imodel> m_model;
	k3d::istate_recorder* const m_state_recorder;
	columns m_columns;
	Glib::RefPtr<Gtk::ListStore> m_view_model;
	Gtk::TreeView m_view;
	bool m_block_update;
	bool m_block_toggle;
};

/////////////////////////////////////////////////////////////////////////////
// property_model

class property_model :
	public imodel
{
public:
	property_model(k3d::iproperty& Property) :
		m_property(Property)
	{
		if(m_property.property_node())
		{
			m_property.property_node()->document().nodes().add_nodes_signal().connect(sigc::hide(m_changed_signal.make_slot()));
			m_property.property_node()->document().nodes().remove_nodes_signal().connect(sigc::hide(m_changed_signal.make_slot()));
			m_property.property_node()->document().nodes().rename_node_signal().connect(sigc::hide(m_changed_signal.make_slot()));
		}

		m_property.property_changed_signal().connect(sigc::hide(m_changed_signal.make_slot()));
	}

	const Glib::ustring label()
	{
		Glib::ustring result = m_property.property_label();

		if(m_property.property_node())
			result = m_property.property_node()->name() + " " + result;

		return result;
	}

	const k3d::inode_collection_property::nodes_t available_nodes()
	{
		if(m_property.property_node())
		{
			const k3d::inode_collection_property::nodes_t all_nodes = m_property.property_node()->document().nodes().collection();
			if(k3d::inode_collection_property* const node_collection_property = dynamic_cast<k3d::inode_collection_property*>(&m_property))
			{
				k3d::inode_collection_property::nodes_t filtered_nodes;
				for(k3d::inode_collection_property::nodes_t::const_iterator node = all_nodes.begin(); node != all_nodes.end(); ++node)
				{
					if(node_collection_property->property_allow(**node))
						filtered_nodes.push_back(*node);
				}

				return filtered_nodes;
			}
			else
			{
				return all_nodes;
			}
		}

		return k3d::inode_collection_property::nodes_t();
	}

	const k3d::inode_collection_property::nodes_t selected_nodes()
	{
		return k3d::property::internal_value<k3d::inode_collection_property::nodes_t>(m_property);
	}

	void set_selected_nodes(const k3d::inode_collection_property::nodes_t& Value)
	{
		return_if_fail(k3d::property::set_internal_value(m_property, Value));
	}

	sigc::connection connect_changed_signal(const sigc::slot<void>& Slot)
	{
		return m_changed_signal.connect(Slot);
	}

private:
	k3d::iproperty& m_property;
	sigc::signal<void> m_changed_signal;
};

/////////////////////////////////////////////////////////////////////////////
// model

imodel* const model(k3d::iproperty& Property)
{
	return new property_model(Property);
}

/////////////////////////////////////////////////////////////////////////////
// control::implementation

class control::implementation
{
public:
	implementation(imodel* const Model, k3d::istate_recorder* StateRecorder) :
		m_model(Model),
		m_state_recorder(StateRecorder)
	{
		m_delete_signal.emit();
	}

	const boost::shared_ptr<imodel> m_model;
	k3d::istate_recorder* const m_state_recorder;
	sigc::signal<void> m_delete_signal;
};

/////////////////////////////////////////////////////////////////////////////
// control

control::control(k3d::icommand_node& Parent, const std::string& Name, imodel* const Model, k3d::istate_recorder* StateRecorder) :
	base(false, 0),
	ui_component(Name, &Parent),
	m_implementation(new implementation(Model, StateRecorder))
{
	Gtk::Button* const edit_button = new Gtk::Button(_("Edit"));
	edit_button->signal_clicked().connect(sigc::mem_fun(*this, &control::on_edit));
	pack_start(*manage(edit_button), Gtk::PACK_SHRINK);
}

control::~control()
{
	delete m_implementation;
}

/*
const k3d::icommand_node::result control::execute_command(const std::string& Command, const std::string& Arguments)
{
	if(Command == "select")
	{
		interactive::activate(*m_menu_button);
		handle_pending_events();
		return_val_if_fail(m_menu_item_select.count(Arguments), RESULT_ERROR);
		interactive::activate(*m_menu_item_select[Arguments]);
		return RESULT_CONTINUE;
	}
	else if(Command == "create")
	{
		interactive::activate(*m_menu_button);
		handle_pending_events();
		return_val_if_fail(m_menu_item_create.count(Arguments), RESULT_ERROR);
		interactive::activate(*m_menu_item_create[Arguments]);
		return RESULT_CONTINUE;
	}
	else if(Command == "edit")
	{
		interactive::activate(*m_edit_button);
		return RESULT_CONTINUE;
	}

	return ui_component::execute_command(Command, Arguments);
}
*/

void control::on_edit()
{
	record_command("edit");
	return_if_fail(m_implementation->m_model.get());

	list_window* const window = new list_window(this, "edit", m_implementation->m_model, m_implementation->m_state_recorder);
	m_implementation->m_delete_signal.connect(sigc::mem_fun(*window, &list_window::close));
	window->show_all();
}

} // namespace node_collection_chooser

} // namespace libk3dngui

