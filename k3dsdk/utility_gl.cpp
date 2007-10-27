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
		\author Tim Shead (tshead@k-3d.com)
*/

#include "algebra.h"
#include "bounding_box3.h"
#include "extension_gl.h"
#include "idocument.h"
#include "imaterial.h"
#include "imaterial_gl.h"
#include "inode.h"
#include "inode_collection.h"
#include "irender_engine_gl.h"
#include "plane.h"
#include "utility_gl.h"

#include <algorithm>
#include <set>

namespace k3d
{

namespace gl
{

namespace detail
{

class redraw
{
public:
	redraw(const irender_engine::redraw_type_t RedrawType) :
		m_redraw_type(RedrawType)
	{
	}

	void operator()(inode* const Object)
	{
		if(irender_engine* const render_engine = dynamic_cast<irender_engine*>(Object))
			render_engine->redraw_request_signal().emit(m_redraw_type);
	}

protected:
	const irender_engine::redraw_type_t m_redraw_type;
};

const GLubyte g_stipple_halftone[] = {
	0xaa, 0xaa, 0xaa, 0xaa, 0x55, 0x55, 0x55, 0x55,
	0xaa, 0xaa, 0xaa, 0xaa, 0x55, 0x55, 0x55, 0x55,
	0xaa, 0xaa, 0xaa, 0xaa, 0x55, 0x55, 0x55, 0x55,
	0xaa, 0xaa, 0xaa, 0xaa, 0x55, 0x55, 0x55, 0x55,
	0xaa, 0xaa, 0xaa, 0xaa, 0x55, 0x55, 0x55, 0x55,
	0xaa, 0xaa, 0xaa, 0xaa, 0x55, 0x55, 0x55, 0x55,
	0xaa, 0xaa, 0xaa, 0xaa, 0x55, 0x55, 0x55, 0x55,
	0xaa, 0xaa, 0xaa, 0xaa, 0x55, 0x55, 0x55, 0x55,
	0xaa, 0xaa, 0xaa, 0xaa, 0x55, 0x55, 0x55, 0x55,
	0xaa, 0xaa, 0xaa, 0xaa, 0x55, 0x55, 0x55, 0x55,
	0xaa, 0xaa, 0xaa, 0xaa, 0x55, 0x55, 0x55, 0x55,
	0xaa, 0xaa, 0xaa, 0xaa, 0x55, 0x55, 0x55, 0x55,
	0xaa, 0xaa, 0xaa, 0xaa, 0x55, 0x55, 0x55, 0x55,
	0xaa, 0xaa, 0xaa, 0xaa, 0x55, 0x55, 0x55, 0x55,
	0xaa, 0xaa, 0xaa, 0xaa, 0x55, 0x55, 0x55, 0x55,
	0xaa, 0xaa, 0xaa, 0xaa, 0x55, 0x55, 0x55, 0x55
	};

const GLfloat g_color_black[4] = { 0, 0, 0, 1 };

const GLfloat g_color_white[4] = { 1, 1, 1, 1 };

class null_material :
	public gl::imaterial
{
public:
	void setup_gl_material()
	{
		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, g_color_black);
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, g_color_white);
		glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, g_color_black);
		glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, g_color_black);

		glPolygonStipple(g_stipple_halftone);
		glEnable(GL_POLYGON_STIPPLE);
	}
};

null_material g_null_material;

class default_material :
	public gl::imaterial
{
public:
	void setup_gl_material()
	{
		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, g_color_black);
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, g_color_white);
		glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, g_color_black);
		glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, g_color_black);

		glDisable(GL_POLYGON_STIPPLE);
	}
};

default_material g_default_material;

} // namespace detail

void draw(const plane& Plane)
{
	const k3d::vector3 n = Plane.normal;

	k3d::vector3 nx(-0.7071, -0.7071, 0);
	k3d::vector3 ny(0.7071, -0.7071, 0);
	const k3d::vector3 x_projection = n ^ vector3(n[1], n[0], n[2]);
	if(x_projection.length())
	{
		nx = normalize(n ^ vector3(n[1], n[0], n[2]));
		ny = normalize(n ^ nx);
	}

	const k3d::point3 origin = k3d::point3(0, 0, 0) + (Plane.normal * Plane.distance);

	glBegin(GL_LINE_LOOP);
		vertex3d(origin - nx + ny);
		vertex3d(origin + nx + ny);
		vertex3d(origin + nx - ny);
		vertex3d(origin - nx - ny);
	glEnd();

	glBegin(GL_LINES);
		vertex3d(origin);
		vertex3d(origin + n);
	glEnd();
}

void draw(const bounding_box3& Box)
{
	glBegin(GL_QUADS);
		glNormal3d(0, 0, -1);
		glVertex3d(Box.nx, Box.py, Box.nz);
		glVertex3d(Box.px, Box.py, Box.nz);
		glVertex3d(Box.px, Box.ny, Box.nz);
		glVertex3d(Box.nx, Box.ny, Box.nz);

		glNormal3d(0, 0, 1);
		glVertex3d(Box.px, Box.py, Box.pz);
		glVertex3d(Box.nx, Box.py, Box.pz);
		glVertex3d(Box.nx, Box.ny, Box.pz);
		glVertex3d(Box.px, Box.ny, Box.pz);

		glNormal3d(0, 1, 0);
		glVertex3d(Box.nx, Box.py, Box.pz);
		glVertex3d(Box.px, Box.py, Box.pz);
		glVertex3d(Box.px, Box.py, Box.nz);
		glVertex3d(Box.nx, Box.py, Box.nz);

		glNormal3d(0, -1, 0);
		glVertex3d(Box.px, Box.ny, Box.nz);
		glVertex3d(Box.nx, Box.ny, Box.nz);
		glVertex3d(Box.nx, Box.ny, Box.pz);
		glVertex3d(Box.px, Box.ny, Box.pz);

		glNormal3d(-1, 0, 0);
		glVertex3d(Box.nx, Box.py, Box.pz);
		glVertex3d(Box.nx, Box.py, Box.nz);
		glVertex3d(Box.nx, Box.ny, Box.nz);
		glVertex3d(Box.nx, Box.ny, Box.pz);

		glNormal3d(1, 0, 0);
		glVertex3d(Box.px, Box.py, Box.nz);
		glVertex3d(Box.px, Box.py, Box.pz);
		glVertex3d(Box.px, Box.ny, Box.pz);
		glVertex3d(Box.px, Box.ny, Box.nz);
	glEnd();
}

void draw_bounding_box(const bounding_box3& Box)
{
	glBegin(GL_LINE_LOOP);
		glVertex3d(Box.nx, Box.ny, Box.nz);
		glVertex3d(Box.px, Box.ny, Box.nz);
		glVertex3d(Box.px, Box.ny, Box.pz);
		glVertex3d(Box.nx, Box.ny, Box.pz);
	glEnd();

	glBegin(GL_LINE_LOOP);
		glVertex3d(Box.nx, Box.py, Box.nz);
		glVertex3d(Box.px, Box.py, Box.nz);
		glVertex3d(Box.px, Box.py, Box.pz);
		glVertex3d(Box.nx, Box.py, Box.pz);
	glEnd();

	glBegin(GL_LINES);
		glVertex3d(Box.nx, Box.ny, Box.nz);
		glVertex3d(Box.nx, Box.py, Box.nz);

		glVertex3d(Box.px, Box.ny, Box.nz);
		glVertex3d(Box.px, Box.py, Box.nz);

		glVertex3d(Box.px, Box.ny, Box.pz);
		glVertex3d(Box.px, Box.py, Box.pz);

		glVertex3d(Box.nx, Box.ny, Box.pz);
		glVertex3d(Box.nx, Box.py, Box.pz);
	glEnd();

	glBegin(GL_LINE_STRIP);
		glVertex3d(Box.nx, Box.py, Box.nz);
		glVertex3d(0.5 * (Box.nx + Box.px), Box.ny, Box.nz);
		glVertex3d(Box.px, Box.py, Box.nz);
	glEnd();
}

void redraw_all(idocument& Document, const irender_engine::redraw_type_t RedrawType)
{
	std::for_each(Document.nodes().collection().begin(), Document.nodes().collection().end(), detail::redraw(RedrawType));
}

void setup_material(iunknown* const Material)
{
	k3d::gl::imaterial* result = &detail::g_null_material;

	if(Material)
		result = &detail::g_default_material;

	if(k3d::gl::imaterial* const material = dynamic_cast<k3d::gl::imaterial*>(Material))
	{
		result = material;
	}
	else if(k3d::imaterial* const material = dynamic_cast<k3d::imaterial*>(Material))
	{
		if(k3d::gl::imaterial* const gl_material = material->gl_material())
			result = gl_material;
	}

	result->setup_gl_material();
}

void tex_image_2d(const bitmap& Bitmap)
{
	assert_not_implemented();
/*
	k3d::basic_bitmap<k3d::basic_rgba<uint8_t> > buffer;
	
	if(extension::query("GL_ARB_texture_non_power_of_two"))
	{
		buffer = Bitmap;
	}
	else
	{
		unsigned long width = Bitmap.width();
		unsigned long height = Bitmap.height();

		if(width & (width - 1))
		{
			width |= (width >> 1);
			width |= (width >> 2);
			width |= (width >> 4);
			width |= (width >> 8);
			width |= (width >> 16);
			width += 1;
			width /= 2;
		}
		
		if(height & (height - 1))
		{
			height |= (height >> 1);
			height |= (height >> 2);
			height |= (height >> 4);
			height |= (height >> 8);
			height |= (height >> 16);
			height += 1;
			height /= 2;
		}
		
		if(width != Bitmap.width() || height != Bitmap.height())
		{
			k3d::log() << debug << "Forcing power-of-two texture" << std::endl;
			buffer.reset(width, height);
			scale_bitmap(Bitmap, buffer);
		}
		else
		{
			buffer = Bitmap;
		}
	}

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	glTexImage2D(
		GL_TEXTURE_2D,
		0,
		GL_RGBA,
		buffer.width(),
		buffer.height(),
		0,
		GL_RGBA,
		GL_UNSIGNED_BYTE,
		buffer.data());
*/
}

} // namespace gl

} // namespace k3d

