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
#include <k3dsdk/blobby.h>
#include <k3dsdk/document_plugin_factory.h>
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
// blobby_point_painter

class blobby_point_painter :
	public k3d::gl::mesh_painter
{
	typedef k3d::gl::mesh_painter base;

public:
	blobby_point_painter(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document)
	{
	}

	void on_paint_mesh(const k3d::mesh& Mesh, const k3d::gl::painter_render_state& RenderState)
	{
		for(k3d::mesh::primitives_t::const_iterator primitive = Mesh.primitives.begin(); primitive != Mesh.primitives.end(); ++primitive)
		{
			boost::scoped_ptr<k3d::blobby::const_primitive> blobby(k3d::blobby::validate(**primitive));
			if(!blobby)
				continue;

			k3d::gl::store_attributes attributes;
			glDisable(GL_LIGHTING);

			const k3d::color color = RenderState.node_selection ? k3d::color(1, 1, 1) : k3d::color(0, 1, 1);
			k3d::gl::color3d(color);

			const k3d::uint_t primitives_begin = 0;
			const k3d::uint_t primitives_end = primitives_begin + blobby->primitives.size();
			for(k3d::uint_t primitive = primitives_begin; primitive != primitives_end; ++primitive)
			{
				const k3d::uint_t first_float = blobby->primitive_first_floats[primitive];
				const k3d::uint_t float_count = blobby->primitive_float_counts[primitive];

				switch(blobby->primitives[primitive])
				{
					case k3d::blobby::CONSTANT:
						break;
					case k3d::blobby::ELLIPSOID:
					{
						return_if_fail(float_count == 16);

						const k3d::point3 point(0, 0, 0);

						glMatrixMode(GL_MODELVIEW);
						glPushMatrix();
						glMultMatrixd(&blobby->floats[first_float]);

						glBegin(GL_POINTS);
						k3d::gl::vertex3d(point);
						glEnd();

						glPopMatrix();
						
						break;
					}
					case k3d::blobby::SEGMENT:
					{
						return_if_fail(float_count == 23);

						const k3d::point3 point1(blobby->floats[first_float + 0], blobby->floats[first_float + 1], blobby->floats[first_float + 2]);
						const k3d::point3 point2(blobby->floats[first_float + 3], blobby->floats[first_float + 4], blobby->floats[first_float + 5]);

						glMatrixMode(GL_MODELVIEW);
						glPushMatrix();
						glMultMatrixd(&blobby->floats[first_float + 7]);

						glBegin(GL_LINES);
						k3d::gl::vertex3d(point1);
						k3d::gl::vertex3d(point2);
						glEnd();

						glPopMatrix();

						break;
					}
				}
			}
		}
	}
	
	void on_select_mesh(const k3d::mesh& Mesh, const k3d::gl::painter_render_state& RenderState, const k3d::gl::painter_selection_state& SelectionState)
	{
		k3d::uint_t primitive_index = 0;
		for(k3d::mesh::primitives_t::const_iterator primitive = Mesh.primitives.begin(); primitive != Mesh.primitives.end(); ++primitive, ++primitive_index)
		{
			boost::scoped_ptr<k3d::blobby::const_primitive> blobby(k3d::blobby::validate(**primitive));
			if(!blobby)
				continue;

			k3d::gl::store_attributes attributes;
			glDisable(GL_LIGHTING);

			k3d::gl::push_selection_token(k3d::selection::PRIMITIVE, primitive_index);
			k3d::gl::push_selection_token(k3d::selection::CONSTANT, 0);
			k3d::gl::push_selection_token(k3d::selection::UNIFORM, 0);

			const k3d::uint_t primitives_begin = 0;
			const k3d::uint_t primitives_end = primitives_begin + blobby->primitives.size();
			for(k3d::uint_t primitive = primitives_begin; primitive != primitives_end; ++primitive)
			{
				k3d::gl::push_selection_token(k3d::selection::VARYING, primitive);
				k3d::gl::push_selection_token(k3d::selection::POINT, primitive);

				const k3d::uint_t first_float = blobby->primitive_first_floats[primitive];
				const k3d::uint_t float_count = blobby->primitive_float_counts[primitive];

				switch(blobby->primitives[primitive])
				{
					case k3d::blobby::CONSTANT:
						break;
					case k3d::blobby::ELLIPSOID:
					{
						return_if_fail(float_count == 16);

						const k3d::point3 point(0, 0, 0);

						glMatrixMode(GL_MODELVIEW);
						glPushMatrix();
						glMultMatrixd(&blobby->floats[first_float]);

						glBegin(GL_POINTS);
						k3d::gl::vertex3d(point);
						glEnd();

						glPopMatrix();
						
						break;
					}
					case k3d::blobby::SEGMENT:
					{
						return_if_fail(float_count == 23);

						const k3d::point3 point1(blobby->floats[first_float + 0], blobby->floats[first_float + 1], blobby->floats[first_float + 2]);
						const k3d::point3 point2(blobby->floats[first_float + 3], blobby->floats[first_float + 4], blobby->floats[first_float + 5]);

						glMatrixMode(GL_MODELVIEW);
						glPushMatrix();
						glMultMatrixd(&blobby->floats[first_float + 7]);

						glBegin(GL_LINES);
						k3d::gl::vertex3d(point1);
						k3d::gl::vertex3d(point2);
						glEnd();

						glPopMatrix();

						break;
					}
				}

				k3d::gl::pop_selection_token(); // POINT
				k3d::gl::pop_selection_token(); // VARYING

			}

			k3d::gl::pop_selection_token(); // UNIFORM
			k3d::gl::pop_selection_token(); // CONSTANT
			k3d::gl::pop_selection_token(); // PRIMITIVE

		}
	}
	
	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<blobby_point_painter, k3d::interface_list<k3d::gl::imesh_painter > > factory(
			k3d::uuid(0x06148329, 0xb2264760, 0x84b25aab, 0xf8401c63),
			"OpenGLBlobbyPointPainter",
			_("Renders blobby primitives using points and lines"),
			"OpenGL Painter",
			k3d::iplugin_factory::EXPERIMENTAL);

		return factory;
	}
};

/////////////////////////////////////////////////////////////////////////////
// blobby_point_painter_factory

k3d::iplugin_factory& blobby_point_painter_factory()
{
	return blobby_point_painter::get_factory();
}

} // namespace painters

} // namespace opengl

} // namespace module

