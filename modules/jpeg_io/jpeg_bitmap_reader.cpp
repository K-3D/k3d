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

#include <k3dsdk/bitmap_source.h>
#include <k3dsdk/document_plugin_factory.h>
#include <k3d-i18n-config.h>
#include <k3dsdk/node.h>
#include <k3dsdk/options.h>
#include <k3dsdk/path.h>
#include <k3dsdk/persistent.h>

#include <boost/gil/extension/io/jpeg_io.hpp>

namespace libk3djpegio
{

/////////////////////////////////////////////////////////////////////////////
// jpeg_bitmap_reader

class jpeg_bitmap_reader :
	public k3d::bitmap_source<k3d::persistent<k3d::node> >
{
	typedef k3d::bitmap_source<k3d::persistent<k3d::node> > base;

public:
	jpeg_bitmap_reader(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_file(init_owner(*this) + init_name("file") + init_label(_("File")) + init_description(_("Browse for an input bitmap")) + init_value(k3d::filesystem::path()) + init_path_mode(k3d::ipath_property::READ) + init_path_type(k3d::options::path::bitmaps()))
	{
		m_file.changed_signal().connect(make_reset_bitmap_slot());
	}

	void on_create_bitmap(k3d::bitmap& Bitmap)
	{
		const k3d::filesystem::path file = m_file.pipeline_value();
		if(file.empty())
			return;

		try
		{
			k3d::log() << info << "Reading " << file.native_console_string() << " using " << get_factory().name() << std::endl;
			boost::gil::jpeg_read_and_convert_image(file.native_filesystem_string(), Bitmap);
		}
		catch(std::exception& e)
		{
			k3d::log() << error << k3d_file_reference << ": caught exception: " << e.what() << std::endl;
		}
		catch(...)
		{
			k3d::log() << error << k3d_file_reference << ": caught unknown exception" << std::endl;
		}
	}

	void on_update_bitmap(k3d::bitmap& Bitmap)
	{
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<jpeg_bitmap_reader,
			k3d::interface_list<k3d::ibitmap_source> > factory(
				k3d::uuid(0xfbee383b, 0x8443128c, 0x6c8b0ba9, 0xda52ceba),
				"JPEGBitmapReader",
				_("Loads a JPEG (*.jpeg) bitmap from the filesystem"),
				"Bitmap BitmapReader",
				k3d::iplugin_factory::STABLE);

		return factory;
	}

private:
	k3d_data(k3d::filesystem::path, immutable_name, change_signal, with_undo, local_storage, no_constraint, path_property, path_serialization) m_file;
};

/////////////////////////////////////////////////////////////////////////////
// jpeg_bitmap_reader_factory

k3d::iplugin_factory& jpeg_bitmap_reader_factory()
{
	return jpeg_bitmap_reader::get_factory();
}

} // namespace libk3djpegio

