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
#include <k3dsdk/mesh.h>
#include <k3dsdk/painter_render_state_gl.h>
#include <k3dsdk/painter_selection_state_gl.h>
#include <k3dsdk/persistent.h>
#include <k3dsdk/selection.h>

namespace libk3dopengl
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
		if(!k3d::validate_blobbies(Mesh))
			return;

		const k3d::mesh::blobbies_t::primitives_t& primitives = *Mesh.blobbies->primitives;
		const k3d::mesh::indices_t& primitive_first_floats = *Mesh.blobbies->primitive_first_floats;
		const k3d::mesh::counts_t& primitive_float_counts = *Mesh.blobbies->primitive_float_counts;
		const k3d::mesh::blobbies_t::floats_t& floats = *Mesh.blobbies->floats;

		k3d::gl::store_attributes attributes;
		glDisable(GL_LIGHTING);

		const k3d::color color = RenderState.node_selection ? k3d::color(1, 1, 1) : k3d::color(0, 1, 1);
		k3d::gl::color3d(color);

		const size_t primitives_begin = 0;
		const size_t primitives_end = primitives_begin + primitives.size();
		for(size_t primitive = primitives_begin; primitive != primitives_end; ++primitive)
		{
			const size_t first_float = primitive_first_floats[primitive];
			const size_t float_count = primitive_float_counts[primitive];

			switch(primitives[primitive])
			{
				case k3d::mesh::blobbies_t::CONSTANT:
					break;
				case k3d::mesh::blobbies_t::ELLIPSOID:
				{
					return_if_fail(float_count == 16);

					const k3d::point3 point(0, 0, 0);

					glMatrixMode(GL_MODELVIEW);
					glPushMatrix();
					glMultMatrixd(&floats[first_float]);

					glBegin(GL_POINTS);
					k3d::gl::vertex3d(point);
					glEnd();

					glPopMatrix();
					
					break;
				}
				case k3d::mesh::blobbies_t::SEGMENT:
				{
					return_if_fail(float_count == 23);

					const k3d::point3 point1(floats[first_float + 0], floats[first_float + 1], floats[first_float + 2]);
					const k3d::point3 point2(floats[first_float + 3], floats[first_float + 4], floats[first_float + 5]);

					glMatrixMode(GL_MODELVIEW);
					glPushMatrix();
					glMultMatrixd(&floats[first_float + 7]);

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
	
	void on_select_mesh(const k3d::mesh& Mesh, const k3d::gl::painter_render_state& RenderState, const k3d::gl::painter_selection_state& SelectionState)
	{
		if(!SelectionState.select_blobbies)
			return;

		assert_not_implemented();
	}
	
	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<blobby_point_painter, k3d::interface_list<k3d::gl::imesh_painter > > factory(
			k3d::uuid(0x06148329, 0xb2264760, 0x84b25aab, 0xf8401c63),
			"OpenGLBlobbyPointPainter",
			_("Renders blobby primitives using points and lines"),
			"OpenGL Painters",
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

} // namespace libk3dopengl

