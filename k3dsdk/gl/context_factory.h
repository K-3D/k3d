#ifndef K3DSDK_GL_CONTEXT_FACTORY_H
#define K3DSDK_GL_CONTEXT_FACTORY_H

// K-3D
// Copyright (c) 1995-2010, Timothy M. Shead
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
#include <k3dsdk/types.h>

namespace k3d
{

namespace gl
{

class context;
	
/// Abstract interface implemented by objects that can create OpenGL render context objects,
class context_factory :
	public virtual iunknown
{
public:
	virtual ~context_factory() {}

	/// Creates a new OpenGL render context for use with the given drawable.  Note that
	/// the drawable could be an X11 XID, a Windows HWND, etc.  The caller is responsible
	/// for the lifetime of the returned object.
	virtual context* create(uint64_t Drawable) = 0;

protected:
	context_factory() {}
	context_factory(const context_factory&) {}
	context_factory& operator=(const context_factory&) { return *this; }
};
} // namespace gl

} // namespace k3d

#endif // !K3DSDK_GL_CONTEXT_FACTORY_H

