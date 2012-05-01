#ifndef K3DSDK_GL_CONTEXT_H
#define K3DSDK_GL_CONTEXT_H

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
#include <k3dsdk/gl.h>
#include <k3dsdk/iunknown.h>
#include <boost/scoped_ptr.hpp>

namespace k3d
{

namespace gl
{

/// Abstract interface implemented by OpenGL context objects
class context :
	public virtual iunknown
{
public:
	virtual ~context() {}

	/// Activates this context for drawing in the calling thread
	void begin();

	/// Returns the currently-active context, if any, or NULL.
	static context* current();
	
	/// Returns the currently-active GLEW context, if any, or NULL. Mostly used internally by GLEW
	static GLEWContext* current_glew_context();

	/// If the context is double-buffered, swaps the front and back buffers.  Otherwise, a no-op.
	void end();
	
	/// Return the GLEW context associated with this context, or NULL if there is none
	GLEWContext* glew_context();

protected:
	context() {}
	context(const context&) {}
	context& operator=(const context&) { return *this; }

	/// Concrete implementations should activate this context for drawing in the calling thread
	virtual void on_begin() = 0;

	/// If the context is double-buffered, concrete implementations should swap the front and back buffers.  Otherwise, a no-op.
	virtual void on_end() = 0;
private:
	boost::scoped_ptr<GLEWContext> m_glew_context;
};

} // namespace gl

} // namespace k3d

#define glewGetContext() k3d::gl::context::current_glew_context()

#endif // !K3DSDK_GL_CONTEXT_H

