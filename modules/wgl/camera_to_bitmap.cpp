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
#include <k3dsdk/bitmap_source.h>
#include <k3dsdk/document_plugin_factory.h>
#include <k3dsdk/gl.h>
#include <k3dsdk/icamera.h>
#include <k3dsdk/irender_viewport_gl.h>
#include <k3dsdk/measurement.h>
#include <k3dsdk/node.h>
#include <k3dsdk/persistent.h>
#include <k3dsdk/win32.h>

#include <iterator>

namespace module
{

namespace wgl
{

class CGLBitmapContext
{
protected:
	BITMAPINFO m_bmi;
	HBITMAP m_hBitmap;
	void *m_pBits;
	HDC m_hDC, m_hOldDC;
	HGLRC m_hGLRC, m_hOldGLRC;

public:
	CGLBitmapContext()
	{
	}

	int GetWidth()
	{
		return m_bmi.bmiHeader.biWidth; 
	}

	int GetHeight() 
	{ 
		return m_bmi.bmiHeader.biHeight; 
	}

	bool ChoosePixelFormat()
	{
		int nMaxWeight = 0;
		int nBestFormat = 0;
		int nPixelFormat = 1;
		PIXELFORMATDESCRIPTOR pfdDesc;
		DWORD dwRequiredFlags = PFD_DRAW_TO_BITMAP | PFD_GENERIC_FORMAT | PFD_SUPPORT_OPENGL | PFD_SUPPORT_GDI;
		DWORD dwRejectedFlags = PFD_GENERIC_ACCELERATED;
		while(DescribePixelFormat(m_hDC, nPixelFormat, sizeof(PIXELFORMATDESCRIPTOR), &pfdDesc))
		{
			if(pfdDesc.cColorBits == 24 && pfdDesc.iLayerType == PFD_TYPE_RGBA && (pfdDesc.dwFlags & dwRequiredFlags) == dwRequiredFlags && (pfdDesc.dwFlags & dwRejectedFlags) == 0)
			{
				int nWeight = pfdDesc.cColorBits * 8 + pfdDesc.cDepthBits * 4 + pfdDesc.cStencilBits + pfdDesc.cAccumBits;
				if(nWeight > nMaxWeight)
				{
					nMaxWeight = nWeight;
					nBestFormat = nPixelFormat;
				}
			}
			nPixelFormat++;
		}
		if(nBestFormat && ::SetPixelFormat(m_hDC, nBestFormat, &pfdDesc))
			return true;

		return false;
	}

	bool Init(HDC hWindowDC, int nWidth, int nHeight)
	{
		// Create the bitmap for this OpenGL context
		memset(&m_bmi, 0, sizeof(BITMAPINFO));
		m_bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		m_bmi.bmiHeader.biWidth = nWidth;
		m_bmi.bmiHeader.biHeight = nHeight;
		m_bmi.bmiHeader.biPlanes = 1;
		m_bmi.bmiHeader.biBitCount = 24;
		m_bmi.bmiHeader.biCompression = BI_RGB;
		m_bmi.bmiHeader.biSizeImage = nWidth * nHeight * 3;
		m_hBitmap = ::CreateDIBSection(hWindowDC, &m_bmi, DIB_RGB_COLORS, &m_pBits, NULL, (DWORD)0);

		// Create the memory DC for the OpenGL context
		m_hDC = ::CreateCompatibleDC(NULL);
		::SelectObject(m_hDC, ::GetCurrentObject(hWindowDC, OBJ_FONT));
		::SelectObject(m_hDC, m_hBitmap);

		// Select a software-rendering pixel format that supports drawing to a bitmap
		// (Must be 24-bit color because the bitmap is 24-bit color)
		if(!ChoosePixelFormat())
			return false;

		// Initialize the OpenGL context
		m_hGLRC = ::wglCreateContext(m_hDC);
		m_hOldDC = ::wglGetCurrentDC();
		m_hOldGLRC = ::wglGetCurrentContext();
		::wglMakeCurrent(m_hDC, m_hGLRC);

/*
		// Initialize the CGLFont object and set up the viewport
		glViewport(0, 0, nWidth, nHeight);
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluOrtho2D(0, nWidth, 0, nHeight);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
*/
		return true;
	}

	void Cleanup()
	{
		::wglMakeCurrent(NULL, NULL);
		::wglDeleteContext(m_hGLRC);
		::wglMakeCurrent(m_hOldDC, m_hOldGLRC);
		::DeleteObject(m_hBitmap);
		::DeleteDC(m_hDC);
	}
};

/////////////////////////////////////////////////////////////////////////////
// camera_to_bitmap

class camera_to_bitmap :
	public k3d::bitmap_source<k3d::persistent<k3d::node> >
{
	typedef k3d::bitmap_source<k3d::persistent<k3d::node> > base;

public:
	camera_to_bitmap(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_camera(init_owner(*this) + init_name("camera") + init_label(_("Camera")) + init_description(_("Camera")) + init_value<k3d::icamera*>(0)),
		m_render_engine(init_owner(*this) + init_name("render_engine") + init_label(_("Render Engine")) + init_description(_("Render Engine")) + init_value<k3d::gl::irender_viewport*>(0)),
		m_width(init_owner(*this) + init_name("width") + init_label(_("Width")) + init_description(_("Bitmap width")) + init_value(256L) + init_step_increment(1) + init_units(typeid(k3d::measurement::scalar)) + init_constraint(constraint::minimum(1L))),
		m_height(init_owner(*this) + init_name("height") + init_label(_("Height")) + init_description(_("Bitmap height")) + init_value(256L) + init_step_increment(1) + init_units(typeid(k3d::measurement::scalar)) + init_constraint(constraint::minimum(1L)))
	{
		m_camera.changed_signal().connect(make_reset_bitmap_slot());
		m_render_engine.changed_signal().connect(make_reset_bitmap_slot());
		m_width.changed_signal().connect(make_reset_bitmap_slot());
		m_height.changed_signal().connect(make_reset_bitmap_slot());
	}

	void on_create_bitmap(k3d::bitmap& Bitmap)
	{
		const k3d::pixel_size_t width = m_width.pipeline_value();
		const k3d::pixel_size_t height = m_height.pipeline_value();
		Bitmap.recreate(width, height);
	}

	void on_update_bitmap(k3d::bitmap& Bitmap)
	{
		k3d::icamera* const camera = m_camera.pipeline_value();
		k3d::gl::irender_viewport* const render_engine = m_render_engine.pipeline_value();
		const k3d::pixel_size_t width = m_width.pipeline_value();
		const k3d::pixel_size_t height = m_height.pipeline_value();

		if(camera && render_engine)
		{
			CGLBitmapContext voodoo;
			return_if_fail(voodoo.Init(0, width, height));
	
			glViewport(0, 0, width, height);

			unsigned long font_begin = 0;
			GLdouble gl_view_matrix[16];
			GLdouble gl_projection_matrix[16];
			GLint gl_viewport[4];

			render_engine->render_viewport(*camera, width, height, font_begin, gl_view_matrix, gl_projection_matrix, gl_viewport);
			glFlush();

			boost::gil::image<boost::gil::rgba8_pixel_t, false> buffer(width, height);
			glPixelStorei(GL_PACK_SWAP_BYTES, GL_FALSE);
			glPixelStorei(GL_PACK_LSB_FIRST, GL_FALSE);
			glPixelStorei(GL_PACK_ROW_LENGTH, 0);
			glPixelStorei(GL_PACK_SKIP_ROWS, 0);
			glPixelStorei(GL_PACK_SKIP_PIXELS, 0);
			glPixelStorei(GL_PACK_ALIGNMENT, 1);
			glReadPixels(0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, &view(buffer)[0]);

			boost::gil::copy_pixels(boost::gil::flipped_up_down_view(boost::gil::color_converted_view<boost::gil::rgba16f_pixel_t>(view(buffer))), view(Bitmap));

			voodoo.Cleanup();
		}
		else
		{
			std::fill(view(Bitmap).begin(), view(Bitmap).end(), k3d::pixel(0.6, 0.6, 0.6, 1.0));
		}
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<camera_to_bitmap,
			k3d::interface_list<k3d::ibitmap_source> > factory(
				k3d::uuid(0x98fe4606, 0x404f2e76, 0xa8836097, 0x26b6be8a),
				"WGLCameraToBitmap",
				_("Renders the scene to a bitmap using WGL"),
				"Bitmap Test",
				k3d::iplugin_factory::EXPERIMENTAL);

		return factory;
	}

private:
	k3d_data(k3d::icamera*, immutable_name, change_signal, with_undo, node_storage, no_constraint, node_property, node_serialization) m_camera;
	k3d_data(k3d::gl::irender_viewport*, immutable_name, change_signal, with_undo, node_storage, no_constraint, node_property, node_serialization) m_render_engine;
	k3d_data(long, immutable_name, change_signal, with_undo, local_storage, with_constraint, measurement_property, with_serialization) m_width;
	k3d_data(long, immutable_name, change_signal, with_undo, local_storage, with_constraint, measurement_property, with_serialization) m_height;
};

/////////////////////////////////////////////////////////////////////////////
// camera_to_bitmap_factory

k3d::iplugin_factory& camera_to_bitmap_factory()
{
	return camera_to_bitmap::get_factory();
}

} // namespace wgl

} // namespace module

