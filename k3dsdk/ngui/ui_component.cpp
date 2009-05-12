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

#include "ui_component.h"

#include <k3dsdk/result.h>

#include <gtkmm/tooltips.h>
#include <gtkmm/widget.h>

namespace k3d
{

namespace ngui
{

////////////////////////////////////////////////////////////////////////////////////////
// ui_component

ui_component::ui_component()
{
}

ui_component::~ui_component()
{
}

Gtk::Tooltips& ui_component::tooltips()
{
	static Gtk::Tooltips tips;
	return tips;
}

void ui_component::set_tip(const Glib::ustring& Tooltip)
{
	Gtk::Widget* const widget = dynamic_cast<Gtk::Widget*>(this);
	return_if_fail(widget);

	tooltips().set_tip(*widget, Tooltip);
}

} // namespace ngui

} // namespace k3d

