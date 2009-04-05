#ifndef K3DSDK_NGUI_HOTKEY_CELL_RENDERER_TEXT_H
#define K3DSDK_NGUI_HOTKEY_CELL_RENDERER_TEXT_H

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

#include <gtkmm/accelgroup.h>
#include <gtkmm/cellrenderertext.h>

namespace k3d
{

namespace ngui
{

/////////////////////////////////////////////////////////////////////////////
// hotkey_cell_renderer_text

/// Gtk::CellRendererText derivative that automatically disables accelerators during editing
class hotkey_cell_renderer_text :
	public Gtk::CellRendererText
{
	typedef Gtk::CellRendererText base;

public:
	hotkey_cell_renderer_text();

	Gtk::CellEditable* start_editing_vfunc(GdkEvent* event, Gtk::Widget& widget, const Glib::ustring& path, const Gdk::Rectangle& background_area, const Gdk::Rectangle& cell_area, Gtk::CellRendererState flags);
	void on_editing_canceled();
	void on_edited (const Glib::ustring& path, const Glib::ustring& new_text);

private:
	/// Stores the parent window while the control has the keyboard focus
	Gtk::Window* m_window;
	/// Stores the global accel group while the control has the keyboard focus
	Glib::RefPtr<Gtk::AccelGroup> m_disabled_accel_group;
};

} // namespace ngui

} // namespace k3d

#endif // !K3DSDK_NGUI_HOTKEY_CELL_RENDERER_TEXT_H


