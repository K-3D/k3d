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
#include <k3dsdk/classes.h>
#include <k3dsdk/i18n.h>
#include <k3dsdk/ibitmap_exporter.h>
#include <k3dsdk/ideletable.h>
#include <k3dsdk/ifile_format.h>
#include <k3dsdk/path.h>
#include <k3dsdk/string_modifiers.h>

#include <boost/gil/extension/io/tiff_io.hpp>

namespace libk3dtiffio
{

/////////////////////////////////////////////////////////////////////////////
// tiff_bitmap_exporter

class tiff_bitmap_exporter :
	public k3d::ifile_format,
	public k3d::ibitmap_exporter,
	public k3d::ideletable
{
public:
	tiff_bitmap_exporter()
	{
	}

	unsigned long priority()
	{
		return 128;
	}

	bool query_can_handle(const k3d::filesystem::path& Path)
	{
		return k3d::filesystem::extension(Path).lowercase().raw() == ".tiff" || k3d::filesystem::extension(Path).lowercase().raw() == ".tif";
	}

	bool write_file(const k3d::filesystem::path& Path, const k3d::bitmap& Bitmap)
	{
		try
		{
			k3d::log() << info << "Writing " << Path.native_console_string() << " using " << get_factory().name() << std::endl;
			boost::gil::tiff_write_view(Path.native_filesystem_string(), boost::gil::color_converted_view<boost::gil::rgb8_pixel_t>(const_view(Bitmap)));
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
		static k3d::application_plugin_factory<tiff_bitmap_exporter,
			k3d::interface_list<k3d::ibitmap_exporter> > factory(
				k3d::classes::TIFFBitmapExporter(),
				"TIFFBitmapExporter",
				_("TIFF (*.tiff)"),
				"Bitmap BitmapExporter");

		return factory;
	}
};

/////////////////////////////////////////////////////////////////////////////
// tiff_bitmap_exporter_factory

k3d::iplugin_factory& tiff_bitmap_exporter_factory()
{
	return tiff_bitmap_exporter::get_factory();
}

} // namespace libk3dtiffio

