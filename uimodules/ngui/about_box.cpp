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

#include "about_box.h"
#include "application_window.h"
#include "button.h"
#include "utility.h"
#include "widget_manip.h"

#include <k3dsdk/i18n.h>
#include <k3dsdk/path.h>
#include <k3dsdk/version.h>

#include <gtkmm/box.h>
#include <gtkmm/buttonbox.h>
#include <gtkmm/frame.h>
#include <gtkmm/image.h>
#include <gtkmm/label.h>
#include <gtkmm/stock.h>

namespace libk3dngui
{

/////////////////////////////////////////////////////////////////////////////
// about_box

/// Creates a modal "about" box that displays program version and copyright
class about_box :
	public application_window
{
	typedef application_window base;

public:
	about_box(Gtk::Window& Parent) :
		base("about_box", 0)
	{
		set_title(_("About K-3D"));
		set_role("about_box");
		set_position(Gtk::WIN_POS_CENTER);

		Gtk::HButtonBox* const box2 = new Gtk::HButtonBox(Gtk::BUTTONBOX_END);
		box2->pack_start(*Gtk::manage(
			new button::control(*this, "close", Gtk::Stock::CLOSE) << connect_button(sigc::mem_fun(*this, &about_box::close))
			), Gtk::PACK_SHRINK);

		Gtk::VBox* const box1 = new Gtk::VBox(false, 10);
		box1->set_border_width(10);
		box1->pack_start(*Gtk::manage(new Gtk::Image(load_pixbuf(k3d::filesystem::native_path(k3d::ustring::from_utf8("about.xpm"))))), Gtk::PACK_EXPAND_WIDGET);
		box1->pack_start(*Gtk::manage(new Gtk::Label() << line_wrap() << center_justify() << set_markup("<big><b>K-3D Version " K3D_VERSION "</b></big>")));
		box1->pack_start(*Gtk::manage(new Gtk::Label(K3D_COPYRIGHT) << line_wrap() << center_justify()));
		box1->pack_start(*Gtk::manage(new Gtk::Label("Portions copyright (c) 2002, Industrial Light & Magic, a division of Lucas Digital Ltd. LLC") << line_wrap() << center_justify()));
		box1->pack_start(*Gtk::manage(new Gtk::Label("See the AUTHORS file for contributors.") << line_wrap() << center_justify()));
		box1->pack_start(*Gtk::manage(new Gtk::Label("This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.  See the COPYING file for details.") << line_wrap() << center_justify()));
		box1->pack_start(*Gtk::manage(box2));

		Gtk::Frame* const frame1 = new Gtk::Frame();
		frame1->set_shadow_type(Gtk::SHADOW_OUT);
		frame1->add(*Gtk::manage(box1));

		add(*Gtk::manage(frame1));

		set_transient_for(Parent);
		show_all();
	}
};

/////////////////////////////////////////////////////////////////////////////
// create_about_box

void create_about_box(Gtk::Window& Parent)
{
	new about_box(Parent);
}

} // namespace libk3dngui


