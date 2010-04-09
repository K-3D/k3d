#ifndef K3DSDK_GL_OFFSCREEN_CONTEXT_FACTORY_H
#define K3DSDK_GL_OFFSCREEN_CONTEXT_FACTORY_H

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

class offscreen_context;
	
/// Abstract interface implemented by objects that can create OpenGL render context objects,
class offscreen_context_factory :
	public virtual iunknown
{
public:
	virtual ~offscreen_context_factory() {}

	/// Creates a new offscreen OpenGL render context with the given width and height in pixels.
	/// The caller is responsible for the lifetime of the returned object.
	virtual offscreen_context* create(const uint_t Width, const uint_t Height) = 0;

protected:
	offscreen_context_factory() {}
	offscreen_context_factory(const offscreen_context_factory&) {}
	offscreen_context_factory& operator=(const offscreen_context_factory&) { return *this; }
};
} // namespace gl

} // namespace k3d

#endif // !K3DSDK_GL_OFFSCREEN_CONTEXT_FACTORY_H

