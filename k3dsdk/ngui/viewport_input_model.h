#ifndef K3DSDK_NGUI_VIEWPORT_INPUT_MODEL_H
#define K3DSDK_NGUI_VIEWPORT_INPUT_MODEL_H

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

#include <gdk/gdkevents.h>

namespace k3d
{

namespace ngui
{

namespace viewport { class control; }

/////////////////////////////////////////////////////////////////////////////
// viewport_input_model

/// Abstract interface for objects that can process user input
class viewport_input_model
{
public:
	/// Called when a mouse button is pressed
	virtual void button_press_event(viewport::control& Viewport, GdkEventButton* Event) = 0;
	/// Called when a mouse button is released
	virtual void button_release_event(viewport::control& Viewport, GdkEventButton* Event) = 0;
	/// Called during mouse movement
	virtual void motion_notify_event(viewport::control& Viewport, GdkEventMotion* Event) = 0;
	/// Called when the mouse wheel is scrolled
	virtual void scroll_event(viewport::control& Viewport, GdkEventScroll* Event) = 0;
	/// Called when a key is pressed
	virtual void key_press_event(viewport::control& Viewport, GdkEventKey* Event) = 0;
	/// Called when a key is released
	virtual void key_release_event(viewport::control& Viewport, GdkEventKey* Event) = 0;

protected:
	viewport_input_model() {}
	viewport_input_model(const viewport_input_model&) {}
	viewport_input_model& operator=(const viewport_input_model&) { return *this; }
	virtual ~viewport_input_model() {}
};

} // namespace ngui

} // namespace k3d

#endif // !K3DSDK_NGUI_VIEWPORT_INPUT_MODEL_H

