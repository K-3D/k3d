#ifndef NGUI_TRANSFORM_H
#define NGUI_TRANSFORM_H

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

#include <vector>

namespace k3d { class inode; }

namespace libk3dngui
{

/// Defines an ordered collection of objects whose transformation properties are chained together
typedef std::vector<k3d::inode*> transform_history_t;

/// Returns the set of objects that makeup the transformation history from an object's parent up-to-and-including the object itself
const transform_history_t parent_to_node_history(k3d::inode& Object);

/// Helper class for dumping history to a stream for debugging
struct show_history
{
	show_history(const transform_history_t& History) : history(History) {}
	const transform_history_t history;
};

std::ostream& operator<<(std::ostream&, const show_history&);

/// Unparents a node
void unparent(k3d::inode& Node);

} // namespace libk3dngui

#endif // !NGUI_TRANSFORM_H

