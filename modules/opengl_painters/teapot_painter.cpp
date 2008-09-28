// K-3D
// Copyright (c) 1995-2008, Timothy M. Shead
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
	\author Tim Shead (tshead@k-3d.com)
*/

#include <k3d-i18n-config.h>
#include <k3dsdk/document_plugin_factory.h>
#include <k3dsdk/mesh.h>
#include <k3dsdk/mesh_painter_gl.h>
#include <k3dsdk/painter_render_state_gl.h>
#include <k3dsdk/teapot.h>
#include <k3dsdk/utility_gl.h>

#include <boost/scoped_ptr.hpp>

namespace module
{

namespace opengl
{

namespace painters
{

/////////////////////////////////////////////////////////////////////////////
// teapot_painter

class teapot_painter :
	public k3d::gl::mesh_painter
{
	typedef k3d::gl::mesh_painter base;

public:
	teapot_painter(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document)
	{
	}

	void on_paint_mesh(const k3d::mesh& Mesh, const k3d::gl::painter_render_state& RenderState)
	{
		for(k3d::mesh::primitives_t::const_iterator primitive = Mesh.primitives.begin(); primitive != Mesh.primitives.end(); ++primitive)
		{
			boost::scoped_ptr<k3d::teapot::primitive> teapot(k3d::teapot::validate(**primitive));
			if(!teapot)
				continue;

			glPolygonOffset(1.0, 1.0);
			glEnable(GL_POLYGON_OFFSET_FILL);
			glEnable(GL_LIGHTING);
			glColor3d(0.8, 0.8, 0.8);

			glMatrixMode(GL_MODELVIEW);
			for(k3d::uint_t i = 0; i != teapot->matrices.size(); ++i)
			{
				glPushMatrix();
				k3d::gl::push_matrix(teapot->matrices[i]);
				glCallList(get_solid_display_list());
				glPopMatrix();
			}

			if(RenderState.node_selection)
			{
				glDisable(GL_LIGHTING);
				glColor3d(1, 1, 1);

				for(k3d::uint_t i = 0; i != teapot->matrices.size(); ++i)
				{
					glPushMatrix();
					k3d::gl::push_matrix(teapot->matrices[i]);
					glCallList(get_wireframe_display_list());
					glPopMatrix();
				}
			}
		}
	}

	void on_select_mesh(const k3d::mesh& Mesh, const k3d::gl::painter_render_state& RenderState, const k3d::gl::painter_selection_state& SelectionState)
	{
		for(k3d::mesh::primitives_t::const_iterator primitive = Mesh.primitives.begin(); primitive != Mesh.primitives.end(); ++primitive)
		{
			boost::scoped_ptr<k3d::teapot::primitive> teapot(k3d::teapot::validate(**primitive));
			if(!teapot)
				continue;

			glDisable(GL_LIGHTING);

			glMatrixMode(GL_MODELVIEW);
			for(k3d::uint_t i = 0; i != teapot->matrices.size(); ++i)
			{
				glPushMatrix();
				k3d::gl::push_matrix(teapot->matrices[i]);
				glCallList(get_solid_display_list());
				glPopMatrix();
			}

		}
	}

	void on_mesh_changed(const k3d::mesh& Mesh, k3d::ihint* Hint)
	{
	}

	GLuint get_solid_display_list()
	{
		static GLuint result = 0;
		if(!result)
		{
			result = glGenLists(1);
			glNewList(result, GL_COMPILE);

			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			glDisable(GL_TEXTURE_1D);
			glDisable(GL_TEXTURE_2D);
			for (int i=0; i<32; i++)
			{
				for (double x=0.0; x<1; x+=0.2)
				{
					for (double y=0.0; y<1; y+=0.2)
					{
						glBegin(GL_POLYGON); //triangular polygons, "top" row
							glNormal3dv(teapotNormal(x,y,i).n);
							glVertex3dv(teapotPoint(x,y,i).n);
							glNormal3dv(teapotNormal(x+0.2,y,i).n);
							glVertex3dv(teapotPoint(x+0.2,y,i).n);
							glNormal3dv(teapotNormal(x,y+0.2,i).n);
							glVertex3dv(teapotPoint(x,y+0.2,i).n);
						glEnd();

						glBegin(GL_POLYGON); //fill up the triangles missed by the other part
							glNormal3dv(teapotNormal(x+0.2,y,i).n);
							glVertex3dv(teapotPoint(x+0.2,y,i).n);
							glNormal3dv(teapotNormal(x+0.2,y+0.2,i).n);
							glVertex3dv(teapotPoint(x+0.2,y+0.2,i).n);
							glNormal3dv(teapotNormal(x,y+0.2,i).n);
							glVertex3dv(teapotPoint(x,y+0.2,i).n);
						glEnd();
					}
				}
			}
			glEndList();
		}

		return result;
	}

	GLuint get_wireframe_display_list()
	{
		static GLuint result = 0;
		if(!result)
		{
			result = glGenLists(1);
			glNewList(result, GL_COMPILE);

			for (int i=0; i<32; i++)
			{
				for (double x=0.0; x<1; x+=0.2)
				{
					for (double y=0.0; y<1; y+=0.2)
					{
						glBegin(GL_LINES); //triangular polygons, "top" row
							glVertex3dv(teapotPoint(x,y,i).n);
							glVertex3dv(teapotPoint(x+0.2,y,i).n);
							glVertex3dv(teapotPoint(x,y+0.2,i).n);
						glEnd();

						glBegin(GL_LINES); //fill up the triangles missed by the other part
							glVertex3dv(teapotPoint(x+0.2,y,i).n);
							glVertex3dv(teapotPoint(x+0.2,y+0.2,i).n);
							glVertex3dv(teapotPoint(x,y+0.2,i).n);
						glEnd();
					}
				}
			}
			glEndList();
		}

		return result;
	}

	k3d::point3 bezierInterp(double t, const k3d::point3 &v0, const k3d::point3 &v1, const k3d::point3 &v2, const k3d::point3 &v3)
	{
		//return the coordinates of the point at position t on a bezier curve with 4 control points
		double B0 = (1-t)*(1-t)*(1-t);
		double B1 = 3*t*(1-t)*(1-t);
		double B2 = 3*t*t*(1-t);
		double B3 = t*t*t;
		k3d::point3 a = v0*B0;
		k3d::point3 b = v1*B1;
		k3d::point3 c = v2*B2;
		k3d::point3 d = v3*B3;
		k3d::point3 res = a+b;
		res+=k3d::to_vector(c);
		res+=k3d::to_vector(d);
		return res; //errors if done in one line (?)
	}

	k3d::point3 teapotPoint(double x, double y, int patch)
	{
		//returns a point on the teapot
		k3d::point3 ypoints[4];
		for(int i = 0; i < 4; i++)
		{
			k3d::point3 v0(teapot_points[teapot_patches[patch][4*i]]);
			k3d::point3 v1(teapot_points[teapot_patches[patch][4*i+1]]);
			k3d::point3 v2(teapot_points[teapot_patches[patch][4*i+2]]);
			k3d::point3 v3(teapot_points[teapot_patches[patch][4*i+3]]);
			ypoints[i]=bezierInterp(x,v0,v1,v2,v3);
		}

		return bezierInterp(y,ypoints[0],ypoints[1],ypoints[2],ypoints[3]);
	}

	k3d::vector3 teapotNormal(double x_in, double y_in, int patch)
	{
		//returns a normal vector on the teapot
		double prec = 0.001; //length of the vectors used to calculate the normals
		double x = x_in;
		double y = y_in;
		//lid has a path consisting of 4 identical points, so calculating the normal will fail using the standard method:
		if(teapotPoint(0,y,patch) == teapotPoint(1,y,patch))
			return k3d::vector3(0,0,1); //normal for the knob on top of the lid
		//make sure we never go outside [0,1] for x and y:
		if(x >= 1-prec)
			x -= prec;
		if(y >= 1-prec)
			y -= prec;
		//cross product of two short vectors through the point should be a good approximation of the normal:
		return (teapotPoint(x+prec,y,patch) - teapotPoint(x,y,patch)) ^ (teapotPoint(x,y+prec,patch) - teapotPoint(x,y,patch));
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<teapot_painter, k3d::interface_list<k3d::gl::imesh_painter> > factory(
			k3d::uuid(0xaa87fdcc, 0xfc47c821, 0x87d74dad, 0xf1f9f833),
			"OpenGLTeapotPainter",
			_("Renders teapot primitives using OpoenGL"),
			"OpenGL Painter",
			k3d::iplugin_factory::EXPERIMENTAL);

		return factory;
	}

	static const k3d::teapot::points_array_t& teapot_points;
	static const k3d::teapot::patches_array_t& teapot_patches;
};

const k3d::teapot::points_array_t& teapot_painter::teapot_points = k3d::teapot::points();
const k3d::teapot::patches_array_t& teapot_painter::teapot_patches = k3d::teapot::patches();

/////////////////////////////////////////////////////////////////////////////
// teapot_painter_factory

k3d::iplugin_factory& teapot_painter_factory()
{
	return teapot_painter::get_factory();
}

} // namespace painters

} // namespace opengl

} // namespace module

