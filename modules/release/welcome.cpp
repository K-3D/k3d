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
#include <k3dsdk/user_interface.h>

#include <boost/assign/list_of.hpp>

#include <stdexcept>

namespace module
{

namespace release
{

class welcome :
	public k3d::iunknown
{
public:
	welcome()
	{
		const k3d::ustring message = k3d::ustring::from_utf8(_("Welcome to K-3D Version 0.8!"));

		const k3d::ustring secondary_message = k3d::ustring::from_utf8(
			"It's been a long time since our last official K-3D release - there have been "
			"many significant internal changes to K-3D, so some older documents may no longer "
			"work.\n\nWe're switching to a six-month release cycle for K-3D, so our next "
			"release will be in October, 2010.\n\nWe hope you enjoy using K-3D, and would love "
			"to see what you create!");

		k3d::user_interface().nag_message("Welcome/0.8", message, secondary_message);
	}
	
	static k3d::iplugin_factory& get_factory()
	{
		static k3d::application_plugin_factory<welcome,
			k3d::interface_list<k3d::iunknown> > factory(
				k3d::uuid(0x12ebafe1, 0x6a41cf73, 0x076b72b5, 0xfad83322),
				"Welcome",
				_("Welcomes users running K-3D for the first time."),
				"Desktop",
				k3d::iplugin_factory::STABLE,
				boost::assign::map_list_of("ngui:application-start", "true"));

		return factory;
	}
};

k3d::iplugin_factory& welcome_factory()
{
	return welcome::get_factory();
}

} // namespace release

} // namespace module
