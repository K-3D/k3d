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

#include <k3dsdk/mesh_painter_gl.h>

namespace libk3ddevelopment
{

class colored_selection_painter : 
	public k3d::gl::mesh_painter
{
	typedef k3d::gl::mesh_painter base;
public:
	colored_selection_painter(k3d::iplugin_factory& Factory,
			k3d::idocument& Document,
			const k3d::color UnselectedMeshColor = k3d::color(0.0, 0.0, 0.0),
			const k3d::color SelectedMeshColor = k3d::color(1.0, 1.0, 1.0),
			const k3d::color SelectedComponentColor = k3d::color(1.0, 0.0, 0.0)) :
		base(Factory, Document),
		m_unselected_mesh_color(init_owner(*this) + init_name("unselected_mesh_color") + init_label(_("Unselected Mesh Color")) + init_description(_("Color the mesh is drawn in when it is unselected")) + init_value(UnselectedMeshColor)),
		m_selected_mesh_color(init_owner(*this) + init_name("selected_mesh_color") + init_label(_("Selected Mesh Color")) + init_description(_("Color unselected components are drawn in when their mesh is selected")) + init_value(SelectedMeshColor)),
		m_selected_component_color(init_owner(*this) + init_name("selected_component_color") + init_label(_("Selected Component Color")) + init_description(_("Color selected components are drawn in")) + init_value(SelectedComponentColor))
	{
		m_unselected_mesh_color.changed_signal().connect(make_async_redraw_slot());
		m_selected_mesh_color.changed_signal().connect(make_async_redraw_slot());
		m_selected_component_color.changed_signal().connect(make_async_redraw_slot());
	}
	
protected:
	/// Return the unselected mesh color
	const k3d::color unselected_mesh_color()
	{
		return m_unselected_mesh_color.pipeline_value();
	}
	
	/// Return the selected mesh color
	const k3d::color selected_mesh_color()
	{
		return m_selected_mesh_color.pipeline_value();
	}
	
	/// Return the selected component color
	const k3d::color selected_component_color()
	{
		return m_selected_component_color.pipeline_value();
	}
	
	
	
private:
	/// Color used when the mesh is not selected
	k3d_data(k3d::color, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_unselected_mesh_color;
	
	/// Color used for unselected components on selected mesh
	k3d_data(k3d::color, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_selected_mesh_color;
	
	/// Color used for selected components
	k3d_data(k3d::color, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_selected_component_color;
}; // class colored_selection_painter

} // namespace libk3ddevelopment

#endif /*COLORED_SELECTION_PAINTER_GL_H_*/
