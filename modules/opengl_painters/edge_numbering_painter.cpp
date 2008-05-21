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
	\author Romain Behar (romainbehar@yahoo.com)
	\author Timothy M. Shead (tshead@k-3d.com)
*/

#include "utility.h"

#include <k3d-i18n-config.h>
#include <k3dsdk/document_plugin_factory.h>
#include <k3dsdk/mesh_operations.h>
#include <k3dsdk/mesh_painter_gl.h>
#include <k3dsdk/painter_render_state_gl.h>
#include <k3dsdk/selection.h>

namespace module
{

namespace opengl
{

namespace painters
{

/////////////////////////////////////////////////////////////////////////////
// edge_numbering_painter

class edge_numbering_painter :
	public k3d::gl::mesh_painter
{
	typedef k3d::gl::mesh_painter base;

public:
	edge_numbering_painter(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_draw_selected(init_owner(*this) + init_name("draw_selected") + init_label(_("Draw Selected")) + init_description(_("Draw normals for selected polygons.")) + init_value(true)),
		m_draw_unselected(init_owner(*this) + init_name("draw_unselected") + init_label(_("Draw Unselected")) + init_description(_("Draw normals for unselected polygons.")) + init_value(false)),
		m_selected_color(init_owner(*this) + init_name("selected_color") + init_label(_("Selected Color")) + init_description(_("Normal color for selected polygons.")) + init_value(k3d::color(0, 1, 0))),
		m_unselected_color(init_owner(*this) + init_name("unselected_color") + init_label(_("Unselected Color")) + init_description(_("Normal color for unselected polygons.")) + init_value(k3d::color(0, 0.5, 0))),
		m_edge_offset(init_owner(*this) + init_name("edge_offset") + init_label(_("Edge Offset")) + init_description(_("Offsets numbers away from their edges.")) + init_value(0.5)),
		m_face_offset(init_owner(*this) + init_name("face_offset") + init_label(_("Face Offset")) + init_description(_("Offsets numbers away from their polygons.")) + init_value(0.2))
	{
		m_draw_selected.changed_signal().connect(make_async_redraw_slot());
		m_draw_unselected.changed_signal().connect(make_async_redraw_slot());
		m_selected_color.changed_signal().connect(make_async_redraw_slot());
		m_unselected_color.changed_signal().connect(make_async_redraw_slot());
		m_edge_offset.changed_signal().connect(make_async_redraw_slot());
		m_face_offset.changed_signal().connect(make_async_redraw_slot());
	}

	template<typename FunctorT>
	void draw(const k3d::mesh::indices_t& FaceFirstLoops, const k3d::mesh::counts_t& FaceLoopCounts, const k3d::mesh::indices_t& LoopFirstEdges, const k3d::mesh::indices_t& EdgePoints, const k3d::mesh::indices_t& ClockwiseEdges, const k3d::mesh::points_t& Points, const k3d::mesh::normals_t& FaceNormals, const k3d::color& Color, const double EdgeOffset, const double FaceOffset, const FunctorT& EdgeTest, const k3d::gl::painter_render_state& RenderState)
	{
		k3d::gl::color3d(Color);
		glListBase(RenderState.gl_ascii_font_list_base);

		const size_t face_begin = 0;
		const size_t face_end = face_begin + FaceFirstLoops.size();
		for(size_t face = face_begin; face != face_end; ++face)
		{
			const size_t loop_begin = FaceFirstLoops[face];
			const size_t loop_end = loop_begin + FaceLoopCounts[face];
			for(size_t loop = loop_begin; loop != loop_end; ++loop)
			{
				const size_t first_edge = LoopFirstEdges[loop];
				for(size_t edge = first_edge; ; )
				{
					if(EdgeTest(edge))
					{
						const std::string text = k3d::string_cast(edge);

						const k3d::point3 vertex1 = Points[EdgePoints[edge]];
						const k3d::point3 vertex2 = Points[EdgePoints[ClockwiseEdges[edge]]];
						const k3d::vector3 edge_vector = vertex2 - vertex1;

						const k3d::point3 position = vertex1 + (0.5 * edge_vector) + (EdgeOffset * k3d::normalize(FaceNormals[face] ^ edge_vector)) + (FaceOffset * k3d::to_vector(k3d::normalize(FaceNormals[face])));

						glRasterPos3d(position[0], position[1], position[2]);
						glCallLists(text.size(), GL_UNSIGNED_BYTE, text.c_str());
					}

					edge = ClockwiseEdges[edge];
					if(edge == first_edge)
						break;
				}
			}
		}
	}

	void on_paint_mesh(const k3d::mesh& Mesh, const k3d::gl::painter_render_state& RenderState)
	{
		const bool draw_selected = m_draw_selected.pipeline_value();
		const bool draw_unselected = m_draw_unselected.pipeline_value();
		if(!draw_selected && !draw_unselected)
			return;

		if(!k3d::validate_polyhedra(Mesh))
			return;

		const double edge_offset = m_edge_offset.pipeline_value();
		const double face_offset = m_face_offset.pipeline_value();

		const k3d::mesh::indices_t& face_first_loops = *Mesh.polyhedra->face_first_loops;
		const k3d::mesh::counts_t& face_loop_counts = *Mesh.polyhedra->face_loop_counts;
		const k3d::mesh::indices_t& loop_first_edges = *Mesh.polyhedra->loop_first_edges;
		const k3d::mesh::indices_t& edge_points = *Mesh.polyhedra->edge_points;
		const k3d::mesh::indices_t& clockwise_edges = *Mesh.polyhedra->clockwise_edges;
		const k3d::mesh::points_t& points = *Mesh.points;

		const size_t face_count = face_first_loops.size();

		// Calculate face normals ...
		k3d::typed_array<k3d::normal3> normals(face_count);
		for(size_t face = 0; face != face_count; ++face)
			normals[face] = k3d::normal(edge_points, clockwise_edges, points, loop_first_edges[face_first_loops[face]]);

		k3d::gl::store_attributes attributes;
		glDisable(GL_LIGHTING);

		if(draw_selected)
			draw(face_first_loops, face_loop_counts, loop_first_edges, edge_points, clockwise_edges, points, normals, m_selected_color.pipeline_value(), edge_offset, face_offset, selected_edges(Mesh), RenderState);

		if(draw_unselected)
			draw(face_first_loops, face_loop_counts, loop_first_edges, edge_points, clockwise_edges, points, normals, m_unselected_color.pipeline_value(), edge_offset, face_offset, unselected_edges(Mesh), RenderState);
	}
	
	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<edge_numbering_painter, k3d::interface_list<k3d::gl::imesh_painter > > factory(
			k3d::uuid(0xd15d2acb, 0xa647d85b, 0x54dfbbb4, 0x69578f6f),
			"OpenGLEdgeNumberingPainter",
			_("Numbers polyhedron edges"),
			"OpenGL Painters",
			k3d::iplugin_factory::EXPERIMENTAL);

		return factory;
	}

private:
	k3d_data(bool, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_draw_selected;
	k3d_data(bool, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_draw_unselected;
	k3d_data(k3d::color, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_selected_color;
	k3d_data(k3d::color, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_unselected_color;
	k3d_data(double, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_edge_offset;
	k3d_data(double, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_face_offset;
};

/////////////////////////////////////////////////////////////////////////////
// edge_numbering_painter_factory

k3d::iplugin_factory& edge_numbering_painter_factory()
{
	return edge_numbering_painter::get_factory();
}

} // namespace painters

} // namespace opengl

} // namespace module


