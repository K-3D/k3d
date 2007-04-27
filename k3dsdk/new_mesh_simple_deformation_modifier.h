#ifndef K3DSDK_NEW_MESH_SIMPLE_DEFORMATION_MODIFIER_H
#define K3DSDK_NEW_MESH_SIMPLE_DEFORMATION_MODIFIER_H

// K-3D
// Copyright (c) 1995-2006, Timothy M. Shead
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
	\author Timothy M. Shead (tshead@k-3d.com)
 */

#include "mesh_selection_sink.h"
#include "new_mesh_modifier.h"
#include "node.h"
#include "persistent.h"

namespace k3d
{

namespace dev
{

/// Mesh modifier implementation for use in plugins that deform a mesh (modify its points) without altering topology, and based solely on the input point positions and selection.  To create a plugin, derive from mesh_simple_deformation_modifier and implement the on_deform_mesh() method.  If you are only modifying points but need more information about the input mesh, consider using mesh_deformation_modifier instead. 
class mesh_simple_deformation_modifier :
	public mesh_selection_sink<mesh_modifier<persistent<node> > >
{
	typedef mesh_selection_sink<mesh_modifier<persistent<node> > > base;

public:
	mesh_simple_deformation_modifier(iplugin_factory& Factory, idocument& Document);

private:
	void on_create_mesh(const mesh& Input, mesh& Output);
	void on_update_mesh(const mesh& Input, mesh& Output);

	/// Implement this method in derived classes and deform the output mesh using its input points and selection.
	virtual void on_deform_mesh(const mesh::points_t& InputPoints, const mesh::selection_t& PointSelection, mesh::points_t& OutputPoints) = 0;
};

} // namespace dev
	
} // namespace k3d

#endif // !K3DSDK_NEW_MESH_SIMPLE_DEFORMATION_MODIFIER

