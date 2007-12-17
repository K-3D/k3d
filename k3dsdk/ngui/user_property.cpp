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
#include "enumeration_chooser.h"
#include "node_window.h"
#include "user_property.h"
#include "widget_manip.h"

#include <k3d-i18n-config.h>
#include <k3dsdk/aqsis_properties.h>
#include <k3dsdk/data.h>
#include <k3dsdk/inode.h>
#include <k3dsdk/ipersistent_container.h>
#include <k3dsdk/iproperty_collection.h>
#include <k3dsdk/properties_ri.h>
#include <k3dsdk/property_collection.h>
#include <k3dsdk/string_cast.h>
#include <k3dsdk/tokens.h>
#include <k3dsdk/user_properties.h>

#include <gtkmm/box.h>
#include <gtkmm/buttonbox.h>
#include <gtkmm/combobox.h>
#include <gtkmm/entry.h>
#include <gtkmm/label.h>
#include <gtkmm/liststore.h>
#include <gtkmm/stock.h>
#include <gtkmm/table.h>

#include <boost/format.hpp>

namespace libk3dngui
{

///////////////////////////////////////////////////////////////////////////////
// add_user_property

/// Implements a window for adding custom user properties to an object
class add_user_property :
	public node_window,
	public k3d::property_collection // needed for enumeration properties
{
	typedef node_window base;

public:
	add_user_property(k3d::inode& Object, k3d::icommand_node& Parent) :
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

private:
	void on_property_type_changed(k3d::iunknown* Hint)
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

	void on_add()
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
				k3d::undoable_new(k3d::user::create_property<k3d::user::k3d_bool_t_property>(name, label, description, node().document(), *property_collection, *persistent_container, &node(), false), node().document());
			else if(user_type == k3d_double_t)
				k3d::undoable_new(k3d::user::create_property<k3d::user::k3d_double_t_property>(name, label, description, node().document(), *property_collection, *persistent_container, &node(), 0.0), node().document());
			else if(user_type == k3d_string_t)
				k3d::undoable_new(k3d::user::create_property<k3d::user::k3d_string_t_property>(name, label, description, node().document(), *property_collection, *persistent_container, &node(), std::string()), node().document());
			else if(user_type == k3d_point3)
				k3d::undoable_new(k3d::user::create_property<k3d::user::k3d_point3_property>(name, label, description, node().document(), *property_collection, *persistent_container, &node(), k3d::point3(0, 0, 0)), node().document());
			else if(user_type == k3d_vector3)
				k3d::undoable_new(k3d::user::create_property<k3d::user::k3d_vector3_property>(name, label, description, node().document(), *property_collection, *persistent_container, &node(), k3d::vector3(0, 0, 0)), node().document());
			else if(user_type == k3d_normal3)
				k3d::undoable_new(k3d::user::create_property<k3d::user::k3d_normal3_property>(name, label, description, node().document(), *property_collection, *persistent_container, &node(), k3d::normal3(0, 0, 0)), node().document());
			else if(user_type == k3d_point4)
				k3d::undoable_new(k3d::user::create_property<k3d::user::k3d_point4_property>(name, label, description, node().document(), *property_collection, *persistent_container, &node(), k3d::point4(0, 0, 0, 0)), node().document());
			else if(user_type == k3d_matrix4)
				k3d::undoable_new(k3d::user::create_property<k3d::user::k3d_matrix4_property>(name, label, description, node().document(), *property_collection, *persistent_container, &node(), k3d::identity3D()), node().document());
			else if(user_type == k3d_color)
				k3d::undoable_new(k3d::user::create_property<k3d::user::k3d_color_property>(name, label, description, node().document(), *property_collection, *persistent_container, &node(), k3d::color(1, 1, 1)), node().document());
			else if(user_type == k3d_mesh)
				k3d::undoable_new(k3d::user::create_property<k3d::user::k3d_mesh_property>(name, label, description, node().document(), *property_collection, *persistent_container, &node(), static_cast<k3d::mesh*>(0)), node().document());
			else if(user_type == k3d_gl_imesh_painter)
				k3d::undoable_new(k3d::user::create_property<k3d::user::k3d_gl_imesh_painter_property>(name, label, description, node().document(), *property_collection, *persistent_container, &node(), static_cast<k3d::gl::imesh_painter*>(0)), node().document());
			else if(user_type == k3d_ri_imesh_painter)
				k3d::undoable_new(k3d::user::create_property<k3d::user::k3d_ri_imesh_painter_property>(name, label, description, node().document(), *property_collection, *persistent_container, &node(), static_cast<k3d::ri::imesh_painter*>(0)), node().document());
			else if(user_type == k3d_imaterial)
				k3d::undoable_new(k3d::user::create_property<k3d::user::k3d_imaterial_property>(name, label, description, node().document(), *property_collection, *persistent_container, &node(), static_cast<k3d::imaterial*>(0)), node().document());
			else if(user_type == k3d_aqsis_idisplacement_layer)
				k3d::undoable_new(k3d::user::create_property<k3d::aqsis::displacement_layer_connection_property>(name, label, description, node().document(), *property_collection, *persistent_container, &node(), static_cast<k3d::inode*>(0)), node().document());
			else if(user_type == k3d_aqsis_isurface_layer)
				k3d::undoable_new(k3d::user::create_property<k3d::aqsis::surface_layer_connection_property>(name, label, description, node().document(), *property_collection, *persistent_container, &node(), static_cast<k3d::inode*>(0)), node().document());
			else if(user_type == k3d_filesystem_path)
				k3d::undoable_new(k3d::user::create_property<k3d::user::k3d_filesystem_path_property>(name, label, description, node().document(), *property_collection, *persistent_container, &node(), k3d::filesystem::path()), node().document());
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


	/// Enumerates the classes of user property that can be created
	typedef enum
	{
		/// Designates "normal" user properties (simple scalar values)
		USER,
		/// Designates RenderMan attribute properties
		ATTRIBUTE,
		/// Designates RenderMan option properties
		OPTION
	} property_types_t;

	static const k3d::ienumeration_property::enumeration_values_t& property_types_values()
	{
		static k3d::ienumeration_property::enumeration_values_t values;
		if(values.empty())
		{
			values.push_back(k3d::ienumeration_property::enumeration_value_t("User", "user",""));
			values.push_back(k3d::ienumeration_property::enumeration_value_t("Renderman Attribute", "attribute",""));
			values.push_back(k3d::ienumeration_property::enumeration_value_t("Renderman Option", "option",""));
		}
		
		return values;
	}

	friend std::ostream& operator<<(std::ostream& Stream, const property_types_t& Value)
	{
		switch(Value)
		{
			case USER:
				Stream << "user";
				break;
			case ATTRIBUTE:
				Stream << "attribute";
				break;
			case OPTION:
				Stream << "option";
				break;
		}
		
		return Stream;
	}

	friend std::istream& operator>>(std::istream& Stream, property_types_t& Value)
	{
		std::string text;
		Stream >> text;
		
		if (text =="user")
			Value = USER;
		else if (text == "attribute")
			Value = ATTRIBUTE;
		else if (text == "option")
			Value = OPTION;
		else
			k3d::log() << k3d_file_reference << ": unknown enumeration [" << text << "]"<< std::endl;
			
		return Stream;
	}

	/// Enumerates the types of "normal" user properties that can be created
	typedef enum
	{
		k3d_bool_t,
		k3d_double_t,
		k3d_string_t,
		k3d_point3,
		k3d_vector3,
		k3d_normal3,
		k3d_point4,
		k3d_matrix4,
		k3d_color,
		k3d_mesh,
		k3d_gl_imesh_painter,
		k3d_ri_imesh_painter,
		k3d_imaterial,
		k3d_aqsis_idisplacement_layer,
		k3d_aqsis_isurface_layer,
		k3d_filesystem_path
	} user_types_t;

	static const k3d::ienumeration_property::enumeration_values_t& user_types_values()
	{
		static k3d::ienumeration_property::enumeration_values_t values;
		if(values.empty())
		{
			values.push_back(k3d::ienumeration_property::enumeration_value_t("Boolean", "k3d::bool_t",""));
			values.push_back(k3d::ienumeration_property::enumeration_value_t("Scalar", "k3d::double_t", ""));
			values.push_back(k3d::ienumeration_property::enumeration_value_t("String", "k3d::string_t", ""));
			values.push_back(k3d::ienumeration_property::enumeration_value_t("3D Point", "k3d::point3", ""));
			values.push_back(k3d::ienumeration_property::enumeration_value_t("3D Vector", "k3d::vector3", ""));
			values.push_back(k3d::ienumeration_property::enumeration_value_t("3D Normal", "k3d::normal3", ""));
			values.push_back(k3d::ienumeration_property::enumeration_value_t("4D Point", "k3d::point4", ""));
			values.push_back(k3d::ienumeration_property::enumeration_value_t("4x4 Matrix", "k3d::matrix4", ""));
			values.push_back(k3d::ienumeration_property::enumeration_value_t("Color", "k3d::color", ""));
			values.push_back(k3d::ienumeration_property::enumeration_value_t("Mesh", "k3d::mesh", ""));
			values.push_back(k3d::ienumeration_property::enumeration_value_t("OpenGL Mesh Painter", "k3d::gl::imesh_painter", "")); 
			values.push_back(k3d::ienumeration_property::enumeration_value_t("RenderMan Mesh Painter", "k3d::ri::imesh_painter", ""));
			values.push_back(k3d::ienumeration_property::enumeration_value_t("Material", "k3d::imaterial", ""));
			values.push_back(k3d::ienumeration_property::enumeration_value_t("Aqsis Displacement Layer", "k3d::aqsis::idisplacement_layer", ""));
			values.push_back(k3d::ienumeration_property::enumeration_value_t("Aqsis Surface Layer", "k3d::aqsis::isurface_layer", ""));
			values.push_back(k3d::ienumeration_property::enumeration_value_t("Filesystem Path", "k3d::filesystem::path", ""));
		}

		return values;
	}

	friend std::ostream& operator<<(std::ostream& Stream, const user_types_t& Value)
	{
		switch(Value)
		{
			case k3d_bool_t:
				Stream << "k3d::bool_t";
				break;
			case k3d_double_t:
				Stream << "k3d::double_t";
				break;
			case k3d_string_t:
				Stream << "k3d::string_t";
				break;
			case k3d_point3:
				Stream << "k3d::point3";
				break;
			case k3d_vector3:
				Stream << "k3d::vector3";
				break;
			case k3d_normal3:
				Stream << "k3d::normal3";
				break;
			case k3d_point4:
				Stream << "k3d::point4";
				break;
			case k3d_matrix4:
				Stream << "k3d::matrix4";
				break;
			case k3d_color:
				Stream << "k3d::color";
				break;
			case k3d_mesh:
				Stream << "k3d::mesh";
				break;
			case k3d_gl_imesh_painter:
				Stream << "k3d::gl::imesh_painter";
				break;
			case k3d_ri_imesh_painter:
				Stream << "k3d::ri::imesh_painter";
				break;
			case k3d_imaterial:
				Stream << "k3d::imaterial";
				break;
			case k3d_aqsis_idisplacement_layer:
				Stream << "k3d::aqsis::idisplacement_layer";
				break;
			case k3d_aqsis_isurface_layer:
				Stream << "k3d::aqsis::isurface_layer";
				break;
			case k3d_filesystem_path:
				Stream << "k3d::filesystem::path";
				break;
		}

		return Stream;
	}

	friend std::istream& operator>>(std::istream& Stream, user_types_t& Value)
	{
		std::string text;
		Stream >> text;

		if (text == "k3d::bool_t")
			Value = k3d_bool_t;
		else if (text == "k3d::double_t")
			Value = k3d_double_t;
		else if (text == "k3d::string_t")
			Value = k3d_string_t;
		else if (text == "k3d::point3")
			Value = k3d_point3;
		else if (text == "k3d::vector3")
			Value = k3d_vector3;
		else if (text == "k3d::normal3")
			Value = k3d_normal3;
		else if (text == "k3d::point4")
			Value = k3d_point4;
		else if (text == "k3d::matrix4")
			Value = k3d_matrix4;
		else if (text == "k3d::color")
			Value = k3d_color;
		else if (text == "k3d::mesh")
			Value = k3d_mesh;
		else if (text == "k3d::gl::imesh_painter")
			Value = k3d_gl_imesh_painter;
		else if (text == "k3d::ri::imesh_painter")
			Value = k3d_ri_imesh_painter;
		else if (text == "k3d::imaterial")
			Value = k3d_imaterial;
		else if (text == "k3d::aqsis::idisplacement_layer")
			Value = k3d_aqsis_idisplacement_layer;
		else if (text == "k3d::aqsis::isurface_layer")
			Value = k3d_aqsis_isurface_layer;
		else if (text == "k3d::filesystem::path")
			Value = k3d_filesystem_path;
		else
			k3d::log() << k3d_file_reference << ": unknown enumeration [" << text << "]"<< std::endl;

		return Stream;
	}

	/// Enumerates the types of RenderMan (attribute or option) user properties that can be created
	typedef enum
	{
		k3d_ri_integer,
		k3d_ri_real,
		k3d_ri_string,
		k3d_ri_point,
		k3d_ri_vector,
		k3d_ri_normal,
		k3d_ri_hpoint,
		k3d_ri_matrix,
		k3d_ri_color
	} ri_types_t;

	static const k3d::ienumeration_property::enumeration_values_t& ri_types_values()
	{
		static k3d::ienumeration_property::enumeration_values_t values;
		if(values.empty())
		{
			values.push_back(k3d::ienumeration_property::enumeration_value_t("Integer", "k3d::ri::integer", ""));
			values.push_back(k3d::ienumeration_property::enumeration_value_t("Real", "k3d::ri::real", ""));
			values.push_back(k3d::ienumeration_property::enumeration_value_t("String", "k3d::ri::string", ""));
			values.push_back(k3d::ienumeration_property::enumeration_value_t("Point", "k3d::ri::point", ""));
			values.push_back(k3d::ienumeration_property::enumeration_value_t("Vector", "k3d::ri::vector", ""));
			values.push_back(k3d::ienumeration_property::enumeration_value_t("Normal", "k3d::ri::normal", ""));
			values.push_back(k3d::ienumeration_property::enumeration_value_t("HPoint", "k3d::ri::hpoint", ""));
			values.push_back(k3d::ienumeration_property::enumeration_value_t("Matrix", "k3d::ri::matrix", ""));
			values.push_back(k3d::ienumeration_property::enumeration_value_t("Color", "k3d::ri::color" ,""));
		}

		return values;
	}

	friend std::ostream& operator<<(std::ostream& Stream, const ri_types_t& Value)
	{
		switch(Value)
		{
			case k3d_ri_integer:
				Stream << "k3d::ri::integer";
				break;
			case k3d_ri_real:
				Stream << "k3d::ri::real";
				break;
			case k3d_ri_string:
				Stream << "k3d::ri::string";
				break;
			case k3d_ri_point:
				Stream << "k3d::ri::point";
				break;
			case k3d_ri_vector:
				Stream << "k3d::ri::vector";
				break;
			case k3d_ri_normal:
				Stream << "k3d::ri::normal";
				break;
			case k3d_ri_hpoint:
				Stream << "k3d::ri::hpoint";
				break;
			case k3d_ri_matrix:
				Stream << "k3d::ri::matrix";
				break;
			case k3d_ri_color:
				Stream << "k3d::ri::color";
		}

		return Stream;
	}

	friend std::istream& operator>>(std::istream& Stream, ri_types_t& Value)
	{
		std::string text;
		Stream >> text;
		
		if (text =="k3d::ri::integer")
			Value = k3d_ri_integer;
		else if (text =="k3d::ri::real")
			Value = k3d_ri_real;
		else if (text =="k3d::ri::string")
			Value = k3d_ri_string;
		else if (text =="k3d::ri::point")
			Value = k3d_ri_point;
		else if (text =="k3d::ri::vector")
			Value = k3d_ri_vector;
		else if (text =="k3d::ri::normal")
			Value = k3d_ri_normal;
		else if (text =="k3d::ri::hpoint")
			Value = k3d_ri_hpoint;
		else if (text =="k3d::ri::matrix")
			Value = k3d_ri_matrix;
		else if (text =="k3d::ri::color")
			Value = k3d_ri_color;
		else
			k3d::log() << k3d_file_reference << ": unknown enumeration [" << text << "]"<< std::endl;
			
		return Stream;
	}

	Gtk::Label* const m_parameter_list_label;
	Gtk::Label* const m_user_type_label;
	Gtk::Label* const m_ri_type_label;
	Gtk::Label* const m_property_type_label;
	entry::control* m_parameter_list_control;

	k3d_data(std::string, no_name, change_signal, no_undo, local_storage, no_constraint, no_property, no_serialization) m_parameter_list;
	k3d_data(std::string, no_name, change_signal, no_undo, local_storage, no_constraint, no_property, no_serialization) m_name;
	k3d_data(std::string, no_name, change_signal, no_undo, local_storage, no_constraint, no_property, no_serialization) m_label;
	k3d_data(std::string, no_name, change_signal, no_undo, local_storage, no_constraint, no_property, no_serialization) m_description;
	k3d_data(user_types_t, immutable_name, change_signal, no_undo, local_storage, no_constraint, enumeration_property, no_serialization) m_user_type;
	k3d_data(ri_types_t, immutable_name, change_signal, no_undo, local_storage, no_constraint, enumeration_property, no_serialization) m_ri_type;
	k3d_data(property_types_t, immutable_name, change_signal, no_undo, local_storage, no_constraint, enumeration_property, no_serialization) m_property_type;
	enumeration_chooser::control* m_user_type_control;
	enumeration_chooser::control* m_ri_type_control;
	enumeration_chooser::control* m_property_type_control;
};

////////////////////////////////////////////////////////////////////////////////////
// add_user_property_dialog

void add_user_property_dialog(k3d::inode& Node, k3d::icommand_node& Parent)
{
	add_user_property* const window = new add_user_property(Node, Parent);
	window->show();
}

} // namespace libk3dngui

