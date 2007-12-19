// K-3D
// Copyright (c) 1995-2005, Timothy M. Shead
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
	\author Tim Shead (tshead@k-3d.com)
*/

#include <k3d-i18n-config.h>
#include <k3dsdk/bitmap_sink.h>
#include <k3dsdk/document_plugin_factory.h>
#include <k3dsdk/node.h>
#include <k3dsdk/persistent.h>

#include <boost/gil/extension/io/png_io.hpp>

namespace module
{

namespace png
{

namespace io
{

/////////////////////////////////////////////////////////////////////////////
// bitmap_writer

class bitmap_writer :
	public k3d::bitmap_sink<k3d::persistent<k3d::node> >
{
	typedef k3d::bitmap_sink<k3d::persistent<k3d::node> > base;

public:
	bitmap_writer(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_file(init_owner(*this) + init_name("file") + init_label(_("File")) + init_description(_("Output file")) + init_value(k3d::filesystem::path()) + init_path_mode(k3d::ipath_property::WRITE) + init_path_type("png_files"))
	{
		m_file.changed_signal().connect(make_input_changed_slot());
	}

	void on_input_changed(k3d::iunknown*)
	{
		try
		{
			const k3d::filesystem::path file = m_file.pipeline_value();
			if(file.empty())
				return;

			k3d::bitmap* const bitmap = m_input_bitmap.pipeline_value();
			if(!bitmap)
				return;

			if(!bitmap->width())
				throw std::runtime_error("bitmap with zero width");

			if(!bitmap->height())
				throw std::runtime_error("bitmap with zero height");

			k3d::log() << info << "Writing " << file.native_console_string() << " using " << get_factory().name() << std::endl;
			boost::gil::png_write_view(file.native_filesystem_string(), boost::gil::color_converted_view<boost::gil::rgba8_pixel_t>(view(*bitmap)));
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

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<bitmap_writer,
			k3d::interface_list<k3d::ibitmap_sink > > factory(
				k3d::uuid(0x3bf6aa31, 0xb944432c, 0xca32a681, 0x8c6e07e8),
				"PNGBitmapWriter",
				_("Bitmap writer that saves PNG (*.png) files"),
				"Bitmap BitmapWriter",
				k3d::iplugin_factory::STABLE);

		return factory;
	}

private:
	k3d_data(k3d::filesystem::path, immutable_name, change_signal, with_undo, local_storage, no_constraint, path_property, path_serialization) m_file;
};

k3d::iplugin_factory& bitmap_writer_factory()
{
	return bitmap_writer::get_factory();
}

} // namespace io

} // namespace png

} // namespace module


