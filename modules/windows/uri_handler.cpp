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
#include <k3dsdk/ideletable.h>
#include <k3dsdk/iuri_handler.h>
#include <k3dsdk/log.h>
#include <k3dsdk/result.h>
#include <k3dsdk/win32.h>

#include <iostream>

namespace module
{

namespace windows
{

/////////////////////////////////////////////////////////////////////////////
// uri_handler

/// Uses the Win32 API to open a URI in the user's preferred application
class uri_handler :
	public k3d::iuri_handler,
	public k3d::ideletable
{
public:
	uri_handler()
	{
	}

	virtual ~uri_handler()
	{
	}

	bool open_uri(const std::string& URI)
	{
		k3d::log() << info << "URI: " << URI << std::endl;
		return_val_if_fail(reinterpret_cast<int>(ShellExecute(0, "open", URI.c_str(), 0, 0, SW_SHOWDEFAULT)) > 32, false);
		return true;
	}
	
	static k3d::iplugin_factory& get_factory()
	{
		static k3d::application_plugin_factory<uri_handler,
			k3d::interface_list<k3d::iuri_handler> > factory(
				k3d::uuid(0x35151cfa, 0x104c40df, 0xaef69401, 0xd6bf1754),
				"WindowsURIHandler",
				_("Opens a URI using the Windows API"),
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

} // namespace windows

} // namespace module

