#ifndef NGUI_IMAGE_TOGGLE_BUTTON_H
#define NGUI_IMAGE_TOGGLE_BUTTON_H

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
		\brief Declares k3d::image_toggle_button::control, which provides a standard MVC UI for boolean data that supports tutorials, undo/redo, etc.
		\author Tim Shead (tshead@k-3d.com)
*/

#include "toggle_button.h"

// Forward declarations
namespace Gtk { class Image; }
namespace Gdk { class Pixbuf; }

namespace libk3dngui
{

namespace image_toggle_button
{

using toggle_button::idata_proxy;

/////////////////////////////////////////////////////////////////////////////
// control

/// Provides a UI for manipulating boolean quantities (i.e. the view and the controller from model-view-controller)
class control :
	public toggle_button::control
{
	typedef toggle_button::control base;

public:
	control(k3d::icommand_node& Parent, const std::string& Name, std::auto_ptr<idata_proxy> Data, Glib::RefPtr<Gdk::Pixbuf> Image);
	control(k3d::icommand_node& Parent, const std::string& Name, std::auto_ptr<idata_proxy> Data, Glib::RefPtr<Gdk::Pixbuf> ActiveImage, Glib::RefPtr<Gdk::Pixbuf> InactiveImage);

private:
	void on_update();

	Gtk::Image* const m_image;
	Glib::RefPtr<Gdk::Pixbuf> m_active_image;
	Glib::RefPtr<Gdk::Pixbuf> m_inactive_image;
};

using toggle_button::data_proxy;
using toggle_button::proxy;

} // namespace image_toggle_button

} // namespace libk3dngui

#endif // !NGUI_IMAGE_TOGGLE_BUTTON_H


