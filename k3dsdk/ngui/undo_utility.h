#ifndef NGUI_UNDO_UTILITY_H
#define NGUI_UNDO_UTILITY_H

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

/** \author Timothy M. Shead <tshead@k-3d.com>
*/

#include <k3dsdk/istate_recorder.h>
#include <vector>

namespace libk3dngui
{

/// Defines an ordered collection of undo/redo hierarchy nodes
typedef std::vector<const k3d::istate_recorder::node*> parent_nodes_t;

/// Returns the set of nodes that are parents of the given node, ordered from parent -> child (could return empty collection)
const parent_nodes_t parent_nodes(const k3d::istate_recorder::node* const Node);
/// Given the state recorder for a document, returns the child that should be used by default for the next redo (if any, could return NULL)
const k3d::istate_recorder::node* next_redo(k3d::istate_recorder& StateRecorder);

} // namespace libk3dngui

#endif // !NGUI_UNDO_UTILITY_H
