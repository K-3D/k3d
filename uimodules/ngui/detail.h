#ifndef NGUI_DETAIL_H
#define NGUI_DETAIL_H

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

/** \file
		\brief Defines useful GUI agnostic functions
		\author Tim Shead (tshead@k-3d.com)
		\author Romain Behar (romainbehar@yahoo.com)
*/

#include <k3dsdk/idocument.h>
#include <k3dsdk/inode_collection.h>
#include <k3dsdk/iplugin_factory.h>

namespace libk3dngui
{

// Forward declarations
class document_state;

/// Instantiates selected nodes (currently, only mesh sources are instantiable)
void instantiate_selected_nodes(document_state& DocumentState);
/// Duplicates selected nodes with their input transformation, if any
void duplicate_selected_nodes(document_state& DocumentState);

} // namespace libk3dngui

#endif // !NGUI_DETAIL_H


