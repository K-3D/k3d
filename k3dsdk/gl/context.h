#ifndef K3DSDK_OPENGL_CONTEXT_H
#define K3DSDK_OPENGL_CONTEXT_H

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

namespace k3d
{

namespace gl
{

class api;

/// Abstract interface implemented by OpenGL context objects
class context :
	public virtual iunknown
{
public:
	virtual ~context() {}

	/// Activates this context for drawing in the calling thread.	
	virtual void make_current() = 0;

	/// If the context is double-buffered, swaps the front and back buffers.  Otherwise, a no-op.
	virtual void swap_buffers() = 0;

	/// Returns an OpenGL drawing API object for rendering using this context.
	virtual const api& draw() = 0;

protected:
	context() {}
	context(const context&) {}
	context& operator=(const context&) { return *this; }
};

} // namespace gl

} // namespace k3d

#endif // !K3DSDK_OPENGL_CONTEXT_H

