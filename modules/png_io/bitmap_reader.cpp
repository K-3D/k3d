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
// License along with this program; if not, read to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

/** \file
	\author Timothy M. Shead
*/

#include <k3d-i18n-config.h>
#include <k3dsdk/bitmap_source.h>
#include <k3dsdk/document_plugin_factory.h>
#include <k3dsdk/node.h>
#include <k3dsdk/options.h>
#include <k3dsdk/path.h>

#include <boost/gil/extension/io/png_io.hpp>

namespace module
{

namespace png
{

namespace io
{

/////////////////////////////////////////////////////////////////////////////
// bitmap_reader

class bitmap_reader :
	public k3d::node,
	public k3d::bitmap_source<bitmap_reader>
{
	typedef k3d::node base;

public:
	bitmap_reader(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_file(init_owner(*this) + init_name("file") + init_label(_("File")) + init_description(_("Browse for an input bitmap")) + init_value(k3d::filesystem::path()) + init_path_mode(k3d::ipath_property::READ) + init_path_type(k3d::options::path::bitmaps()))
	{
		m_file.changed_signal().connect(k3d::hint::converter<
			k3d::hint::convert<k3d::hint::any, k3d::hint::none> >(make_update_bitmap_slot()));
	}

	void on_resize_bitmap(k3d::bitmap& Bitmap)
	{
		const k3d::filesystem::path file = m_file.pipeline_value();
		if(file.empty())
			return;

		try
		{
			k3d::log() << info << "Reading " << file.native_console_string() << " using " << get_factory().name() << std::endl;
			boost::gil::png_read_and_convert_image(file.native_filesystem_string(), Bitmap);
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

	void on_assign_pixels(k3d::bitmap& Bitmap)
	{
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<bitmap_reader,
			k3d::interface_list<k3d::ibitmap_source> > factory(
				k3d::uuid(0x20fb9aae, 0x1e4fb977, 0xc4dbcfad, 0x9d66568e),
				"PNGBitmapReader",
				_("Loads a PNG (*.png) bitmap from the filesystem"),
				"Bitmap BitmapReader",
				k3d::iplugin_factory::STABLE);

		return factory;
	}

private:
	k3d_data(k3d::filesystem::path, immutable_name, change_signal, with_undo, local_storage, no_constraint, path_property, path_serialization) m_file;
};

/////////////////////////////////////////////////////////////////////////////
// bitmap_reader_factory

k3d::iplugin_factory& bitmap_reader_factory()
{
	return bitmap_reader::get_factory();
}

} // namespace io

} // namespace png

} // namespace module


