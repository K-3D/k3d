// K-3D
// Copyright (c) 1995-2010, Timothy M. Shead
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
#include <k3dsdk/application_plugin_factory.h>
#include <k3dsdk/iuser_interface.h>
#include <k3dsdk/module.h>
#include <k3dsdk/ngui/application_window.h>
#include <k3dsdk/result.h>
#include <k3dsdk/user_interface.h>

#include <gtkmm/label.h>

#include <boost/scoped_ptr.hpp>
#include <boost/assign/list_of.hpp>

using namespace k3d::ngui;

namespace module
{

namespace ngui
{

namespace aqsis
{

class dialog :
	public application_window,
	public k3d::iunknown
{
	typedef application_window base;

public:
	dialog()
	{
		Gtk::Label* const label = new Gtk::Label(_("Logging SpaceNavigator events to stderr ...\nClose window to cancel logging."));
		add(*Gtk::manage(label));

		set_border_width(10);

		set_role("aqsis");
		show_all();
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::application_plugin_factory<dialog> factory(
			k3d::uuid(0xa467cfdd, 0x2545d997, 0x07ecf8a2, 0x042455fb),
			"NGUIAqsis",
			_("Provides an embedded Aqsis RenderMan render engine."),
			"NGUI Dialog",
			k3d::iplugin_factory::EXPERIMENTAL,
			boost::assign::map_list_of("ngui:component-type", "dialog")
			);

		return factory;
	}
};

} // namespace aqsis

} // namespace ngui

} // namespace module

K3D_MODULE_START(Registry)
	Registry.register_factory(module::ngui::aqsis::dialog::get_factory());
K3D_MODULE_END

