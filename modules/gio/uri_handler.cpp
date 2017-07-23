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
		\author Timothy M. Shead (tshead@k-3d.com)
*/

#include <k3d-i18n-config.h>
#include <k3dsdk/application_plugin_factory.h>
#include <k3dsdk/iuri_handler.h>
#include <k3dsdk/log.h>
#include <k3dsdk/result.h>

#include <iostream>

#include <giomm/appinfo.h>

namespace module
{

namespace gio
{

/// Uses the GIO API to open a URI in the user's preferred application
class uri_handler :
	public k3d::iuri_handler
{
public:
	uri_handler()
	{
	}

	~uri_handler()
	{
	}


	bool open_uri(const std::string& URI)
	{
		return Gio::AppInfo::launch_default_for_uri(URI);
	}
	
	static k3d::iplugin_factory& get_factory()
	{
		static k3d::application_plugin_factory<uri_handler,
			k3d::interface_list<k3d::iuri_handler> > factory(
				k3d::uuid(0xac560e92, 0x1d31478b, 0x9139ace8, 0x1bb0ae31),
				"GioURIHandler",
				_("Opens a URI using the GIO libraries"),
				"Desktop",
				k3d::iplugin_factory::STABLE);

		return factory;
	}
};

/////////////////////////////////////////////////////////////////////////////
// uri_handler_factory

k3d::iplugin_factory& uri_handler_factory()
{
	return uri_handler::get_factory();
}

} // namespace gio

} // namespace module
