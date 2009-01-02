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
#include <k3dsdk/cubic_curve.h>
#include <k3dsdk/document_plugin_factory.h>
#include <k3dsdk/mesh_operations.h>
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
// cubic_curve_painter

class cubic_curve_painter :
	public k3d::gl::mesh_painter
{
	typedef k3d::gl::mesh_painter base;

public:
	cubic_curve_painter(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document)
	{
	}

	void on_paint_mesh(const k3d::mesh& Mesh, const k3d::gl::painter_render_state& RenderState)
	{
		for(k3d::mesh::primitives_t::const_iterator primitive = Mesh.primitives.begin(); primitive != Mesh.primitives.end(); ++primitive)
		{
			boost::scoped_ptr<k3d::cubic_curve::const_primitive> cubic_curve(k3d::cubic_curve::validate(**primitive));
			if(!cubic_curve)
				continue;
			
			const k3d::mesh::points_t& points = *Mesh.points;

			k3d::gl::store_attributes attributes;
			glDisable(GL_LIGHTING);

			const k3d::color color = RenderState.node_selection ? k3d::color(1, 1, 1) : k3d::color(0, 0, 0);
			const k3d::color selected_color = RenderState.show_component_selection ? k3d::color(1, 0, 0) : color;

			const unsigned int v_count = 8;
			const GLint v_order = 4;
			const GLint v_stride = 3;

			glEnable(GL_MAP1_VERTEX_3);
			glDisable(GL_AUTO_NORMAL);
			glMapGrid1d(v_count, 0.0, 1.0);
			GLdouble patch_points[4 * 3];

			const k3d::uint_t group_begin = 0;
			const k3d::uint_t group_end = group_begin + cubic_curve->first_curves.size();
			for(k3d::uint_t group = group_begin; group != group_end; ++group)
			{
				const k3d::uint_t curve_begin = cubic_curve->first_curves[group];
				const k3d::uint_t curve_end = curve_begin + cubic_curve->curve_counts[group];
				for(k3d::uint_t curve = curve_begin; curve != curve_end; ++curve)
				{
					const k3d::uint_t curve_point_begin = cubic_curve->curve_first_points[curve];
					const k3d::uint_t curve_point_end = curve_point_begin + cubic_curve->curve_point_counts[curve];

					k3d::gl::color3d(cubic_curve->curve_selections[curve] ? selected_color : color);

					for(k3d::uint_t curve_point = curve_point_begin; curve_point_end - curve_point >= 4; curve_point += 3)
					{
						GLdouble* pp = patch_points;

						*pp++ = points[cubic_curve->curve_points[curve_point]][0];
						*pp++ = points[cubic_curve->curve_points[curve_point]][1];
						*pp++ = points[cubic_curve->curve_points[curve_point]][2];
						*pp++ = points[cubic_curve->curve_points[curve_point+1]][0];
						*pp++ = points[cubic_curve->curve_points[curve_point+1]][1];
						*pp++ = points[cubic_curve->curve_points[curve_point+1]][2];
						*pp++ = points[cubic_curve->curve_points[curve_point+2]][0];
						*pp++ = points[cubic_curve->curve_points[curve_point+2]][1];
						*pp++ = points[cubic_curve->curve_points[curve_point+2]][2];
						*pp++ = points[cubic_curve->curve_points[curve_point+3]][0];
						*pp++ = points[cubic_curve->curve_points[curve_point+3]][1];
						*pp++ = points[cubic_curve->curve_points[curve_point+3]][2];

						glMap1d(GL_MAP1_VERTEX_3, 0, 1, v_stride, v_order, patch_points);

						glEvalMesh1(GL_LINE, 0, v_count);
					}
				}
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
			boost::scoped_ptr<k3d::cubic_curve::const_primitive> cubic_curve(k3d::cubic_curve::validate(**primitive));
			if(!cubic_curve)
				continue;
			
			const k3d::mesh::points_t& points = *Mesh.points;

			k3d::gl::store_attributes attributes;
			glDisable(GL_LIGHTING);

			k3d::gl::push_selection_token(k3d::selection::PRIMITIVE, primitive_index);

			const unsigned int v_count = 8;
			const GLint v_order = 4;
			const GLint v_stride = 3;

			glEnable(GL_MAP1_VERTEX_3);
			glDisable(GL_AUTO_NORMAL);
			glMapGrid1d(v_count, 0.0, 1.0);
			GLdouble patch_points[4 * 3];

			const k3d::uint_t group_begin = 0;
			const k3d::uint_t group_end = group_begin + cubic_curve->first_curves.size();
			for(k3d::uint_t group = group_begin; group != group_end; ++group)
			{
				k3d::gl::push_selection_token(k3d::selection::CONSTANT, group);

				const k3d::uint_t curve_begin = cubic_curve->first_curves[group];
				const k3d::uint_t curve_end = curve_begin + cubic_curve->curve_counts[group];
				k3d::uint_t curve_index = 0;
				for(k3d::uint_t curve = curve_begin; curve != curve_end; ++curve, ++curve_index)
				{
					k3d::gl::push_selection_token(k3d::selection::UNIFORM, curve_index);

					const k3d::uint_t curve_point_begin = cubic_curve->curve_first_points[curve];
					const k3d::uint_t curve_point_end = curve_point_begin + cubic_curve->curve_point_counts[curve];

					for(k3d::uint_t curve_point = curve_point_begin; curve_point_end - curve_point >= 4; curve_point += 3)
					{
						GLdouble* pp = patch_points;

						*pp++ = points[cubic_curve->curve_points[curve_point]][0];
						*pp++ = points[cubic_curve->curve_points[curve_point]][1];
						*pp++ = points[cubic_curve->curve_points[curve_point]][2];
						*pp++ = points[cubic_curve->curve_points[curve_point+1]][0];
						*pp++ = points[cubic_curve->curve_points[curve_point+1]][1];
						*pp++ = points[cubic_curve->curve_points[curve_point+1]][2];
						*pp++ = points[cubic_curve->curve_points[curve_point+2]][0];
						*pp++ = points[cubic_curve->curve_points[curve_point+2]][1];
						*pp++ = points[cubic_curve->curve_points[curve_point+2]][2];
						*pp++ = points[cubic_curve->curve_points[curve_point+3]][0];
						*pp++ = points[cubic_curve->curve_points[curve_point+3]][1];
						*pp++ = points[cubic_curve->curve_points[curve_point+3]][2];

						glMap1d(GL_MAP1_VERTEX_3, 0, 1, v_stride, v_order, patch_points);

						glEvalMesh1(GL_LINE, 0, v_count);
					}

					k3d::gl::pop_selection_token(); // UNIFORM
				}

				k3d::gl::pop_selection_token(); // CONSTANT
			}

			k3d::gl::pop_selection_token(); // PRIMITIVE
		}
	}
	
	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<cubic_curve_painter, k3d::interface_list<k3d::gl::imesh_painter > > factory(
			k3d::uuid(0xcb30d291, 0x47e146a3, 0xb5167d0f, 0x6f56bd61),
			"OpenGLCubicCurvePainter",
			_("Renders cubic curves"),
			"OpenGL Painter",
			k3d::iplugin_factory::EXPERIMENTAL);

		return factory;
	}
};

/////////////////////////////////////////////////////////////////////////////
// cubic_curve_painter_factory

k3d::iplugin_factory& cubic_curve_painter_factory()
{
	return cubic_curve_painter::get_factory();
}

} // namespace painters

} // namespace opengl

} // namespace module


