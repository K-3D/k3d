#ifndef K3DSDK_ISHADER_COLLECTION_RI_H_H
#define K3DSDK_ISHADER_COLLECTION_RI_H_H

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
	\author Tim Shead (tshead@k-3d.com)
*/

#include "iunknown.h"
#include "types_ri.h"

namespace k3d
{

namespace ri
{

/// Abstract interface to a collection of RenderMan shaders stored by source path (used to keep track of shaders that need to be compiled)
class ishader_collection :
	public virtual iunknown
{
public:
	virtual void use_shader(const path& Path) = 0;

protected:
	ishader_collection() {}
	ishader_collection(const ishader_collection& ) {}
	ishader_collection& operator=(const ishader_collection& ) { return *this; }
	virtual ~ishader_collection() {}
};

} // namespace ri

} // namespace k3d

#endif // !K3DSDK_ISHADER_COLLECTION_RI_H

