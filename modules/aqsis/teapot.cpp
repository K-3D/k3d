// K-3D
// Copyright (c) 1995-2006, Timothy M. Shead
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
		\brief Implements the Teapot K-3D object, which renders a teapot primitive in render engines that support one (Aqsis!)
		\author Tim Shead (tshead@k-3d.com)
*/

#include <k3d-i18n-config.h>
#include <k3dsdk/algebra.h>
#include <k3dsdk/document_plugin_factory.h>
#include <k3dsdk/imaterial.h>
#include <k3dsdk/material_client.h>
#include <k3dsdk/node.h>
#include <k3dsdk/persistent.h>
#include <k3dsdk/renderable_gl.h>
#include <k3dsdk/renderable_ri.h>
#include <k3dsdk/selection.h>
#include <k3dsdk/transformable.h>

namespace module
{

namespace aqsis
{

const double teapot_points[306][3] =
{
	{1.4,0.0,2.4},
	{1.4,-0.784,2.4},
	{0.784,-1.4,2.4},
	{0.0,-1.4,2.4},
	{1.3375,0.0,2.53125},
	{1.3375,-0.749,2.53125},
	{0.749,-1.3375,2.53125},
	{0.0,-1.3375,2.53125},
	{1.4375,0.0,2.53125},
	{1.4375,-0.805,2.53125},
	{0.805,-1.4375,2.53125},
	{0.0,-1.4375,2.53125},
	{1.5,0.0,2.4},
	{1.5,-0.84,2.4},
	{0.84,-1.5,2.4},
	{0.0,-1.5,2.4},
	{-0.784,-1.4,2.4},
	{-1.4,-0.784,2.4},
	{-1.4,0.0,2.4},
	{-0.749,-1.3375,2.53125},
	{-1.3375,-0.749,2.53125},
	{-1.3375,0.0,2.53125},
	{-0.805,-1.4375,2.53125},
	{-1.4375,-0.805,2.53125},
	{-1.4375,0.0,2.53125},
	{-0.84,-1.5,2.4},
	{-1.5,-0.84,2.4},
	{-1.5,0.0,2.4},
	{-1.4,0.784,2.4},
	{-0.784,1.4,2.4},
	{0.0,1.4,2.4},
	{-1.3375,0.749,2.53125},
	{-0.749,1.3375,2.53125},
	{0.0,1.3375,2.53125},
	{-1.4375,0.805,2.53125},
	{-0.805,1.4375,2.53125},
	{0.0,1.4375,2.53125},
	{-1.5,0.84,2.4},
	{-0.84,1.5,2.4},
	{0.0,1.5,2.4},
	{0.784,1.4,2.4},
	{1.4,0.784,2.4},
	{0.749,1.3375,2.53125},
	{1.3375,0.749,2.53125},
	{0.805,1.4375,2.53125},
	{1.4375,0.805,2.53125},
	{0.84,1.5,2.4},
	{1.5,0.84,2.4},
	{1.75,0.0,1.875},
	{1.75,-0.98,1.875},
	{0.98,-1.75,1.875},
	{0.0,-1.75,1.875},
	{2.0,0.0,1.35},
	{2.0,-1.12,1.35},
	{1.12,-2.0,1.35},
	{0.0,-2.0,1.35},
	{2.0,0.0,0.9},
	{2.0,-1.12,0.9},
	{1.12,-2.0,0.9},
	{0.0,-2.0,0.9},
	{-0.98,-1.75,1.875},
	{-1.75,-0.98,1.875},
	{-1.75,0.0,1.875},
	{-1.12,-2.0,1.35},
	{-2.0,-1.12,1.35},
	{-2.0,0.0,1.35},
	{-1.12,-2.0,0.9},
	{-2.0,-1.12,0.9},
	{-2.0,0.0,0.9},
	{-1.75,0.98,1.875},
	{-0.98,1.75,1.875},
	{0.0,1.75,1.875},
	{-2.0,1.12,1.35},
	{-1.12,2.0,1.35},
	{0.0,2.0,1.35},
	{-2.0,1.12,0.9},
	{-1.12,2.0,0.9},
	{0.0,2.0,0.9},
	{0.98,1.75,1.875},
	{1.75,0.98,1.875},
	{1.12,2.0,1.35},
	{2.0,1.12,1.35},
	{1.12,2.0,0.9},
	{2.0,1.12,0.9},
	{2.0,0.0,0.45},
	{2.0,-1.12,0.45},
	{1.12,-2.0,0.45},
	{0.0,-2.0,0.45},
	{1.5,0.0,0.225},
	{1.5,-0.84,0.225},
	{0.84,-1.5,0.225},
	{0.0,-1.5,0.225},
	{1.5,0.0,0.15},
	{1.5,-0.84,0.15},
	{0.84,-1.5,0.15},
	{0.0,-1.5,0.15},
	{-1.12,-2.0,0.45},
	{-2.0,-1.12,0.45},
	{-2.0,0.0,0.45},
	{-0.84,-1.5,0.225},
	{-1.5,-0.84,0.225},
	{-1.5,0.0,0.225},
	{-0.84,-1.5,0.15},
	{-1.5,-0.84,0.15},
	{-1.5,0.0,0.15},
	{-2.0,1.12,0.45},
	{-1.12,2.0,0.45},
	{0.0,2.0,0.45},
	{-1.5,0.84,0.225},
	{-0.84,1.5,0.225},
	{0.0,1.5,0.225},
	{-1.5,0.84,0.15},
	{-0.84,1.5,0.15},
	{0.0,1.5,0.15},
	{1.12,2.0,0.45},
	{2.0,1.12,0.45},
	{0.84,1.5,0.225},
	{1.5,0.84,0.225},
	{0.84,1.5,0.15},
	{1.5,0.84,0.15},
	{-1.6,0.0,2.025},
	{-1.6,-0.3,2.025},
	{-1.5,-0.3,2.25},
	{-1.5,0.0,2.25},
	{-2.3,0.0,2.025},
	{-2.3,-0.3,2.025},
	{-2.5,-0.3,2.25},
	{-2.5,0.0,2.25},
	{-2.7,0.0,2.025},
	{-2.7,-0.3,2.025},
	{-3.0,-0.3,2.25},
	{-3.0,0.0,2.25},
	{-2.7,0.0,1.8},
	{-2.7,-0.3,1.8},
	{-3.0,-0.3,1.8},
	{-3.0,0.0,1.8},
	{-1.5,0.3,2.25},
	{-1.6,0.3,2.025},
	{-2.5,0.3,2.25},
	{-2.3,0.3,2.025},
	{-3.0,0.3,2.25},
	{-2.7,0.3,2.025},
	{-3.0,0.3,1.8},
	{-2.7,0.3,1.8},
	{-2.7,0.0,1.575},
	{-2.7,-0.3,1.575},
	{-3.0,-0.3,1.35},
	{-3.0,0.0,1.35},
	{-2.5,0.0,1.125},
	{-2.5,-0.3,1.125},
	{-2.65,-0.3,0.9375},
	{-2.65,0.0,0.9375},
	{-2.0,-0.3,0.9},
	{-1.9,-0.3,0.6},
	{-1.9,0.0,0.6},
	{-3.0,0.3,1.35},
	{-2.7,0.3,1.575},
	{-2.65,0.3,0.9375},
	{-2.5,0.3,1.125},
	{-1.9,0.3,0.6},
	{-2.0,0.3,0.9},
	{1.7,0.0,1.425},
	{1.7,-0.66,1.425},
	{1.7,-0.66,0.6},
	{1.7,0.0,0.6},
	{2.6,0.0,1.425},
	{2.6,-0.66,1.425},
	{3.1,-0.66,0.825},
	{3.1,0.0,0.825},
	{2.3,0.0,2.1},
	{2.3,-0.25,2.1},
	{2.4,-0.25,2.025},
	{2.4,0.0,2.025},
	{2.7,0.0,2.4},
	{2.7,-0.25,2.4},
	{3.3,-0.25,2.4},
	{3.3,0.0,2.4},
	{1.7,0.66,0.6},
	{1.7,0.66,1.425},
	{3.1,0.66,0.825},
	{2.6,0.66,1.425},
	{2.4,0.25,2.025},
	{2.3,0.25,2.1},
	{3.3,0.25,2.4},
	{2.7,0.25,2.4},
	{2.8,0.0,2.475},
	{2.8,-0.25,2.475},
	{3.525,-0.25,2.49375},
	{3.525,0.0,2.49375},
	{2.9,0.0,2.475},
	{2.9,-0.15,2.475},
	{3.45,-0.15,2.5125},
	{3.45,0.0,2.5125},
	{2.8,0.0,2.4},
	{2.8,-0.15,2.4},
	{3.2,-0.15,2.4},
	{3.2,0.0,2.4},
	{3.525,0.25,2.49375},
	{2.8,0.25,2.475},
	{3.45,0.15,2.5125},
	{2.9,0.15,2.475},
	{3.2,0.15,2.4},
	{2.8,0.15,2.4},
	{0.0,0.0,3.15},
	{0.0,-0.002,3.15},
	{0.002,0.0,3.15},
	{0.8,0.0,3.15},
	{0.8,-0.45,3.15},
	{0.45,-0.8,3.15},
	{0.0,-0.8,3.15},
	{0.0,0.0,2.85},
	{0.2,0.0,2.7},
	{0.2,-0.112,2.7},
	{0.112,-0.2,2.7},
	{0.0,-0.2,2.7},
	{-0.002,0.0,3.15},
	{-0.45,-0.8,3.15},
	{-0.8,-0.45,3.15},
	{-0.8,0.0,3.15},
	{-0.112,-0.2,2.7},
	{-0.2,-0.112,2.7},
	{-0.2,0.0,2.7},
	{0.0,0.002,3.15},
	{-0.8,0.45,3.15},
	{-0.45,0.8,3.15},
	{0.0,0.8,3.15},
	{-0.2,0.112,2.7},
	{-0.112,0.2,2.7},
	{0.0,0.2,2.7},
	{0.45,0.8,3.15},
	{0.8,0.45,3.15},
	{0.112,0.2,2.7},
	{0.2,0.112,2.7},
	{0.4,0.0,2.55},
	{0.4,-0.224,2.55},
	{0.224,-0.4,2.55},
	{0.0,-0.4,2.55},
	{1.3,0.0,2.55},
	{1.3,-0.728,2.55},
	{0.728,-1.3,2.55},
	{0.0,-1.3,2.55},
	{1.3,0.0,2.4},
	{1.3,-0.728,2.4},
	{0.728,-1.3,2.4},
	{0.0,-1.3,2.4},
	{-0.224,-0.4,2.55},
	{-0.4,-0.224,2.55},
	{-0.4,0.0,2.55},
	{-0.728,-1.3,2.55},
	{-1.3,-0.728,2.55},
	{-1.3,0.0,2.55},
	{-0.728,-1.3,2.4},
	{-1.3,-0.728,2.4},
	{-1.3,0.0,2.4},
	{-0.4,0.224,2.55},
	{-0.224,0.4,2.55},
	{0.0,0.4,2.55},
	{-1.3,0.728,2.55},
	{-0.728,1.3,2.55},
	{0.0,1.3,2.55},
	{-1.3,0.728,2.4},
	{-0.728,1.3,2.4},
	{0.0,1.3,2.4},
	{0.224,0.4,2.55},
	{0.4,0.224,2.55},
	{0.728,1.3,2.55},
	{1.3,0.728,2.55},
	{0.728,1.3,2.4},
	{1.3,0.728,2.4},
	{0.0,0.0,0.0},
	{1.5,0.0,0.15},
	{1.5,0.84,0.15},
	{0.84,1.5,0.15},
	{0.0,1.5,0.15},
	{1.5,0.0,0.075},
	{1.5,0.84,0.075},
	{0.84,1.5,0.075},
	{0.0,1.5,0.075},
	{1.425,0.0,0.0},
	{1.425,0.798,0.0},
	{0.798,1.425,0.0},
	{0.0,1.425,0.0},
	{-0.84,1.5,0.15},
	{-1.5,0.84,0.15},
	{-1.5,0.0,0.15},
	{-0.84,1.5,0.075},
	{-1.5,0.84,0.075},
	{-1.5,0.0,0.075},
	{-0.798,1.425,0.0},
	{-1.425,0.798,0.0},
	{-1.425,0.0,0.0},
	{-1.5,-0.84,0.15},
	{-0.84,-1.5,0.15},
	{0.0,-1.5,0.15},
	{-1.5,-0.84,0.075},
	{-0.84,-1.5,0.075},
	{0.0,-1.5,0.075},
	{-1.425,-0.798,0.0},
	{-0.798,-1.425,0.0},
	{0.0,-1.425,0.0},
	{0.84,-1.5,0.15},
	{1.5,-0.84,0.15},
	{0.84,-1.5,0.075},
	{1.5,-0.84,0.075},
	{0.798,-1.425,0.0},
	{1.425,-0.798,0.0}
};

unsigned int teapot_patches[32][16] =
{
	// Rim
	{1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16},
	{4,17,18,19,8,20,21,22,12,23,24,25,16,26,27,28},
	{19,29,30,31,22,32,33,34,25,35,36,37,28,38,39,40},
	{31,41,42,1,34,43,44,5,37,45,46,9,40,47,48,13},
	// Body
	{13,14,15,16,49,50,51,52,53,54,55,56,57,58,59,60},
	{16,26,27,28,52,61,62,63,56,64,65,66,60,67,68,69},
	{28,38,39,40,63,70,71,72,66,73,74,75,69,76,77,78},
	{40,47,48,13,72,79,80,49,75,81,82,53,78,83,84,57},
	{57,58,59,60,85,86,87,88,89,90,91,92,93,94,95,96},
	{60,67,68,69,88,97,98,99,92,100,101,102,96,103,104,105},
	{69,76,77,78,99,106,107,108,102,109,110,111,105,112,113,114},
	{78,83,84,57,108,115,116,85,111,117,118,89,114,119,120,93},
	// Handle
	{121,122,123,124,125,126,127,128,129,130,131,132,133,134,135,136},
	{124,137,138,121,128,139,140,125,132,141,142,129,136,143,144,133},
	{133,134,135,136,145,146,147,148,149,150,151,152,69,153,154,155},
	{136,143,144,133,148,156,157,145,152,158,159,149,155,160,161,69},
	// Spout
	{162,163,164,165,166,167,168,169,170,171,172,173,174,175,176,177},
	{165,178,179,162,169,180,181,166,173,182,183,170,177,184,185,174},
	{174,175,176,177,186,187,188,189,190,191,192,193,194,195,196,197},
	{177,184,185,174,189,198,199,186,193,200,201,190,197,202,203,194},
	// Lid
	{204,204,204,204,207,208,209,210,211,211,211,211,212,213,214,215},
	{204,204,204,204,210,217,218,219,211,211,211,211,215,220,221,222},
	{204,204,204,204,219,224,225,226,211,211,211,211,222,227,228,229},
	{204,204,204,204,226,230,231,207,211,211,211,211,229,232,233,212},
	{212,213,214,215,234,235,236,237,238,239,240,241,242,243,244,245},
	{215,220,221,222,237,246,247,248,241,249,250,251,245,252,253,254},
	{222,227,228,229,248,255,256,257,251,258,259,260,254,261,262,263},
	{229,232,233,212,257,264,265,234,260,266,267,238,263,268,269,242},
	// Bottom (not on original teapot)
	{270,270,270,270,279,280,281,282,275,276,277,278,271,272,273,274},
	{270,270,270,270,282,289,290,291,278,286,287,288,274,283,284,285},
	{270,270,270,270,291,298,299,300,288,295,296,297,285,292,293,294},
	{270,270,270,270,300,305,306,279,297,303,304,275,294,301,302,271}
};

/////////////////////////////////////////////////////////////////////////////
// teapot

class teapot :
	public k3d::material_client<k3d::gl::renderable<k3d::ri::renderable<k3d::transformable<k3d::persistent<k3d::node> > > > >
{
	typedef k3d::material_client<k3d::gl::renderable<k3d::ri::renderable<k3d::transformable<k3d::persistent<k3d::node> > > > > base;

public:
	teapot(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document)
	{
		m_selection_weight.changed_signal().connect(make_async_redraw_slot());
		m_input_matrix.changed_signal().connect(make_async_redraw_slot());

		// Adjust the patch indices from one-based to zero-based to make things easier elsewhere ...
		static bool adjust_indices = true;
		if(adjust_indices)
		{
			adjust_indices = false;
			for(int i = 0; i < 32; ++i)
			{
				for(int j = 0; j < 16; ++j)
					teapot_patches[i][j] -= 1;
			}
		}
	}

	void on_gl_draw(const k3d::gl::render_state& State)
	{
		if(get_selection_weight())
		{
			glPolygonOffset(1.0, 1.0);
			glEnable(GL_POLYGON_OFFSET_FILL);
		}

		k3d::gl::setup_material(m_material.pipeline_value());

		// Draw solid polygons:
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glEnable(GL_LIGHTING);
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

		if(get_selection_weight())
		{
			glDisable(GL_LIGHTING);
			glColor3d(1, 1, 1);

			// Draw lines:
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

			glDisable(GL_POLYGON_OFFSET_FILL);
		}
	}

	void on_gl_select(const k3d::gl::render_state& State, const k3d::gl::selection_state& SelectState)
	{
		k3d::gl::push_selection_token(this);

		// Draw solid polygons:
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glEnable(GL_LIGHTING);
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

		k3d::gl::pop_selection_token();
	}

	void on_renderman_render(const k3d::ri::render_state& State)
	{
		k3d::ri::setup_material(m_material.pipeline_value(), State);
		State.stream.RiGeometryV("teapot");
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<teapot,
				k3d::interface_list<k3d::itransform_source,
				k3d::interface_list<k3d::itransform_sink > > > factory(
			k3d::uuid(0xb761f071, 0xf7ed4297, 0x9449028d, 0x2f6236f0),
			"Teapot",
			_("Renders a teapot primitive in render engines that support one (Aqsis!)"),
			"Aqsis",
			k3d::iplugin_factory::STABLE);

		return factory;
	}

private:
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
};

/////////////////////////////////////////////////////////////////////////////
// teapot_factory

k3d::iplugin_factory& teapot_factory()
{
	return teapot::get_factory();
}

} // namespace aqsis

} // namespace module

