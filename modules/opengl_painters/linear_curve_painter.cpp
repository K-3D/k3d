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
#include <k3dsdk/painter_selection_state_gl.h>
#include <k3dsdk/persistent.h>
#include <k3dsdk/selection.h>

namespace module
{

namespace opengl
{

namespace painters
{

/////////////////////////////////////////////////////////////////////////////
// linear_curve_painter

class linear_curve_painter :
	public k3d::gl::mesh_painter
{
	typedef k3d::gl::mesh_painter base;

public:
	linear_curve_painter(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document)
	{
	}

	void on_paint_mesh(const k3d::mesh& Mesh, const k3d::gl::painter_render_state& RenderState)
	{
		if(!k3d::validate_linear_curve_groups(Mesh))
			return;

		const k3d::mesh::indices_t& first_curves = *Mesh.linear_curve_groups->first_curves;
		const k3d::mesh::counts_t& curve_counts = *Mesh.linear_curve_groups->curve_counts;
		const k3d::mesh::bools_t& periodic_curves = *Mesh.linear_curve_groups->periodic_curves;
		const k3d::mesh::indices_t& curve_first_points = *Mesh.linear_curve_groups->curve_first_points;
		const k3d::mesh::counts_t& curve_point_counts = *Mesh.linear_curve_groups->curve_point_counts;
		const k3d::mesh::selection_t& curve_selection = *Mesh.linear_curve_groups->curve_selection;
		const k3d::mesh::indices_t& curve_points = *Mesh.linear_curve_groups->curve_points;
		const k3d::mesh::points_t& points = *Mesh.points;
		
		k3d::gl::store_attributes attributes;
		glDisable(GL_LIGHTING);

		const k3d::color color = RenderState.node_selection ? k3d::color(1, 1, 1) : k3d::color(0, 0, 0);
		const k3d::color selected_color = RenderState.show_component_selection ? k3d::color(1, 0, 0) : color;

		const size_t group_begin = 0;
		const size_t group_end = group_begin + first_curves.size();
		for(size_t group = group_begin; group != group_end; ++group)
		{
			const GLenum curve_wrap = periodic_curves[group] ? GL_LINE_LOOP : GL_LINE_STRIP;

			const size_t curve_begin = first_curves[group];
			const size_t curve_end = curve_begin + curve_counts[group];
			for(size_t curve = curve_begin; curve != curve_end; ++curve)
			{
				const size_t curve_point_begin = curve_first_points[curve];
				const size_t curve_point_end = curve_point_begin + curve_point_counts[curve];

				k3d::gl::color3d(curve_selection[curve] ? selected_color : color);

				glBegin(curve_wrap);
				for(size_t curve_point = curve_point_begin; curve_point != curve_point_end; ++curve_point)
					k3d::gl::vertex3d(points[curve_points[curve_point]]);
				glEnd();
			}
		}
	}
	
	void on_select_mesh(const k3d::mesh& Mesh, const k3d::gl::painter_render_state& RenderState, const k3d::gl::painter_selection_state& SelectionState)
	{
		if(!SelectionState.select_linear_curves)
			return;

		if(!k3d::validate_linear_curve_groups(Mesh))
			return;

		const k3d::mesh::indices_t& first_curves = *Mesh.linear_curve_groups->first_curves;
		const k3d::mesh::counts_t& curve_counts = *Mesh.linear_curve_groups->curve_counts;
		const k3d::mesh::bools_t& periodic_curves = *Mesh.linear_curve_groups->periodic_curves;
		const k3d::mesh::indices_t& curve_first_points = *Mesh.linear_curve_groups->curve_first_points;
		const k3d::mesh::counts_t& curve_point_counts = *Mesh.linear_curve_groups->curve_point_counts;
		const k3d::mesh::indices_t& curve_points = *Mesh.linear_curve_groups->curve_points;
		const k3d::mesh::points_t& points = *Mesh.points;
		
		k3d::gl::store_attributes attributes;
		glDisable(GL_LIGHTING);

		const size_t group_begin = 0;
		const size_t group_end = group_begin + first_curves.size();
		for(size_t group = group_begin; group != group_end; ++group)
		{
			const GLenum curve_wrap = periodic_curves[group] ? GL_LINE_LOOP : GL_LINE_STRIP;

			const size_t curve_begin = first_curves[group];
			const size_t curve_end = curve_begin + curve_counts[group];
			for(size_t curve = curve_begin; curve != curve_end; ++curve)
			{
				const size_t curve_point_begin = curve_first_points[curve];
				const size_t curve_point_end = curve_point_begin + curve_point_counts[curve];

				k3d::gl::push_selection_token(k3d::selection::ABSOLUTE_LINEAR_CURVE, curve);

				glBegin(curve_wrap);
				for(size_t curve_point = curve_point_begin; curve_point != curve_point_end; ++curve_point)
					k3d::gl::vertex3d(points[curve_points[curve_point]]);
				glEnd();

				k3d::gl::pop_selection_token(); // ABSOLUTE_LINEAR_CURVE
			}
		}
	}
	
	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<linear_curve_painter, k3d::interface_list<k3d::gl::imesh_painter > > factory(
			k3d::uuid(0x6acd1d8e, 0x954f4b3a, 0xbce4c296, 0x523ebacd),
			"OpenGLLinearCurvePainter",
			_("Renders linear curves"),
			"OpenGL Painters",
			k3d::iplugin_factory::EXPERIMENTAL);

		return factory;
	}
};

/////////////////////////////////////////////////////////////////////////////
// linear_curve_painter_factory

k3d::iplugin_factory& linear_curve_painter_factory()
{
	return linear_curve_painter::get_factory();
}

} // namespace painters

} // namespace opengl

} // namespace module


