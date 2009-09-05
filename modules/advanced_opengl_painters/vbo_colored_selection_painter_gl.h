#ifndef MODULES_ADVANCED_OPENGL_PAINTERS_VBO_COLORED_SELECTION_PAINTER_GL_H
#define MODULES_ADVANCED_OPENGL_PAINTERS_VBO_COLORED_SELECTION_PAINTER_GL_H

// K-3D
// Copyright (c) 1995-2009, Timothy M. Shead
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

/** \file Base class for mesh painters that change colors by component- and mesh selection
 */

#include <k3d-i18n-config.h>
#include <k3dsdk/algebra.h>
#include <k3dsdk/data.h>
#include <k3dsdk/measurement.h>
#include <k3dsdk/imesh_painter_gl.h>
#include <k3dsdk/node.h>
#include <k3dsdk/painter_render_state_gl.h>
#include <k3dsdk/painter_selection_state_gl.h>
#include <k3dsdk/utility_gl.h>

#include "vbo.h"

namespace module
{

namespace opengl
{

namespace painters
{

class vbo_colored_selection_painter :
	public k3d::node,
	public k3d::gl::imesh_painter
{
	typedef k3d::node base;
public:
	vbo_colored_selection_painter(k3d::iplugin_factory& Factory,
			k3d::idocument& Document,
			const k3d::color UnselectedMeshColor = k3d::color(0.0, 0.0, 0.0),
			const k3d::color SelectedMeshColor = k3d::color(1.0, 1.0, 1.0),
			const k3d::color SelectedComponentColor = k3d::color(1.0, 0.0, 0.0),
			const k3d::color SelectedParentMeshColor = k3d::color(0.5, 0.5, 0.5)) :
		base(Factory, Document),
		m_enabled(init_owner(*this) + init_name("enabled") + init_label(_("Enabled")) + init_description(_("Enable / disable drawing for this painter.")) + init_value(true)),
		m_unselected_mesh_color(init_owner(*this) + init_name("unselected_mesh_color") + init_label(_("Unselected Mesh Color")) + init_description(_("Color the mesh is drawn in when it is unselected")) + init_value(UnselectedMeshColor)),
		m_parent_selected_mesh_color(init_owner(*this) + init_name("child_mesh_color") + init_label(_("Child Mesh Color")) + init_description(_("Color the mesh is drawn in when its parent is selected")) + init_value(SelectedParentMeshColor)),
		m_selected_mesh_color(init_owner(*this) + init_name("selected_mesh_color") + init_label(_("Selected Mesh Color")) + init_description(_("Color unselected components are drawn in when their mesh is selected")) + init_value(SelectedMeshColor)),
		m_selected_component_color(init_owner(*this) + init_name("selected_component_color") + init_label(_("Selected Component Color")) + init_description(_("Color selected components are drawn in")) + init_value(SelectedComponentColor))
	{
		m_unselected_mesh_color.changed_signal().connect(sigc::mem_fun(*this, &vbo_colored_selection_painter::make_unselected_texture));
		m_parent_selected_mesh_color.changed_signal().connect(sigc::mem_fun(*this, &vbo_colored_selection_painter::make_parent_selected_texture));
		m_selected_mesh_color.changed_signal().connect(sigc::mem_fun(*this, &vbo_colored_selection_painter::make_selected_texture));
		m_selected_component_color.changed_signal().connect(sigc::mem_fun(*this, &vbo_colored_selection_painter::make_component_texture));
	}
	
	void paint_mesh(const k3d::mesh& Mesh, const k3d::gl::painter_render_state& RenderState, k3d::iproperty::changed_signal_t& ChangedSignal)
	{
		if(!m_enabled.pipeline_value())
			return;

		clean_vbo_state();

		if(!glIsTexture(m_unselected_name) || !glIsTexture(m_parent_selected_name) || !glIsTexture(m_selected_name) || !glIsTexture(m_component_name))
			init_textures();

		k3d::gl::store_attributes attributes;

		glEnable(GL_TEXTURE_1D);
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

		if(RenderState.show_component_selection && RenderState.node_selection)
			bind_selected_component_color();
		else if(RenderState.node_selection)
			bind_selected_mesh_color();
		else
			bind_unselected_mesh_color(RenderState.parent_selection);

		on_paint_mesh(Mesh, RenderState, ChangedSignal);
		clean_vbo_state();
	}

	void select_mesh(const k3d::mesh& Mesh, const k3d::gl::painter_render_state& RenderState, const k3d::gl::painter_selection_state& SelectionState, k3d::iproperty::changed_signal_t& ChangedSignal)
	{
		if(m_enabled.pipeline_value())
		{
			clean_vbo_state();
			on_select_mesh(Mesh, RenderState, SelectionState, ChangedSignal);
			clean_vbo_state();
		}
	}

protected:
	/// Returns a slot that will schedule an asynchronous screen update when called
	sigc::slot<void, k3d::ihint*> make_async_redraw_slot()
	{
		return sigc::mem_fun(*this, &vbo_colored_selection_painter::async_redraw);
	}

	/// Schedules an asynchronous screen update
	void async_redraw(k3d::ihint*)
	{
		k3d::gl::redraw_all(document(), k3d::gl::irender_viewport::ASYNCHRONOUS);
	}

	/// Implement this in derived classes to draw mesh components
	virtual void on_paint_mesh(const k3d::mesh& Mesh, const k3d::gl::painter_render_state& RenderState, k3d::iproperty::changed_signal_t& ChangedSignal)
	{
	}

	/// Implement this in derived classes to draw mesh components for selection
	virtual void on_select_mesh(const k3d::mesh& Mesh, const k3d::gl::painter_render_state& RenderState, const k3d::gl::painter_selection_state& SelectionState, k3d::iproperty::changed_signal_t& ChangedSignal)
	{
	}

	/// Return the unselected mesh color. May be different if the parent mesh is selected
	void bind_unselected_mesh_color(bool ParentSelected)
	{
		if(ParentSelected)
		{
			return_if_fail(glIsTexture(m_parent_selected_name));
			glBindTexture(GL_TEXTURE_1D, m_parent_selected_name);
		}
		else
		{
			return_if_fail(glIsTexture(m_unselected_name));
			glBindTexture(GL_TEXTURE_1D, m_unselected_name);
		}
	}
	
	/// Return the selected mesh color
	void bind_selected_mesh_color()
	{
		return_if_fail(glIsTexture(m_selected_name));
		glBindTexture(GL_TEXTURE_1D, m_selected_name);
	}
	
	/// Return the selected component color
	void bind_selected_component_color()
	{
		return_if_fail(glIsTexture(m_component_name));
		glBindTexture(GL_TEXTURE_1D, m_component_name);
	}
	
private:
	void init_textures()
	{
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glGenTextures(1, &m_unselected_name);
		glGenTextures(1, &m_parent_selected_name);
		glGenTextures(1, &m_selected_name);
		glGenTextures(1, &m_component_name);
		make_unselected_texture(0);
		make_parent_selected_texture(0);
		make_selected_texture(0);
		make_component_texture(0);
	}

	void setup_texture(GLuint Name, GLubyte Texels[][3])
	{
		glBindTexture(GL_TEXTURE_1D, Name);
		return_if_fail(glIsTexture(Name));
		glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexImage1D(GL_TEXTURE_1D, 0, GL_RGB, TEX_SIZE, 0, GL_RGB, GL_UNSIGNED_BYTE, Texels);
	}

	void set_uniform_color(const k3d::color& Color, GLubyte ColorArray[][3])
	{
		for(k3d::uint_t i = 0; i != TEX_SIZE; ++i)
		{
			ColorArray[i][0] = static_cast<GLubyte>(Color.red*255);
			ColorArray[i][1] = static_cast<GLubyte>(Color.green*255);
			ColorArray[i][2] = static_cast<GLubyte>(Color.blue*255);
		}
	}

	void make_unselected_texture(k3d::ihint* Hint)
	{
		set_uniform_color(m_unselected_mesh_color.pipeline_value(), m_unselected_texels);
		setup_texture(m_unselected_name, m_unselected_texels);
		async_redraw(0);
	}

	void make_parent_selected_texture(k3d::ihint* Hint)
	{
		set_uniform_color(m_parent_selected_mesh_color.pipeline_value(), m_parent_selected_texels);
		setup_texture(m_parent_selected_name, m_parent_selected_texels);
		async_redraw(0);
	}

	void make_selected_texture(k3d::ihint* Hint)
	{
		set_uniform_color(m_selected_mesh_color.pipeline_value(), m_selected_texels);
		setup_texture(m_selected_name, m_selected_texels);
		make_component_texture(0);
	}

	void make_component_texture(k3d::ihint* Hint)
	{
		const k3d::color c1 = m_selected_mesh_color.pipeline_value();
		const k3d::color c2 = m_selected_component_color.pipeline_value();
		for(k3d::uint_t i = 0; i != TEX_SIZE; ++i)
		{
			const k3d::double_t weight = static_cast<k3d::double_t>(i) / static_cast<k3d::double_t>(TEX_SIZE - 1);
			k3d::color mixed = k3d::mix(c1, c2, weight);
			m_component_texels[i][0] = static_cast<GLubyte>(mixed.red*255);
			m_component_texels[i][1] = static_cast<GLubyte>(mixed.green*255);
			m_component_texels[i][2] = static_cast<GLubyte>(mixed.blue*255);
		}
		setup_texture(m_component_name, m_component_texels);
		async_redraw(0);
	}

	/// Used to enable/disable the painter
	k3d_data(bool, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_enabled;

	/// Color used when the mesh is not selected
	k3d_data(k3d::color, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_unselected_mesh_color;
	
	/// Color used when the mesh is not selected, but its parent is
	k3d_data(k3d::color, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_parent_selected_mesh_color;
	
	/// Color used for unselected components on selected mesh
	k3d_data(k3d::color, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_selected_mesh_color;
	
	/// Color used for selected components
	k3d_data(k3d::color, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_selected_component_color;

	static const k3d::uint_t TEX_SIZE = 256;

	/// 1D Texture arrays for each selection type
	GLubyte m_unselected_texels[TEX_SIZE][3];
	GLubyte m_parent_selected_texels[TEX_SIZE][3];
	GLubyte m_selected_texels[TEX_SIZE][3];
	GLubyte m_component_texels[TEX_SIZE][3];
	GLuint m_unselected_name;
	GLuint m_parent_selected_name;
	GLuint m_selected_name;
	GLuint m_component_name;
}; // class vbo_colored_selection_painter

} // namespace opengl

} // namespace painters

} // namespace module

#endif // !MODULES_ADVANCED_OPENGL_PAINTERS_VBO_COLORED_SELECTION_PAINTER_GL_H

