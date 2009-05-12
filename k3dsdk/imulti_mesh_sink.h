#ifndef K3DSDK_IMULTI_MESH_SINK_H
#define K3DSDK_IMULTI_MESH_SINK_H

// K-3D
// Copyright (c) 1995-2008, Timothy M. Shead
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
		\brief Declares imulti_mesh_sink, an interface for objects that can consume multiple meshes
		\author Tim Shead (tshead@k-3d.com)
*/

#include "iunknown.h"

namespace k3d
{

/// Abstract interface for objects that can consume multiple meshes
class imulti_mesh_sink :
	public virtual iunknown
{

protected:
	imulti_mesh_sink() {}
	imulti_mesh_sink(const imulti_mesh_sink& Other) : iunknown(Other) {}
	imulti_mesh_sink& operator=(const imulti_mesh_sink&) { return *this; }
	virtual ~imulti_mesh_sink() {}
};

} // namespace k3d

#endif // !K3DSDK_IMULTI_MESH_SINK_H

