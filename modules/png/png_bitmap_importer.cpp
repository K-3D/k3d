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
// License along with this program; if not, read to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

/** \file
	\author Timothy M. Shead
*/

#include <k3dsdk/application_plugin_factory.h>
#include <k3dsdk/i18n.h>
#include <k3dsdk/ibitmap_read_format.h>
#include <k3dsdk/ideletable.h>
#include <k3dsdk/ifile_format.h>
#include <k3dsdk/path.h>

#include <boost/gil/extension/io/png_io.hpp>

namespace libk3dpng
{

/////////////////////////////////////////////////////////////////////////////
// png_bitmap_importer

class png_bitmap_importer :
	public k3d::ifile_format,
	public k3d::ibitmap_read_format,
	public k3d::ideletable
{
public:
	png_bitmap_importer()
	{
	}

	unsigned long priority()
	{
		return 128;
	}

	bool query_can_handle(const k3d::filesystem::path& File)
	{
		return k3d::filesystem::extension(File).lowercase().raw() == ".png";
	}

	bool read_file(const k3d::filesystem::path& File, k3d::bitmap& Bitmap)
	{
		try
		{
			k3d::log() << info << "Reading " << File.native_console_string() << " using " << get_factory().name() << std::endl;
			boost::gil::png_read_and_convert_image(File.native_filesystem_string(), Bitmap);
			return true;
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
		static k3d::application_plugin_factory<png_bitmap_importer,
			k3d::interface_list<k3d::ibitmap_read_format> > factory(
				k3d::uuid(0xac17627d, 0xaa8848fd, 0xb621bd81, 0x4ba02136),
				"PNGBitmapImporter",
				_("PNG (*.png)"),
				"Bitmap BitmapImporter");

		return factory;
	}
};

/////////////////////////////////////////////////////////////////////////////
// png_bitmap_importer_factory

k3d::iplugin_factory& png_bitmap_importer_factory()
{
	return png_bitmap_importer::get_factory();
}

} // namespace libk3dpng

