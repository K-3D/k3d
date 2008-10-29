#ifndef K3DSDK_NGUI_NULL_VIEWPORT_INPUT_MODEL_H
#define K3DSDK_NGUI_NULL_VIEWPORT_INPUT_MODEL_H

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

#include "viewport_input_model.h"

namespace libk3dngui
{

/////////////////////////////////////////////////////////////////////////////
// null_viewport_input_model

/// Do-nothing implementation of viewport_input_model
class null_viewport_input_model :
	public viewport_input_model
{
public:
	virtual void button_press_event(viewport::control& Viewport, GdkEventButton* Event);
	virtual void button_release_event(viewport::control& Viewport, GdkEventButton* Event);
	virtual void motion_notify_event(viewport::control& Viewport, GdkEventMotion* Event);
	virtual void scroll_event(viewport::control& Viewport, GdkEventScroll* Event);
	virtual void key_press_event(viewport::control& Viewport, GdkEventKey* Event);
	virtual void key_release_event(viewport::control& Viewport, GdkEventKey* Event);
};

} // namespace libk3dngui

#endif // !K3DSDK_NGUI_NULL_VIEWPORT_INPUT_MODEL_H

