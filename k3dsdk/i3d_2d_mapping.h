#ifndef K3DSDK_I3D_2D_MAPPING_H
#define K3DSDK_I3D_2D_MAPPING_H

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

#include "iunknown.h"
#include "bounding_box.h"
#include "vectors.h"

namespace k3d
{

/// Abstract interface for properties that represent filesystem paths (typically used to alter presentation)
class i3d_2d_mapping :
	public virtual iunknown
{
public:
	virtual const point2 map_3d_to_2d(const point3& Position, const bounding_box3& Bounds) = 0;

protected:
	i3d_2d_mapping() {}
	i3d_2d_mapping(const i3d_2d_mapping&) {}
	i3d_2d_mapping& operator = (const i3d_2d_mapping&) { return *this; }
	virtual ~i3d_2d_mapping() {}
};

} // namespace k3d

#endif // K3DSDK_I3D_2D_MAPPING_H

