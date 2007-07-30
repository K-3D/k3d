#ifndef K3DSDK_MESH_PAINTER_GL_H
#define K3DSDK_MESH_PAINTER_GL_H

// K-3D
// Copyright (c) 1995-2007, Timothy M. Shead
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
#include "imesh_painter_gl.h"
#include "node.h"
#include "persistent.h"
#include "utility_gl.h"

namespace k3d
{

namespace gl
{

/// Provides a boilerplate implementation of k3d::gl::imesh_painter
class mesh_painter :
	public persistent<node>,
	public imesh_painter
{
	typedef persistent<node> base;

public:
	mesh_painter(iplugin_factory& Factory, idocument& Document) :
		base(Factory, Document),
		m_enabled(init_owner(*this) + init_name("enabled") + init_label(_("Enabled")) + init_description(_("Enable / disable drawing for this painter.")) + init_value(true))
	{
		m_enabled.changed_signal().connect(make_async_redraw_slot());
	}

	void paint_mesh(const mesh& Mesh, const painter_render_state& RenderState)
	{
		if(m_enabled.pipeline_value())
			on_paint_mesh(Mesh, RenderState);
	}

	void select_mesh(const mesh& Mesh, const painter_render_state& RenderState, const painter_selection_state& SelectionState)
	{
		if(m_enabled.pipeline_value())
			on_select_mesh(Mesh, RenderState, SelectionState);
	}

	void mesh_changed(const mesh& Mesh, iunknown* Hint)
	{
		// Note - on_mesh_changed() is always called whether we're enabled or not to prevent caching of stale data
		on_mesh_changed(Mesh, Hint);
	}

protected:
	/// Returns a slot that will schedule an asynchronous screen update when called
	sigc::slot<void, iunknown*> make_async_redraw_slot()
	{
		return sigc::mem_fun(*this, &mesh_painter::async_redraw);
	}

	/// Schedules an asynchronous screen update
	void async_redraw(iunknown*)
	{
		redraw_all(document(), irender_engine::ASYNCHRONOUS);
	}

	/// Implement this in derived classes to draw mesh components
	virtual void on_paint_mesh(const mesh& Mesh, const painter_render_state& RenderState)
	{
	}

	/// Implement this in derived classes to draw mesh components for selection
	virtual void on_select_mesh(const mesh& Mesh, const painter_render_state& RenderState, const painter_selection_state& SelectionState)
	{
	}

	/// Implement this in derived classes to clear cached data when a mesh changes
	virtual void on_mesh_changed(const mesh& Mesh, iunknown* Hint)
	{
	}

	/// Used to enable/disable the painter
	k3d_data(bool, data::immutable_name, data::change_signal, data::with_undo, data::local_storage, data::no_constraint, data::writable_property, data::with_serialization) m_enabled;
};

} // namespace gl

} // namespace k3d

#endif // K3DSDK_MESH_PAINTER_GL_H

