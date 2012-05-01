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
#include <k3dsdk/log.h>
#include <k3dsdk/log_control.h>

namespace k3d
{

namespace gl
{

static context*& g_current()
{
	static context* storage = 0;
	return storage;
}

static GLEWContext*& g_current_glew()
{
	static GLEWContext* storage = 0;
	return storage;
}

void context::begin()
{
	g_current() = this;
	g_current_glew() = glew_context();
	on_begin();
}

context* context::current()
{
	return g_current();
}

GLEWContext* context::current_glew_context()
{
	return g_current_glew();
}

void context::end()
{
	on_end();
	g_current() = 0;
}

GLEWContext* context::glew_context()
{
	if(!m_glew_context)
	{
		on_begin();
		m_glew_context.reset(new GLEWContext());
		g_current_glew() = m_glew_context.get();
		GLenum err = glewInit(); // needs to be called after context creation
		if (GLEW_OK != err)
		{
			k3d::log() << error << "GLEW init failed: " << glewGetErrorString(err) << std::endl;
			m_glew_context.reset();
		}
		on_end();
		assert(m_glew_context);
	}
	return m_glew_context.get();
}

} // namespace gl

} // namespace k3d

