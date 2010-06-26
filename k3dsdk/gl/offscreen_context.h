#ifndef K3DSDK_GL_OFFSCREEN_CONTEXT_H
#define K3DSDK_GL_OFFSCREEN_CONTEXT_H

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

#include <k3dsdk/gl/context.h>

namespace k3d
{

namespace gl
{

class api;

/// Abstract interface implemented by OpenGL context objects
class offscreen_context :
	public context
{
public:
	virtual ~offscreen_context() {}

	/// Returns a half-open range of bytes that store the offscreen buffer.
	virtual const uint8_t* buffer_begin() = 0;
	/// Returns a half-open range of bytes that store the offscreen buffer.
	virtual const uint8_t* buffer_end() = 0;

protected:
	offscreen_context() {}
	offscreen_context(const offscreen_context&) {}
	offscreen_context& operator=(const offscreen_context&) { return *this; }
};

} // namespace gl

} // namespace k3d

#endif // !K3DSDK_GL_OFFSCREEN_CONTEXT_H

