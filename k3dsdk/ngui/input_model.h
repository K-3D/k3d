#ifndef NGUI_INPUT_MODEL_H
#define NGUI_INPUT_MODEL_H

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

namespace libk3dngui
{

/////////////////////////////////////////////////////////////////////////////
// input_model

/// Abstract interface for objects that can process user input
class input_model
{
public:
	/// Called when a mouse button is pressed
	virtual void button_press_event(GdkEventButton* Event) = 0;
	/// Called when a mouse button is released
	virtual void button_release_event(GdkEventButton* Event) = 0;
	/// Called during mouse movement
	virtual void motion_notify_event(GdkEventMotion* Event) = 0;
	/// Called when the mouse wheel is scrolled
	virtual void scroll_event(GdkEventScroll* Event) = 0;
	/// Called when a key is pressed
	virtual void key_press_event(GdkEventKey* Event) = 0;
	/// Called when a key is released
	virtual void key_release_event(GdkEventKey* Event) = 0;

protected:
	input_model() {}
	input_model(const input_model&) {}
	input_model& operator=(const input_model&) { return *this; }
	virtual ~input_model() {}
};

} // namespace libk3dngui

#endif // NGUI_INPUT_MODEL_H

