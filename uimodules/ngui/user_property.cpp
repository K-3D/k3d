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

#include <k3dsdk/aqsis_properties.h>
#include <k3dsdk/i18n.h>
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
	m_property_types(Gtk::ListStore::create(m_type_columns)),
	m_user_types(Gtk::ListStore::create(m_type_columns)),
	m_renderman_types(Gtk::ListStore::create(m_type_columns)),
	m_property_type(new Gtk::ComboBox()),
	m_value_type(new Gtk::ComboBox()),
	m_parameter_list_label(new Gtk::Label(_("Parameter List"))),
	m_parameter_list_control(0),
	m_parameter_list(init_value(std::string(""))),
	m_name(init_value(std::string(""))),
	m_label(init_value(std::string(""))),
	m_description(init_value(std::string("")))
{
	m_parameter_list_control = new entry::control(*this, "list", entry::proxy(m_parameter_list));

	set_title(_("Add User Property:"));
	set_role("add_user_property");
	set_position(Gtk::WIN_POS_CENTER);

	add_property_type("user", _("User"));
	add_property_type("attribute", _("RenderMan Attribute"));
	add_property_type("option", _("RenderMan Option"));
	
	add_user_type(k3d::type_string<bool>());
	add_user_type(k3d::type_string<long>());
	add_user_type(k3d::type_string<unsigned long>());
	add_user_type(k3d::type_string<double>());
	add_user_type(k3d::type_string<std::string>());
	add_user_type(k3d::type_string<k3d::point3>());
	add_user_type(k3d::type_string<k3d::vector3>());
	add_user_type(k3d::type_string<k3d::normal3>());
	add_user_type(k3d::type_string<k3d::point4>());
	add_user_type(k3d::type_string<k3d::matrix4>());
	add_user_type(k3d::type_string<k3d::color>());
//	add_user_type(k3d::type_string<k3d::legacy::mesh*>());
	add_user_type(k3d::type_string<k3d::mesh*>());
	add_user_type(k3d::type_string<k3d::gl::imesh_painter*>(), _("OpenGL Mesh Painter"));
	add_user_type(k3d::type_string<k3d::ri::imesh_painter*>(), _("RenderMan Mesh Painter"));
	add_user_type("aqsis_displacement_layer", _("Aqsis Displacement Layer"));
	add_user_type("aqsis_surface_layer", _("Aqsis Surface Layer"));

	add_renderman_type(k3d::type_string<k3d::ri::integer>(), _("Integer"));
	add_renderman_type(k3d::type_string<k3d::ri::real>(), _("Real"));
	add_renderman_type(k3d::type_string<k3d::ri::string>(), _("String"));
	add_renderman_type(k3d::type_string<k3d::ri::point>(), _("Point"));
	add_renderman_type(k3d::type_string<k3d::ri::vector>(), _("Vector"));
	add_renderman_type(k3d::type_string<k3d::ri::normal>(), _("Normal"));
	add_renderman_type(k3d::type_string<k3d::ri::hpoint>(), _("HPoint"));
	add_renderman_type(k3d::type_string<k3d::ri::matrix>(), _("Matrix"));
	add_renderman_type(k3d::type_string<k3d::ri::color>(), _("Color"));

	m_property_type->set_model(m_property_types);
	m_property_type->set_active(0);
	m_property_type->pack_start(m_type_columns.label);
	tooltips().set_tip(*m_property_type, _("Choose property type"));
	
	m_value_type->pack_start(m_type_columns.label);
	tooltips().set_tip(*m_value_type, _("Choose value type"));

	Gtk::Table* const table = Gtk::manage(new Gtk::Table(6, 2, false));
	table->set_col_spacings(4);

	table->attach(*Gtk::manage(new Gtk::Label(_("Property Type"))), 0, 1, 0, 1, Gtk::SHRINK, Gtk::SHRINK);
	table->attach(*Gtk::manage(m_property_type), 1, 2, 0, 1);

	table->attach(*Gtk::manage(new Gtk::Label(_("Value Type"))), 0, 1, 1, 2, Gtk::SHRINK, Gtk::SHRINK);
	table->attach(*Gtk::manage(m_value_type), 1, 2, 1, 2);

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

	m_property_type->signal_changed().connect(sigc::mem_fun(*this, &add_user_property::on_property_type_changed));
	on_property_type_changed();
}

void add_user_property::add_property_type(const std::string& Type, const std::string& Label)
{
	Gtk::TreeRow row = *m_property_types->append();
	row[m_type_columns.type] = Type;
	row[m_type_columns.label] = Label;
}

void add_user_property::add_user_type(const std::string& Type)
{
	Gtk::TreeRow row = *m_user_types->append();
	row[m_type_columns.type] = Type;
	row[m_type_columns.label] = Type;
}

void add_user_property::add_user_type(const std::string& Type, const std::string& Label)
{
	Gtk::TreeRow row = *m_user_types->append();
	row[m_type_columns.type] = Type;
	row[m_type_columns.label] = Label;
}

void add_user_property::add_renderman_type(const std::string& Type, const std::string& Label)
{
	Gtk::TreeRow row = *m_renderman_types->append();
	row[m_type_columns.type] = Type;
	row[m_type_columns.label] = Label;
}

void add_user_property::on_property_type_changed()
{
	switch(m_property_type->get_active_row_number())
	{
		case 0:
			m_value_type->set_model(m_user_types);
			m_value_type->set_active(0);
			m_parameter_list_label->hide();
			m_parameter_list_control->hide();
			break;
		case 1:
		case 2:
			m_value_type->set_model(m_renderman_types);
			m_value_type->set_active(0);
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

	const std::string property_type = m_property_type->get_active()->get_value(m_type_columns.type);
	const std::string value_type = m_value_type->get_active()->get_value(m_type_columns.type);
	const std::string parameter_list = m_parameter_list.value();
	const std::string name = m_name.value();
	const std::string label = m_label.value();
	const std::string description = m_description.value();

	k3d::record_state_change_set change_set(node().document(), "Add user property " + name, K3D_CHANGE_SET_CONTEXT);
	
	if(node().document().state_recorder().current_change_set())
		node().document().state_recorder().current_change_set()->record_old_state(new k3d::user::property_container(node()));
		
	if(property_type == "user")
	{
		if(value_type == k3d::type_string<bool>())
			k3d::undoable_new(k3d::user::create_property<k3d::user::bool_property>(name, label, description, node().document(), *property_collection, *persistent_container, &node(), false), node().document());
		else if(value_type == k3d::type_string<long>())
			k3d::undoable_new(k3d::user::create_property<k3d::user::long_property>(name, label, description, node().document(), *property_collection, *persistent_container, &node(), 0), node().document());
		else if(value_type == k3d::type_string<unsigned long>())
			k3d::undoable_new(k3d::user::create_property<k3d::user::unsigned_long_property>(name, label, description, node().document(), *property_collection, *persistent_container, &node(), 0), node().document());
		else if(value_type == k3d::type_string<double>())
			k3d::undoable_new(k3d::user::create_property<k3d::user::double_property>(name, label, description, node().document(), *property_collection, *persistent_container, &node(), 0.0), node().document());
		else if(value_type == k3d::type_string<std::string>())
			k3d::undoable_new(k3d::user::create_property<k3d::user::string_property>(name, label, description, node().document(), *property_collection, *persistent_container, &node(), std::string()), node().document());
		else if(value_type == k3d::type_string<k3d::point3>())
			k3d::undoable_new(k3d::user::create_property<k3d::user::point3_property>(name, label, description, node().document(), *property_collection, *persistent_container, &node(), k3d::point3(0, 0, 0)), node().document());
		else if(value_type == k3d::type_string<k3d::vector3>())
			k3d::undoable_new(k3d::user::create_property<k3d::user::vector3_property>(name, label, description, node().document(), *property_collection, *persistent_container, &node(), k3d::vector3(0, 0, 0)), node().document());
		else if(value_type == k3d::type_string<k3d::normal3>())
			k3d::undoable_new(k3d::user::create_property<k3d::user::normal3_property>(name, label, description, node().document(), *property_collection, *persistent_container, &node(), k3d::normal3(0, 0, 0)), node().document());
		else if(value_type == k3d::type_string<k3d::point4>())
			k3d::undoable_new(k3d::user::create_property<k3d::user::point4_property>(name, label, description, node().document(), *property_collection, *persistent_container, &node(), k3d::point4(0, 0, 0, 0)), node().document());
		else if(value_type == k3d::type_string<k3d::matrix4>())
			k3d::undoable_new(k3d::user::create_property<k3d::user::matrix4_property>(name, label, description, node().document(), *property_collection, *persistent_container, &node(), k3d::identity3D()), node().document());
		else if(value_type == k3d::type_string<k3d::color>())
			k3d::undoable_new(k3d::user::create_property<k3d::user::color_property>(name, label, description, node().document(), *property_collection, *persistent_container, &node(), k3d::color(1, 1, 1)), node().document());
//		else if(value_type == k3d::type_string<k3d::legacy::mesh*>())
//			k3d::undoable_new(k3d::user::create_property<k3d::user::mesh_property>(name, label, description, node().document(), *property_collection, *persistent_container, &node(), static_cast<k3d::legacy::mesh*>(0)), node().document());
		else if(value_type == k3d::type_string<k3d::mesh*>())
			k3d::undoable_new(k3d::user::create_property<k3d::user::mesh_property>(name, label, description, node().document(), *property_collection, *persistent_container, &node(), static_cast<k3d::mesh*>(0)), node().document());
		else if(value_type == k3d::type_string<k3d::gl::imesh_painter*>())
			k3d::undoable_new(k3d::user::create_property<k3d::user::gl_mesh_painter_property>(name, label, description, node().document(), *property_collection, *persistent_container, &node(), static_cast<k3d::gl::imesh_painter*>(0)), node().document());
		else if(value_type == k3d::type_string<k3d::ri::imesh_painter*>())
			k3d::undoable_new(k3d::user::create_property<k3d::user::ri_mesh_painter_property>(name, label, description, node().document(), *property_collection, *persistent_container, &node(), static_cast<k3d::ri::imesh_painter*>(0)), node().document());
		else if(value_type == "aqsis_displacement_layer")
			k3d::undoable_new(k3d::user::create_property<k3d::aqsis::displacement_layer_connection_property>(name, label, description, node().document(), *property_collection, *persistent_container, &node(), static_cast<k3d::inode*>(0)), node().document());
		else if(value_type == "aqsis_surface_layer")
			k3d::undoable_new(k3d::user::create_property<k3d::aqsis::surface_layer_connection_property>(name, label, description, node().document(), *property_collection, *persistent_container, &node(), static_cast<k3d::inode*>(0)), node().document());
		else		
			k3d::log() << error << k3d_file_reference << ": unknown user property value type: [" << value_type << "]" << std::endl;
	}
	else
	{
		const k3d::irenderman_property::parameter_type_t parameter_type =
			(property_type == "attribute") ? k3d::irenderman_property::ATTRIBUTE : k3d::irenderman_property::OPTION;

		if(value_type == k3d::type_string<k3d::ri::integer>())
			k3d::undoable_new(k3d::ri::create_property<k3d::ri::integer_property>(parameter_type, parameter_list, name, label, description, node().document(), *property_collection, *persistent_container, &node(), 0), node().document());
		else if(value_type == k3d::type_string<k3d::ri::real>())
			k3d::undoable_new(k3d::ri::create_property<k3d::ri::real_property>(parameter_type, parameter_list, name, label, description, node().document(), *property_collection, *persistent_container, &node(), 0.0), node().document());
		else if(value_type == k3d::type_string<k3d::ri::string>())
			k3d::undoable_new(k3d::ri::create_property<k3d::ri::string_property>(parameter_type, parameter_list, name, label, description, node().document(), *property_collection, *persistent_container, &node(), k3d::ri::string("")), node().document());
		else if(value_type == k3d::type_string<k3d::ri::point>())
			k3d::undoable_new(k3d::ri::create_property<k3d::ri::point_property>(parameter_type, parameter_list, name, label, description, node().document(), *property_collection, *persistent_container, &node(), k3d::ri::point(0, 0, 0)), node().document());
		else if(value_type == k3d::type_string<k3d::ri::vector>())
			k3d::undoable_new(k3d::ri::create_property<k3d::ri::vector_property>(parameter_type, parameter_list, name, label, description, node().document(), *property_collection, *persistent_container, &node(), k3d::ri::vector(0, 0, 0)), node().document());
		else if(value_type == k3d::type_string<k3d::ri::normal>())
			k3d::undoable_new(k3d::ri::create_property<k3d::ri::normal_property>(parameter_type, parameter_list, name, label, description, node().document(), *property_collection, *persistent_container, &node(), k3d::ri::normal(0, 0, 0)), node().document());
		else if(value_type == k3d::type_string<k3d::ri::hpoint>())
			k3d::undoable_new(k3d::ri::create_property<k3d::ri::hpoint_property>(parameter_type, parameter_list, name, label, description, node().document(), *property_collection, *persistent_container, &node(), k3d::ri::hpoint(0, 0, 0, 0)), node().document());
		else if(value_type == k3d::type_string<k3d::ri::matrix>())
			k3d::undoable_new(k3d::ri::create_property<k3d::ri::matrix_property>(parameter_type, parameter_list, name, label, description, node().document(), *property_collection, *persistent_container, &node(), k3d::identity3D()), node().document());
		else if(value_type == k3d::type_string<k3d::ri::color>())
			k3d::undoable_new(k3d::ri::create_property<k3d::ri::color_property>(parameter_type, parameter_list, name, label, description, node().document(), *property_collection, *persistent_container, &node(), k3d::ri::color(0, 0, 0)), node().document());
		else		
			k3d::log() << error << "unknown RenderMan property value type: [" << value_type << "]" << std::endl;
	}

	if(node().document().state_recorder().current_change_set())
		node().document().state_recorder().current_change_set()->record_new_state(new k3d::user::property_container(node()));

	close();
}

} // namespace libk3dngui

