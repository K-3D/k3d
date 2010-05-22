// K-3D
// Copyright (c) 1995-2009, Timothy M. Shead
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
#include <k3dsdk/iunknown.h>
#include <k3dsdk/iuser_interface.h>
#include <k3dsdk/log.h>
#include <k3dsdk/types.h>
#include <k3dsdk/user_interface.h>

#include <boost/assign/list_of.hpp>

#include <stdexcept>

#include <dbus/dbus-glib.h>

namespace module
{

namespace compiz
{

class compiz_check :
	public k3d::iunknown
{
public:
	compiz_check()
	{
		g_type_init();

		GError* error = 0;
		DBusGConnection* const connection = dbus_g_bus_get(DBUS_BUS_SESSION, &error);
		if(connection)
		{
			DBusGProxy* const proxy = dbus_g_proxy_new_for_name(connection, DBUS_SERVICE_DBUS, DBUS_PATH_DBUS, DBUS_INTERFACE_DBUS);
			if(proxy)
			{
				gboolean compiz_running = false;
				if(dbus_g_proxy_call(proxy, "NameHasOwner", &error, G_TYPE_STRING, "org.freedesktop.compiz", G_TYPE_INVALID, G_TYPE_BOOLEAN, &compiz_running, G_TYPE_INVALID))
				{
					if(compiz_running)
					{
						k3d::user_interface().nag_message(
							"CompizCheck/1",
							k3d::ustring::from_utf8(_("Found running Compiz window manager.")),
							k3d::ustring::from_utf8(_("Depending on your system hardware, Compiz may interfere with rendering in K-3D and other OpenGL applications.  If you have problems (black viewport, problems with viewport updates, etc), try disabling Compiz and re-running K-3D.")));
					}
				}
				else
				{
					k3d::log() << error << "Error testing for org.freedesktop.compiz: " << error->message << std::endl;
				}

				g_object_unref(proxy);
			}
			else
			{
				k3d::log() << error << "Error creating proxy for org.freedesktop.DBus" << std::endl;
			}	
		}
		else
		{
			k3d::log() << error << "Error connecting to DBus session bus: " << error->message << std::endl;
		}

		if(error)
		{
			g_error_free(error);
		}
	}
	
	static k3d::iplugin_factory& get_factory()
	{
		static k3d::application_plugin_factory<compiz_check,
			k3d::interface_list<k3d::iunknown> > factory(
				k3d::uuid(0x0f389f4a, 0xaf41ac00, 0xfc7fe4bf, 0x3524c1eb),
				"CompizCheck",
				_("Warns users that the Compiz window manager may interfere with OpenGL rendering in K-3D"),
				"Desktop",
				k3d::iplugin_factory::STABLE,
				boost::assign::map_list_of("ngui:application-start", "true")("qtui:application-start", "true"));

		return factory;
	}
};

k3d::iplugin_factory& compiz_check_factory()
{
	return compiz_check::get_factory();
}

} // namespace compiz

} // namespace module
