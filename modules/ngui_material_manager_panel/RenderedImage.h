#ifndef RENDEREDIMAGE_H
#define RENDEREDIMAGE_H

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
//
// ---------------------
//

#include <gtkmm/drawingarea.h>

#include <k3dsdk/share.h>
#include <k3dsdk/system.h>

namespace module
{
namespace ngui
{
namespace material_manager
{
namespace mechanics
{

const k3d::string_t holderImgFile  = "renderingShader.png";

class RenderedImage: public Gtk::DrawingArea
{
 public:

  RenderedImage(k3d::filesystem::path _imgPath);

  ~RenderedImage();
	  
 public:

  //Preview Reqest Signal Handler
  bool onUpdatePreview();

  //Variable Accessors
  k3d::filesystem::path imgFilePath()			{return m_imgFilePath;}
  k3d::filesystem::path imgHolderPath()		{return m_imgHolderPath;}

  //Variable Mutators
  void setImgFilePath(const k3d::filesystem::path &p)		{m_imgFilePath = p;}
  void setImgHolderPath(const k3d::filesystem::path &p)	{m_imgHolderPath = p;}

 public:

  k3d::filesystem::path m_imgFilePath;
  k3d::filesystem::path m_imgHolderPath;

  static const k3d::string_t m_holderImgFileName;

 protected:

  //Override Deafult Signal Handler For Draw
  virtual bool on_expose_event(GdkEventExpose* event);


};//RenderedImage


/* //Static Variable Definitions */
/* const k3d::string_t RenderedImage::m_holderImgFileName = "renderingShader.png"; */

/* RenderedImage::RenderedImage(k3d::filesystem::path _imgPath) */
/* { */
/*   setImgFilePath(_imgPath); */

/*   //Remove Preview Render from tmp dir -> Required at uncertain close events occur */
/*   k3d::filesystem::remove(m_imgFilePath); */

/*   //Build Holder Image File Path (if no render preview file found) */
/*   m_imgHolderPath = k3d::share_path() / k3d::filesystem::generic_path("ngui/rasterized") */
/*     / k3d::filesystem::generic_path(m_holderImgFileName); */
/* } */


/* RenderedImage::~RenderedImage() */
/* { */
/*   //Remove Preview Render from tmp dir */
/*   k3d::filesystem::remove(m_imgFilePath); */
/* } */


/* bool RenderedImage::on_expose_event(GdkEventExpose* event) */
/* { */
/*   try */
/*     { */
/*       Glib::RefPtr<Gdk::Pixbuf> image = Gdk::Pixbuf::create_from_file(m_imgFilePath.native_filesystem_string()); */
/*       image->render_to_drawable(get_window(), get_style()->get_black_gc(), */
/*                                 0, 0, 10, 10, image->get_width(), image->get_height(), */
/*                                 Gdk::RGB_DITHER_NONE, 0, 0); */
/*     } */
/*   catch(Glib::FileError) */
/*     { */
/*       //No Image File. Display Default Image Holder */
/*       Glib::RefPtr<Gdk::Pixbuf> image = Gdk::Pixbuf::create_from_file(m_imgHolderPath.native_filesystem_string()); */
/*       image->render_to_drawable(get_window(), get_style()->get_black_gc(), */
/*                                 0, 0, 10, 10, image->get_width(), image->get_height(), */
/*                                 Gdk::RGB_DITHER_NONE, 0, 0); */
/*     } */
/*   catch(Glib::Error) */
/*     { */

/*       //Should not hopefully Get Here! */
/*     } */

/*   return true; */
/* }//on_expose_event */


} //namespace mechanics

} //namespace material_manager

} //namespace ngui

} //namespace module

#endif
