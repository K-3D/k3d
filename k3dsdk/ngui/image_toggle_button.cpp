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

#include "image_toggle_button.h"

#include <gtkmm/image.h>

namespace libk3dngui
{

namespace image_toggle_button
{

/////////////////////////////////////////////////////////////////////////////
// control

control::control(k3d::icommand_node& Parent, const k3d::string_t& Name, imodel* const Model, k3d::istate_recorder* const StateRecorder, Glib::RefPtr<Gdk::Pixbuf> Image) :
	base(Parent, Name, Model, StateRecorder),
	m_image(new Gtk::Image()),
	m_active_image(Image),
	m_inactive_image(Image)
{
	add(*manage(m_image));
	on_update();
}

control::control(k3d::icommand_node& Parent, const k3d::string_t& Name, imodel* const Model, k3d::istate_recorder* const StateRecorder, Glib::RefPtr<Gdk::Pixbuf> ActiveImage, Glib::RefPtr<Gdk::Pixbuf> InactiveImage) :
	base(Parent, Name, Model, StateRecorder),
	m_image(new Gtk::Image()),
	m_active_image(ActiveImage),
	m_inactive_image(InactiveImage)
{
	add(*manage(m_image));
	on_update();
}

void control::on_update()
{
	m_image->set(get_active() ? m_active_image : m_inactive_image);
}

} // namespace image_toggle_button

} // namespace libk3dngui

