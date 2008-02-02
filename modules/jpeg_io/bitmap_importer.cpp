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

#include <k3d-i18n-config.h>
#include <k3dsdk/application_plugin_factory.h>
#include <k3dsdk/ibitmap_importer.h>
#include <k3dsdk/path.h>
#include <k3dsdk/string_modifiers.h>

#include <boost/assign/list_of.hpp>
#include <boost/gil/extension/io/jpeg_io.hpp>

namespace module
{

namespace jpeg
{

namespace io
{

/////////////////////////////////////////////////////////////////////////////
// bitmap_importer

class bitmap_importer :
	public k3d::ibitmap_importer
{
public:
	bitmap_importer()
	{
	}

	bool read_file(const k3d::filesystem::path& Path, k3d::bitmap& Bitmap)
	{
		try
		{
			k3d::log() << info << "Reading " << Path.native_console_string() << " using " << get_factory().name() << std::endl;
			boost::gil::jpeg_read_and_convert_image(Path.native_filesystem_string(), Bitmap);
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
		static k3d::application_plugin_factory<bitmap_importer,
			k3d::interface_list<k3d::ibitmap_importer> > factory(
				k3d::uuid(0xfb924031, 0x25c242af, 0xa2e1398e, 0x35000e3c),
				"JPEGBitmapImporter",
				_("JPEG (*.jpeg)"),
				"Bitmap BitmapImporter",
				k3d::iplugin_factory::STABLE,
				boost::assign::map_list_of("k3d:load-order", "8")("k3d:mime-types", "image/jpeg"));

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

} // namespace jpeg

} // namespace module

