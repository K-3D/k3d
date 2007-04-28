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

#include "collapsible_frame.h"
#include "event_button.h"
#include "ui_component.h"
#include "widget_manip.h"

#include <k3dsdk/i18n.h>
#include <k3dsdk/log.h>
#include <k3dsdk/signal_system.h>

#include <gtkmm/arrow.h>
#include <gtkmm/box.h>
#include <gtkmm/label.h>
#include <gtkmm/main.h>
#include <gtkmm/menu.h>
#include <gtkmm/menuitem.h>
#include <gtkmm/tooltips.h>

#include <iostream>

namespace libk3dngui
{

namespace collapsible_frame
{

/////////////////////////////////////////////////////////////////////////////
// group::implementation

class group::implementation
{
public:
	sigc::signal<void> expand_all_signal;
	sigc::signal<void> collapse_all_signal;
};

/////////////////////////////////////////////////////////////////////////////
// group

group::group() :
	m_implementation(new implementation())
{
}

group::~group()
{
	delete m_implementation;
}

void group::expand_all()
{
	m_implementation->expand_all_signal.emit();
}

void group::collapse_all()
{
	m_implementation->collapse_all_signal.emit();
}

/////////////////////////////////////////////////////////////////////////////
// control::implementation

class control::implementation :
	public sigc::trackable
{
public:
	implementation(const Glib::ustring& Name, Gtk::Frame& Frame) :
		frame(Frame),
		arrow(Gtk::ARROW_DOWN, Gtk::SHADOW_NONE)
	{
		Gtk::HBox* const hbox = new Gtk::HBox(false);
		hbox->pack_start(arrow, Gtk::PACK_SHRINK);
		hbox->pack_start(*Gtk::manage(new Gtk::Label(Name)), Gtk::PACK_EXPAND_WIDGET);

		button.add(*Gtk::manage(hbox));
		button.set_relief(Gtk::RELIEF_NONE);
		button.signal_clicked().connect(sigc::mem_fun(*this, &implementation::on_toggle));
		button.signal_button_press_event().connect(sigc::mem_fun(*this, &implementation::on_button_press_event));

		ui_component::tooltips().set_tip(button, _("LMB-Click to expand/collapse.  RMB-Click for other options."));

		frame.set_label_widget(button);
		frame.set_shadow_type(Gtk::SHADOW_NONE);

		context_menu.items().push_back(*Gtk::manage(
			new Gtk::MenuItem(_("Expand")) <<
			connect_menu_item(sigc::mem_fun(*this, &implementation::expand))));

		context_menu.items().push_back(*Gtk::manage(
			new Gtk::MenuItem(_("Collapse")) <<
			connect_menu_item(sigc::mem_fun(*this, &implementation::collapse))));

		context_menu.items().push_back(*Gtk::manage(
			new Gtk::MenuItem(_("Expand All")) <<
			connect_menu_item(expand_all_signal.make_slot())));

		context_menu.items().push_back(*Gtk::manage(
			new Gtk::MenuItem(_("Collapse All")) <<
			connect_menu_item(collapse_all_signal.make_slot())));
	}

	void expand()
	{
		arrow.property_arrow_type().set_value(Gtk::ARROW_DOWN);

		if(frame.get_child())
			frame.get_child()->show();
	}

	void collapse()
	{
		arrow.property_arrow_type().set_value(Gtk::ARROW_RIGHT);

		if(frame.get_child())
			frame.get_child()->hide();
	}

	bool is_collapsed()
	{
		return arrow.property_arrow_type().get_value() != Gtk::ARROW_DOWN;
	}

	void on_toggle()
	{
		if(is_collapsed())
			expand();
		else
			collapse();
	}

	bool on_button_press_event(GdkEventButton* Event)
	{
		if(Event->button != 3)
			return false;

		context_menu.show_all();
		context_menu.popup(3, gtk_get_current_event_time());

		return true;
	}

	Gtk::Frame& frame;
	Gtk::Arrow arrow;
	event_button button;

	Gtk::Menu context_menu;

	sigc::signal<void> expand_all_signal;
	sigc::signal<void> collapse_all_signal;
};

/////////////////////////////////////////////////////////////////////////////
// control

control::control(const Glib::ustring& Name) :
	m_implementation(new implementation(Name, *this))
{
}

control::control(const Glib::ustring& Name, group& Group) :
	m_implementation(new implementation(Name, *this))
{
	Group.m_implementation->expand_all_signal.connect(sigc::mem_fun(*this, &control::expand));
	Group.m_implementation->collapse_all_signal.connect(sigc::mem_fun(*this, &control::collapse));

	m_implementation->expand_all_signal.connect(Group.m_implementation->expand_all_signal.make_slot());
	m_implementation->collapse_all_signal.connect(Group.m_implementation->collapse_all_signal.make_slot());
}

control::~control()
{
	delete m_implementation;
}

void control::expand()
{
	m_implementation->expand();
}

void control::collapse()
{
	m_implementation->collapse();
}

bool control::is_collapsed()
{
	return m_implementation->is_collapsed();
}

Gtk::Button& control::toggle_button()
{
	return m_implementation->button;
}

} // namespace collapsible_frame

} // namespace libk3dngui


