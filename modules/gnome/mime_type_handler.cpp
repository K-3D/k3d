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

#include <k3d-i18n-config.h>
#include <k3dsdk/application_plugin_factory.h>
#include <k3dsdk/path.h>
#include <k3dsdk/imime_type_handler.h>
#include <k3dsdk/log.h>
#include <k3dsdk/result.h>

#include <boost/assign/list_of.hpp>

#include <libgnomevfs/gnome-vfs.h>
#include <libgnomevfs/gnome-vfs-mime-handlers.h>

namespace module
{

namespace gnome
{

/// Uses the Gnome API to identify MIME types.
class mime_type_handler :
	public k3d::imime_type_handler
{
public:
	mime_type_handler()
	{
		if(!gnome_vfs_initialized())
			return_if_fail(gnome_vfs_init());
	}

	~mime_type_handler()
	{
	}

	k3d::bool_t identify_mime_type(const k3d::filesystem::path& File, k3d::string_t& FileType)
	{
		const char* const mime_type = gnome_vfs_get_mime_type_for_name(File.native_filesystem_string().c_str());
		return_val_if_fail(mime_type, false);

		if(k3d::string_t(mime_type) == k3d::string_t(GNOME_VFS_MIME_TYPE_UNKNOWN))
			return false;

		FileType = mime_type;

		k3d::log() << info << "Identified " << File.native_console_string() << " as " << FileType << " using " << get_factory().name() << std::endl;
		return true;
	}

	k3d::bool_t identify_mime_type(const k3d::string_t& Data, k3d::string_t& DataType)
	{
		return false;
	}
	
	static k3d::iplugin_factory& get_factory()
	{
		static k3d::application_plugin_factory<mime_type_handler,
			k3d::interface_list<k3d::imime_type_handler> > factory(
				k3d::uuid(0x8939ae52, 0x0342a2fc, 0x7976e5b5, 0xd6873980),
				"GnomeMIMETypeHandler",
				_("Identifies a file's MIME Type using the Gnome API"),
				"Desktop",
				k3d::iplugin_factory::STABLE,
				boost::assign::map_list_of("k3d:load-order", "8"));

		return factory;
	}
};

/////////////////////////////////////////////////////////////////////////////
// mime_type_handler_factory

k3d::iplugin_factory& mime_type_handler_factory()
{
	return mime_type_handler::get_factory();
}

} // namespace gnome

} // namespace module

