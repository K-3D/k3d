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
	\author Timothy M. Shead (tshead@k-3d.com)
*/

#include <k3d-i18n-config.h>
#include <k3dsdk/bitmap_source.h>
#include <k3dsdk/classes.h>
#include <k3dsdk/document_plugin_factory.h>
#include <k3dsdk/ibitmap_importer.h>
#include <k3dsdk/ipipeline_profiler.h>
#include <k3dsdk/measurement.h>
#include <k3dsdk/mime_types.h>
#include <k3dsdk/node.h>
#include <k3dsdk/options.h>
#include <k3dsdk/path.h>
#include <k3dsdk/persistent.h>
#include <k3dsdk/plugins.h>

#include <boost/scoped_ptr.hpp>

#include <iterator>

namespace module
{

namespace bitmap
{

/////////////////////////////////////////////////////////////////////////////
// reader

class reader :
	public k3d::persistent<k3d::node>,
	public k3d::bitmap_source<reader>
{
	typedef k3d::persistent<k3d::node> base;

public:
	reader(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_file(init_owner(*this) + init_name("file") + init_label(_("File")) + init_description(_("Browse for an input bitmap")) + init_value(k3d::filesystem::path()) + init_path_mode(k3d::ipath_property::READ) + init_path_type(k3d::options::path::bitmaps()))
	{
		m_file.changed_signal().connect(make_reset_bitmap_slot());
	}

	void on_create_bitmap(k3d::bitmap& Bitmap)
	{
		k3d::ipipeline_profiler::profile profile(document().pipeline_profiler(), *this, "Create Bitmap");
        const k3d::filesystem::path file = m_file.pipeline_value();
		if(!k3d::filesystem::exists(file))
			return;

		const k3d::mime::type mime_type = k3d::mime::type::lookup(file);
		if(mime_type.empty())
		{
			k3d::log() << error << "couldn't identify MIME type for file [" << file.native_console_string() << "]" << std::endl;
			return;
		}

		const k3d::plugin::factory::collection_t factories = k3d::plugin::factory::lookup<k3d::ibitmap_importer>(mime_type);
		if(factories.empty())
		{
			k3d::log() << error << "no plugins available to load MIME type [" << mime_type.str() << "]" << std::endl;
			return;
		}

		for(k3d::plugin::factory::collection_t::const_iterator factory = factories.begin(); factory != factories.end(); ++factory)
		{
			boost::scoped_ptr<k3d::ibitmap_importer> importer(k3d::plugin::create<k3d::ibitmap_importer>(**factory));
			if(!importer)
				continue;

			if(!importer->read_file(file, Bitmap))
			{
				k3d::log() << error << "factory [" << (**factory).name() << "] couldn't load file [" << file.native_console_string() << "]" << std::endl;
				continue;
			}

			return;
		}
	}

	void on_update_bitmap(k3d::bitmap& Bitmap)
	{
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<reader,
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
// reader_factory

k3d::iplugin_factory& reader_factory()
{
	return reader::get_factory();
}

} // namespace bitmap

} // namespace module

