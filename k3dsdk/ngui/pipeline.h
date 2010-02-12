#ifndef K3DSDK_NGUI_PIPELINE_H
#define K3DSDK_NGUI_PIPELINE_H

// K-3D
// Copyright (c) 1995-2010, Timothy M. Shead
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
	\author Tim Shead (tshead@k-3d.com)
	\author Romain Behar (romainbehar@yahoo.com)
*/

#include <vector>

namespace k3d
{

class idocument;
class inode;
class iplugin_factory;

namespace ngui
{

namespace pipeline
{

/// Creates a new node, performing many standard initialization operations.
inode* create_node(idocument& Document, iplugin_factory& Factory);

/// Makes a set of new nodes visible, adding them to all current render engines.
void show_nodes(idocument& Document, const std::vector<inode*>& Nodes);

/// Duplicates first node's transformation into a FrozenTransformationa and connects it to second node
void freeze_transformation(inode& FromNode, inode& ToNode, idocument& Document);
/// Instantiates the given node, returning the mesh instance if it was a mesh or null otherwise
inode* instantiate_mesh(idocument& Document, inode& Node);
/// Instantiates (creates MeshInstance outputs for) nodes - note, only mesh sources are instantiable.
void instantiate_selected_nodes(idocument& Document);
/// Duplicates nodes with their input transformation, if any
void duplicate_selected_nodes(idocument& Document);

} // namespace pipeline

} // namespace ngui

} // namespace k3d

#endif // !K3DSDK_NGUI_PIPELINE_H


