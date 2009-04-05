#ifndef K3DSDK_NGUI_WIDGET_MANIP_H
#define K3DSDK_NGUI_WIDGET_MANIP_H

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

#include <k3dsdk/result.h>
#include <k3dsdk/signal_system.h>

#include <gtkmm/accelgroup.h>
#include <gtkmm/label.h>
#include <gtkmm/tooltips.h>
#include <glibmm/ustring.h>

namespace k3d
{

namespace ngui
{

/////////////////////////////////////////////////////////////////////////////
// set_tooltip

/// Convenience object for setting the tooltip on a component inline
struct set_tooltip
{
	set_tooltip(const Glib::ustring& Tooltip) : tooltip(Tooltip) { }
	const Glib::ustring tooltip;
};

template<typename T>
T* operator<<(T* LHS, const set_tooltip& RHS)
{
	return_val_if_fail(LHS, LHS);
	LHS->tooltips().set_tip(*LHS, RHS.tooltip);
	return LHS;
}

/////////////////////////////////////////////////////////////////////////////
// set_insensitive

/// Convenience object for disabling a widget inline
struct set_insensitive
{
};

template<typename T>
T* operator<<(T* LHS, const set_insensitive& RHS)
{
	return_val_if_fail(LHS, LHS);
	LHS->set_sensitive(false);
	return LHS;
}

/////////////////////////////////////////////////////////////////////////////
// connect_button

/// Convenience object for connecting a button to a slot inline
struct connect_button
{
	typedef sigc::slot<void> slot_t;
	connect_button(const slot_t& Slot) : slot(Slot) { }
	slot_t slot;
};

template<typename T>
T* operator<<(T* LHS, const connect_button& RHS)
{
	return_val_if_fail(LHS, LHS);
	LHS->signal_clicked().connect(RHS.slot);
	return LHS;
}

/////////////////////////////////////////////////////////////////////////////
// connect_menu_item

/// Convenience object for connecting a menu item to a slot inline
struct connect_menu_item
{
	typedef sigc::slot<void> slot_t;
	connect_menu_item(const slot_t& Slot) : slot(Slot) { }
	slot_t slot;
};

template<typename T>
T* operator<<(T* LHS, const connect_menu_item& RHS)
{
	return_val_if_fail(LHS, LHS);
	LHS->signal_activate().connect(RHS.slot);
	return LHS;
}

/////////////////////////////////////////////////////////////////////////////
// connect_signal_changed

/// Convenience object for connecting a menu item to a slot inline
struct connect_signal_changed
{
	typedef sigc::slot<void> slot_t;
	connect_signal_changed(const slot_t& Slot) : slot(Slot) { }
	slot_t slot;
};

template<typename T>
T* operator<<(T* LHS, const connect_signal_changed& RHS)
{
	return_val_if_fail(LHS, LHS);
	LHS->signal_changed().connect(RHS.slot);
	return LHS;
}

/////////////////////////////////////////////////////////////////////////////
// line_wrap

struct line_wrap
{
};

template<typename T>
T* operator<<(T* LHS, const line_wrap& RHS)
{
	return_val_if_fail(LHS, LHS);
	LHS->set_line_wrap(true);
	return LHS;
}

/////////////////////////////////////////////////////////////////////////////
// center_justify

struct center_justify
{
};

template<typename T>
T* operator<<(T* LHS, const center_justify& RHS)
{
	return_val_if_fail(LHS, LHS);
	LHS->set_justify(Gtk::JUSTIFY_CENTER);
	return LHS;
}

/////////////////////////////////////////////////////////////////////////////
// right_justify

struct right_justify
{
};

template<typename T>
T* operator<<(T* LHS, const right_justify& RHS)
{
	return_val_if_fail(LHS, LHS);
	LHS->set_justify(Gtk::JUSTIFY_RIGHT);
	return LHS;
}

/////////////////////////////////////////////////////////////////////////////
// alignment

struct alignment
{
	alignment(const float XAlign, const float YAlign) : xalign(XAlign), yalign (YAlign) { }
	const float xalign;
	const float yalign;
};

template<typename T>
T* operator<<(T* LHS, const alignment& RHS)
{
	return_val_if_fail(LHS, LHS);
	LHS->set_alignment(RHS.xalign, RHS.yalign);
	return LHS;
}

/////////////////////////////////////////////////////////////////////////////
// set_markup

struct set_markup
{
	set_markup(const Glib::ustring& Markup) : markup(Markup) { }
	const Glib::ustring markup;
};

template<typename T>
T* operator<<(T* LHS, const set_markup& RHS)
{
	return_val_if_fail(LHS, LHS);
	LHS->set_markup(RHS.markup);
	return LHS;
}

/////////////////////////////////////////////////////////////////////////////
// make_toolbar_button

struct make_toolbar_button
{
};

template<typename T>
T* operator<<(T* LHS, const make_toolbar_button& RHS)
{
	return_val_if_fail(LHS, LHS);

	LHS->set_name("k3d-toolbar-button");
	LHS->set_relief(Gtk::RELIEF_NONE);

	return LHS;
}

/////////////////////////////////////////////////////////////////////////////
// set_accelerator_path

struct set_accelerator_path
{
	set_accelerator_path(const Glib::ustring& Path, const Glib::RefPtr<Gtk::AccelGroup>& Group) :
		path(Path),
		group(Group)
	{
	}

	const Glib::ustring path;
	const Glib::RefPtr<Gtk::AccelGroup> group;
};

template<typename T>
T* operator<<(T* LHS, const set_accelerator_path& RHS)
{
	return_val_if_fail(LHS, LHS);
	static_cast<Gtk::Widget*>(LHS)->set_accel_path(RHS.path, RHS.group);
	return LHS;
}

/////////////////////////////////////////////////////////////////////////////
// enable_dynamic_accelerators

struct enable_dynamic_accelerators
{
};

void enable_dynamic_accelerators_impl(Gtk::Widget*);

template<typename T>
T* operator<<(T* LHS, const enable_dynamic_accelerators& RHS)
{
	return_val_if_fail(LHS, LHS);
	enable_dynamic_accelerators_impl(LHS);
	return LHS;
}

///////////////////////////////////////////////////////////////////////////////
// disable_recording

struct disable_recording
{
};

template<typename T>
T* operator<<(T* LHS, const disable_recording& RHS)
{
	return_val_if_fail(LHS, LHS);
	LHS->enable_recording(false);
	return LHS;
}

} // namespace ngui

} // namespace k3d

#endif // !K3DSDK_NGUI_WIDGET_MANIP_H

