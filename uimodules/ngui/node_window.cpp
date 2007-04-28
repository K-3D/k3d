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

#include "node_window.h"

#include <k3dsdk/inode.h>

#include <gdk/gdkkeysyms.h>

namespace libk3dngui
{

/////////////////////////////////////////////////////////////////////////////
// node_window

node_window::node_window(k3d::inode& Object, k3d::icommand_node& Parent, const std::string& Name) :
	base(Gtk::WINDOW_TOPLEVEL),
	ui_component(Name, &Parent),
	m_node(Object)
{
	Object.deleted_signal().connect(sigc::mem_fun(*this, &node_window::close));
}

k3d::inode& node_window::node()
{
	return m_node;
}

bool node_window::on_key_press_event(GdkEventKey* event)
{
	if(event->keyval == GDK_Escape)
	{
		safe_close();
		return true;
	}

	return base::on_key_press_event(event);
}

bool node_window::on_delete_event(GdkEventAny* event)
{
	safe_close();
	return true;
}

bool node_window::safe_to_close()
{
	return on_safe_to_close();
}

void node_window::safe_close()
{
	if(safe_to_close())
		close();
}

void node_window::close()
{
	on_close();
	delete this;
}

bool node_window::on_safe_to_close()
{
	return true;
}

void node_window::on_close()
{
}

} // namespace libk3dngui

