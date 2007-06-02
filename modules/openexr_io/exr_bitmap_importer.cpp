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
	\author Anders Dahnielson (anders@dahnielson.com)
*/

#include <k3dsdk/application_plugin_factory.h>
#include <k3dsdk/i18n.h>
#include <k3dsdk/ibitmap_read_format.h>
#include <k3dsdk/ideletable.h>
#include <k3dsdk/ifile_format.h>
#include <k3dsdk/path.h>

#include <ImfInputFile.h>

namespace libk3dopenexrio
{

/////////////////////////////////////////////////////////////////////////////
// exr_bitmap_importer

class exr_bitmap_importer :
	public k3d::ifile_format,
	public k3d::ibitmap_read_format,
	public k3d::ideletable
{
public:
	exr_bitmap_importer()
	{
	}

	unsigned long priority()
	{
		return 128;
	}

	bool query_can_handle(const k3d::filesystem::path& File)
	{
		// See if we can read it ...
		try
		{
			Imf::InputFile file(File.native_filesystem_string().c_str());
		}
		catch(const std::exception& e)
		{
			return false;
		}

		return true;
	}

	bool read_file(const k3d::filesystem::path& File, k3d::bitmap& Bitmap)
	{
		try 
		{
			k3d::log() << info << "Reading " << File.native_console_string() << " using " << get_factory().name() << std::endl;

 			Imf::InputFile file(File.native_filesystem_string().c_str());

 			Imath::Box2i dw = file.header().dataWindow();
   			const int width  = dw.max.x - dw.min.x + 1;
   			const int height = dw.max.y - dw.min.y + 1;
	
			Bitmap.recreate(width, height);

  			Imf::FrameBuffer frameBuffer;

			frameBuffer.insert("R",
					   Imf::Slice(Imf::HALF,
						      (char *) &view(Bitmap)[0] + 0 * sizeof(half),
						      sizeof(half) * 4,
						      sizeof(half) * 4 * width,
						      1, 1,
						      0.0));
			
			frameBuffer.insert("G",
					   Imf::Slice(Imf::HALF,
						      (char *) &view(Bitmap)[0] + 1 * sizeof(half),
						      sizeof(half) * 4,
						      sizeof(half) * 4 * width,
						      1, 1,
						      0.0));
			
			frameBuffer.insert("B",
					   Imf::Slice(Imf::HALF,
						      (char *) &view(Bitmap)[0] + 2 * sizeof(half),
						      sizeof(half) * 4,
						      sizeof(half) * 4 * width,
						      1, 1,
						      0.0));
			
			frameBuffer.insert("A",
					   Imf::Slice(Imf::HALF,
						      (char *) &view(Bitmap)[0] + 3 * sizeof(half),
						      sizeof(half) * 4,
						      sizeof(half) * 4 * width,
						      1, 1,
						      0.0));

			file.setFrameBuffer(frameBuffer);
 			file.readPixels(dw.min.y, dw.max.y);
			return true;
		}
		catch(const std::exception& e)
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
		static k3d::application_plugin_factory<exr_bitmap_importer,
			k3d::interface_list<k3d::ibitmap_read_format> > factory(
				k3d::uuid(0xdb5255b7, 0xb9c243a4, 0x81eb2645, 0x1d80ecac),
				"EXRBitmapImporter",
				_("OpenEXR (*.exr)"),
				"Bitmap BitmapImporter");

		return factory;
	}
};

/////////////////////////////////////////////////////////////////////////////
// exr_bitmap_importer_factory

k3d::iplugin_factory& exr_bitmap_importer_factory()
{
	return exr_bitmap_importer::get_factory();
}

} // namespace libk3dopenexrio

