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

#include "toolbar.h"

#include <k3dsdk/command_tree.h>
#include <k3dsdk/result.h>

namespace libk3dngui
{

namespace toolbar
{

/////////////////////////////////////////////////////////////////////////////
// control

control::control(k3d::icommand_node& Parent, const std::string& Name) :
	base(false, 0)
{
	k3d::command_tree().add(*this, Name, &Parent);

	set_name("k3d-toolbar");
}

Gtk::HBox& control::row(const unsigned int Row)
{
	while(get_children().size() < Row + 1)
		base::pack_start(*Gtk::manage(new Gtk::HBox(false, 0)));

	std::vector<Gtk::Widget*> rows = get_children();
	Gtk::HBox* const row = dynamic_cast<Gtk::HBox*>(rows[Row]);
	assert_critical(row);
	return *row;
}

} // namespace toolbar

} // namespace libk3dngui


