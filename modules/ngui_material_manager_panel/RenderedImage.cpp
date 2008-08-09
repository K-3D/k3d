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
const k3d::string_t RenderedImage::m_pview_bg_filename = "material_pview_bg.png";



RenderedImage::RenderedImage(k3d::filesystem::path _imgPath)
{
  setImgFilePath(_imgPath);

  //Remove Preview Render from tmp dir -> Required at uncertain close events occur
  k3d::filesystem::remove(m_imgFilePath);

  //Build Holder Image File Path (if no render preview file found)
  m_imgHolderPath = k3d::share_path() / k3d::filesystem::generic_path("ngui/rasterized") 
    / k3d::filesystem::generic_path(m_holderImgFileName);


  //Build Pview Background Image Path
  m_pview_bg_path = k3d::share_path() / k3d::filesystem::generic_path("ngui/rasterized") 
    / k3d::filesystem::generic_path(m_pview_bg_filename);

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
     //Place The Background Image In First Layer
     if(k3d::filesystem::exists(m_pview_bg_path))
       {
         //This Variable Is Temporary
         const int image_dimension = 200;

         //Tile The Bckground Image
         for(int x_tile = 10; x_tile < image_dimension; x_tile += 100)
           {
             for(int y_tile = 10; y_tile < image_dimension; y_tile += 100)
               {
                 Glib::RefPtr<Gdk::Pixbuf> bg_image = Gdk::Pixbuf::create_from_file(m_pview_bg_path.native_filesystem_string());
                 bg_image->render_to_drawable(get_window(), get_style()->get_black_gc(),
                                              0, 0, x_tile, y_tile, bg_image->get_width(), bg_image->get_height(),
                                              Gdk::RGB_DITHER_NONE, 0, 0);

               }
           }
       }//if


     //Place The Render Preview On Second Layer. (Background Will Display Only If This Has Alpha
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

