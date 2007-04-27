#ifndef NGUI_CONTEXT_MENU_H
#define NGUI_CONTEXT_MENU_H

// K-3D
// Copyright (c) 1995-2005, Timothy M. Shead
//
// Contact: tshead@k-3d.com
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public
// License as published by the Free Software Foundation; either
// version 2 of the License, or (at your argument) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Public License for more details.
//
// You should have received a copy of the GNU General Public
// License along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

/**
		\author Timothy M. Shead (tshead@k-3d.com)
*/

// Forward declarations
namespace k3d { class icommand_node; }
namespace Gtk { class Menu; }

namespace libk3dngui
{

// Forward declarations
class document_state;

/// Returns a menu suitable for use as a context-sensitive menu for the given object
Gtk::Menu* create_context_menu(document_state& DocumentState, k3d::icommand_node& Parent);

} // namespace libk3dngui

#endif // !NGUI_CONTEXT_MENU_H


