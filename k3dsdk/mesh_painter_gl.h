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

#include <k3d-i18n-config.h>
#include <k3dsdk/data.h>
#include <k3dsdk/imesh_painter_gl.h>
#include <k3dsdk/node.h>
#include <k3dsdk/utility_gl.h>

namespace k3d
{

namespace gl
{

/// Provides a boilerplate implementation of k3d::gl::imesh_painter
class mesh_painter :
	public node,
	public imesh_painter
{
	typedef node base;

public:
	mesh_painter(iplugin_factory& Factory, idocument& Document) :
		base(Factory, Document),
		m_enabled(init_owner(*this) + init_name("enabled") + init_label(_("Enabled")) + init_description(_("Enable / disable drawing for this painter.")) + init_value(true))
	{
		m_enabled.changed_signal().connect(make_async_redraw_slot());
	}

	void paint_mesh(const mesh& Mesh, const painter_render_state& RenderState, iproperty::changed_signal_t& ChangedSignal)
	{
		if(m_enabled.pipeline_value())
			on_paint_mesh(Mesh, RenderState, ChangedSignal);
	}

	void select_mesh(const mesh& Mesh, const painter_render_state& RenderState, const painter_selection_state& SelectionState, iproperty::changed_signal_t& ChangedSignal)
	{
		if(m_enabled.pipeline_value())
			on_select_mesh(Mesh, RenderState, SelectionState, ChangedSignal);
	}

protected:
	/// Returns a slot that will schedule an asynchronous screen update when called
	sigc::slot<void, ihint*> make_async_redraw_slot()
	{
		return sigc::mem_fun(*this, &mesh_painter::async_redraw);
	}

	/// Schedules an asynchronous screen update
	void async_redraw(ihint*)
	{
		redraw_all(document(), irender_viewport::ASYNCHRONOUS);
	}

	/// Implement this in derived classes to draw mesh components
	virtual void on_paint_mesh(const mesh& Mesh, const painter_render_state& RenderState, iproperty::changed_signal_t& ChangedSignal)
	{
	}

	/// Implement this in derived classes to draw mesh components for selection
	virtual void on_select_mesh(const mesh& Mesh, const painter_render_state& RenderState, const painter_selection_state& SelectionState, iproperty::changed_signal_t& ChangedSignal)
	{
	}

	/// Used to enable/disable the painter
	k3d_data(bool, data::immutable_name, data::change_signal, data::with_undo, data::local_storage, data::no_constraint, data::writable_property, data::with_serialization) m_enabled;
};

} // namespace gl

} // namespace k3d

#endif // !K3DSDK_MESH_PAINTER_GL_H

