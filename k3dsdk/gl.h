#ifndef K3DSDK_GL_H
#define K3DSDK_GL_H

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

#include "k3d-platform-config.h"

#if defined K3D_API_WIN32
	#include "win32.h"
#endif // !K3D_API_WIN32

#define GLEW_MX // multiple-context glew as per http://glew.sourceforge.net/advanced.html
#include <glew/glew.h> // Needed for cross-platform OpenGL > 1.1

// Keep track of current glew context. Viewport is responsible for setting this so glewGetContext works correctly
class glew_context
{
public:
	/// Singleton implementation
	static glew_context& instance();
	
	/// Set the glew context
	void set_context(GLEWContext* Context);
	
	/// Get the glew context
	GLEWContext* context();
private:
	glew_context() : m_context(0) {}

	static glew_context* m_instance;
	GLEWContext* m_context;
};

// Required if GLEW_MX is defined:
#define glewGetContext() glew_context::instance().context()

#endif // K3DSDK_GL_H

