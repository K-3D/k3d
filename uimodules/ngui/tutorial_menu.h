#ifndef NGUI_TUTORIAL_MENU_H
#define NGUI_TUTORIAL_MENU_H

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
		\brief Declares functions for displaying a "Tutorial Menu" dialog from which the user can choose among the set of available tutorial scripts
		\author Tim Shead (tshead@k-3d.com)
*/

namespace libk3dngui
{

/// Displays a tutorial menu dialog
void create_tutorial_menu();

/// Returns true iff tutorial playback is in progress
const bool tutorial_playing();

} // namespace libk3dngui

#endif // NGUI_TUTORIAL_MENU_H

