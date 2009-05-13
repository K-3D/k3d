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
#include "widget_manip.h"

#include <gtkmm/accelmap.h>
#include <gtkmm/imagemenuitem.h>
#include <gtkmm/menuitem.h>

namespace k3d
{

namespace ngui
{

namespace detail
{

/// Duplicated from GTK sources because there's no gtk_widget_get_accel_path()
typedef struct
{
  GQuark         path_quark;
  GtkWidget     *widget;
  GtkAccelGroup *accel_group;
  GClosure      *closure;
} AccelPath;

const std::string get_accel_path(Gtk::Widget* Widget)
{
	return_val_if_fail(Widget, std::string());

	static GQuark quark_accel_path = g_quark_from_static_string("gtk-accel-path");
	AccelPath* const apath = reinterpret_cast<AccelPath*>(g_object_get_qdata(G_OBJECT(Widget->gobj()), quark_accel_path));
	return apath ? g_quark_to_string(apath->path_quark) : "";
}

bool dynamic_accelerator_on_enter_notify_event(GdkEventCrossing*, Gtk::Widget* Widget)
{
	return_val_if_fail(Widget, false);

	// Unless assign hotkeys mode is enabled, we're done ...
	if(!application_state::instance().assign_hotkeys())
		return false;

	// If the widget doesn't have an accelerator path, we're done ...
	if(get_accel_path(Widget).empty())
		return false;

	Widget->grab_focus();
	return false;
}

bool dynamic_accelerator_on_key_press_event(GdkEventKey*, Gtk::Widget* Widget)
{
	return_val_if_fail(Widget, false);

	// Unless assign hotkeys mode is enabled, we're done ...
	if(!application_state::instance().assign_hotkeys())
		return false;

	// If the widget doesn't have an accelerator path, we're done ...
	const std::string accel_path = get_accel_path(Widget);
	if(accel_path.empty())
		return false;

	Gtk::AccelMap::add_entry(accel_path, Gtk::AccelKey("q").get_key(), Gdk::ModifierType(0));

	return false;
}

} // namespace detail

void enable_dynamic_accelerators_impl(Gtk::Widget* const Widget)
{
	return_if_fail(Widget);

	// Menu items already support dynamic hotkey assignment, so we're done ...
	Gtk::MenuItem* const menu_item = dynamic_cast<Gtk::MenuItem*>(Widget);
	if(menu_item)
		return;

	Widget->signal_enter_notify_event().connect(sigc::bind(sigc::ptr_fun(detail::dynamic_accelerator_on_enter_notify_event), Widget));
	Widget->signal_key_press_event().connect(sigc::bind(sigc::ptr_fun(detail::dynamic_accelerator_on_key_press_event), Widget));
}

Gtk::Label* get_label(Gtk::ImageMenuItem& widget)
{
	return dynamic_cast<Gtk::Label*>(widget.get_child());
}

} // namespace ngui

} // namespace k3d


