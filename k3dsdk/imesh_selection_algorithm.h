#ifndef K3DSDK_IMESH_SELECTION_ALGORITHM_H
#define K3DSDK_IMESH_SELECTION_ALGORITHM_H

// K-3D
// Copyright (c) 1995-2009, Timothy M. Shead
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

#include <k3dsdk/iunknown.h>
#include <k3dsdk/mesh.h>
#include <k3dsdk/selection.h>

namespace k3d
{

/// Abstract interface for objects that can create mesh selections
class imesh_selection_algorithm :
	public virtual iunknown
{
public:
	virtual const selection::set create_mesh_selection(const mesh& Mesh, const selection::set& CurrentSelection) = 0;

protected:
	imesh_selection_algorithm() {}
	imesh_selection_algorithm(const imesh_selection_algorithm& Other) : iunknown(Other) {}
	imesh_selection_algorithm& operator=(const imesh_selection_algorithm&) { return *this; }
	virtual ~imesh_selection_algorithm() {}
};

} // namespace k3d

#endif // !K3DSDK_IMESH_SELECTION_ALGORITHM_H

