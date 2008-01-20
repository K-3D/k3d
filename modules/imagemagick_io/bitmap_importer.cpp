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
	\author Brett W. McCoy (bmccoy@chapelperilous.net)
	\author Timothy M. Shead (tshead@k-3d.com)
*/

#include <k3d-i18n-config.h>
#include <k3dsdk/application_plugin_factory.h>
#include <k3dsdk/ibitmap_importer.h>
#include <k3dsdk/ideletable.h>
#include <k3dsdk/ifile_format.h>
#include <k3dsdk/path.h>

#include <Magick++.h>

#include <boost/assign/list_of.hpp>

namespace module
{

namespace imagemagick
{

namespace io
{

/////////////////////////////////////////////////////////////////////////////
// bitmap_importer

/// Reads a wide variety of image file formats using the ImageMagick C++ bindings
class bitmap_importer :
	public k3d::ifile_format,
	public k3d::ibitmap_importer,
	public k3d::ideletable
{
public:
	bitmap_importer()
	{
	}

	bool read_file(const k3d::filesystem::path& File, k3d::bitmap& Bitmap)
	{
		try
		{
			k3d::log() << info << "Reading " << File.native_console_string() << " using " << get_factory().name() << std::endl;

			Magick::Image image;
			image.read(File.native_filesystem_string());
			
			const unsigned long width = image.columns();
			const unsigned long height = image.rows();
			
			return_val_if_fail(width, false);
			return_val_if_fail(height, false);

			Bitmap.recreate(width, height);

			Magick::ColorRGB color_rgb;
			k3d::bitmap::view_t::iterator destination = view(Bitmap).begin();
			
			for(unsigned long y = 0; y < height; y++) 
			{
				for(unsigned long x = 0; x < width; x++) 
				{
					color_rgb = image.pixelColor(x, y);

					get_color(*destination, boost::gil::red_t()) = color_rgb.red();
					get_color(*destination, boost::gil::green_t()) = color_rgb.green();
					get_color(*destination, boost::gil::blue_t()) = color_rgb.blue();
					get_color(*destination, boost::gil::alpha_t()) = 1.0 - color_rgb.alpha();
					
					++destination;
				}
			}

			return true;
		}
		catch(Magick::Exception& e)
		{
			k3d::log() << error << k3d_file_reference << ": caught exception: " << e.what() << std::endl;
			return false;
		}
		catch(std::exception& e)
		{
			k3d::log() << error << k3d_file_reference << ": caught exception: " << e.what() << std::endl;
			return false;
		}
		catch(...)
		{
			k3d::log() << error << k3d_file_reference << ": caught unknown exception" << std::endl;
			return false;
		}

	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::application_plugin_factory<bitmap_importer, 
			k3d::interface_list<k3d::ibitmap_importer> > factory(
				k3d::uuid(0x4eb70e35, 0x4e654a53, 0xa8e4b07c, 0x4219d946),
				"ImageMagickBitmapImporter",
				_("ImageMagick Formats ( many )"),
				"Bitmap BitmapImporter",
				k3d::iplugin_factory::STABLE,
				boost::assign::map_list_of("k3d:load-order", "128")("k3d:mime-types", "image/bmp image/x-sun-raster"));
 
		return factory;
	}
};

/////////////////////////////////////////////////////////////////////////////
// bitmap_importer_factory

k3d::iplugin_factory& bitmap_importer_factory()
{
	return bitmap_importer::get_factory();
}

} // namespace io

} // namespace imagemagick

} // namespace module


