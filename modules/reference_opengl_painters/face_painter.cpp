// K-3D
// Copyright (c) 1995-2005, Timothy M. Shead
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

/** \file Painter that triangulates its input first
 * 	\author Bart Janssens (bart.janssens@lid.kviv.be)
 */

#include "colored_selection_painter_gl.h"

#include <k3d-i18n-config.h>
#include <k3dsdk/document_plugin_factory.h>
#include <k3dsdk/gl.h>
#include <k3dsdk/mesh_painter_gl.h>
#include <k3dsdk/painter_render_state_gl.h>
#include <k3dsdk/painter_selection_state_gl.h>
#include <k3dsdk/polyhedron.h>
#include <k3dsdk/selection.h>
#include <k3dsdk/triangulator.h>
#include <k3dsdk/utility_gl.h>

#include <boost/scoped_ptr.hpp>

namespace module
{

namespace opengl
{

namespace painters
{

/////////////////////////////////////////////////////////////////////////////
// face_painter

class face_painter :
	public colored_selection_painter
{
	typedef colored_selection_painter base;

public:
	face_painter(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document, k3d::color(0.2, 0.2, 0.2), k3d::color(0.6, 0.6, 0.6))
	{
	}

	class paint_triangles :
		public k3d::triangulator
	{
	public:
		paint_triangles(const k3d::mesh::points_t& Points, const k3d::mesh::selection_t& FaceSelections, const k3d::color& Color, const k3d::color& SelectedColor) :
			points(Points),
			face_selections(FaceSelections),
			color(Color),
			selected_color(SelectedColor)
		{
		}

		void start_processing(const k3d::mesh& SourceMesh)
		{
			glBegin(GL_TRIANGLES);
		}

		void start_face(const k3d::uint_t Face)
		{
			const k3d::color& face_color = face_selections[Face] ? selected_color : color;
			k3d::gl::material(GL_FRONT_AND_BACK, GL_DIFFUSE, face_color);
		}

		void add_vertex(const k3d::point3& Coordinates, k3d::uint_t Vertices[4], k3d::uint_t Edges[4], k3d::double_t Weights[4], k3d::uint_t& NewVertex)
		{
			NewVertex = points.size();
			points.push_back(Coordinates);
		}

		void add_triangle(k3d::uint_t Vertices[3], k3d::uint_t Edges[3])
		{
			k3d::gl::normal3d(k3d::polyhedron::normal(points[Vertices[0]], points[Vertices[1]], points[Vertices[2]]));
			k3d::gl::vertex3d(points[Vertices[0]]);
			k3d::gl::vertex3d(points[Vertices[1]]);
			k3d::gl::vertex3d(points[Vertices[2]]);
		}

		void finish_processing(const k3d::mesh& SourceMesh)
		{
			glEnd();
		}

	private:
		std::vector<k3d::point3> points;
		const k3d::mesh::selection_t& face_selections;
		const k3d::color color;
		const k3d::color selected_color;
	};

	void on_paint_mesh(const k3d::mesh& Mesh, const k3d::gl::painter_render_state& RenderState)
	{
		for(k3d::mesh::primitives_t::const_iterator primitive = Mesh.primitives.begin(); primitive != Mesh.primitives.end(); ++primitive)
		{
			boost::scoped_ptr<k3d::polyhedron::const_primitive> polyhedron(k3d::polyhedron::validate(Mesh, **primitive));
			if(!polyhedron.get())
				continue;
			
			if(k3d::polyhedron::is_sds(*polyhedron))
				continue;
		
			k3d::gl::store_attributes attributes;
	
			glFrontFace(RenderState.inside_out ? GL_CCW : GL_CW);
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			k3d::gl::set(GL_CULL_FACE, RenderState.draw_two_sided);
	
			glEnable(GL_LIGHTING);
			glEnable(GL_POLYGON_OFFSET_FILL);
			glPolygonOffset(1.0, 1.0);
			
			const k3d::color color = RenderState.node_selection ? selected_mesh_color() : unselected_mesh_color(RenderState.parent_selection);
			const k3d::color selected_color = RenderState.show_component_selection ? selected_component_color() : color;

			paint_triangles painter(*Mesh.points, polyhedron->face_selections, color, selected_color);
			painter.process(Mesh, *polyhedron);
		}
	}
	
	class select_triangles :
		public k3d::triangulator
	{
	public:
		select_triangles(const k3d::mesh::points_t& Points) :
			points(Points)
		{
		}

		void start_processing(const k3d::mesh& SourceMesh)
		{
		}

		void start_face(const k3d::uint_t Face)
		{
			k3d::gl::push_selection_token(k3d::selection::UNIFORM, Face);
			glBegin(GL_TRIANGLES);
		}

		void add_vertex(const k3d::point3& Coordinates, k3d::uint_t Vertices[4], k3d::uint_t Edges[4], k3d::double_t Weights[4], k3d::uint_t& NewVertex)
		{
			NewVertex = points.size();
			points.push_back(Coordinates);
		}

		void add_triangle(k3d::uint_t Vertices[3], k3d::uint_t Edges[3])
		{
			k3d::gl::vertex3d(points[Vertices[0]]);
			k3d::gl::vertex3d(points[Vertices[1]]);
			k3d::gl::vertex3d(points[Vertices[2]]);
		}

		void finish_face(const k3d::uint_t Face)
		{
			glEnd(); // GL_TRIANGLES
			k3d::gl::pop_selection_token(); // UNIFORM
		}

		void finish_processing(const k3d::mesh& SourceMesh)
		{
		}

	private:
		std::vector<k3d::point3> points;
	};

	void on_select_mesh(const k3d::mesh& Mesh, const k3d::gl::painter_render_state& RenderState, const k3d::gl::painter_selection_state& SelectionState)
	{
		if(!SelectionState.select_component.count(k3d::selection::UNIFORM))
			return;
	
		k3d::uint_t primitive_index = 0;
		for(k3d::mesh::primitives_t::const_iterator primitive = Mesh.primitives.begin(); primitive != Mesh.primitives.end(); ++primitive, ++primitive_index)
		{
			boost::scoped_ptr<k3d::polyhedron::const_primitive> polyhedron(k3d::polyhedron::validate(Mesh, **primitive));
			if(!polyhedron.get())
				continue;
			
			if(k3d::polyhedron::is_sds(*polyhedron))
				continue;

			k3d::gl::store_attributes attributes;

			glFrontFace(RenderState.inside_out ? GL_CCW : GL_CW);
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			k3d::gl::set(GL_CULL_FACE, !SelectionState.select_backfacing);
	
			glDisable(GL_LIGHTING);
			glEnable(GL_POLYGON_OFFSET_FILL);
			glPolygonOffset(1.0, 1.0);
			
			k3d::gl::push_selection_token(k3d::selection::PRIMITIVE, primitive_index);

			select_triangles painter(*Mesh.points);
			painter.process(Mesh, *polyhedron);

			k3d::gl::pop_selection_token(); // PRIMITIVE
		}
	}
	
	void on_mesh_changed(const k3d::mesh& Mesh, k3d::ihint* Hint)
	{
	}
	
	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<face_painter, k3d::interface_list<k3d::gl::imesh_painter > > factory(
			k3d::uuid(0xd4abf63c, 0x2242c17e, 0x2afcb18d, 0x0a8ebdd5),
			"OpenGLFacePainter",
			_("Renders mesh faces, after trianglulating them (OpenGL 1.1)"),
			"OpenGL Painter",
			k3d::iplugin_factory::EXPERIMENTAL);

		return factory;
	}
};

/////////////////////////////////////////////////////////////////////////////
// face_painter_factory

k3d::iplugin_factory& face_painter_factory()
{
	return face_painter::get_factory();
}

} // namespace opengl

} // namespace painters

} // namespace module

