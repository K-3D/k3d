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
		\brief Implements k3d::node_chooser, which provides a user interface for either creating new, or choosing existing, K-3D nodes
		\author Tim Shead (tshead@k-3d.com)
*/

#include <gtkmm/arrow.h>
#include <gtkmm/button.h>
#include <gtkmm/label.h>
#include <gtkmm/menu.h>
#include <gtkmm/menuitem.h>

#include <gtk/gtkmain.h>

#include "document_state.h"
#include "interactive.h"
#include "node_chooser.h"
#include "utility.h"

#include <k3d-i18n-config.h>
#include <k3dsdk/application.h>
#include <k3dsdk/plugin.h>
#include <k3dsdk/iapplication.h>
#include <k3dsdk/idocument.h>
#include <k3dsdk/inode.h>
#include <k3dsdk/inode_property.h>
#include <k3dsdk/ipipeline.h>
#include <k3dsdk/iplugin_factory.h>
#include <k3dsdk/istate_recorder.h>
#include <k3dsdk/nodes.h>
#include <k3dsdk/state_change_set.h>
#include <k3dsdk/string_cast.h>

namespace libk3dngui
{

namespace node_chooser
{

namespace detail
{

/// Provides an implementation of k3d::node_chooser::iselection_filter that filters based on a k3d::iproperty node
class property_filter :
	public iselection_filter
{
public:
	property_filter(k3d::iproperty& Property) :
		m_node_property(dynamic_cast<k3d::inode_property*>(&Property))
	{
	}

	bool allow_none()
	{
		return m_node_property && m_node_property->property_allow_none();
	}

	bool allow(k3d::iplugin_factory& Factory)
	{
		return m_node_property && m_node_property->property_allow(Factory);
	}

	bool allow(k3d::inode& Object)
	{
		return m_node_property && m_node_property->property_allow(Object);
	}

private:
	k3d::inode_property* const m_node_property;
};

struct sort_by_name
{
	bool operator()(k3d::iplugin_factory* LHS, k3d::iplugin_factory* RHS)
	{
		return LHS->name() < RHS->name();
	}

	bool operator()(k3d::inode* LHS, k3d::inode* RHS)
	{
		return LHS->name() < RHS->name();
	}
};

} // namespace detail

/////////////////////////////////////////////////////////////////////////////
// control

control::control(k3d::icommand_node& Parent, const std::string& Name, std::auto_ptr<idata_proxy> Data, std::auto_ptr<iselection_filter> Filter) :
	base(false, 0),
	ui_component(Name, &Parent),
	m_data(Data),
	m_filter(Filter),
	m_label(new Gtk::Label()),
	m_menu_button(new Gtk::Button()),
	m_edit_button(new Gtk::Button())
{
	m_menu_button->add(*Gtk::manage(m_label));
	m_menu_button->signal_clicked().connect(sigc::mem_fun(*this, &control::on_choose));
	pack_start(*manage(m_menu_button), Gtk::PACK_EXPAND_WIDGET);

	m_edit_button->add(*Gtk::manage(new Gtk::Arrow(Gtk::ARROW_RIGHT, Gtk::SHADOW_NONE)));
	m_edit_button->signal_clicked().connect(sigc::mem_fun(*this, &control::on_edit));
	pack_start(*manage(m_edit_button), Gtk::PACK_SHRINK);

	data_changed(0);

	if(m_data.get())
	{
		m_data->document().document().nodes().add_nodes_signal().connect(sigc::mem_fun(*this, &control::on_nodes_added));
		m_data->document().document().nodes().remove_nodes_signal().connect(sigc::mem_fun(*this, &control::on_nodes_removed));
		m_data->changed_signal().connect(sigc::mem_fun(*this, &control::data_changed));
	}
}

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

void control::on_nodes_added(const k3d::inode_collection::nodes_t&)
{
	reset_menu();
}

void control::on_nodes_removed(const k3d::inode_collection::nodes_t&)
{
	reset_menu();
}

void control::data_changed(k3d::iunknown*)
{
	return_if_fail(m_data.get());

	m_label->set_text(m_data->node() ? m_data->node()->name() : _("--None--"));
	reset_menu();
}

void control::reset_menu()
{
	m_menu.reset();
	m_menu_item_create.clear();
	m_menu_item_select.clear();
}

void control::on_choose()
{
	return_if_fail(m_data.get());
	return_if_fail(m_filter.get());

	// Build the menu on-demand ...
	if(!m_menu.get())
	{
		m_menu.reset(new Gtk::Menu());

		// Insert a "none" choice ...
		if(m_filter->allow_none())
		{
			Gtk::MenuItem* const menu_item = new Gtk::MenuItem(_("--None--"));
			m_menu_item_select[std::string()] = menu_item;
			menu_item->signal_activate().connect(sigc::mem_fun(*this, &control::on_select_none));
			m_menu->items().push_back(*manage(menu_item));
		}

		// Insert choices for creating new nodes ...
		std::vector<k3d::iplugin_factory*> sorted_factories(k3d::application().plugins().begin(), k3d::application().plugins().end());
		std::sort(sorted_factories.begin(), sorted_factories.end(), detail::sort_by_name());
		for(std::vector<k3d::iplugin_factory*>::const_iterator factory = sorted_factories.begin(); factory != sorted_factories.end(); ++factory)
		{
			// Filter choices ...
			if(!m_filter->allow(**factory))
				continue;

			// Create a "new" menu item for each factory ...
			const std::string label = k3d::string_cast(boost::format(_("--New %1%")) % (*factory)->name());
			Gtk::MenuItem* const menu_item = new Gtk::MenuItem(label);
			m_menu_item_create[(*factory)->name()] = menu_item;
			menu_item->signal_activate().connect(sigc::bind(sigc::mem_fun(*this, &control::on_create_node), *factory));
			m_menu->items().push_back(*manage(menu_item));
		}

		// Insert choices for selecting existing nodes ...
		k3d::inode_collection::nodes_t sorted_nodes = m_data->document().document().nodes().collection();
		std::sort(sorted_nodes.begin(), sorted_nodes.end(), detail::sort_by_name());
		for(k3d::inode_collection::nodes_t::const_iterator handle = sorted_nodes.begin(); handle != sorted_nodes.end(); ++handle)
		{
			k3d::inode* const node = *handle;

			// Filter choices ...
			if(!m_filter->allow(*node))
				continue;

			// Create a menu item for each node ...
			Gtk::MenuItem* const menu_item = new Gtk::MenuItem(node->name());
			m_menu_item_select[node->name()] = menu_item;
			menu_item->signal_activate().connect(sigc::bind(sigc::mem_fun(*this, &control::on_select_node), node));
			m_menu->items().push_back(*manage(menu_item));
		}
	}

	m_menu->show_all();
	m_menu->popup(1, gtk_get_current_event_time());
}

void control::on_select_none()
{
	record_command("select");

	return_if_fail(m_data.get());

	if(m_data->state_recorder)
		m_data->state_recorder->start_recording(k3d::create_state_change_set(K3D_CHANGE_SET_CONTEXT), K3D_CHANGE_SET_CONTEXT);

	m_data->set_node(0);

	if(m_data->state_recorder)
		m_data->state_recorder->commit_change_set(m_data->state_recorder->stop_recording(K3D_CHANGE_SET_CONTEXT), _("Select None"), K3D_CHANGE_SET_CONTEXT);
}

void control::on_create_node(k3d::iplugin_factory* const Factory)
{
	return_if_fail(Factory);

	record_command("create", Factory->name());

	return_if_fail(m_data.get());

	if(m_data->state_recorder)
		m_data->state_recorder->start_recording(k3d::create_state_change_set(K3D_CHANGE_SET_CONTEXT), K3D_CHANGE_SET_CONTEXT);

	k3d::inode* const node = k3d::plugin::create<k3d::inode>(*Factory, m_data->document().document(), k3d::unique_name(m_data->document().document().nodes(), Factory->name()));
	assert_warning(node);

	m_data->set_node(node);

	if(m_data->state_recorder)
		m_data->state_recorder->commit_change_set(m_data->state_recorder->stop_recording(K3D_CHANGE_SET_CONTEXT), k3d::string_cast(boost::format(_("Create %1%")) % Factory->name()), K3D_CHANGE_SET_CONTEXT);

	if(node)
		m_data->document().view_node_properties_signal().emit(node);
}

void control::on_select_node(k3d::inode* const Object)
{
	return_if_fail(Object);

	record_command("select", Object->name());

	return_if_fail(m_data.get());

	if(m_data->state_recorder)
		m_data->state_recorder->start_recording(k3d::create_state_change_set(K3D_CHANGE_SET_CONTEXT), K3D_CHANGE_SET_CONTEXT);

	m_data->set_node(Object);

	if(m_data->state_recorder)
		m_data->state_recorder->commit_change_set(m_data->state_recorder->stop_recording(K3D_CHANGE_SET_CONTEXT), k3d::string_cast(boost::format(_("Select %1%")) % Object->name()), K3D_CHANGE_SET_CONTEXT);
}

void control::on_edit()
{
	record_command("edit");
	return_if_fail(m_data.get());

	if(!m_data->node())
		return;

	m_data->document().view_node_properties_signal().emit(m_data->node());
}

std::auto_ptr<iselection_filter> filter(k3d::iproperty& Data)
{
	return std::auto_ptr<iselection_filter>(new detail::property_filter(Data));
}

} // namespace node_chooser

} // namespace libk3dngui

