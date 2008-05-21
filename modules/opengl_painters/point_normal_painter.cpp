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
		\author Timothy M. Shead (tshead@k-3d.com)
*/

#include <k3dsdk/document_plugin_factory.h>
#include <k3d-i18n-config.h>
#include <k3dsdk/mesh_painter_gl.h>
#include <k3dsdk/mesh_operations.h>
#include <k3dsdk/painter_render_state_gl.h>
#include <k3dsdk/selection.h>

#include "normal_cache.h"

namespace module
{

namespace opengl
{

namespace painters
{

/////////////////////////////////////////////////////////////////////////////
// point_normal_painter

class point_normal_painter :
	public k3d::gl::mesh_painter
{
	typedef k3d::gl::mesh_painter base;

public:
	point_normal_painter(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_draw_selected(init_owner(*this) + init_name("draw_selected") + init_label(_("Draw Selected")) + init_description(_("Draw normals for selected polygons")) + init_value(true)),
		m_draw_unselected(init_owner(*this) + init_name("draw_unselected") + init_label(_("Draw Unselected")) + init_description(_("Draw normals for unselected polygons")) + init_value(false)),
		m_selected_color(init_owner(*this) + init_name("selected_color") + init_label(_("Selected Color")) + init_description(_("Normal color for selected polygons")) + init_value(k3d::color(0, 1, 1))),
		m_unselected_color(init_owner(*this) + init_name("unselected_color") + init_label(_("Unselected Color")) + init_description(_("Normal color for unselected polygons")) + init_value(k3d::color(0, 0.6, 0.6)))
	{
		m_draw_selected.changed_signal().connect(make_async_redraw_slot());
		m_draw_unselected.changed_signal().connect(make_async_redraw_slot());
		m_selected_color.changed_signal().connect(make_async_redraw_slot());
		m_unselected_color.changed_signal().connect(make_async_redraw_slot());
	}

	void on_paint_mesh(const k3d::mesh& Mesh, const k3d::gl::painter_render_state& RenderState)
	{
		const bool draw_selected = m_draw_selected.pipeline_value() && RenderState.show_component_selection;
		const bool draw_unselected = m_draw_unselected.pipeline_value();

		if(!draw_selected && !draw_unselected)
			return;

		if(!k3d::validate_polyhedra(Mesh))
			return;

		const k3d::mesh::selection_t& point_selection = *Mesh.point_selection;
		const k3d::mesh::points_t& points = *Mesh.points;

		const k3d::uint_t point_count = points.size();
		
		normal_cache& n_cache = get_data<normal_cache>(&Mesh, this);

		k3d::gl::store_attributes attributes;
		glDisable(GL_LIGHTING);

		if(draw_selected)
		{
			k3d::gl::color3d(m_selected_color.pipeline_value());

			glBegin(GL_LINES);
			for(k3d::uint_t point = 0; point != point_count; ++point)
			{
				if(point_selection[point])
				{
					k3d::gl::vertex3d(points[point]);
					k3d::gl::vertex3d(points[point] + k3d::to_point(n_cache.point_normals(this).at(point)));
				}
			}
			glEnd();
		}

		if(draw_unselected)
		{
			k3d::gl::color3d(m_unselected_color.pipeline_value());

			glBegin(GL_LINES);
			for(k3d::uint_t point = 0; point != point_count; ++point)
			{
				if(!point_selection[point])
				{
					k3d::gl::vertex3d(points[point]);
					k3d::gl::vertex3d(points[point] + k3d::to_point(n_cache.point_normals(this).at(point)));
				}
			}
			glEnd();
		}
	}
	
	void on_mesh_changed(const k3d::mesh& Mesh, k3d::iunknown* Hint)
	{
		if(!k3d::validate_polyhedra(Mesh))
			return;
		
		schedule_data<normal_cache>(&Mesh, Hint, this);
	}
	
	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<point_normal_painter, k3d::interface_list<k3d::gl::imesh_painter > > factory(
			k3d::uuid(0x2285917f, 0xfe40cc4f, 0xb462449f, 0xf5172d0a),
			"OpenGLPointNormalPainter",
			_("Renders polyhedron point normal vectors"),
			"OpenGL Painters",
			k3d::iplugin_factory::EXPERIMENTAL);

		return factory;
	}
	
	

private:
	k3d_data(bool, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_draw_selected;
	k3d_data(bool, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_draw_unselected;
	k3d_data(k3d::color, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_selected_color;
	k3d_data(k3d::color, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_unselected_color;
};

/////////////////////////////////////////////////////////////////////////////
// point_normal_painter_factory

k3d::iplugin_factory& point_normal_painter_factory()
{
	return point_normal_painter::get_factory();
}

} // namespace painters

} // namespace opengl

} // namespace module
