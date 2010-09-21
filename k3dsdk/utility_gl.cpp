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

#include <k3dsdk/algebra.h>
#include <k3dsdk/bounding_box3.h>
//#include <k3dsdk/gl/extension.h>
#include <k3dsdk/icamera.h>
#include <k3dsdk/idocument.h>
#include <k3dsdk/imaterial.h>
#include <k3dsdk/inode.h>
#include <k3dsdk/inode_collection.h>
#include <k3dsdk/iprojection.h>
#include <k3dsdk/irender_viewport_gl.h>
#include <k3dsdk/material.h>
#include <k3dsdk/plane.h>
#include <k3dsdk/rectangle.h>
#include <k3dsdk/utility_gl.h>

#include <boost/gil/extension/numeric/resample.hpp>
#include <boost/gil/extension/numeric/sampler.hpp>

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
	redraw(const irender_viewport::redraw_type_t RedrawType) :
		m_redraw_type(RedrawType)
	{
	}

	void operator()(inode* const Object)
	{
		if(irender_viewport* const render_engine = dynamic_cast<irender_viewport*>(Object))
			render_engine->redraw_request_signal().emit(m_redraw_type);
	}

protected:
	const irender_viewport::redraw_type_t m_redraw_type;
};

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

void redraw_all(idocument& Document, const irender_viewport::redraw_type_t RedrawType)
{
	std::for_each(Document.nodes().collection().begin(), Document.nodes().collection().end(), detail::redraw(RedrawType));
}

void tex_image_2d(const bitmap& Source)
{
	// Compute power-of-two dimensions ...
	unsigned long width = Source.width();
	unsigned long height = Source.height();

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

	boost::gil::rgba8_image_t target(0, 0);
	assert_not_implemented();
/*
	if(extension::query("GL_ARB_texture_non_power_of_two") || (width == Source.width() && height == Source.height()))
	{
		target.recreate(Source.width(), Source.height());
		boost::gil::copy_and_convert_pixels(boost::gil::const_view(Source), boost::gil::view(target));
	}
	else
*/
	{
		target.recreate(width, height);
		boost::gil::resize_view(boost::gil::color_converted_view<boost::gil::rgba8_pixel_t>(boost::gil::const_view(Source)), boost::gil::view(target), boost::gil::bilinear_sampler());
	}

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexImage2D(
		GL_TEXTURE_2D,
		0,
		GL_RGBA,
		target.width(),
		target.height(),
		0,
		GL_RGBA,
		GL_UNSIGNED_BYTE,
		&boost::gil::view(target)[0]);
}

const point3 project(const point3& WorldCoords)
{
	k3d::point3 result;
	
	GLdouble modelview[16];
	glGetDoublev(GL_MODELVIEW_MATRIX, modelview);
	
	GLdouble projection[16];
	glGetDoublev(GL_PROJECTION_MATRIX, projection);
	
	GLint viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);

	GLint error = gluProject(
		WorldCoords[0],
		WorldCoords[1],
		WorldCoords[2],
		modelview,
		projection,
		viewport,
		&result[0],
		&result[1],
		&result[2]);
	
	return_val_if_fail(error, result);

	return result;
}

void calculate_projection(icamera& Camera, const uint_t PixelWidth, const uint_t PixelHeight, rectangle& WindowRect, rectangle& CameraRect, double& Near, double& Far, bool& Orthographic)
{
	return_if_fail(PixelWidth && PixelHeight);

	if(k3d::iperspective* const perspective = dynamic_cast<k3d::iperspective*>(&Camera.projection()))
	{
		Orthographic = false;

		CameraRect.x1 = k3d::property::pipeline_value<double>(perspective->left());
		CameraRect.x2 = k3d::property::pipeline_value<double>(perspective->right());
		if(CameraRect.x2 < CameraRect.x1)
			std::swap(CameraRect.x1, CameraRect.x2);

		CameraRect.y1 = k3d::property::pipeline_value<double>(perspective->top());
		CameraRect.y2 = k3d::property::pipeline_value<double>(perspective->bottom());
		if(CameraRect.y1 < CameraRect.y2)
			std::swap(CameraRect.y1, CameraRect.y2);

		Near = k3d::property::pipeline_value<double>(perspective->near());
		Far = k3d::property::pipeline_value<double>(perspective->far());

		return_if_fail(CameraRect.x1 != CameraRect.x2 && CameraRect.y1 != CameraRect.y2);

		const double frustum_ratio = (CameraRect.x2 - CameraRect.x1) / (CameraRect.y1 - CameraRect.y2);
		const double raster_ratio = static_cast<double>(PixelWidth) / static_cast<double>(PixelHeight);

		if(raster_ratio > frustum_ratio)
		{
			const double width = 0.5 * raster_ratio * CameraRect.height();

			WindowRect.x1 = ((CameraRect.x1 + CameraRect.x2) * 0.5) - width;
			WindowRect.x2 = ((CameraRect.x1 + CameraRect.x2) * 0.5) + width;
			WindowRect.y1 = CameraRect.y1;
			WindowRect.y2 = CameraRect.y2;
		}
		else
		{
			const double height = 0.5 * CameraRect.width() / raster_ratio;

			WindowRect.x1 = CameraRect.x1;
			WindowRect.x2 = CameraRect.x2;
			WindowRect.y1 = ((CameraRect.y1 + CameraRect.y2) * 0.5) + height;
			WindowRect.y2 = ((CameraRect.y1 + CameraRect.y2) * 0.5) - height;
		}

		return;
	}
	else if(k3d::iorthographic* const orthographic = dynamic_cast<k3d::iorthographic*>(&Camera.projection()))
	{
		Orthographic = true;

		CameraRect.x1 = k3d::property::pipeline_value<double>(orthographic->left());
		CameraRect.x2 = k3d::property::pipeline_value<double>(orthographic->right());
		if(CameraRect.x2 < CameraRect.x1)
			std::swap(CameraRect.x1, CameraRect.x2);

		CameraRect.y1 = k3d::property::pipeline_value<double>(orthographic->top());
		CameraRect.y2 = k3d::property::pipeline_value<double>(orthographic->bottom());
		if(CameraRect.y1 < CameraRect.y2)
			std::swap(CameraRect.y1, CameraRect.y2);

		Near = k3d::property::pipeline_value<double>(orthographic->near());
		Far = k3d::property::pipeline_value<double>(orthographic->far());

		return_if_fail(CameraRect.x1 != CameraRect.x2 && CameraRect.y1 != CameraRect.y2);

		const double frustum_ratio = (CameraRect.x2 - CameraRect.x1) / (CameraRect.y1 - CameraRect.y2);
		const double raster_ratio = static_cast<double>(PixelWidth) / static_cast<double>(PixelHeight);

		if(raster_ratio > frustum_ratio)
		{
			const double width = 0.5 * raster_ratio * CameraRect.height();

			WindowRect.x1 = ((CameraRect.x1 + CameraRect.x2) * 0.5) - width;
			WindowRect.x2 = ((CameraRect.x1 + CameraRect.x2) * 0.5) + width;
			WindowRect.y1 = CameraRect.y1;
			WindowRect.y2 = CameraRect.y2;
		}
		else
		{
			const double height = 0.5 * CameraRect.width() / raster_ratio;

			WindowRect.x1 = CameraRect.x1;
			WindowRect.x2 = CameraRect.x2;
			WindowRect.y1 = ((CameraRect.y1 + CameraRect.y2) * 0.5) + height;
			WindowRect.y2 = ((CameraRect.y1 + CameraRect.y2) * 0.5) - height;
		}

		return;
	}

	k3d::log() << error << k3d_file_reference << ": unknown projection type" << std::endl;
}


} // namespace gl

} // namespace k3d

