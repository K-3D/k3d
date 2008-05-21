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
#include <k3dsdk/mesh.h>
#include <k3dsdk/mesh_operations.h>
#include <k3dsdk/mesh_painter_gl.h>
#include <k3dsdk/painter_render_state_gl.h>
#include <k3dsdk/painter_selection_state_gl.h>
#include <k3dsdk/selection.h>

#include <utility>

namespace libk3dquadremesh
{

/////////////////////////////////////////////////////////////////////////////
// face_painter

class pgp_face_painter :
	public k3d::gl::mesh_painter
{
	typedef k3d::gl::mesh_painter base;

public:
	GLubyte iso_tex [512][512][3];
	GLubyte check_tex [512][512][3];
	GLuint texture[2];

	pgp_face_painter(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_draw_pre_theta(init_owner(*this) + init_name("draw_pre_theta") + init_label(_("Draw Pre-Optimization Theta")) + init_description(_("Draw Pre-Optimization Theta")) + init_value(false)),
		m_draw_pre_phi(init_owner(*this) + init_name("draw_pre_phi") + init_label(_("Draw Pre-Optimization Phi")) + init_description(_("Draw Pre-Optimization Phi")) + init_value(false)),
		m_draw_theta(init_owner(*this) + init_name("draw_theta") + init_label(_("Draw Optimized Theta")) + init_description(_("Draw Optimized Theta")) + init_value(false)),
		m_draw_phi(init_owner(*this) + init_name("draw_phi") + init_label(_("Draw Optimized Theta")) + init_description(_("Draw Optimized Theta")) + init_value(false)),
		m_draw_iso(init_owner(*this) + init_name("draw_iso") + init_label(_("Draw Iso-line Texture")) + init_description(_("Draw Iso-line Texture")) + init_value(true)),
		m_draw_check(init_owner(*this) + init_name("draw_check") + init_label(_("Draw Checkerboard Texture")) + init_description(_("Draw Checkerboard Texture")) + init_value(false)),
		m_tex_scale(init_owner(*this) + init_name("tex_scale") + init_label(_("Texture Scaling")) + init_description(_("Texture Scaling")) + init_value(1))
	{
		m_draw_pre_theta.changed_signal().connect(make_async_redraw_slot());
		m_draw_pre_phi.changed_signal().connect(make_async_redraw_slot());
		m_draw_theta.changed_signal().connect(make_async_redraw_slot());
		m_draw_phi.changed_signal().connect(make_async_redraw_slot());
		m_draw_iso.changed_signal().connect(make_async_redraw_slot());
		m_draw_check.changed_signal().connect(make_async_redraw_slot());
		m_tex_scale.changed_signal().connect(make_async_redraw_slot());
		
		glGenTextures( 2, texture );	
		glEnable(GL_TEXTURE_2D);

		for(int i = 0; i < 512; i++) {
			for(int j = 0; j < 512; j++) {
				int I = i % (512);
				int J = j % (512);
				int X = 512;// % IPIX/4;
				int w = 32;
				iso_tex[i][j][0] = iso_tex[i][j][1] = iso_tex[i][j][2] = 255;
				check_tex[i][j][0] = check_tex[i][j][1] = check_tex[i][j][2] = 255;
				if(J < X/w ||  J > X-X/w)
					iso_tex[i][j][0] = iso_tex[i][j][1] = 0;
				if(I < X/w ||  I > X-X/w)
					iso_tex[i][j][1] = iso_tex[i][j][2] = 0;
				if((I < X/w ||  I > X-X/w) && (J < X/w || J > X-X/w))
					iso_tex[i][j][0] = iso_tex[i][j][2] = 255;

				if(J <= X/2 && I >= X/2)
					check_tex[i][j][0] = check_tex[i][j][1] = check_tex[i][j][2] = 0;
				if(J >= X/2 && I <= X/2)
					check_tex[i][j][0] = check_tex[i][j][1] = check_tex[i][j][2] = 0;

			}
		}
		
		glBindTexture( GL_TEXTURE_2D, texture[0] );
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); 
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT); 
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

		glTexImage2D(GL_TEXTURE_2D, 0, 3, 512, 512, 0, GL_RGB, GL_UNSIGNED_BYTE, iso_tex);
		
		glBindTexture( GL_TEXTURE_2D, texture[1] );
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); 
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT); 
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

		glTexImage2D(GL_TEXTURE_2D, 0, 3, 512, 512, 0, GL_RGB, GL_UNSIGNED_BYTE, check_tex);
				
		glDisable(GL_TEXTURE_2D);

	}

	
	~pgp_face_painter() {
		glDeleteTextures( 2, texture);
	}
	void bad_mesh(const k3d::mesh& Mesh, const k3d::gl::painter_render_state& RenderState)
	{
		std::cout << "d" << std::flush;
		const k3d::mesh::indices_t& face_first_loops = *Mesh.polyhedra->face_first_loops;
		const k3d::mesh::selection_t& face_selection = *Mesh.polyhedra->face_selection;
		const k3d::mesh::indices_t& loop_first_edges = *Mesh.polyhedra->loop_first_edges;
		const k3d::mesh::indices_t& edge_points = *Mesh.polyhedra->edge_points;
		const k3d::mesh::indices_t& clockwise_edges = *Mesh.polyhedra->clockwise_edges;
		const k3d::mesh::points_t& points = *Mesh.points;
		const size_t face_count = face_first_loops.size();
		
		k3d::typed_array<k3d::normal3> normals(face_count, k3d::normal3(0, 0, 1));
		for(size_t face = 0; face != face_count; ++face) {
			const size_t edge = loop_first_edges[face_first_loops[face]];
			k3d::point3 a = points[edge_points[edge]];
			k3d::point3 b = points[edge_points[clockwise_edges[edge]]];
			k3d::point3 c = points[edge_points[clockwise_edges[clockwise_edges[edge]]]];
			a -= k3d::to_vector(b);
			c -= k3d::to_vector(b);
			normals[face] = k3d::normal3(c[1]*a[2]-a[1]*c[2], c[2]*a[0]-a[2]*c[0], c[0]*a[1]-a[0]*c[1]);
			double n = normals[face].length();
			if(n > 0.00000001)
				normals[face] /= n;
		}


		k3d::gl::store_attributes attributes;
		glDisable(GL_POLYGON_OFFSET_FILL);

		k3d::color color = k3d::color(0.7, 0.7, 0.7);	
		glEnable(GL_LIGHTING);
		k3d::gl::material(GL_FRONT_AND_BACK, GL_DIFFUSE, color);
		for(size_t face = 0; face != face_count; ++face) {
			int count = 0;
			k3d::gl::normal3d(normals[face]);
			glBegin(GL_POLYGON);
			const size_t first_edge = loop_first_edges[face_first_loops[face]];
			for(size_t edge = first_edge; ; )
			{
				k3d::gl::vertex3d(points[edge_points[edge]]);
				edge = clockwise_edges[edge];
				count++;
				if(edge == first_edge || count > edge_points.size()+1)
					break;
			}
			glEnd();
		}

		color = k3d::color(1, 0, 0);
		glLineWidth(3.0);
		glDisable(GL_LIGHTING);
		glEnable(GL_POLYGON_OFFSET_LINE);
		glPolygonOffset( 1.0, 1.0 );
		glBegin(GL_LINES);
		const size_t edge_count = edge_points.size();
		for(size_t edge = 0; edge != edge_count; ++edge)
		{
			k3d::gl::color3d(color);
			k3d::gl::vertex3d(points[edge_points[edge]]);
			k3d::gl::vertex3d(points[edge_points[clockwise_edges[edge]]]);
		}
		glEnd();
		glDisable(GL_POLYGON_OFFSET_LINE);

	}
	void on_paint_mesh(const k3d::mesh& Mesh, const k3d::gl::painter_render_state& RenderState)
	{
		bad_mesh(Mesh, RenderState);  return;
		if(!k3d::validate_polyhedra(Mesh))
		{ bad_mesh(Mesh, RenderState);  return; }
			
		if (k3d::is_sds(Mesh))
			return;
		
		if(Mesh.polyhedra->face_varying_data.find("PGP_pre_theta_color") == Mesh.polyhedra->face_varying_data.end())
		{bad_mesh(Mesh, RenderState);  return; }
		if(Mesh.polyhedra->face_varying_data.find("PGP_pre_phi_color") == Mesh.polyhedra->face_varying_data.end())
		{bad_mesh(Mesh, RenderState);  return; }
		if(Mesh.polyhedra->face_varying_data.find("PGP_uv") == Mesh.polyhedra->face_varying_data.end())
		{bad_mesh(Mesh, RenderState);  return; }

		const k3d::mesh::indices_t& face_first_loops = *Mesh.polyhedra->face_first_loops;
		const k3d::mesh::selection_t& face_selection = *Mesh.polyhedra->face_selection;
		const k3d::mesh::indices_t& loop_first_edges = *Mesh.polyhedra->loop_first_edges;
		const k3d::mesh::indices_t& edge_points = *Mesh.polyhedra->edge_points;
		const k3d::mesh::indices_t& clockwise_edges = *Mesh.polyhedra->clockwise_edges;
		const k3d::mesh::points_t& points = *Mesh.points;
		const k3d::typed_array < k3d::color > & c_theta = dynamic_cast<const k3d::typed_array < k3d::color > & >(*((*(Mesh.polyhedra->face_varying_data.find("PGP_pre_theta_color"))).second)); 
		const k3d::typed_array < k3d::color > & c_phi   = dynamic_cast<const k3d::typed_array < k3d::color > & >(*((*(Mesh.polyhedra->face_varying_data.find("PGP_pre_phi_color"))).second)); 
		const k3d::typed_array < std::pair<double, double> > & uv   = dynamic_cast<const k3d::typed_array < std::pair<double, double> > & >(*((*(Mesh.polyhedra->face_varying_data.find("PGP_uv"))).second)); 
		const size_t face_count = face_first_loops.size();

		// Calculate face normals ...
		k3d::typed_array<k3d::normal3> normals(face_count, k3d::normal3(0, 0, 1));
		for(size_t face = 0; face != face_count; ++face)
			normals[face] = k3d::normal(edge_points, clockwise_edges, points, loop_first_edges[face_first_loops[face]]);

		k3d::gl::store_attributes attributes;
		glEnable(GL_LIGHTING);

		double scale = (double)m_tex_scale.pipeline_value();
		if(scale == 0.0) scale = 1.0;
		if(scale < 0.0) {
			scale = -1.0/(scale); 
		}

		const k3d::color color = k3d::color(0.8, 0.8, 0.8);
		const k3d::color selected_color = RenderState.show_component_selection ? k3d::color(1, 0, 0) : color;

		glFrontFace(GL_CW);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		k3d::gl::set(GL_CULL_FACE, RenderState.draw_two_sided);

		//glEnable(GL_POLYGON_OFFSET_FILL);
		//glPolygonOffset(1.0, 1.0);
		
		bool draw_texture = m_draw_iso.pipeline_value() || m_draw_check.pipeline_value();
		
		if(draw_texture) {
			glEnable(GL_TEXTURE_2D);
			if(m_draw_iso.pipeline_value())
				glBindTexture( GL_TEXTURE_2D, texture[0] );
			else
				glBindTexture( GL_TEXTURE_2D, texture[1] );
		}

		for(size_t face = 0; face != face_count; ++face) {
			k3d::gl::normal3d(normals[face]);

			glBegin(GL_POLYGON);
			const size_t first_edge = loop_first_edges[face_first_loops[face]];
			for(size_t edge = first_edge; ; )
			{
				if(draw_texture) 
					glTexCoord2d(scale*uv[edge].first, scale*uv[edge].second);
				else if(m_draw_theta.pipeline_value())
					k3d::gl::material(GL_FRONT_AND_BACK, GL_DIFFUSE, c_theta[edge]);
				else
					k3d::gl::material(GL_FRONT_AND_BACK, GL_DIFFUSE, c_phi[edge]);

				k3d::gl::vertex3d(points[edge_points[edge]]);
				
				edge = clockwise_edges[edge];
				if(edge == first_edge)
					break;
			}
			glEnd();
		}
		if(draw_texture)
			glDisable(GL_TEXTURE_2D);
	}
	
	void on_select_mesh(const k3d::mesh& Mesh, const k3d::gl::painter_render_state& RenderState, const k3d::gl::painter_selection_state& SelectionState)
	{
		return;
		if(!SelectionState.select_faces)
			return;

		if(!k3d::validate_polyhedra(Mesh))
			return;
		
		if (k3d::is_sds(Mesh))
			return;

		const k3d::mesh::indices_t& face_first_loops = *Mesh.polyhedra->face_first_loops;
		const k3d::mesh::indices_t& loop_first_edges = *Mesh.polyhedra->loop_first_edges;
		const k3d::mesh::indices_t& edge_points = *Mesh.polyhedra->edge_points;
		const k3d::mesh::indices_t& clockwise_edges = *Mesh.polyhedra->clockwise_edges;
		const k3d::mesh::points_t& points = *Mesh.points;

		const size_t face_count = face_first_loops.size();

		k3d::gl::store_attributes attributes;
		glDisable(GL_LIGHTING);

		glFrontFace(GL_CW);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		k3d::gl::set(GL_CULL_FACE, RenderState.draw_two_sided);

		for(size_t face = 0; face != face_count; ++face)
		{
			k3d::gl::push_selection_token(k3d::selection::ABSOLUTE_FACE, face);

			glBegin(GL_POLYGON);
			const size_t first_edge = loop_first_edges[face_first_loops[face]];
			for(size_t edge = first_edge; ; )
			{
				k3d::gl::vertex3d(points[edge_points[edge]]);
				edge = clockwise_edges[edge];
				if(edge == first_edge)
					break;
			}
			glEnd();

			k3d::gl::pop_selection_token(); // ABSOLUTE_FACE
		}
	}
	
	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<pgp_face_painter, k3d::interface_list<k3d::gl::imesh_painter > > factory(
			k3d::uuid(0x1d35ab47, 0x4ed0c54b, 0xfa5ccc89, 0xe1f683d3),
			"OpenGLPGPFacePainter",
			_("Renders mesh faces with PGP"),
			"OpenGL Painters",
			k3d::iplugin_factory::EXPERIMENTAL);

		return factory;
	}

private:
	k3d_data(bool, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_draw_pre_theta;
	k3d_data(bool, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_draw_pre_phi;
	k3d_data(bool, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_draw_theta;
	k3d_data(bool, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_draw_phi;
	k3d_data(bool, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_draw_iso;
	k3d_data(bool, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_draw_check;
	k3d_data(k3d::int32_t, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_tex_scale;

};

/////////////////////////////////////////////////////////////////////////////
// face_painter_factory

k3d::iplugin_factory& pgp_face_painter_factory()
{
	return pgp_face_painter::get_factory();
}

} // namespace libk3dopengl

