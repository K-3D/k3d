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
#include <k3dsdk/data.h>
#include <gtkmm/liststore.h>

namespace Gtk { class ComboBox; }

namespace libk3dngui
{

namespace entry { class control; }
	
///////////////////////////////////////////////////////////////////////////////
// add_user_property

/// Implements a window for adding custom user properties to an object
class add_user_property :
	public node_window
{
	typedef node_window base;

public:
	add_user_property(k3d::inode& Object, k3d::icommand_node& Parent);

private:
	void add_property_type(const std::string& Type, const std::string& Label);
	void add_user_type(const std::string& Type);
	void add_user_type(const std::string& Type, const std::string& Label);
	void add_renderman_type(const std::string& Type, const std::string& Label);

	void on_property_type_changed();
	void on_add();

	class type_columns :
		public Gtk::TreeModelColumnRecord
	{
	public:
		type_columns()
		{
			add(type);
			add(label);
		}

		Gtk::TreeModelColumn<Glib::ustring> type;
		Gtk::TreeModelColumn<Glib::ustring> label;
	};
	type_columns m_type_columns;
	Glib::RefPtr<Gtk::ListStore> m_property_types;
	Glib::RefPtr<Gtk::ListStore> m_user_types;
	Glib::RefPtr<Gtk::ListStore> m_renderman_types;

	Gtk::ComboBox* const m_property_type;
	Gtk::ComboBox* const m_value_type;
	Gtk::Label* const m_parameter_list_label;
	entry::control* m_parameter_list_control;

	k3d_data(std::string, no_name, change_signal, no_undo, local_storage, no_constraint, no_property, no_serialization) m_parameter_list;
	k3d_data(std::string, no_name, change_signal, no_undo, local_storage, no_constraint, no_property, no_serialization) m_name;
	k3d_data(std::string, no_name, change_signal, no_undo, local_storage, no_constraint, no_property, no_serialization) m_label;
	k3d_data(std::string, no_name, change_signal, no_undo, local_storage, no_constraint, no_property, no_serialization) m_description;
};

} // namespace libk3dngui

#endif // NGUI_USER_PROPERTY_H

