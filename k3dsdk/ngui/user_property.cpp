// K-3D
// Copyright (c) 1995-2004, Timothy M. Shead
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
#include "user_property.h"
#include "widget_manip.h"

#include <k3d-i18n-config.h>
#include <k3dsdk/aqsis_properties.h>
#include <k3dsdk/inode.h>
#include <k3dsdk/ipersistent_container.h>
#include <k3dsdk/iproperty_collection.h>
#include <k3dsdk/properties_ri.h>
#include <k3dsdk/string_cast.h>
#include <k3dsdk/tokens.h>
#include <k3dsdk/user_properties.h>

#include <gtkmm/box.h>
#include <gtkmm/buttonbox.h>
#include <gtkmm/combobox.h>
#include <gtkmm/entry.h>
#include <gtkmm/label.h>
#include <gtkmm/stock.h>
#include <gtkmm/table.h>

#include <boost/format.hpp>

namespace libk3dngui
{

///////////////////////////////////////////////////////////////////////////////
// add_user_property

add_user_property::add_user_property(k3d::inode& Object, k3d::icommand_node& Parent) :
	base(Object, Parent, "add_user_property"),
	m_parameter_list_label(new Gtk::Label(_("Parameter List"))),
	m_user_type_label(new Gtk::Label(_("User Type"))),
	m_ri_type_label(new Gtk::Label(_("Renderman Type"))),
	m_property_type_label(new Gtk::Label(_("Property Type"))),
	m_parameter_list_control(0),
	m_parameter_list(init_value(std::string(""))),
	m_name(init_value(std::string(""))),
	m_label(init_value(std::string(""))),
	m_description(init_value(std::string(""))),
	m_user_type(init_owner(*this) + init_name("user_type") + init_label("") + init_description("") + init_value(k3d_bool_t) + init_enumeration(user_types_values())),
	m_ri_type(init_owner(*this) + init_name("ri_type") + init_label("") + init_description("") + init_value(k3d_ri_integer) + init_enumeration(ri_types_values())),
	m_property_type(init_owner(*this) + init_name("property_type") + init_label("") + init_description("") + init_value(USER) + init_enumeration(property_types_values()))
{
	m_parameter_list_control = new entry::control(*this, "list", entry::proxy(m_parameter_list));
	m_user_type_control = new enumeration_chooser::control(*this, "user_type", enumeration_chooser::proxy(static_cast<k3d::iproperty&>(m_user_type)));
	m_ri_type_control = new enumeration_chooser::control(*this, "ri_type", enumeration_chooser::proxy(static_cast<k3d::iproperty&>(m_ri_type)));
	m_property_type_control = new enumeration_chooser::control(*this, "property_type", enumeration_chooser::proxy(static_cast<k3d::iproperty&>(m_property_type)));

	set_title(_("Add User Property:"));
	set_role("add_user_property");
	set_position(Gtk::WIN_POS_CENTER);

	Gtk::Table* const table = Gtk::manage(new Gtk::Table(6, 2, false));
	table->set_col_spacings(4);

	table->attach(*Gtk::manage(m_property_type_label), 0, 1, 0, 1, Gtk::SHRINK, Gtk::SHRINK);
	table->attach(*Gtk::manage(m_property_type_control), 1, 2, 0, 1);

	table->attach(*Gtk::manage(m_user_type_label), 0, 1, 1, 2, Gtk::SHRINK, Gtk::SHRINK);
	table->attach(*Gtk::manage(m_user_type_control), 1, 2, 1, 2);
	
	table->attach(*Gtk::manage(m_ri_type_label), 0, 1, 1, 2, Gtk::SHRINK, Gtk::SHRINK);
	table->attach(*Gtk::manage(m_ri_type_control), 1, 2, 1, 2);

	table->attach(*Gtk::manage(m_parameter_list_label), 0, 1, 2, 3, Gtk::SHRINK, Gtk::SHRINK);
	table->attach(*Gtk::manage(m_parameter_list_control), 1, 2, 2, 3);

	table->attach(*Gtk::manage(new Gtk::Label(_("Name"))), 0, 1, 3, 4, Gtk::SHRINK, Gtk::SHRINK);
	table->attach(*Gtk::manage(new entry::control(*this, "name", entry::proxy(m_name))), 1, 2, 3, 4);

	table->attach(*Gtk::manage(new Gtk::Label(_("Label"))), 0, 1, 4, 5, Gtk::SHRINK, Gtk::SHRINK);
	table->attach(*Gtk::manage(new entry::control(*this, "label", entry::proxy(m_label))), 1, 2, 4, 5);

	table->attach(*Gtk::manage(new Gtk::Label(_("Description"))), 0, 1, 5, 6, Gtk::SHRINK, Gtk::SHRINK);
	table->attach(*Gtk::manage(new entry::control(*this, "description", entry::proxy(m_description))), 1, 2, 5, 6);

	Gtk::HButtonBox* const box = new Gtk::HButtonBox(Gtk::BUTTONBOX_END);
	box->pack_start(*Gtk::manage(new button::control(*this, "cancel", _("Cancel"), Gtk::Stock::CANCEL)
		<< connect_button(sigc::mem_fun(*this, &add_user_property::close))));
	box->pack_start(*Gtk::manage(new button::control(*this, "add", _("Add"), Gtk::Stock::ADD)
		<< connect_button(sigc::mem_fun(*this, &add_user_property::on_add))));

	Gtk::VBox* const vbox = Gtk::manage(new Gtk::VBox(false, 10));
	vbox->set_border_width(10);
	vbox->pack_start(*Gtk::manage(new Gtk::Label(k3d::string_cast(boost::format(_("Add a user property to %1%:")) % node().name()))), Gtk::PACK_SHRINK);
	vbox->pack_start(*table, Gtk::PACK_SHRINK);
	vbox->pack_start(*box, Gtk::PACK_SHRINK);

	add(*vbox);
	
	show_all();

	m_property_type.changed_signal().connect(sigc::mem_fun(*this, &add_user_property::on_property_type_changed));
	on_property_type_changed(0);
}

void add_user_property::on_property_type_changed(k3d::iunknown* Hint)
{
	switch(m_property_type.internal_value())
	{
		case USER:
			m_parameter_list_label->hide();
			m_parameter_list_control->hide();
			m_user_type_label->show();
			m_user_type_control->show();
			m_ri_type_label->hide();
			m_ri_type_control->hide();
			break;
		case OPTION:
		case ATTRIBUTE:
			m_user_type_label->hide();
			m_user_type_control->hide();
			m_ri_type_label->show();
			m_ri_type_control->show();
			m_parameter_list_label->show();
			m_parameter_list_control->show();
			break;
	}
}

void add_user_property::on_add()
{
	k3d::iproperty_collection* const property_collection = dynamic_cast<k3d::iproperty_collection*>(&node());
	return_if_fail(property_collection);

	k3d::ipersistent_container* const persistent_container = dynamic_cast<k3d::ipersistent_container*>(&node());
	return_if_fail(persistent_container);

	const property_types_t property_type = m_property_type.internal_value();
	const user_types_t user_type = m_user_type.internal_value();
	const ri_types_t ri_type = m_ri_type.internal_value();
	const std::string parameter_list = m_parameter_list.internal_value();
	const std::string name = m_name.internal_value();
	const std::string label = m_label.internal_value();
	const std::string description = m_description.internal_value();

	k3d::record_state_change_set change_set(node().document(), "Add user property " + name, K3D_CHANGE_SET_CONTEXT);
	
	if(node().document().state_recorder().current_change_set())
		node().document().state_recorder().current_change_set()->record_old_state(new k3d::user::property_container(node()));
		
	if(property_type == USER)
	{
		if(user_type == k3d_bool_t)
			k3d::undoable_new(k3d::user::create_property<k3d::user::bool_property>(name, label, description, node().document(), *property_collection, *persistent_container, &node(), false), node().document());
		else if(user_type == k3d_double_t)
			k3d::undoable_new(k3d::user::create_property<k3d::user::double_property>(name, label, description, node().document(), *property_collection, *persistent_container, &node(), 0.0), node().document());
		else if(user_type == k3d_string_t)
			k3d::undoable_new(k3d::user::create_property<k3d::user::string_property>(name, label, description, node().document(), *property_collection, *persistent_container, &node(), std::string()), node().document());
		else if(user_type == k3d_point3)
			k3d::undoable_new(k3d::user::create_property<k3d::user::point3_property>(name, label, description, node().document(), *property_collection, *persistent_container, &node(), k3d::point3(0, 0, 0)), node().document());
		else if(user_type == k3d_vector3)
			k3d::undoable_new(k3d::user::create_property<k3d::user::vector3_property>(name, label, description, node().document(), *property_collection, *persistent_container, &node(), k3d::vector3(0, 0, 0)), node().document());
		else if(user_type == k3d_normal3)
			k3d::undoable_new(k3d::user::create_property<k3d::user::normal3_property>(name, label, description, node().document(), *property_collection, *persistent_container, &node(), k3d::normal3(0, 0, 0)), node().document());
		else if(user_type == k3d_point4)
			k3d::undoable_new(k3d::user::create_property<k3d::user::point4_property>(name, label, description, node().document(), *property_collection, *persistent_container, &node(), k3d::point4(0, 0, 0, 0)), node().document());
		else if(user_type == k3d_matrix4)
			k3d::undoable_new(k3d::user::create_property<k3d::user::matrix4_property>(name, label, description, node().document(), *property_collection, *persistent_container, &node(), k3d::identity3D()), node().document());
		else if(user_type == k3d_color)
			k3d::undoable_new(k3d::user::create_property<k3d::user::color_property>(name, label, description, node().document(), *property_collection, *persistent_container, &node(), k3d::color(1, 1, 1)), node().document());
		else if(user_type == k3d_mesh)
			k3d::undoable_new(k3d::user::create_property<k3d::user::mesh_property>(name, label, description, node().document(), *property_collection, *persistent_container, &node(), static_cast<k3d::mesh*>(0)), node().document());
		else if(user_type == k3d_gl_imesh_painter)
			k3d::undoable_new(k3d::user::create_property<k3d::user::gl_mesh_painter_property>(name, label, description, node().document(), *property_collection, *persistent_container, &node(), static_cast<k3d::gl::imesh_painter*>(0)), node().document());
		else if(user_type == k3d_ri_imesh_painter)
			k3d::undoable_new(k3d::user::create_property<k3d::user::ri_mesh_painter_property>(name, label, description, node().document(), *property_collection, *persistent_container, &node(), static_cast<k3d::ri::imesh_painter*>(0)), node().document());
		else if(user_type == k3d_aqsis_idisplacement_layer)
			k3d::undoable_new(k3d::user::create_property<k3d::aqsis::displacement_layer_connection_property>(name, label, description, node().document(), *property_collection, *persistent_container, &node(), static_cast<k3d::inode*>(0)), node().document());
		else if(user_type == k3d_aqsis_isurface_layer)
			k3d::undoable_new(k3d::user::create_property<k3d::aqsis::surface_layer_connection_property>(name, label, description, node().document(), *property_collection, *persistent_container, &node(), static_cast<k3d::inode*>(0)), node().document());
		else		
			k3d::log() << error << k3d_file_reference << ": unknown user property value type: [" << user_type << "]" << std::endl;
	}
	else
	{
		const k3d::irenderman_property::parameter_type_t parameter_type =
			(property_type == ATTRIBUTE) ? k3d::irenderman_property::ATTRIBUTE : k3d::irenderman_property::OPTION;

		if(ri_type == k3d_ri_integer)
			k3d::undoable_new(k3d::ri::create_property<k3d::ri::integer_property>(parameter_type, parameter_list, name, label, description, node().document(), *property_collection, *persistent_container, &node(), 0), node().document());
		else if(ri_type == k3d_ri_real)
			k3d::undoable_new(k3d::ri::create_property<k3d::ri::real_property>(parameter_type, parameter_list, name, label, description, node().document(), *property_collection, *persistent_container, &node(), 0.0), node().document());
		else if(ri_type == k3d_ri_string)
			k3d::undoable_new(k3d::ri::create_property<k3d::ri::string_property>(parameter_type, parameter_list, name, label, description, node().document(), *property_collection, *persistent_container, &node(), k3d::ri::string("")), node().document());
		else if(ri_type == k3d_ri_point)
			k3d::undoable_new(k3d::ri::create_property<k3d::ri::point_property>(parameter_type, parameter_list, name, label, description, node().document(), *property_collection, *persistent_container, &node(), k3d::ri::point(0, 0, 0)), node().document());
		else if(ri_type == k3d_ri_vector)
			k3d::undoable_new(k3d::ri::create_property<k3d::ri::vector_property>(parameter_type, parameter_list, name, label, description, node().document(), *property_collection, *persistent_container, &node(), k3d::ri::vector(0, 0, 0)), node().document());
		else if(ri_type == k3d_ri_normal)
			k3d::undoable_new(k3d::ri::create_property<k3d::ri::normal_property>(parameter_type, parameter_list, name, label, description, node().document(), *property_collection, *persistent_container, &node(), k3d::ri::normal(0, 0, 0)), node().document());
		else if(ri_type == k3d_ri_hpoint)
			k3d::undoable_new(k3d::ri::create_property<k3d::ri::hpoint_property>(parameter_type, parameter_list, name, label, description, node().document(), *property_collection, *persistent_container, &node(), k3d::ri::hpoint(0, 0, 0, 0)), node().document());
		else if(ri_type == k3d_ri_matrix)
			k3d::undoable_new(k3d::ri::create_property<k3d::ri::matrix_property>(parameter_type, parameter_list, name, label, description, node().document(), *property_collection, *persistent_container, &node(), k3d::identity3D()), node().document());
		else if(ri_type == k3d_ri_color)
			k3d::undoable_new(k3d::ri::create_property<k3d::ri::color_property>(parameter_type, parameter_list, name, label, description, node().document(), *property_collection, *persistent_container, &node(), k3d::ri::color(0, 0, 0)), node().document());
		else		
			k3d::log() << error << "unknown RenderMan property value type: [" << ri_type << "]" << std::endl;
	}

	if(node().document().state_recorder().current_change_set())
		node().document().state_recorder().current_change_set()->record_new_state(new k3d::user::property_container(node()));

	close();
}

} // namespace libk3dngui

