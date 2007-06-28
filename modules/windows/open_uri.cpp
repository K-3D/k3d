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

#include <k3dsdk/application_plugin_factory.h>
#include <k3dsdk/i18n.h>
#include <k3dsdk/ideletable.h>
#include <k3dsdk/iuri.h>
#include <k3dsdk/log.h>
#include <k3dsdk/result.h>
#include <k3dsdk/win32.h>

#include <iostream>

namespace libk3dwindows
{

/////////////////////////////////////////////////////////////////////////////
// open_uri_implementation

class open_uri_implementation :
	public k3d::iuri,
	public k3d::ideletable
{
public:
	open_uri_implementation()
	{
	}

	virtual ~open_uri_implementation()
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
		static k3d::application_plugin_factory<open_uri_implementation,
			k3d::interface_list<k3d::iuri> > factory(
				k3d::uuid(0x35151cfa, 0x104c40df, 0xaef69401, 0xd6bf1754),
				"WindowsOpenURI",
				_("Opens a URI using the Windows API"),
				"Desktop",
				k3d::iplugin_factory::STABLE);

		return factory;
	}
};

/////////////////////////////////////////////////////////////////////////////
// open_uri_factory

k3d::iplugin_factory& open_uri_factory()
{
	return open_uri_implementation::get_factory();
}

} // namespace libk3dwindows

