#ifndef NGUI_INTERACTIVE_H
#define NGUI_INTERACTIVE_H

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
		\brief Declares functions and helpers for simulating user input and driving interactive tutorials
		\author Tim Shead (tshead@k-3d.com)
*/

#include <k3dsdk/vectors.h>
#include <string>

namespace Gtk { class Button; }
namespace Gtk { class CellRenderer; }
namespace Gtk { class ComboBox; }
namespace Gtk { class Entry; }
namespace Gtk { class MenuItem; }
namespace Gtk { class TreeIter; }
namespace Gtk { class TreeView; }
namespace Gtk { class TreeViewColumn; }
namespace Gtk { class Widget; }

namespace k3d { class timer; }

namespace libk3dngui
{

namespace interactive
{

/// Returns the current pointer screen coordinates
const k3d::point2 get_pointer();
	
/// Instantaneously warps the pointer from its current location to the given screen coordinates
void warp_pointer(const k3d::point2& Coords);
/// Instantaneously warps the pointer from its current location to the given screen coordinates, synchronizing with the given timestamp
void warp_pointer(const k3d::point2& Coords, const double Timestamp, const k3d::timer& Timer);
/// Instantaneously warps the pointer from its current location to the given widget coordinates
void warp_pointer(Gtk::Widget& Widget, const k3d::point2& Coords);
/// Instantaneously warps the pointer from its current location to the given widget coordinates, synchronizing with the given timestamp
void warp_pointer(Gtk::Widget& Widget, const k3d::point2& Coords, const double Timestamp, const k3d::timer& Timer);
/// Instantaneously warps the pointer from its current location to the center of a widget
void warp_pointer(Gtk::Widget& Widget);
/// Moves the pointer from its current location to the center of a widget
void move_pointer(Gtk::Widget& Widget);
/// Moves the pointer from its current location to the given widget coordinates
void move_pointer(Gtk::Widget& Widget, const k3d::point2& Coords);
/// Moves the pointer from its current location to the given cell in a TreeView
void move_pointer(Gtk::TreeView& TreeView, Gtk::TreeViewColumn& Column, const Gtk::TreeIter& Row);
/// Does whatever is required (raises windows, changes notebook pages, walks menus, etc.) to make a widget visible
void show(Gtk::Widget& Widget);
/// Overlays graphics on the screen to draw attention to a widget
void highlight(Gtk::Widget& Widget);
/// Enters text into an editable control, simulating user input
void set_text(Gtk::Entry& Entry, const std::string& Text);
/// Activates a button
void activate(Gtk::Button& Button);
/// Activates a menu item
void activate(Gtk::MenuItem& MenuItem);
/// Selects a cell in a treeview
void select_row(Gtk::TreeView& TreeView, Gtk::TreeViewColumn& Column, const Gtk::TreeIter& Row);
/// Enters text into a treeview cell, simulating user input
void set_text(Gtk::TreeView& TreeView, Gtk::TreeViewColumn& Column, Gtk::CellRenderer& Cell, const Gtk::TreeIter& Row, const std::string& Text);
/// Selects a row in a combo box
void select_row(Gtk::ComboBox& ComboBox, const Gtk::TreeIter& Row);

} // namespace interactive

} // namespace libk3dngui

#endif // !NGUI_INTERACTIVE_H

