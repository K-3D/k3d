#ifndef K3DSDK_NGUI_IMAGE_TOGGLE_BUTTON_H
#define K3DSDK_NGUI_IMAGE_TOGGLE_BUTTON_H

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

#include "toggle_button.h"

namespace Gtk { class Image; }
namespace Gdk { class Pixbuf; }

namespace k3d
{

namespace ngui
{

namespace image_toggle_button
{

using toggle_button::imodel;
using toggle_button::model;

/////////////////////////////////////////////////////////////////////////////
// control

/// Specialization of toggle_button::control that displays different images dependent on the button state
class control :
	public toggle_button::control
{
	typedef toggle_button::control base;

public:
	control(k3d::icommand_node& Parent, const k3d::string_t& Name, imodel* const Model, k3d::istate_recorder* const StateRecorder, Glib::RefPtr<Gdk::Pixbuf> Image);
	control(k3d::icommand_node& Parent, const k3d::string_t& Name, imodel* const Model, k3d::istate_recorder* const StateRecorder, Glib::RefPtr<Gdk::Pixbuf> ActiveImage, Glib::RefPtr<Gdk::Pixbuf> InactiveImage);

private:
	void on_update();

	Gtk::Image* const m_image;
	Glib::RefPtr<Gdk::Pixbuf> m_active_image;
	Glib::RefPtr<Gdk::Pixbuf> m_inactive_image;
};

} // namespace image_toggle_button

} // namespace ngui

} // namespace k3d

#endif // !K3DSDK_NGUI_IMAGE_TOGGLE_BUTTON_H

