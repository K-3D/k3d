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
#include <k3dsdk/painter_render_state_gl.h>
#include <k3dsdk/painter_selection_state_gl.h>
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
// color_face_painter

class color_face_painter :
	public k3d::gl::mesh_painter
{
	typedef k3d::gl::mesh_painter base;

public:
	color_face_painter(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_color_array(init_owner(*this) + init_name("color_array") + init_label(_("Color Array")) + init_description(_("Specifies the array name to be used for face colors")) + init_value(std::string("Cs"))),
		m_array_type(init_owner(*this) + init_name("array_type") + init_label(_("Array Type")) + init_description(_("Type of array to use")) + init_value(k3d::string_t("face")) + init_values(component_values()))
	{
		m_color_array.changed_signal().connect(make_async_redraw_slot());
		m_array_type.changed_signal().connect(make_async_redraw_slot());
	}

	void on_paint_mesh(const k3d::mesh& Mesh, const k3d::gl::painter_render_state& RenderState, k3d::iproperty::changed_signal_t& ChangedSignal)
	{
		for(k3d::mesh::primitives_t::const_iterator primitive = Mesh.primitives.begin(); primitive != Mesh.primitives.end(); ++primitive)
		{
			boost::scoped_ptr<k3d::polyhedron::const_primitive> polyhedron(k3d::polyhedron::validate(Mesh, **primitive));
			if(!polyhedron.get())
				continue;
			
			if(k3d::polyhedron::is_sds(*polyhedron))
				continue;
	
			const k3d::mesh::points_t& points = *Mesh.points;
	
			const k3d::uint_t face_count = polyhedron->face_first_loops.size();
	
			// Calculate face normals ...
			k3d::typed_array<k3d::normal3> normals(face_count, k3d::normal3(0, 0, 1));
			for(k3d::uint_t face = 0; face != face_count; ++face)
				normals[face] = k3d::polyhedron::normal(polyhedron->vertex_points, polyhedron->clockwise_edges, points, polyhedron->loop_first_edges[polyhedron->face_first_loops[face]]);
	
			// Define a default face color array (in case the user's choice of color array doesn't exist) ...
			k3d::typed_array<k3d::color> default_color_array;
	
			// Get the color array ...
			color_array_proxy color(m_array_type.pipeline_value(), m_color_array.pipeline_value(), *polyhedron, Mesh.point_attributes);
	
			k3d::gl::store_attributes attributes;
			glEnable(GL_LIGHTING);
	
			glFrontFace(RenderState.inside_out ? GL_CCW : GL_CW);
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			k3d::gl::set(GL_CULL_FACE, RenderState.draw_two_sided);
	
			glEnable(GL_POLYGON_OFFSET_FILL);
			glPolygonOffset(1.0, 1.0);
			
			const k3d::uint_t face_begin = 0;
			const k3d::uint_t face_end = face_begin + polyhedron->face_shells.size();
			for(k3d::uint_t face = face_begin; face != face_end; ++face)
			{
				k3d::gl::normal3d(normals[face]);
	
				glBegin(GL_POLYGON);
				const k3d::uint_t first_edge = polyhedron->loop_first_edges[polyhedron->face_first_loops[face]];
				for(k3d::uint_t edge = first_edge; ; )
				{
					k3d::gl::material(GL_FRONT_AND_BACK, GL_DIFFUSE, color(face, edge));
					k3d::gl::vertex3d(points[polyhedron->vertex_points[edge]]);
					edge = polyhedron->clockwise_edges[edge];
					if(edge == first_edge)
						break;
				}
				glEnd();
			}
		}
	}
	
	void on_select_mesh(const k3d::mesh& Mesh, const k3d::gl::painter_render_state& RenderState, const k3d::gl::painter_selection_state& SelectionState, k3d::iproperty::changed_signal_t& ChangedSignal)
	{
		if(!SelectionState.select_component.count(k3d::selection::FACE))
			return;

		k3d::uint_t primitive_index = 0;
		for(k3d::mesh::primitives_t::const_iterator primitive = Mesh.primitives.begin(); primitive != Mesh.primitives.end(); ++primitive, ++primitive_index)
		{
			boost::scoped_ptr<k3d::polyhedron::const_primitive> polyhedron(k3d::polyhedron::validate(Mesh, **primitive));
			if(!polyhedron.get())
				continue;
		
			if(k3d::polyhedron::is_sds(*polyhedron))
				continue;
	
			k3d::gl::push_selection_token(k3d::selection::PRIMITIVE, primitive_index);

			const k3d::mesh::points_t& points = *Mesh.points;
	
			const k3d::uint_t face_count = polyhedron->face_first_loops.size();
	
			k3d::gl::store_attributes attributes;
			glDisable(GL_LIGHTING);
	
			glFrontFace(RenderState.inside_out ? GL_CCW : GL_CW);
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			k3d::gl::set(GL_CULL_FACE, RenderState.draw_two_sided);
	
			for(k3d::uint_t face = 0; face != face_count; ++face)
			{
				k3d::gl::push_selection_token(k3d::selection::FACE, face);
	
				glBegin(GL_POLYGON);
				const k3d::uint_t first_edge = polyhedron->loop_first_edges[polyhedron->face_first_loops[face]];
				for(k3d::uint_t edge = first_edge; ; )
				{
					k3d::gl::vertex3d(points[polyhedron->vertex_points[edge]]);
					edge = polyhedron->clockwise_edges[edge];
					if(edge == first_edge)
						break;
				}
				glEnd();
	
				k3d::gl::pop_selection_token(); // FACE
			}

			k3d::gl::pop_selection_token(); // PRIMITIVE
		}
	}
	
	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<color_face_painter, k3d::interface_list<k3d::gl::imesh_painter > > factory(
			k3d::uuid(0x60b5a9cf, 0xee4fe64c, 0x7559c98f, 0x9ed5b56b),
			"OpenGLColorFacePainter",
			_("Renders mesh faces using a per-face color array"),
			"OpenGL Painter",
			k3d::iplugin_factory::EXPERIMENTAL);

		return factory;
	}

private:
	const k3d::ilist_property<k3d::string_t>::values_t& component_values()
	{
		static k3d::ilist_property<k3d::string_t>::values_t values;
		if(values.empty())
		{
			values.push_back("constant");
			values.push_back("edge");
			values.push_back("face");
			values.push_back("vertex");
			values.push_back("point");
		}

		return values;
	}
	
	struct color_array_proxy
	{
		// Note: we assume the polyhedra are valid here
		color_array_proxy(const k3d::string_t ArrayType,
				const k3d::string_t& ArrayName,
				const k3d::polyhedron::const_primitive& Polyhedron,
				const k3d::table& PointData) :
					m_array_type(ArrayType),
					m_vertex_points(Polyhedron.vertex_points)
		{
			m_color_array = 0;
			if(m_array_type == "constant")
				m_color_array = Polyhedron.constant_attributes.lookup<k3d::mesh::colors_t>(ArrayName);
			else if(m_array_type == "face")
				m_color_array = Polyhedron.face_attributes.lookup<k3d::mesh::colors_t>(ArrayName);
			else if(m_array_type == "edge")
				m_color_array = Polyhedron.edge_attributes.lookup<k3d::mesh::colors_t>(ArrayName);
			else if(m_array_type == "vertex")
				m_color_array = Polyhedron.vertex_attributes.lookup<k3d::mesh::colors_t>(ArrayName);
			else if(m_array_type == "point")
				m_color_array = PointData.lookup<k3d::mesh::colors_t>(ArrayName);
			//assert_not_reached();
		}
		
		const k3d::color operator()(const k3d::uint_t Face, const k3d::uint_t Edge)
		{
			if(!m_color_array)
				return k3d::color(0.9,0.9,0.9);
			if(m_array_type == "constant")
				return m_color_array->at(0);
			else if(m_array_type == "face")
				return m_color_array->at(Face);
			else if(m_array_type == "edge" || m_array_type == "vertex")
				return m_color_array->at(Edge);
			else if(m_array_type == "point")
				return m_color_array->at(m_vertex_points[Edge]);
			//assert_not_reached();
		}
		
	private:
		const k3d::string_t m_array_type;
		const k3d::mesh::colors_t* m_color_array;
		const k3d::mesh::indices_t& m_vertex_points;
	};
	
	k3d_data(k3d::string_t, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_color_array;
	k3d_data(k3d::string_t, immutable_name, change_signal, with_undo, local_storage, no_constraint, list_property, with_serialization) m_array_type;
};

/////////////////////////////////////////////////////////////////////////////
// color_face_painter_factory

k3d::iplugin_factory& color_face_painter_factory()
{
	return color_face_painter::get_factory();
}

} // namespace painters

} // namespace opengl

} // namespace module


