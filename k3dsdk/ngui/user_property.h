#ifndef NGUI_USER_PROPERTY_H
#define NGUI_USER_PROPERTY_H

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

#include "node_window.h"
#include "enumeration_chooser.h"
#include <k3dsdk/data.h>
#include <k3dsdk/property_collection.h>
#include <gtkmm/liststore.h>

namespace Gtk { class ComboBox; }

namespace libk3dngui
{

typedef enum
{
	K3D_bool,
	K3D_long,
	K3D_unsigned_long,
	K3D_double,
	K3D_string,
	K3D_point3,
	K3D_vector3,
	K3D_normal3,
	K3D_point4,
	K3D_matrix4,
	K3D_color,
	K3D_mesh,
	K3D_GL_imesh_painter,
	K3D_RI_imesh_painter,
	K3D_aqsis_displacement_layer,
	K3D_aqsis_surface_layer
} user_types_t;

typedef enum
{
	RI_integer,
	RI_real,
	RI_string,
	RI_point,
	RI_vector,
	RI_normal,
	RI_hpoint,
	RI_matrix,
	RI_color
} ri_types_t;

typedef enum
{
	USER,
	ATTRIBUTE,
	OPTION
} property_types_t;

namespace entry { class control; }
	
///////////////////////////////////////////////////////////////////////////////
// add_user_property

/// Implements a window for adding custom user properties to an object
class add_user_property :
	public node_window,
	public k3d::property_collection // needed for enumeration properties
{
	typedef node_window base;

public:
	add_user_property(k3d::inode& Object, k3d::icommand_node& Parent);

private:

	void on_property_type_changed(k3d::iunknown* Hint);
	void on_add();

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

static const k3d::ienumeration_property::enumeration_values_t& user_types_values()
{
	static k3d::ienumeration_property::enumeration_values_t values;
	if(values.empty())
	{
		values.push_back(k3d::ienumeration_property::enumeration_value_t("bool", "bool",""));
		values.push_back(k3d::ienumeration_property::enumeration_value_t("long", "long", ""));
		values.push_back(k3d::ienumeration_property::enumeration_value_t("unsigned long", "unsigned_long", ""));
		values.push_back(k3d::ienumeration_property::enumeration_value_t("double", "double", ""));
		values.push_back(k3d::ienumeration_property::enumeration_value_t("std::string", "string", ""));
		values.push_back(k3d::ienumeration_property::enumeration_value_t("k3d::point3", "point3", ""));
		values.push_back(k3d::ienumeration_property::enumeration_value_t("k3d::vector3", "vector3", ""));
		values.push_back(k3d::ienumeration_property::enumeration_value_t("k3d::normal3", "normal3", ""));
		values.push_back(k3d::ienumeration_property::enumeration_value_t("k3d::point4", "point4", ""));
		values.push_back(k3d::ienumeration_property::enumeration_value_t("k3d::matrix4", "matrix4", ""));
		values.push_back(k3d::ienumeration_property::enumeration_value_t("k3d::color", "color", ""));
		values.push_back(k3d::ienumeration_property::enumeration_value_t("k3d::mesh", "mesh", ""));
		values.push_back(k3d::ienumeration_property::enumeration_value_t("k3d::gl::imesh_painter", "GL_imesh_painter", "")); 
		values.push_back(k3d::ienumeration_property::enumeration_value_t("k3d::ri::imesh_painter", "RI_imesh_painter", ""));
		values.push_back(k3d::ienumeration_property::enumeration_value_t("Aqsis displacement layer", "aqsis_displacement_layer", ""));
		values.push_back(k3d::ienumeration_property::enumeration_value_t("Aqsis surface layer", "aqsis_surface_layer", ""));
	}

	return values;
}

std::ostream& operator<<(std::ostream& Stream, const user_types_t& Value)
{
	switch(Value)
	{
		case K3D_bool:
			Stream << "bool";
			break;
		case K3D_long:
			Stream << "long";
			break;
		case K3D_unsigned_long:
			Stream << "unsigned_long";
			break;
		case K3D_double:
			Stream << "double";
			break;
		case K3D_string:
			Stream << "string";
			break;
		case K3D_point3:
			Stream << "point3";
			break;
		case K3D_vector3:
			Stream << "vector3";
			break;
		case K3D_normal3:
			Stream << "normal3";
			break;
		case K3D_point4:
			Stream << "point4";
			break;
		case K3D_matrix4:
			Stream << "matrix4";
			break;
		case K3D_color:
			Stream << "color";
			break;
		case K3D_mesh:
			Stream << "mesh";
			break;
		case K3D_GL_imesh_painter:
			Stream << "GL_imesh_painter";
			break;
		case K3D_RI_imesh_painter:
			Stream << "RI_imesh_painter";
			break;
		case K3D_aqsis_displacement_layer:
			Stream << "aqsis_displacement_layer";
			break;
		case K3D_aqsis_surface_layer:
			Stream << "aqsis_surface_layer";
			break;
	}

	return Stream;
}

std::istream& operator>>(std::istream& Stream, user_types_t& Value)
{
	std::string text;
	Stream >> text;

	if (text == "bool")
		Value = K3D_bool;
	else if (text == "long")	
		Value = K3D_long;
	else if (text == "unsigned_long")
		Value = K3D_unsigned_long;
	else if (text == "double")
		Value = K3D_double;
	else if (text == "string")
		Value = K3D_string;
	else if (text == "point3")
		Value = K3D_point3;
	else if (text == "vector3")
		Value = K3D_vector3;
	else if (text == "normal3")
		Value = K3D_normal3;
	else if (text == "point4")
		Value = K3D_point4;
	else if (text == "matrix4")
		Value = K3D_matrix4;
	else if (text == "color")
		Value = K3D_color;
	else if (text == "mesh")
		Value = K3D_mesh;
	else if (text == "GL_imesh_painter")
		Value = K3D_GL_imesh_painter;
	else if (text == "RI_imesh_painter")
		Value = K3D_RI_imesh_painter;
	else if (text == "aqsis_displacement_layer")
		Value = K3D_aqsis_displacement_layer;
	else if (text == "aqsis_surface_layer")
		Value = K3D_aqsis_surface_layer;
	else
		k3d::log() << k3d_file_reference << ": unknown enumeration [" << text << "]"<< std::endl;

	return Stream;
}

static const k3d::ienumeration_property::enumeration_values_t& ri_types_values()
{
	static k3d::ienumeration_property::enumeration_values_t values;
	if(values.empty())
	{
		values.push_back(k3d::ienumeration_property::enumeration_value_t("Integer", "integer", ""));
		values.push_back(k3d::ienumeration_property::enumeration_value_t("Real", "real", ""));
		values.push_back(k3d::ienumeration_property::enumeration_value_t("String", "string", ""));
		values.push_back(k3d::ienumeration_property::enumeration_value_t("Point", "point", ""));
		values.push_back(k3d::ienumeration_property::enumeration_value_t("Vector", "vector", ""));
		values.push_back(k3d::ienumeration_property::enumeration_value_t("Normal", "normal", ""));
		values.push_back(k3d::ienumeration_property::enumeration_value_t("HPoint", "hpoint", ""));
		values.push_back(k3d::ienumeration_property::enumeration_value_t("Matrix", "matrix", ""));
		values.push_back(k3d::ienumeration_property::enumeration_value_t("Color", "color" ,""));
	}

	return values;
}

std::ostream& operator<<(std::ostream& Stream, const ri_types_t& Value)
{
	switch(Value)
	{
		case RI_integer:
			Stream << "integer";
			break;
		case RI_real:
			Stream << "real";
			break;
		case RI_string:
			Stream << "string";
			break;
		case RI_point:
			Stream << "point";
			break;
		case RI_vector:
			Stream << "vector";
			break;
		case RI_normal:
			Stream << "normal";
			break;
		case RI_hpoint:
			Stream << "hpoint";
			break;
		case RI_matrix:
			Stream << "matrix";
			break;
		case RI_color:
			Stream << "color";
	}

	return Stream;
}

std::istream& operator>>(std::istream& Stream, ri_types_t& Value)
{
	std::string text;
	Stream >> text;
	
	if (text =="integer")
		Value = RI_integer;
	else if (text =="real")
		Value = RI_real;
	else if (text =="string")
		Value = RI_string;
	else if (text =="point")
		Value = RI_point;
	else if (text =="vector")
		Value = RI_vector;
	else if (text =="normal")
		Value = RI_normal;
	else if (text =="hpoint")
		Value = RI_hpoint;
	else if (text =="matrix")
		Value = RI_matrix;
	else if (text =="color")
		Value = RI_color;
	else
		k3d::log() << k3d_file_reference << ": unknown enumeration [" << text << "]"<< std::endl;
		
	return Stream;
}

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

std::ostream& operator<<(std::ostream& Stream, const property_types_t& Value)
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

std::istream& operator>>(std::istream& Stream, property_types_t& Value)
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

} // namespace libk3dngui

#endif // NGUI_USER_PROPERTY_H

