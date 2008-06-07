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

/** \file
	\author Tim Shead (tshead@k-3d.com)
*/

#include "button.h"
#include "entry.h"
#include "enumeration_chooser.h"
#include "messages.h"
#include "node_window.h"
#include "properties.h"
#include "widget_manip.h"

#include <k3d-i18n-config.h>
#include <k3dsdk/data.h>
#include <k3dsdk/inode.h>
#include <k3dsdk/ipersistent_collection.h>
#include <k3dsdk/iproperty_collection.h>
#include <k3dsdk/properties.h>
#include <k3dsdk/property_collection.h>
#include <k3dsdk/property_types.h>
#include <k3dsdk/property_types_ri.h>
#include <k3dsdk/string_cast.h>
#include <k3dsdk/tokens.h>
#include <k3dsdk/type_registry.h>
#include <k3dsdk/user_properties.h>

#include <gtkmm/box.h>
#include <gtkmm/buttonbox.h>
#include <gtkmm/label.h>
#include <gtkmm/stock.h>
#include <gtkmm/table.h>

#include <boost/format.hpp>
#include <boost/mpl/for_each.hpp>

using namespace libk3dngui;

namespace k3d
{

namespace ngui
{

namespace property
{

///////////////////////////////////////////////////////////////////////////////
// create_dialog

/// Implements a dialog for adding custom user properties to a node
class create_dialog :
	public node_window,
	public k3d::property_collection // needed for enumeration properties
{
	typedef node_window base;

public:
	create_dialog(k3d::inode& Object, k3d::icommand_node& Parent) :
		base(Object, Parent, "add_user_property"),
		m_property_type(init_owner(*this) + init_name("property_type") + init_label("") + init_description("") + init_value(string_t("generic_property")) + init_enumeration(property_type_values())),
		m_name(init_value(string_t(""))),
		m_label(init_value(string_t(""))),
		m_description(init_value(string_t(""))),

		m_generic_type(init_owner(*this) + init_name("user_type") + init_label("") + init_description("") + init_value(string_t("k3d::bool_t")) + init_enumeration(generic_type_values())),
		m_renderman_type(init_owner(*this) + init_name("renderman_type") + init_label("") + init_description("") + init_value(string_t("k3d::int32_t")) + init_enumeration(renderman_type_values())),
		m_renderman_name(init_value(string_t(""))),

		m_ignore_name_change(false),
		m_ignore_label_change(false),
		m_label_tracks_name(true),

		m_generic_type_label(0),
		m_renderman_type_label(0),
		m_renderman_name_label(0),
	
		m_generic_type_control(0),
		m_renderman_type_control(0),
		m_name_control(0),
		m_renderman_name_control(0)
	{
		set_title(_("Add User Property:"));
		set_role("add_user_property");
		set_position(Gtk::WIN_POS_CENTER);

		Gtk::Table* const table = new Gtk::Table(6, 2, false);
		table->set_col_spacings(4);
		
		table->attach(*Gtk::manage(new Gtk::Label(_("Property Type"))), 0, 1, 0, 1, Gtk::SHRINK, Gtk::SHRINK);
		table->attach(*Gtk::manage(new enumeration_chooser::control(*this, "property_type", enumeration_chooser::model(m_property_type))), 1, 2, 0, 1);

		m_generic_type_label = new Gtk::Label(_("Data Type"));
		m_generic_type_control = new enumeration_chooser::control(*this, "generic_type", enumeration_chooser::model(m_generic_type));
		table->attach(*Gtk::manage(m_generic_type_label), 0, 1, 1, 2, Gtk::SHRINK, Gtk::SHRINK);
		table->attach(*Gtk::manage(m_generic_type_control), 1, 2, 1, 2);
	
		m_renderman_type_label = new Gtk::Label(_("Data Type"));
		m_renderman_type_control = new enumeration_chooser::control(*this, "renderman_type", enumeration_chooser::model(m_renderman_type));
		table->attach(*Gtk::manage(m_renderman_type_label), 0, 1, 2, 3, Gtk::SHRINK, Gtk::SHRINK);
		table->attach(*Gtk::manage(m_renderman_type_control), 1, 2, 2, 3);

		m_renderman_name_label = new Gtk::Label(_("Attribute Name"));
		m_renderman_name_control = new entry::control(*this, "renderman_name", entry::model(m_renderman_name), 0);
		table->attach(*Gtk::manage(m_renderman_name_label), 0, 1, 4, 5, Gtk::SHRINK, Gtk::SHRINK);
		table->attach(*Gtk::manage(m_renderman_name_control), 1, 2, 4, 5);

		m_name_control = new entry::control(*this, "name", entry::model(m_name), 0)
			<< connect_signal_changed(sigc::mem_fun(*this, &create_dialog::on_name_changed));

		table->attach(*Gtk::manage(new Gtk::Label(_("Name"))), 0, 1, 5, 6, Gtk::SHRINK, Gtk::SHRINK);
		table->attach(*Gtk::manage(m_name_control), 1, 2, 5, 6);

		table->attach(*Gtk::manage(new Gtk::Label(_("Label"))), 0, 1, 6, 7, Gtk::SHRINK, Gtk::SHRINK);
		table->attach(*Gtk::manage(new entry::control(*this, "label", entry::model(m_label), 0)
			<< connect_signal_changed(sigc::mem_fun(*this, &create_dialog::on_label_changed))), 1, 2, 6, 7);

		table->attach(*Gtk::manage(new Gtk::Label(_("Description"))), 0, 1, 7, 8, Gtk::SHRINK, Gtk::SHRINK);
		table->attach(*Gtk::manage(new entry::control(*this, "description", entry::model(m_description), 0)), 1, 2, 7, 8);

		Gtk::HButtonBox* const button_box = new Gtk::HButtonBox(Gtk::BUTTONBOX_END);
		button_box->pack_start(*Gtk::manage(new button::control(*this, "cancel", _("Cancel"), Gtk::Stock::CANCEL)
			<< connect_button(sigc::mem_fun(*this, &create_dialog::close))));
		button_box->pack_start(*Gtk::manage(new button::control(*this, "add", _("Add"), Gtk::Stock::ADD)
			<< connect_button(sigc::mem_fun(*this, &create_dialog::on_add))));

		Gtk::VBox* const vbox = Gtk::manage(new Gtk::VBox(false, 10));
		vbox->set_border_width(10);
		vbox->pack_start(*Gtk::manage(new Gtk::Label(k3d::string_cast(boost::format(_("Add a user property to %1%:")) % node().name()))), Gtk::PACK_SHRINK);
		vbox->pack_start(*Gtk::manage(table), Gtk::PACK_SHRINK);
		vbox->pack_start(*Gtk::manage(button_box), Gtk::PACK_SHRINK);

		add(*vbox);

		show_all();

		m_property_type.changed_signal().connect(sigc::hide(sigc::mem_fun(*this, &create_dialog::on_property_type_changed)));
		on_property_type_changed();
	}

private:
	void on_property_type_changed()
	{
		const string_t property_type = m_property_type.internal_value();

		m_generic_type_label->property_visible().set_value(property_type == "generic_property");
		m_generic_type_control->property_visible().set_value(property_type == "generic_property");

		m_renderman_type_label->property_visible().set_value(property_type == "renderman_attribute" || property_type == "renderman_option");
		m_renderman_type_control->property_visible().set_value(property_type == "renderman_attribute" || property_type == "renderman_option");

		m_renderman_name_label->set_label(property_type == "renderman_attribute" ? _("Attribute Name") : _("Option Name"));
		m_renderman_name_label->property_visible().set_value(property_type == "renderman_attribute" || property_type == "renderman_option");
		m_renderman_name_control->property_visible().set_value(property_type == "renderman_attribute" || property_type == "renderman_option");
	}


	void on_name_changed()
	{
		if(m_ignore_name_change)
			return;

		m_ignore_name_change = true;

		string_t name_text = m_name_control->get_text();
		std::replace(name_text.begin(), name_text.end(), ' ', '_');
		m_name.set_value(name_text);

		m_ignore_name_change = false;

		if(m_label_tracks_name)
		{
			m_ignore_label_change = true;

			string_t label_text = name_text;

			bool capitalize = true;
			for(k3d::uint_t i = 0; i != label_text.size(); ++i)
			{
				if(capitalize)
				{
					label_text[i] = std::toupper(label_text[i]);
					capitalize = false;
				}

				if(label_text[i] == '_')
				{
					label_text[i] = ' ';
					capitalize = true;
				}
			}
			m_label.set_value(label_text);

			m_ignore_label_change = false;
		}
	}

	void on_label_changed()
	{
		if(m_ignore_label_change)
			return;

		m_label_tracks_name = false;
	}

	void on_add()
	{
		k3d::iproperty_collection* const property_collection = dynamic_cast<k3d::iproperty_collection*>(&node());
		return_if_fail(property_collection);

		k3d::ipersistent_collection* const persistent_collection = dynamic_cast<k3d::ipersistent_collection*>(&node());
		return_if_fail(persistent_collection);

		const string_t property_type = m_property_type.internal_value();

		const string_t name = m_name.internal_value();
		const string_t label = m_label.internal_value();
		const string_t description = m_description.internal_value();

		const string_t generic_type = m_generic_type.internal_value();

		const string_t renderman_type = m_renderman_type.internal_value();
		const string_t renderman_name = m_renderman_name.internal_value();

		if(name.empty())
		{
			error_message(_("You must provide a Name to create a property."));
			return;
		}

		if(label.empty())
		{
			error_message(_("You must provide a Label to create a property."));
			return;
		}

		if(property_type == "renderman_attribute" && renderman_name.empty())
		{
			error_message(_("You must provide an Attribute Name to create a RenderMan Attribute."));
			return;
		}

		if(property_type == "renderman_option" && renderman_name.empty())
		{
			error_message(_("You must provide an Option Name to create a RenderMan Option."));
			return;
		}

		k3d::record_state_change_set change_set(node().document(), "Add user property " + name, K3D_CHANGE_SET_CONTEXT);
		
		if(node().document().state_recorder().current_change_set())
			node().document().state_recorder().current_change_set()->record_old_state(new k3d::user::property_container(node()));

		iproperty* new_property = 0;
		if(property_type == "generic_property")
			new_property = k3d::property::create(node(), generic_type, name, label, description);
		else if(property_type == "renderman_attribute")
			new_property = k3d::property::ri::create_attribute(node(), renderman_type, renderman_name, name, label, description);
		else if(property_type == "renderman_option")
			new_property = k3d::property::ri::create_option(node(), renderman_type, renderman_name, name, label, description);

		if(new_property)
			undoable_new(new_property, node().document());

		if(node().document().state_recorder().current_change_set())
			node().document().state_recorder().current_change_set()->record_new_state(new k3d::user::property_container(node()));

		if(!new_property)
		{
			error_message(_("Internal error creating user property."));
			return;
		}

		close();
	}

	static const k3d::ienumeration_property::enumeration_values_t& property_type_values()
	{
		static k3d::ienumeration_property::enumeration_values_t values;
		if(values.empty())
		{
			values.push_back(k3d::ienumeration_property::enumeration_value_t("Generic Property", "generic_property",""));
			values.push_back(k3d::ienumeration_property::enumeration_value_t("RenderMan Attribute", "renderman_attribute",""));
			values.push_back(k3d::ienumeration_property::enumeration_value_t("RenderMan Option", "renderman_option", ""));
		}

		return values;
	}

	static const k3d::ienumeration_property::enumeration_values_t& generic_type_values()
	{
		static k3d::ienumeration_property::enumeration_values_t values;
		if(values.empty())
		{
			values.push_back(k3d::ienumeration_property::enumeration_value_t("Boolean", "k3d::bool_t",""));
			values.push_back(k3d::ienumeration_property::enumeration_value_t("Integer", "k3d::int32_t",""));
			values.push_back(k3d::ienumeration_property::enumeration_value_t("Scalar", "k3d::double_t", ""));
			values.push_back(k3d::ienumeration_property::enumeration_value_t("String", "k3d::string_t", ""));
			values.push_back(k3d::ienumeration_property::enumeration_value_t("3D Point", "k3d::point3", ""));
			values.push_back(k3d::ienumeration_property::enumeration_value_t("3D Vector", "k3d::vector3", ""));
			values.push_back(k3d::ienumeration_property::enumeration_value_t("3D Normal", "k3d::normal3", ""));
			values.push_back(k3d::ienumeration_property::enumeration_value_t("4D Point", "k3d::point4", ""));
			values.push_back(k3d::ienumeration_property::enumeration_value_t("4x4 Matrix", "k3d::matrix4", ""));
			values.push_back(k3d::ienumeration_property::enumeration_value_t("Color", "k3d::color", ""));
			values.push_back(k3d::ienumeration_property::enumeration_value_t("Mesh", "k3d::mesh*", ""));
			values.push_back(k3d::ienumeration_property::enumeration_value_t("OpenGL Mesh Painter", "k3d::gl::imesh_painter*", "")); 
			values.push_back(k3d::ienumeration_property::enumeration_value_t("RenderMan Mesh Painter", "k3d::ri::imesh_painter*", ""));
			values.push_back(k3d::ienumeration_property::enumeration_value_t("Material", "k3d::imaterial*", ""));
			values.push_back(k3d::ienumeration_property::enumeration_value_t("Filesystem Path", "k3d::filesystem::path", ""));
			values.push_back(k3d::ienumeration_property::enumeration_value_t("Generic Node", "k3d::inode*", ""));
		}

		return values;
	}

	static const k3d::ienumeration_property::enumeration_values_t& renderman_type_values()
	{
		static k3d::ienumeration_property::enumeration_values_t values;
		if(values.empty())
		{
			values.push_back(k3d::ienumeration_property::enumeration_value_t("Integer", "k3d::int32_t", ""));
			values.push_back(k3d::ienumeration_property::enumeration_value_t("Real", "k3d::double_t", ""));
			values.push_back(k3d::ienumeration_property::enumeration_value_t("String", "k3d::string_t", ""));
			values.push_back(k3d::ienumeration_property::enumeration_value_t("Point", "k3d::point3", ""));
			values.push_back(k3d::ienumeration_property::enumeration_value_t("Vector", "k3d::vector3", ""));
			values.push_back(k3d::ienumeration_property::enumeration_value_t("Normal", "k3d::normal3", ""));
			values.push_back(k3d::ienumeration_property::enumeration_value_t("HPoint", "k3d::point4", ""));
			values.push_back(k3d::ienumeration_property::enumeration_value_t("Matrix", "k3d::matrix4", ""));
			values.push_back(k3d::ienumeration_property::enumeration_value_t("Color", "k3d::color" ,""));
		}

		return values;
	}

	k3d_data(string_t, immutable_name, change_signal, no_undo, local_storage, no_constraint, enumeration_property, no_serialization) m_property_type;

	k3d_data(string_t, no_name, change_signal, no_undo, local_storage, no_constraint, no_property, no_serialization) m_name;
	k3d_data(string_t, no_name, change_signal, no_undo, local_storage, no_constraint, no_property, no_serialization) m_label;
	k3d_data(string_t, no_name, change_signal, no_undo, local_storage, no_constraint, no_property, no_serialization) m_description;

	k3d_data(string_t, immutable_name, change_signal, no_undo, local_storage, no_constraint, enumeration_property, no_serialization) m_generic_type;

	k3d_data(string_t, immutable_name, change_signal, no_undo, local_storage, no_constraint, enumeration_property, no_serialization) m_renderman_type;
	k3d_data(string_t, no_name, change_signal, no_undo, local_storage, no_constraint, no_property, no_serialization) m_renderman_name;
	

	k3d::bool_t m_ignore_name_change;
	k3d::bool_t m_ignore_label_change;
	k3d::bool_t m_label_tracks_name;

	Gtk::Label* m_generic_type_label;
	Gtk::Label* m_renderman_type_label;
	Gtk::Label* m_renderman_name_label;

	enumeration_chooser::control* m_generic_type_control;
	enumeration_chooser::control* m_renderman_type_control;
	entry::control* m_name_control;
	entry::control* m_renderman_name_control;
};

////////////////////////////////////////////////////////////////////////////////////
// create

void create(inode& Node, icommand_node& Parent)
{
	new create_dialog(Node, Parent);
}

} // namespace property

} // namespace ngui

} // namespace k3d

