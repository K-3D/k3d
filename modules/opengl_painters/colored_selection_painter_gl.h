#ifndef COLORED_SELECTION_PAINTER_GL_H_
#define COLORED_SELECTION_PAINTER_GL_H_
// K-3D
// Copyright (c) 1995-2005, Timothy M. Shead
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

#include <k3dsdk/measurement.h>
#include <k3dsdk/mesh_painter_gl.h>

namespace module
{

namespace opengl
{

namespace painters
{

class colored_selection_painter : 
	public k3d::gl::mesh_painter
{
	typedef k3d::gl::mesh_painter base;
public:
	/// Use 4-component color for alpha
	typedef k3d::basic_rgba<double, k3d::color_traits<double> > color_t;
	colored_selection_painter(k3d::iplugin_factory& Factory,
			k3d::idocument& Document,
			const k3d::color UnselectedMeshColor = k3d::color(0.0, 0.0, 0.0),
			const k3d::color SelectedMeshColor = k3d::color(1.0, 1.0, 1.0),
			const k3d::color SelectedComponentColor = k3d::color(1.0, 0.0, 0.0),
			const k3d::color SelectedParentMeshColor = k3d::color(0.5, 0.5, 0.5)) :
		base(Factory, Document),
		m_unselected_mesh_color(init_owner(*this) + init_name("unselected_mesh_color") + init_label(_("Unselected Mesh Color")) + init_description(_("Color the mesh is drawn in when it is unselected")) + init_value(UnselectedMeshColor)),
		m_parent_selected_mesh_color(init_owner(*this) + init_name("child_mesh_color") + init_label(_("Child Mesh Color")) + init_description(_("Color the mesh is drawn in when its parent is selected")) + init_value(SelectedParentMeshColor)),
		m_selected_mesh_color(init_owner(*this) + init_name("selected_mesh_color") + init_label(_("Selected Mesh Color")) + init_description(_("Color unselected components are drawn in when their mesh is selected")) + init_value(SelectedMeshColor)),
		m_selected_component_color(init_owner(*this) + init_name("selected_component_color") + init_label(_("Selected Component Color")) + init_description(_("Color selected components are drawn in")) + init_value(SelectedComponentColor)),
		m_alpha(init_owner(*this) + init_name("alpha") + init_label(_("Alpha")) + init_description(_("Set the transparency (0 is transparent)")) + init_value(1.0) + init_constraint(k3d::data::constraint::minimum(0.0, k3d::data::constraint::maximum<double>(1.0))) + init_step_increment(0.1) + init_units(typeid(k3d::measurement::scalar)))
	{
		m_unselected_mesh_color.changed_signal().connect(make_async_redraw_slot());
		m_parent_selected_mesh_color.changed_signal().connect(make_async_redraw_slot());
		m_selected_mesh_color.changed_signal().connect(make_async_redraw_slot());
		m_selected_component_color.changed_signal().connect(make_async_redraw_slot());
		m_alpha.changed_signal().connect(make_async_redraw_slot());
	}
	
protected:
	/// Return the unselected mesh color. May be different if the parent mesh is selected
	const color_t unselected_mesh_color(bool ParentSelected)
	{
		k3d::color c = ParentSelected ? m_parent_selected_mesh_color.pipeline_value() : m_unselected_mesh_color.pipeline_value(); 
		return color_t(c.red, c.green, c.blue, m_alpha.pipeline_value());
	}
	
	/// Return the selected mesh color
	const color_t selected_mesh_color()
	{
		k3d::color c = m_selected_mesh_color.pipeline_value();
		return color_t(c.red, c.green, c.blue, m_alpha.pipeline_value());
	}
	
	/// Return the selected component color
	const color_t selected_component_color()
	{
		k3d::color c = m_selected_component_color.pipeline_value();
		return color_t(c.red, c.green, c.blue, m_alpha.pipeline_value());
	}
	
	/// Enable alpha blending, if needed
	void enable_blending()
	{
		if (m_alpha.pipeline_value() != 1.0)
		{
			glDisable(GL_DEPTH_TEST);
			glDisable(GL_CULL_FACE);
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		}
		else
		{
			disable_blending();
		}
	}
	
	/// Disable alpha blending
	void disable_blending()
	{
		glDisable(GL_BLEND);
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);
	}
	
private:
	/// Color used when the mesh is not selected
	k3d_data(k3d::color, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_unselected_mesh_color;
	
	/// Color used when the mesh is not selected, but its parent is
	k3d_data(k3d::color, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_parent_selected_mesh_color;
	
	/// Color used for unselected components on selected mesh
	k3d_data(k3d::color, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_selected_mesh_color;
	
	/// Color used for selected components
	k3d_data(k3d::color, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_selected_component_color;
	
	/// Alpha channel
	k3d_data(double, immutable_name, change_signal, with_undo, local_storage, with_constraint, measurement_property, with_serialization) m_alpha;
}; // class colored_selection_painter

/// Helper function to pass color to OpenGL
inline void color4d(const colored_selection_painter::color_t& Color)
{
	glColor4d(Color.red, Color.green, Color.blue, Color.alpha);
}

} // namespace opengl

} // namespace painters

} // namespace module

#endif /*COLORED_SELECTION_PAINTER_GL_H_*/

