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
		\author Tim Shead (tshead@k-3d.com)
*/

#include "image_menu_item.h"
#include "interactive.h"
#include "utility.h"

#include <k3dsdk/command_tree.h>
#include <k3dsdk/log.h>

#include <gtkmm/menu.h>
#include <gtkmm/stockitem.h>
#include <gtkmm/tooltips.h>

#include <iostream>

namespace libk3dngui
{

namespace image_menu_item
{

/////////////////////////////////////////////////////////////////////////////
// control

control::control(k3d::icommand_node& Parent, const std::string& Name) :
	base()
{
	k3d::command_tree().add(*this, Name, &Parent);
}

control::control(k3d::icommand_node& Parent, const std::string& Name, Gtk::Widget& image, const Glib::ustring& label, bool mnemonic) :
	base(image, label, mnemonic)
{
	k3d::command_tree().add(*this, Name, &Parent);
}

control::control(k3d::icommand_node& Parent, const std::string& Name, const Glib::ustring& label, bool mnemonic) :
	base(label, mnemonic)
{
	k3d::command_tree().add(*this, Name, &Parent);
}

control::control(k3d::icommand_node& Parent, const std::string& Name, const Gtk::StockID& stock_id) :
	base(stock_id)
{
	k3d::command_tree().add(*this, Name, &Parent);
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

Gtk::Label* control::get_label()
{
	return dynamic_cast<Gtk::Label*>(get_child());
}

void control::on_activate()
{
	record_command("activate");
	base::on_activate();
}

} // namespace image_menu_item

/////////////////////////////////////////////////////////////////////////////
// connect

image_menu_item::control* connect(image_menu_item::control* const Control, const sigc::slot<void>& Slot)
{
	return_val_if_fail(Control, 0);

	Control->signal_activate().connect(Slot);
	return Control;
}

} // namespace libk3dngui


