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
		\author Tim Shead (tshead@k-3d.com)
*/

#include "application_state.h"
#include "application_window.h"
#include "assign_hotkeys_dialog.h"
#include "button.h"
#include "widget_manip.h"

#include <k3d-i18n-config.h>

#include <gtkmm/box.h>
#include <gtkmm/buttonbox.h>
#include <gtkmm/label.h>
#include <gtkmm/settings.h>
#include <gtkmm/stock.h>

namespace libk3dngui
{

/////////////////////////////////////////////////////////////////////////////
// assign_hotkeys_dialog

class assign_hotkeys_dialog :
	public application_window
{
	typedef application_window base;
public:
	assign_hotkeys_dialog() :
		base("assign_hotkeys", 0)
	{
		is_open = true;

		set_title(_("Assign Hotkeys:"));
		set_role("assign_hotkeys");
		set_keep_above(true);

		Gtk::HButtonBox* const box2 = new Gtk::HButtonBox(Gtk::BUTTONBOX_END);
		box2->pack_start(*Gtk::manage(
			new button::control(*this, "close", Gtk::Stock::CLOSE)
			<< connect_button(sigc::mem_fun(*this, &assign_hotkeys_dialog::close))
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

	~assign_hotkeys_dialog()
	{
		Gtk::Settings::get_default()->property_gtk_can_change_accels() = false;

		application_state::instance().enable_hotkey_assignment(false);
		is_open = false;
	}

	static bool is_open;
};

bool assign_hotkeys_dialog::is_open = false;

void assign_hotkeys()
{
	if(assign_hotkeys_dialog::is_open)
		return;

	new assign_hotkeys_dialog();
}

} // namespace libk3dngui

