#ifndef NGUI_MENU_ITEM_H
#define NGUI_MENU_ITEM_H

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

#include "ui_component.h"
#include <k3dsdk/signal_system.h>
#include <gtkmm/menuitem.h>

namespace libk3dngui
{

namespace menu_item
{

/////////////////////////////////////////////////////////////////////////////
// control

/// Provides a standard button control that is interactive (can participate in tutorials and macros)
class control :
        public Gtk::MenuItem,
	public ui_component
{
	typedef Gtk::MenuItem base;

public:
	control(k3d::icommand_node& Parent, const std::string& Name);
	control(k3d::icommand_node& Parent, const std::string& Name, Gtk::Widget& Widget);
	control(k3d::icommand_node& Parent, const std::string& Name, const Glib::ustring& label, bool mnemonic = false);

	void on_activate();
	const k3d::icommand_node::result execute_command(const std::string& Command, const std::string& Arguments);
};

} // namespace menu_item

} // namespace libk3dngui

#endif // NGUI_MENU_ITEM_H


