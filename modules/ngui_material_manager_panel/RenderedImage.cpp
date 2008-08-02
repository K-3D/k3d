#include "RenderedImage.h"

#include <iostream>
#include <k3dsdk/log.h>

namespace module
{
namespace ngui
{
namespace material_manager
{
namespace mechanics
{

//Static Variable Definitions
const k3d::string_t RenderedImage::m_holderImgFileName = "renderingShader.png";

RenderedImage::RenderedImage(k3d::filesystem::path _imgPath)
{
  setImgFilePath(_imgPath);

  //Remove Preview Render from tmp dir -> Required at uncertain close events occur
  k3d::filesystem::remove(m_imgFilePath);

  //Build Holder Image File Path (if no render preview file found)
  m_imgHolderPath = k3d::share_path() / k3d::filesystem::generic_path("ngui/rasterized") 
    / k3d::filesystem::generic_path(m_holderImgFileName);

}


RenderedImage::~RenderedImage()
{
  //Remove Preview Render from tmp dir
  k3d::filesystem::remove(m_imgFilePath);
}


bool RenderedImage::on_expose_event(GdkEventExpose* event)
{
	try
	{
		if(k3d::filesystem::exists(m_imgFilePath))
		{
				Glib::RefPtr<Gdk::Pixbuf> image = Gdk::Pixbuf::create_from_file(m_imgFilePath.native_filesystem_string());
				image->render_to_drawable(get_window(), get_style()->get_black_gc(),
																	0, 0, 10, 10, image->get_width(), image->get_height(),
																	Gdk::RGB_DITHER_NONE, 0, 0);
		}
		else
		{
			//No Image File. Display Default Image Holder
			Glib::RefPtr<Gdk::Pixbuf> image = Gdk::Pixbuf::create_from_file(m_imgHolderPath.native_filesystem_string());
			image->render_to_drawable(get_window(), get_style()->get_black_gc(),
																0, 0, 10, 10, image->get_width(), image->get_height(),
																Gdk::RGB_DITHER_NONE, 0, 0);
		}
	}
  catch(...)
	{

		//Should not hopefully Get Here!
	}

  return true;

}//on_expose_event



} //namespace mechanics

} //namespace material_manager

} //namespace ngui

} //namespace module

