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

/** \file Singleton class to store the glew context, needed for multiple OpenGL windows on Win32
		\author Bart Janssens (bart.janssens@lid.kviv.be)
*/

#if defined K3D_PLATFORM_WIN32

#include "gl.h"

glew_context* glew_context::m_instance = 0;

glew_context& glew_context::instance()
{
	if (!m_instance)
		m_instance = new glew_context();
	return *m_instance;
}

void glew_context::set_context(GLEWContext* Context)
{
	m_context = Context;
}

GLEWContext* glew_context::context()
{
	return m_context;
}

#endif
