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
	\author Tim Shead (tshead@k-3d.com)
*/

#include <k3d-i18n-config.h>
#include <k3dsdk/application_plugin_factory.h>
#include <k3dsdk/command_tree.h>
#include <k3dsdk/ngui/application_state.h>
#include <k3dsdk/ngui/application_window.h>
#include <k3dsdk/ngui/button.h>
#include <k3dsdk/ngui/widget_manip.h>
#include <k3dsdk/module.h>

#include <gtkmm/box.h>
#include <gtkmm/buttonbox.h>
#include <gtkmm/label.h>
#include <gtkmm/settings.h>
#include <gtkmm/stock.h>

using namespace libk3dngui;

namespace module
{

namespace ngui
{

namespace assign_hotkeys
{

/////////////////////////////////////////////////////////////////////////////
// dialog

class dialog :
	public application_window
{
	typedef application_window base;
public:
	dialog()
	{
		k3d::command_tree().add(*this, "assign_hotkeys");

		is_open = true;

		set_title(_("Assign Hotkeys:"));
		set_role("assign_hotkeys");
		set_keep_above(true);

		Gtk::HButtonBox* const box2 = new Gtk::HButtonBox(Gtk::BUTTONBOX_END);
		box2->pack_start(*Gtk::manage(
			new button::control(*this, "close", Gtk::Stock::CLOSE)
			<< connect_button(sigc::mem_fun(*this, &dialog::close))
			), Gtk::PACK_SHRINK);

		Gtk::VBox* const box1 = new Gtk::VBox(false, 10);
		box1->set_border_width(10);
		box1->pack_start(*Gtk::manage(new Gtk::Label() << line_wrap() << center_justify() << set_markup(_("<big><b>Assign Hotkeys Mode</b></big>"))));
		box1->pack_start(*Gtk::manage(new Gtk::Label(_("To assign hotkeys, hover the mouse over a menu item, and hit the desired hotkey combination.  Use \"delete\" to remove hotkeys.  Close this window to turn-off Assign Hotkeys Mode.")) << line_wrap() << center_justify()));
		box1->pack_start(*Gtk::manage(box2));

		add(*Gtk::manage(box1));

		show_all();

		application_state::instance().enable_hotkey_assignment(true);

		Gtk::Settings::get_default()->property_gtk_can_change_accels() = true;
	}

	~dialog()
	{
		Gtk::Settings::get_default()->property_gtk_can_change_accels() = false;

		application_state::instance().enable_hotkey_assignment(false);
		is_open = false;
	}

	static bool is_open;

	/// Provides a custom plugin factory for creating assign_hotkeys::dialog instances
	class plugin_factory :
		public k3d::plugin_factory,
		public k3d::iapplication_plugin_factory
	{
	public:
		plugin_factory() :
			k3d::plugin_factory(k3d::uuid(0x5924a92d, 0x664e6000, 0xaa69218f, 0x6cbc7e5c),
				"NGUIAssignHotkeysDialog",
				_("Provides a user-interface for assigning menu hotkeys."),
				"NGUI Dialog",
				k3d::iplugin_factory::EXPERIMENTAL)
		{
		}

		virtual k3d::iunknown* create_plugin()
		{
			if(dialog::is_open)
				return 0;

			return new dialog();
		}

		bool implements(const std::type_info& InterfaceType)
		{
			return false;
		}

		const interfaces_t interfaces()
		{
			return interfaces_t();
		}
	};

	static k3d::iplugin_factory& get_factory()
	{
		static dialog::plugin_factory factory;
		return factory;
	}
};

bool dialog::is_open = false;

} // namespace assign_hotkeys

} // namespace ngui

} // namespace module

K3D_MODULE_START(Registry)
	Registry.register_factory(module::ngui::assign_hotkeys::dialog::get_factory());
K3D_MODULE_END

