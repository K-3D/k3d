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

#include <aqsis/ri/ri.h>

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
		Gtk::Label* const label = new Gtk::Label(_("Rendering using in-process Aqsis ...\nClose window to cancel rendering."));
		add(*Gtk::manage(label));

		set_border_width(10);

		set_role("aqsis");
		show_all();

		static RtFloat fov = 45, intensity = 0.5;
		static RtFloat Ka = 0.5, Kd = 0.8, Ks = 0.2;
		static RtPoint from = {0,0,1}, to = {0,10,0};
		RiBegin (RI_NULL);
		RiFormat (512, 512, 1);
		RiPixelSamples (2, 2);
		RiFrameBegin (1);
		RiDisplay ("test1.tiff", "framebuffer", "rgb", RI_NULL);
		RiProjection ("perspective", "fov", &fov, RI_NULL);
		RiRotate(-116.344, 0, 0, 1);
		RiRotate(-47.9689, 1, 0, 0);
		RiRotate(-123.69, 0, 1, 0);
		RiTranslate(15, -20, -10);
		RiWorldBegin ();
		RiSphere (5, -5, 5, 360, RI_NULL);
		RiWorldEnd ();
		RiFrameEnd ();
		RiEnd ();
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

