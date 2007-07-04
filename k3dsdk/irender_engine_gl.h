#ifndef K3DSDK_IRENDER_ENGINE_GL_H
#define K3DSDK_IRENDER_ENGINE_GL_H

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

#include "gl.h"
#include "iunknown.h"
#include "signal_system.h"

namespace k3d
{

class icamera;
class rectangle;

namespace gl
{

/// Abstract interface for objects that can render the document using OpenGL
class irender_engine :
	public virtual iunknown
{
public:
	/// Returns the normalized device coordinates for the viewport, allowing for mismatches between the aspect ratio of the window and the camera
	virtual bool get_ndc(icamera& Camera, const unsigned long PixelWidth, const unsigned long PixelHeight, rectangle& CameraRect, rectangle& WindowRect) = 0;

	/** \brief Redraws the document
	    \note The caller must setup an OpenGL render context before calling this method, and must call glFlush() after it returns */
	virtual void redraw(icamera& Camera, const unsigned long PixelWidth, const unsigned long PixelHeight, const unsigned long FontListBase, GLdouble ViewMatrix[16], GLdouble ProjectionMatrix[16], GLint Viewport[4]) = 0;

	/// Enumerates redraw request types
	typedef enum
	{
		SYNCHRONOUS,
		ASYNCHRONOUS
	} redraw_type_t;

	/// Used to signal any connected user interface components that a redraw is requested
	typedef sigc::signal<void, redraw_type_t> redraw_request_signal_t;
	virtual redraw_request_signal_t& redraw_request_signal() = 0;

protected:
	irender_engine() {}
	irender_engine(const irender_engine&) {}
	irender_engine& operator=(const irender_engine&) { return *this; }
	virtual ~irender_engine() {}
};

} // namespace gl

} // namespace k3d

#endif // K3DSDK_IRENDER_ENGINE_GL_H

