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

#include "hotkey_cell_renderer_text.h"
#include <gtkmm/window.h>

namespace libk3dngui
{

/////////////////////////////////////////////////////////////////////////////
// hotkey_cell_renderer_text

hotkey_cell_renderer_text::hotkey_cell_renderer_text() :
	m_window(0)
{
}
	
Gtk::CellEditable* hotkey_cell_renderer_text::start_editing_vfunc(GdkEvent* event, Gtk::Widget& widget, const Glib::ustring& path, const Gdk::Rectangle& background_area, const Gdk::Rectangle& cell_area, Gtk::CellRendererState flags)
{
	m_window = dynamic_cast<Gtk::Window*>(widget.get_toplevel());
	if(m_window)
		{
			m_disabled_accel_group = m_window->get_accel_group();
			m_window->remove_accel_group(m_window->get_accel_group());
		}
	
	return base::start_editing_vfunc(event, widget, path, background_area, cell_area, flags);
}

void hotkey_cell_renderer_text::on_editing_canceled()
{
	if(m_window)
		{
			m_window->add_accel_group(m_disabled_accel_group);
			m_disabled_accel_group.clear();
		}

	base::on_editing_canceled();
}

void hotkey_cell_renderer_text::on_edited(const Glib::ustring& path, const Glib::ustring& new_text)
{
	if(m_window)
		{
			m_window->add_accel_group(m_disabled_accel_group);
			m_disabled_accel_group.clear();
		}

	base::on_edited(path, new_text);
}

} // namespace libk3dngui


