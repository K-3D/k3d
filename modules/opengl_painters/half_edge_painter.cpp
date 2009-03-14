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

/** \file Paints oriented half-edges. Based on half_edge_painter
	\author Timothy M. Shead (tshead@k-3d.com)
	\author Bart Janssens (bart.janssens@lid.kviv.be)
*/

#include "utility.h"

#include <k3d-i18n-config.h>
#include <k3dsdk/document_plugin_factory.h>
#include <k3dsdk/measurement.h>
#include <k3dsdk/mesh_painter_gl.h>
#include <k3dsdk/painter_render_state_gl.h>
#include <k3dsdk/polyhedron.h>
#include <k3dsdk/selection.h>

#include <boost/scoped_ptr.hpp>

namespace module
{

namespace opengl
{

namespace painters
{

/////////////////////////////////////////////////////////////////////////////
// half_edge_painter

class half_edge_painter :
	public k3d::gl::mesh_painter
{
	typedef k3d::gl::mesh_painter base;

public:
	half_edge_painter(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_draw_selected(init_owner(*this) + init_name("draw_selected") + init_label(_("Draw Selected")) + init_description(_("Draw normals for selected polygons")) + init_value(true)),
		m_draw_unselected(init_owner(*this) + init_name("draw_unselected") + init_label(_("Draw Unselected")) + init_description(_("Draw normals for unselected polygons")) + init_value(false)),
		m_selected_color(init_owner(*this) + init_name("selected_color") + init_label(_("Selected Color")) + init_description(_("Normal color for selected polygons")) + init_value(k3d::color(0, 1, 1))),
		m_unselected_color(init_owner(*this) + init_name("unselected_color") + init_label(_("Unselected Color")) + init_description(_("Normal color for unselected polygons")) + init_value(k3d::color(0, 0.6, 0.6))),
		m_offset(init_owner(*this) + init_name("offset") + init_label(_("Offset")) + init_description(_("Offset from edge")) + init_value(0.1) + init_step_increment(0.1) + init_units(typeid(k3d::measurement::distance))),
		m_arrow_size(init_owner(*this) + init_name("arrow_size") + init_label(_("Arrow size")) + init_description(_("Size for the arrow")) + init_value(0.1) + init_step_increment(0.1) + init_units(typeid(k3d::measurement::distance)))
	{
		m_draw_selected.changed_signal().connect(make_async_redraw_slot());
		m_draw_unselected.changed_signal().connect(make_async_redraw_slot());
		m_selected_color.changed_signal().connect(make_async_redraw_slot());
		m_unselected_color.changed_signal().connect(make_async_redraw_slot());
		m_offset.changed_signal().connect(make_async_redraw_slot());
		m_arrow_size.changed_signal().connect(make_async_redraw_slot());
	}

	template<typename FunctorT>
	void draw(const k3d::polyhedron::const_primitive& Polyhedron, const k3d::mesh::points_t& Points, const k3d::color& Color, const FunctorT& EdgeTest)
	{
		const size_t edge_count = Polyhedron.edge_points.size();

		k3d::gl::color3d(Color);

		glBegin(GL_LINES);
		for(size_t edge = 0; edge != edge_count; ++edge)
		{
			if(EdgeTest(edge))
			{
				const k3d::point3 first_point = Points[Polyhedron.edge_points[edge]];
				const k3d::point3 second_point = Points[Polyhedron.edge_points[Polyhedron.clockwise_edges[edge]]];
				k3d::vector3 edge_vector = k3d::normalize(second_point - first_point);
				k3d::normal3 normal_vector = k3d::normalize(k3d::polyhedron::normal(Polyhedron.edge_points, Polyhedron.clockwise_edges, Points, edge));
				k3d::vector3 offset_vector = normal_vector ^ edge_vector;

				const k3d::point3 glyph_point1 = first_point + (offset_vector + edge_vector) * m_offset.pipeline_value();
				const k3d::point3 glyph_point2 = second_point + (offset_vector - edge_vector) * m_offset.pipeline_value();
				const k3d::point3 glyph_point3 = glyph_point2 + (offset_vector - edge_vector) * m_arrow_size.pipeline_value();

				k3d::gl::vertex3d(glyph_point1);
				k3d::gl::vertex3d(glyph_point2);
				k3d::gl::vertex3d(glyph_point2);
				k3d::gl::vertex3d(glyph_point3);
			}
		}
		glEnd();
	}

	void on_paint_mesh(const k3d::mesh& Mesh, const k3d::gl::painter_render_state& RenderState)
	{
		const k3d::bool_t draw_selected = m_draw_selected.pipeline_value() && RenderState.show_component_selection;
		const k3d::bool_t draw_unselected = m_draw_unselected.pipeline_value();
		if(!draw_selected && !draw_unselected)
			return;

		boost::scoped_ptr<k3d::polyhedron::const_primitive> polyhedron(k3d::polyhedron::validate(Mesh));
		if(!polyhedron)
			return;

		k3d::gl::store_attributes attributes;
		glDisable(GL_LIGHTING);

		if(draw_selected)
			draw(*polyhedron, *Mesh.points, m_selected_color.pipeline_value(), selected_edges(Mesh));

		if(draw_unselected)
			draw(*polyhedron, *Mesh.points, m_unselected_color.pipeline_value(), unselected_edges(Mesh));
	}
	
	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<half_edge_painter, k3d::interface_list<k3d::gl::imesh_painter > > factory(
			k3d::uuid(0xf6484bfb, 0xaa46c77c, 0x1ec5f895, 0xee3f6905),
			"OpenGLHalfEdgePainter",
			_("Draws arrows to show polyhedron half edge orientation"),
			"OpenGL Painter",
			k3d::iplugin_factory::EXPERIMENTAL);

		return factory;
	}

private:
	k3d_data(k3d::bool_t, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_draw_selected;
	k3d_data(k3d::bool_t, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_draw_unselected;
	k3d_data(k3d::color, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_selected_color;
	k3d_data(k3d::color, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_unselected_color;
	k3d_data(double, immutable_name, change_signal, with_undo, local_storage, no_constraint, measurement_property, with_serialization) m_offset;
	k3d_data(double, immutable_name, change_signal, with_undo, local_storage, no_constraint, measurement_property, with_serialization) m_arrow_size;
};

/////////////////////////////////////////////////////////////////////////////
// half_edge_painter_factory

k3d::iplugin_factory& half_edge_painter_factory()
{
	return half_edge_painter::get_factory();
}

} // namespace painters

} // namespace opengl

} // namespace module


