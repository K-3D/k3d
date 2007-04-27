#ifndef NGUI_SELECTION_H
#define NGUI_SELECTION_H

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

#include <iosfwd>

namespace libk3dngui
{

/// Enumerates available interactive selection modes
typedef enum
{
	/// Select individual nodes
	SELECT_NODES,
	/// Select points / point groups
	SELECT_POINTS,
	/// Select polyhedron edges / curves
	SELECT_LINES,
	/// Select polyhedron faces / patches
	SELECT_FACES
} selection_mode_t;

std::ostream& operator<<(std::ostream& Stream, const selection_mode_t& Value);
std::istream& operator>>(std::istream& Stream, selection_mode_t& Value);

} // namespace libk3dngui

#endif // NGUI_SELECTION_H


