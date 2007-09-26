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
	k3d_aqsis_idisplacement_layer,
	k3d_aqsis_isurface_layer
} user_types_t;

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
		values.push_back(k3d::ienumeration_property::enumeration_value_t("Aqsis Displacement Layer", "k3d::aqsis::idisplacement_layer", ""));
		values.push_back(k3d::ienumeration_property::enumeration_value_t("Aqsis Surface Layer", "k3d::aqsis::isurface_layer", ""));
	}

	return values;
}

std::ostream& operator<<(std::ostream& Stream, const user_types_t& Value)
{
	switch(Value)
	{
		case k3d_bool_t:
			Stream << "k3d::bool_t";
			break;
/*
		case k3d_long:
			Stream << "long";
			break;
		case k3d_unsigned_long:
			Stream << "unsigned_long";
			break;
*/
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
		case k3d_aqsis_idisplacement_layer:
			Stream << "k3d::aqsis::idisplacement_layer";
			break;
		case k3d_aqsis_isurface_layer:
			Stream << "k3d::aqsis::isurface_layer";
			break;
	}

	return Stream;
}

std::istream& operator>>(std::istream& Stream, user_types_t& Value)
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
	else if (text == "k3d::aqsis::idisplacement_layer")
		Value = k3d_aqsis_idisplacement_layer;
	else if (text == "k3d::aqsis::isurface_layer")
		Value = k3d_aqsis_isurface_layer;
	else
		k3d::log() << k3d_file_reference << ": unknown enumeration [" << text << "]"<< std::endl;

	return Stream;
}

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

std::ostream& operator<<(std::ostream& Stream, const ri_types_t& Value)
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

std::istream& operator>>(std::istream& Stream, ri_types_t& Value)
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

