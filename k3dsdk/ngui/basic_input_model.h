#ifndef K3DSDK_NGUI_BASIC_INPUT_MODEL_H
#define K3DSDK_NGUI_BASIC_INPUT_MODEL_H

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

#include "input_model.h"
#include <k3dsdk/signal_system.h>

namespace k3d
{

namespace ngui
{

/////////////////////////////////////////////////////////////////////////////
// basic_input_model

/// Implementation of input_model that synthesizes a more useful collection of input events
class basic_input_model :
	public input_model
{
public:
	basic_input_model();
	virtual ~basic_input_model();

	virtual void button_press_event(GdkEventButton* Event);
	virtual void button_release_event(GdkEventButton* Event);
	virtual void motion_notify_event(GdkEventMotion* Event);
	virtual void scroll_event(GdkEventScroll* Event);
	virtual void key_press_event(GdkEventKey* Event);
	virtual void key_release_event(GdkEventKey* Event);

	sigc::connection connect_mouse_move(const sigc::slot<void, const GdkEventMotion&>& Slot);
	
	sigc::connection connect_lbutton_down(const sigc::slot<void, const GdkEventButton&>& Slot);
	sigc::connection connect_lbutton_up(const sigc::slot<void, const GdkEventButton&>& Slot);
	sigc::connection connect_lbutton_click(const sigc::slot<void, const GdkEventButton&>& Slot);
	sigc::connection connect_lbutton_double_click(const sigc::slot<void, const GdkEventButton&>& Slot);
	sigc::connection connect_lbutton_triple_click(const sigc::slot<void, const GdkEventButton&>& Slot);
	sigc::connection connect_lbutton_start_drag(const sigc::slot<void, const GdkEventMotion&>& Slot);
	sigc::connection connect_lbutton_drag(const sigc::slot<void, const GdkEventMotion&>& Slot);
	sigc::connection connect_lbutton_end_drag(const sigc::slot<void, const GdkEventButton&>& Slot);

	sigc::connection connect_mbutton_down(const sigc::slot<void, const GdkEventButton&>& Slot);
	sigc::connection connect_mbutton_up(const sigc::slot<void, const GdkEventButton&>& Slot);
	sigc::connection connect_mbutton_click(const sigc::slot<void, const GdkEventButton&>& Slot);
	sigc::connection connect_mbutton_double_click(const sigc::slot<void, const GdkEventButton&>& Slot);
	sigc::connection connect_mbutton_triple_click(const sigc::slot<void, const GdkEventButton&>& Slot);
	sigc::connection connect_mbutton_start_drag(const sigc::slot<void, const GdkEventMotion&>& Slot);
	sigc::connection connect_mbutton_drag(const sigc::slot<void, const GdkEventMotion&>& Slot);
	sigc::connection connect_mbutton_end_drag(const sigc::slot<void, const GdkEventButton&>& Slot);

	sigc::connection connect_rbutton_down(const sigc::slot<void, const GdkEventButton&>& Slot);
	sigc::connection connect_rbutton_up(const sigc::slot<void, const GdkEventButton&>& Slot);
	sigc::connection connect_rbutton_click(const sigc::slot<void, const GdkEventButton&>& Slot);
	sigc::connection connect_rbutton_double_click(const sigc::slot<void, const GdkEventButton&>& Slot);
	sigc::connection connect_rbutton_triple_click(const sigc::slot<void, const GdkEventButton&>& Slot);
	sigc::connection connect_rbutton_start_drag(const sigc::slot<void, const GdkEventMotion&>& Slot);
	sigc::connection connect_rbutton_drag(const sigc::slot<void, const GdkEventMotion&>& Slot);
	sigc::connection connect_rbutton_end_drag(const sigc::slot<void, const GdkEventButton&>& Slot);

	sigc::connection connect_scroll(const sigc::slot<void, const GdkEventScroll&>& Slot);
	sigc::connection connect_key_press(const sigc::slot<void, const GdkEventKey&>& Slot);
	sigc::connection connect_key_release(const sigc::slot<void, const GdkEventKey&>& Slot);

private:
	struct implementation;
	implementation* const m_implementation;
};

} // namespace ngui

} // namespace k3d

#endif // !K3DSDK_NGUI_BASIC_INPUT_MODEL_H


