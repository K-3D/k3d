// K-3D
// Copyright (c) 1995-2004, Timothy M. Shead
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

#include "button.h"
#include "utility.h"

#include <k3dsdk/log.h>

#include <gtkmm/alignment.h>
#include <gtkmm/box.h>
#include <gtkmm/image.h>
#include <gtkmm/label.h>
#include <gtkmm/stock.h>

#include <iostream>

namespace k3d
{

namespace ngui
{

namespace button
{

/////////////////////////////////////////////////////////////////////////////
// control

control::control() :
	base()
{
	set_name("k3d-button");
}

control::control(const Glib::ustring& label, bool mnemonic) :
	base(label, mnemonic)
{
	set_name("k3d-button");
}

control::control(const Gtk::StockID& stock_id) :
	base(stock_id)
{
	set_name("k3d-button");
}

control::control(const Glib::ustring& label, const Gtk::StockID& stock_id) :
	base()
{
	set_name("k3d-button");

	Gtk::Image* const image = new Gtk::Image();
	Gtk::Stock::lookup(stock_id, Gtk::ICON_SIZE_BUTTON, *image);

	Gtk::Label* const label1 = new Gtk::Label(label, true);

	Gtk::HBox* const hbox = new Gtk::HBox(false, 2);
	hbox->pack_start(*manage(image), false, false, 0);
	hbox->pack_start(*manage(label1), false, false, 0);

	Gtk::Alignment* const alignment = new Gtk::Alignment(0.5, 0.5, 0.0, 0.0);
	alignment->add(*manage(hbox));

	add(*manage(alignment));

	alignment->show_all();
}

control::control(Gtk::Widget& widget) :
	base()
{
	set_name("k3d-button");

	add(widget);
}

void control::on_clicked()
{
	base::on_clicked();
}

} // namespace button

} // namespace ngui

} // namespace k3d

