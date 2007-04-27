#ifndef K3DSDK_IMATERIAL_H
#define K3DSDK_IMATERIAL_H

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

/** \file
		\brief Declares k3d::imaterial, an abstract interface implemented by nodes that can act as surface materials
		\author Tim Shead (tshead@k-3d.com)
*/

#include "iunknown.h"

namespace k3d
{

namespace gl { class imaterial; }
namespace ri { class imaterial; }
namespace yafray { class imaterial; }

/// Abstract interface implemented by objects that can act as geometric surface materials
class imaterial :
	public virtual k3d::iunknown
{
public:
	virtual gl::imaterial* gl_material() = 0;
	virtual ri::imaterial* ri_material() = 0;
	virtual yafray::imaterial* yafray_material() = 0;

protected:
	imaterial() {}
	imaterial(const imaterial& RHS) {}
	imaterial& operator=(const imaterial& RHS) { return *this; }
	virtual ~imaterial() {}
};

} // namespace k3d

#endif // K3DSDK_IMATERIAL_H
