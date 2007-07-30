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
		\author Timothy M. Shead (tshead@k-3d.com)
*/

#include <k3dsdk/auto_ptr.h>
#include <k3dsdk/bitmap_source.h>
#include <k3dsdk/classes.h>
#include <k3dsdk/document_plugin_factory.h>
#include <k3dsdk/file_filter.h>
#include <k3d-i18n-config.h>
#include <k3dsdk/ibitmap_importer.h>
#include <k3dsdk/measurement.h>
#include <k3dsdk/node.h>
#include <k3dsdk/options.h>
#include <k3dsdk/path.h>
#include <k3dsdk/persistent.h>

using namespace k3d::xml;

#include <iterator>

namespace libk3dbitmap
{

/////////////////////////////////////////////////////////////////////////////
// bitmap_reader

class bitmap_reader :
	public k3d::bitmap_source<k3d::persistent<k3d::node> >
{
	typedef k3d::bitmap_source<k3d::persistent<k3d::node> > base;

public:
	bitmap_reader(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_file(init_owner(*this) + init_name("file") + init_label(_("File")) + init_description(_("Browse for an input bitmap")) + init_value(k3d::filesystem::path()) + init_path_mode(k3d::ipath_property::READ) + init_path_type(k3d::options::path::bitmaps()))
	{
		m_file.changed_signal().connect(make_reset_bitmap_slot());
	}

	void on_create_bitmap(k3d::bitmap& Bitmap)
	{
		const k3d::filesystem::path file = m_file.pipeline_value();
		if(!k3d::filesystem::exists(file))
			return;

		k3d::auto_ptr<k3d::ibitmap_importer> filter(k3d::auto_file_filter<k3d::ibitmap_importer>(file));
		return_if_fail(filter.get());

		filter->read_file(file, Bitmap);
	}

	void on_update_bitmap(k3d::bitmap& Bitmap)
	{
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<bitmap_reader,
			k3d::interface_list<k3d::ibitmap_source> > factory(
				k3d::classes::FileBitmap(),
				"BitmapReader",
				_("Loads a bitmap from the filesystem"),
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

} // namespace libk3dbitmap

