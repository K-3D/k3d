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
#include <k3dsdk/mesh_painter_gl.h>
#include <k3dsdk/mesh.h>
#include <k3dsdk/mesh_operations.h>
#include <k3dsdk/named_array_operations.h>
#include <k3dsdk/painter_render_state_gl.h>
#include <k3dsdk/persistent.h>
#include <k3dsdk/selection.h>

namespace module
{

namespace opengl
{

namespace painters
{

/////////////////////////////////////////////////////////////////////////////
// face_normal_painter

class normal_array_painter :
	public k3d::gl::mesh_painter
{
	typedef k3d::gl::mesh_painter base;

public:
	normal_array_painter(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_array_name(init_owner(*this) + init_name("array_name") + init_label(_("Array Name")) + init_description(_("Name of the array to visualize.")) + init_value(k3d::string_t("N"))),
		m_color(init_owner(*this) + init_name("color") + init_label(_("Color")) + init_description(_("Color")) + init_value(k3d::color(1, 0, 0))),
		m_scale(init_owner(*this) + init_name("scale") + init_label(_("Scale")) + init_description(_("Scaling of vectors")) + init_value(1.0))
	{
		m_array_name.changed_signal().connect(make_async_redraw_slot());
		m_color.changed_signal().connect(make_async_redraw_slot());
		m_scale.changed_signal().connect(make_async_redraw_slot());
	}

	void on_paint_mesh(const k3d::mesh& Mesh, const k3d::gl::painter_render_state& RenderState)
	{
		if(!k3d::validate_polyhedra(Mesh))
			return;

		const k3d::string_t array_name = m_array_name.pipeline_value();
		const k3d::mesh::normals_t* const array = k3d::get_array<k3d::mesh::normals_t>(Mesh.vertex_data, array_name);
		if(!array)
		{
			k3d::log() << error << "can't find normal array [" << array_name << "]" << std::endl;
			return;
		}

		k3d::gl::store_attributes attributes;
		glDisable(GL_LIGHTING);
		k3d::gl::color3d(m_color.pipeline_value());

		const k3d::double_t scale = m_scale.pipeline_value();

		const k3d::mesh::points_t& points = *Mesh.points;
		const k3d::uint_t point_begin = 0;
		const k3d::uint_t point_end = point_begin + points.size();

		glBegin(GL_LINES);
		for(k3d::uint_t point = point_begin; point != point_end; ++point)
		{
			k3d::gl::vertex3d(points[point]);
			k3d::gl::vertex3d(points[point] + k3d::to_vector(scale * (*array)[point]));
		} 
		glEnd();
	}
	
	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<normal_array_painter, k3d::interface_list<k3d::gl::imesh_painter > > factory(
			k3d::uuid(0x893f3dff, 0x9648bc46, 0x5674c48f, 0x05df5463),
			"OpenGLNormalArrayPainter",
			_("Visualizes normal-vector arrays"),
			"OpenGL Painters",
			k3d::iplugin_factory::EXPERIMENTAL);

		return factory;
	}

private:
	k3d_data(k3d::string_t, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_array_name;
	k3d_data(k3d::color, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_color;
	k3d_data(double, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_scale;
};

/////////////////////////////////////////////////////////////////////////////
// face_normal_painter_factory

k3d::iplugin_factory& normal_array_painter_factory()
{
	return normal_array_painter::get_factory();
}

} // namespace painters

} // namespace opengl

} // namespace module


