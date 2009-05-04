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
#include <k3dsdk/painter_render_state_gl.h>
#include <k3dsdk/polyhedron.h>
#include <k3dsdk/selection.h>

#include <boost/scoped_ptr.hpp>

namespace k3d
{

/// Enumerates mesh storage classes
enum storage_class_t
{
	STORAGE_CLASS_CONSTANT,
	STORAGE_CLASS_UNIFORM,
	STORAGE_CLASS_VARYING,
	STORAGE_CLASS_FACEVARYING,
	STORAGE_CLASS_VERTEX,
};

/// Returns descriptions of allowed mesh storage values, for use with enumeration properties
const ienumeration_property::enumeration_values_t& storage_class_values()
{
	static ienumeration_property::enumeration_values_t values;
	if(values.empty())
	{
		values.push_back(ienumeration_property::enumeration_value_t("Constant", "constant", ""));
		values.push_back(ienumeration_property::enumeration_value_t("Uniform", "uniform", ""));
		values.push_back(ienumeration_property::enumeration_value_t("Varying", "varying", ""));
		values.push_back(ienumeration_property::enumeration_value_t("Face-Varying", "facevarying", ""));
		values.push_back(ienumeration_property::enumeration_value_t("Vertex", "vertex", ""));
	}
	
	return values;
}

/// Serialization
std::ostream& operator<<(std::ostream& Stream, const storage_class_t& Value)
{
	switch(Value)
	{
		case STORAGE_CLASS_CONSTANT:
			Stream << "constant";
			break;
		case STORAGE_CLASS_UNIFORM:
			Stream << "uniform";
			break;
		case STORAGE_CLASS_VARYING:
			Stream << "varying";
			break;
		case STORAGE_CLASS_FACEVARYING:
			Stream << "facevarying";
			break;
		case STORAGE_CLASS_VERTEX:
			Stream << "vertex";
			break;
	}

	return Stream;
}

/// Serialization
std::istream& operator>>(std::istream& Stream, storage_class_t& Value)
{
	string_t c;
	Stream >> c;

	if("constant" == c)
		Value = STORAGE_CLASS_CONSTANT;
	else if("uniform" == c)
		Value = STORAGE_CLASS_UNIFORM;
	else if("varying" == c)
		Value = STORAGE_CLASS_VARYING;
	else if("facevarying" == c)
		Value = STORAGE_CLASS_FACEVARYING;
	else if("vertex" == c)
		Value = STORAGE_CLASS_VERTEX;
	else
		log() << error << k3d_file_reference << ": could not extract value [" << c << "]" << std::endl;

	return Stream;
}

} // namespace k3d

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
		m_storage_class(init_owner(*this) + init_name("storage_class") + init_label(_("Storage Class")) + init_description(_("Array storage class")) + init_value(k3d::STORAGE_CLASS_FACEVARYING) + init_enumeration(k3d::storage_class_values())),
		m_array_name(init_owner(*this) + init_name("array_name") + init_label(_("Array Name")) + init_description(_("Name of the array to visualize.")) + init_value(k3d::string_t("N"))),
		m_color(init_owner(*this) + init_name("color") + init_label(_("Color")) + init_description(_("Color")) + init_value(k3d::color(1, 0, 0))),
		m_normalize(init_owner(*this) + init_name("normalize") + init_label(_("Normalize")) + init_description(_("Normalize vectors for display")) + init_value(true)),
		m_scale(init_owner(*this) + init_name("scale") + init_label(_("Scale")) + init_description(_("Scale vectors for display")) + init_value(1.0))
	{
		m_storage_class.changed_signal().connect(make_async_redraw_slot());
		m_array_name.changed_signal().connect(make_async_redraw_slot());
		m_color.changed_signal().connect(make_async_redraw_slot());
		m_normalize.changed_signal().connect(make_async_redraw_slot());
		m_scale.changed_signal().connect(make_async_redraw_slot());
	}

	inline static void draw_line(const k3d::point3& Point, const k3d::normal3& Normal, const k3d::bool_t Normalize, const k3d::double_t Scale)
	{
		k3d::gl::vertex3d(Point);
		if(Normalize)
			k3d::gl::vertex3d(Point + k3d::to_vector(Scale * k3d::normalize(Normal)));
		else
			k3d::gl::vertex3d(Point + k3d::to_vector(Scale * Normal));
	}

	void on_paint_mesh(const k3d::mesh& Mesh, const k3d::gl::painter_render_state& RenderState)
	{
		const k3d::storage_class_t storage_class = m_storage_class.pipeline_value();
		const k3d::string_t array_name = m_array_name.pipeline_value();
		const k3d::color color = m_color.pipeline_value();
		const k3d::bool_t normalize = m_normalize.pipeline_value();
		const k3d::double_t scale = m_scale.pipeline_value();

		switch(storage_class)
		{
			case k3d::STORAGE_CLASS_UNIFORM:
			{
				for(k3d::mesh::primitives_t::const_iterator primitive = Mesh.primitives.begin(); primitive != Mesh.primitives.end(); ++primitive)
				{
					boost::scoped_ptr<k3d::polyhedron::const_primitive> polyhedron(k3d::polyhedron::validate(**primitive));
					if(!polyhedron.get())
						continue;
					if(const k3d::mesh::normals_t* const array = polyhedron->uniform_data.lookup<k3d::mesh::normals_t>(array_name))
					{
						k3d::gl::store_attributes attributes;
						glDisable(GL_LIGHTING);
						k3d::gl::color3d(color);

						const k3d::mesh::points_t& points = *Mesh.points;

						glBegin(GL_LINES);
						const k3d::uint_t face_begin = 0;
						const k3d::uint_t face_end = face_begin + polyhedron->face_first_loops.size();
						for(k3d::uint_t face = face_begin; face != face_end; ++face)
							draw_line(k3d::polyhedron::center(polyhedron->edge_points, polyhedron->clockwise_edges, points, polyhedron->loop_first_edges[polyhedron->face_first_loops[face]]), (*array)[face], normalize, scale);
						glEnd();
					}
				}
				break;
			}

			case k3d::STORAGE_CLASS_FACEVARYING:
			{
				for(k3d::mesh::primitives_t::const_iterator primitive = Mesh.primitives.begin(); primitive != Mesh.primitives.end(); ++primitive)
				{
					boost::scoped_ptr<k3d::polyhedron::const_primitive> polyhedron(k3d::polyhedron::validate(**primitive));
					if(!polyhedron.get())
						continue;
					if(const k3d::mesh::normals_t* const array = polyhedron->face_varying_data.lookup<k3d::mesh::normals_t>(array_name))
					{
						k3d::gl::store_attributes attributes;
						glDisable(GL_LIGHTING);
						k3d::gl::color3d(color);

						const k3d::mesh::points_t& points = *Mesh.points;

						glBegin(GL_LINES);
						const k3d::uint_t face_begin = 0;
						const k3d::uint_t face_end = face_begin + polyhedron->face_first_loops.size();
						for(k3d::uint_t face = face_begin; face != face_end; ++face)
						{
							const k3d::uint_t loop_begin = polyhedron->face_first_loops[face];
							const k3d::uint_t loop_end = loop_begin + polyhedron->face_loop_counts[face];
							for(k3d::uint_t loop = loop_begin; loop != loop_end; ++loop)
							{
								const k3d::uint_t first_edge = polyhedron->loop_first_edges[loop];
								for(k3d::uint_t edge = first_edge; ;)
								{
									draw_line(points[polyhedron->edge_points[edge]], (*array)[edge], normalize, scale);

									edge = polyhedron->clockwise_edges[edge];
									if(edge == first_edge)
										break;
								}
							}
						}
						glEnd();
					}
				}
				break;
			}

			case k3d::STORAGE_CLASS_VERTEX:
			{
				for(k3d::mesh::primitives_t::const_iterator primitive = Mesh.primitives.begin(); primitive != Mesh.primitives.end(); ++primitive)
				{
					boost::scoped_ptr<k3d::polyhedron::const_primitive> polyhedron(k3d::polyhedron::validate(**primitive));
					if(!polyhedron.get())
						continue;
					if(const k3d::mesh::normals_t* const array = Mesh.vertex_data.lookup<k3d::mesh::normals_t>(array_name))
					{
						k3d::gl::store_attributes attributes;
						glDisable(GL_LIGHTING);
						k3d::gl::color3d(color);

						const k3d::mesh::points_t& points = *Mesh.points;
						const k3d::uint_t point_begin = 0;
						const k3d::uint_t point_end = point_begin + points.size();

						glBegin(GL_LINES);
						for(k3d::uint_t point = point_begin; point != point_end; ++point)
							draw_line(points[point], (*array)[point], normalize, scale);
						glEnd();
					}
				}
				break;
			}
		}
	}
	
	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<normal_array_painter, k3d::interface_list<k3d::gl::imesh_painter > > factory(
			k3d::uuid(0x893f3dff, 0x9648bc46, 0x5674c48f, 0x05df5463),
			"OpenGLNormalArrayPainter",
			_("Visualizes normal-vector arrays"),
			"OpenGL Painter",
			k3d::iplugin_factory::EXPERIMENTAL);

		return factory;
	}

private:
	k3d_data(k3d::storage_class_t, immutable_name, change_signal, with_undo, local_storage, no_constraint, enumeration_property, with_serialization) m_storage_class;
	k3d_data(k3d::string_t, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_array_name;
	k3d_data(k3d::color, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_color;
	k3d_data(k3d::bool_t, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_normalize;
	k3d_data(k3d::double_t, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_scale;
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


