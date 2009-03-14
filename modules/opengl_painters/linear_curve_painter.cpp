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

#include <k3d-i18n-config.h>
#include <k3dsdk/document_plugin_factory.h>
#include <k3dsdk/linear_curve.h>
#include <k3dsdk/mesh_painter_gl.h>
#include <k3dsdk/painter_render_state_gl.h>
#include <k3dsdk/painter_selection_state_gl.h>
#include <k3dsdk/selection.h>

#include <boost/scoped_ptr.hpp>

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
		for(k3d::mesh::primitives_t::const_iterator primitive = Mesh.primitives.begin(); primitive != Mesh.primitives.end(); ++primitive)
		{
			boost::scoped_ptr<k3d::linear_curve::const_primitive> linear_curve(k3d::linear_curve::validate(**primitive));
			if(!linear_curve)
				continue;
			
			const k3d::mesh::points_t& points = *Mesh.points;

			k3d::gl::store_attributes attributes;
			glDisable(GL_LIGHTING);

			const k3d::color color = RenderState.node_selection ? k3d::color(1, 1, 1) : k3d::color(0, 0, 0);
			const k3d::color selected_color = RenderState.show_component_selection ? k3d::color(1, 0, 0) : color;

			const GLenum curve_wrap = linear_curve->periodic[0] ? GL_LINE_LOOP : GL_LINE_STRIP;

			const k3d::uint_t curve_begin = 0;
			const k3d::uint_t curve_end = curve_begin + linear_curve->curve_first_points.size();
			for(k3d::uint_t curve = curve_begin; curve != curve_end; ++curve)
			{
				const k3d::uint_t curve_point_begin = linear_curve->curve_first_points[curve];
				const k3d::uint_t curve_point_end = curve_point_begin + linear_curve->curve_point_counts[curve];

				k3d::gl::color3d(linear_curve->curve_selections[curve] ? selected_color : color);

				glBegin(curve_wrap);
				for(k3d::uint_t curve_point = curve_point_begin; curve_point != curve_point_end; ++curve_point)
					k3d::gl::vertex3d(points[linear_curve->curve_points[curve_point]]);
				glEnd();
			}
		}
	}
	
	void on_select_mesh(const k3d::mesh& Mesh, const k3d::gl::painter_render_state& RenderState, const k3d::gl::painter_selection_state& SelectionState)
	{
		if(!SelectionState.select_uniform)
			return;

		k3d::uint_t primitive_index = 0;
		for(k3d::mesh::primitives_t::const_iterator primitive = Mesh.primitives.begin(); primitive != Mesh.primitives.end(); ++primitive, ++primitive_index)
		{
			boost::scoped_ptr<k3d::linear_curve::const_primitive> linear_curve(k3d::linear_curve::validate(**primitive));
			if(!linear_curve)
				continue;
			
			const k3d::mesh::points_t& points = *Mesh.points;

			k3d::gl::store_attributes attributes;
			glDisable(GL_LIGHTING);

			k3d::gl::push_selection_token(k3d::selection::PRIMITIVE, primitive_index);

			const GLenum curve_wrap = linear_curve->periodic[0] ? GL_LINE_LOOP : GL_LINE_STRIP;

			const k3d::uint_t curve_begin = 0;
			const k3d::uint_t curve_end = curve_begin + linear_curve->curve_first_points.size();
			for(k3d::uint_t curve = curve_begin; curve != curve_end; ++curve)
			{
				k3d::gl::push_selection_token(k3d::selection::UNIFORM, curve);

				const k3d::uint_t curve_point_begin = linear_curve->curve_first_points[curve];
				const k3d::uint_t curve_point_end = curve_point_begin + linear_curve->curve_point_counts[curve];

				glBegin(curve_wrap);
				for(k3d::uint_t curve_point = curve_point_begin; curve_point != curve_point_end; ++curve_point)
					k3d::gl::vertex3d(points[linear_curve->curve_points[curve_point]]);
				glEnd();

				k3d::gl::pop_selection_token(); // UNIFORM
			}

			k3d::gl::pop_selection_token(); // PRIMITIVE
		}
	}
	
	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<linear_curve_painter, k3d::interface_list<k3d::gl::imesh_painter > > factory(
			k3d::uuid(0x6acd1d8e, 0x954f4b3a, 0xbce4c296, 0x523ebacd),
			"OpenGLLinearCurvePainter",
			_("Renders linear curves"),
			"OpenGL Painter",
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


