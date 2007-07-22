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

#include <k3d-i18n-config.h>
#include <k3d-version-config.h>
#include "splash_box.h"

#include <k3dsdk/ngui/utility.h>
#include <k3dsdk/ngui/widget_manip.h>

#include <k3dsdk/log.h>
#include <k3dsdk/path.h>

#include <gtkmm/box.h>
#include <gtkmm/frame.h>
#include <gtkmm/image.h>
#include <gtkmm/label.h>

#include <iostream>

// Temporary hack
using namespace libk3dngui;

namespace module
{

namespace ngui
{

/////////////////////////////////////////////////////////////////////////////
// splash_box

splash_box::splash_box(const k3d::filesystem::path& SharePath) :
	base(Gtk::WINDOW_TOPLEVEL),
	m_startup_message(new Gtk::Label("", Gtk::ALIGN_CENTER, Gtk::ALIGN_CENTER))
{
	set_title(_("Starting K-3D ..."));
	set_role("splash_box");
	set_position(Gtk::WIN_POS_CENTER_ALWAYS);
	set_decorated(false);
	set_border_width(8);

	Gtk::VBox* const vbox = new Gtk::VBox(false, 0);

	vbox->pack_start(*Gtk::manage(
		new Gtk::Label()
		<< line_wrap()
		<< center_justify()
		<< set_markup("<big><b>K-3D Version " K3D_VERSION "</b></big>")));

	vbox->pack_start(*Gtk::manage(
		new Gtk::Label(K3D_COPYRIGHT)
		<< line_wrap()
		<< center_justify()));

	vbox->pack_start(*Gtk::manage(
		new Gtk::Label("Portions copyright (c) 2002, Industrial Light & Magic, a division of Lucas Digital Ltd. LLC")
		<< line_wrap()
		<< center_justify()));

	vbox->pack_start(*Gtk::manage(
		new Gtk::Label("See \"Help > About\" for contributors.")
		<< line_wrap()
		<< center_justify()));

	vbox->pack_start(*Gtk::manage(
		new Gtk::Label("This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.  See \"Help > About\" for details.")
		<< line_wrap()
		<< center_justify()));

	Gtk::HBox* const hbox = new Gtk::HBox(false, 8);

	const k3d::filesystem::path path = SharePath / k3d::filesystem::generic_path("splash.xpm");
	try
	{
		Gtk::Frame* const frame = new Gtk::Frame();
		frame->set_shadow_type(Gtk::SHADOW_OUT);
		frame->add(*Gtk::manage(new Gtk::Image(Gdk::Pixbuf::create_from_file(path.native_filesystem_string()))));

		// Note: we can't use our convenience load_pixbuf() function here, because k3d::application() isn't initialized, yet ...
		hbox->pack_start(*Gtk::manage(frame));
	}
	catch(...)
	{
		k3d::log() << error << "Error loading image [" << path.native_console_string() << "]" << std::endl;
	}

	hbox->pack_start(*Gtk::manage(vbox));

	Gtk::VBox* const vbox2 = new Gtk::VBox(false, 8);
	vbox2->pack_start(*Gtk::manage(hbox));
	vbox2->pack_start(*Gtk::manage(m_startup_message));

	add(*Gtk::manage(vbox2));

	show_all();
	handle_pending_events();
}

void splash_box::on_startup_message(const std::string& Message)
{
	m_startup_message->set_text(Message);
	handle_pending_events();
}

} // namespace ngui

} // namespace module


