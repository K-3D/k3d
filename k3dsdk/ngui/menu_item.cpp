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

#include "interactive.h"
#include "menu_item.h"

#include <k3dsdk/command_tree.h>

#include <gtkmm/menu.h>

#include <iostream>

namespace libk3dngui
{

namespace menu_item
{

/////////////////////////////////////////////////////////////////////////////
// control

control::control(k3d::icommand_node& Parent, const std::string& Name) :
	base()
{
	k3d::command_tree().add(*this, Name, &Parent);

	set_name("k3d-menu-item");
}

control::control(k3d::icommand_node& Parent, const std::string& Name, Gtk::Widget& Widget) :
	base(Widget)
{
	k3d::command_tree().add(*this, Name, &Parent);

	set_name("k3d-menu-item");
}

control::control(k3d::icommand_node& Parent, const std::string& Name, const Glib::ustring& label, bool mnemonic) :
	base(label, mnemonic)
{
	k3d::command_tree().add(*this, Name, &Parent);

	set_name("k3d-menu-item");
}

const k3d::icommand_node::result control::execute_command(const std::string& Command, const std::string& Arguments)
{
	if(Command == "activate")
	{
		interactive::activate(*this);
		return RESULT_CONTINUE;
	}

	return ui_component::execute_command(Command, Arguments);
}

void control::on_activate()
{
	record_command("activate");
	base::on_activate();
}

} // namespace menu_item

} // namespace libk3dngui

