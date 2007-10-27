#ifndef K3DSDK_IMATERIAL_GL_H
#define K3DSDK_IMATERIAL_GL_H

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

namespace k3d
{

class icamera;
class rectangle;

namespace gl
{

class selection_state;
	
/// Abstract interface for returning selection data
class iselection_engine :
	public virtual iunknown
{
public:
	/**
	  \brief Draws the document in OpenGL selection mode, so selection "hits" can be extracted
	  \param SelectionState The geometry components to be drawn
	  \param PixelWidth The width of the viewport in pixels
	  \param PixelHeight The width of the viewport in pixels
	  \param FontListBase
	  \param Rectangle Defines the selection region as a rectangle in screen (pixel coordinates)
	  \param ViewMatrix Returns the OpenGL view matrix used for drawing (the camera-to-world matrix)
	  \param ProjectionMatrix Returns the OpenGL projection matrix used for drawing
	  \param Viewport Returns the OpenGL viewport used for drawing
	  \note The caller must setup the OpenGL render context, allocate the selection buffer, put OpenGL in selection mode, and call glFlush() when done
	*/
	virtual void select(const selection_state& SelectionState, icamera& Camera, const unsigned long PixelWidth, const unsigned long PixelHeight, const unsigned long FontListBase, const rectangle& Rectangle, GLdouble ViewMatrix[16], GLdouble ProjectionMatrix[16], GLint Viewport[4]) = 0;

protected:
	iselection_engine() {}
	iselection_engine(const iselection_engine&) {}
	iselection_engine& operator=(const iselection_engine&) { return *this; }
	virtual ~iselection_engine() {}
};

} // namespace gl

} // namespace k3d

#endif // K3DSDK_IMATERIAL_GL_H

