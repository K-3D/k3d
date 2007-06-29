#ifndef K3DSDK_DRAWABLE_GL_H
#define K3DSDK_DRAWABLE_GL_H

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

#include "data.h"
#include "gl.h"
#include "k3d-i18n-config.h"
#include "idrawable_gl.h"
#include "irender_engine_gl.h"
#include "render_state_gl.h"
#include "utility_gl.h"

namespace k3d
{

class bounding_box3;
class idocument;
class plane;

namespace gl
{

class selection_state;
	
/**	\brief Provides a boilerplate implementation of k3d::gl::idrawable
	\param base_t Must derive from k3d::transformable
*/
template<typename base_t>
class drawable :
	public base_t,
	public idrawable
{
public:
	drawable(iplugin_factory& Factory, idocument& Document) :
		base_t(Factory, Document),
		m_visible(init_owner(*this) + init_name("viewport_visible") + init_label(_("Viewport Visible")) + init_description(_("Controls whether this node will be visibile in the viewport.")) + init_value(true)),
		m_nurbs_renderer(0)
	{
		m_visible.changed_signal().connect(make_async_redraw_slot());
	}

	~drawable()
	{
		if(m_nurbs_renderer)
			gluDeleteNurbsRenderer(m_nurbs_renderer);
	}

	void gl_draw(const render_state& State)
	{
		if(!m_visible.value())
			return;

		store_attributes attributes;

		push_draw();
		on_gl_draw(State);
		pop_draw();
	}

	void gl_select(const render_state& State, const selection_state& SelectState)
	{
		if(!m_visible.value())
			return;

		store_attributes attributes;

		push_draw();
		on_gl_select(State, SelectState);
		pop_draw();
	}

protected:
	sigc::slot<void, iunknown*> make_async_redraw_slot()
	{
		return sigc::mem_fun(*this, &drawable<base_t>::async_redraw);
	}

	void async_redraw(iunknown*)
	{
		redraw_all(base_t::document(), irender_engine::ASYNCHRONOUS);
	}

	typedef GLUnurbsObj* nurbs_renderer_t;
	nurbs_renderer_t nurbs_renderer(const render_state& State)
	{
		if(!m_nurbs_renderer)
		{
			m_nurbs_renderer = gluNewNurbsRenderer();

			// Important!  We load our own matrices for efficiency (saves round-trips to the server) and to prevent problems with selection
			gluNurbsProperty(m_nurbs_renderer, GLU_AUTO_LOAD_MATRIX, GL_FALSE);
			gluNurbsProperty(m_nurbs_renderer, GLU_CULLING, GL_TRUE);
		}

		GLfloat gl_modelview_matrix[16];
		glGetFloatv(GL_MODELVIEW_MATRIX, gl_modelview_matrix);
		gluLoadSamplingMatrices(m_nurbs_renderer, gl_modelview_matrix, State.gl_projection_matrix, State.gl_viewport);

		return m_nurbs_renderer;
	}

protected:
	/// Set to true iff the object should be visible in OpenGL viewports
	k3d_data(bool, data::immutable_name, data::change_signal, data::with_undo, data::local_storage, data::no_constraint, data::writable_property, data::with_serialization) m_visible;

private:
	void push_draw()
	{
		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		push_matrix(base_t::matrix());
	}

	void pop_draw()
	{
		glMatrixMode(GL_MODELVIEW);
		glPopMatrix();
	}

	virtual void on_gl_draw(const render_state& State) = 0;
	virtual void on_gl_select(const render_state& State, const selection_state& SelectState) = 0;

	/// OpenGL NURBS renderer
	nurbs_renderer_t m_nurbs_renderer;
};

} // namespace gl

} // namespace k3d

#endif // K3DSDK_DRAWABLE_GL_H

