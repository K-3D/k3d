// K-3D
// Copyright (c) 1995-2006, Timothy M. Shead
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

#include "basic_input_model.h"

#include <k3dsdk/log.h>
#include <k3dsdk/vectors.h>

#include <map>

namespace libk3dngui
{

/////////////////////////////////////////////////////////////////////////////
// basic_input_model::implementation

struct basic_input_model::implementation
{
	implementation() :
		double_click(false),
		triple_click(false),
		dragging(0)
	{
	}

	const bool button(const unsigned long Button) const
	{
		button_state_t::const_iterator button = button_state.find(Button);
		if(button == button_state.end())
			return false;

		return button->second;
	}

	typedef std::map<unsigned long, bool> button_state_t;
	button_state_t button_state;

	bool double_click;
	bool triple_click;
	unsigned long dragging;
	k3d::point2 start_mouse;

	sigc::signal<void, const GdkEventMotion&> mouse_move;

	sigc::signal<void, const GdkEventButton&> lbutton_down;
	sigc::signal<void, const GdkEventButton&> lbutton_up;
	sigc::signal<void, const GdkEventButton&> lbutton_click;
	sigc::signal<void, const GdkEventButton&> lbutton_double_click;
	sigc::signal<void, const GdkEventButton&> lbutton_triple_click;
	sigc::signal<void, const GdkEventMotion&> lbutton_start_drag;
	sigc::signal<void, const GdkEventMotion&> lbutton_drag;
	sigc::signal<void, const GdkEventButton&> lbutton_end_drag;

	sigc::signal<void, const GdkEventButton&> mbutton_down;
	sigc::signal<void, const GdkEventButton&> mbutton_up;
	sigc::signal<void, const GdkEventButton&> mbutton_click;
	sigc::signal<void, const GdkEventButton&> mbutton_double_click;
	sigc::signal<void, const GdkEventButton&> mbutton_triple_click;
	sigc::signal<void, const GdkEventMotion&> mbutton_start_drag;
	sigc::signal<void, const GdkEventMotion&> mbutton_drag;
	sigc::signal<void, const GdkEventButton&> mbutton_end_drag;

	sigc::signal<void, const GdkEventButton&> rbutton_down;
	sigc::signal<void, const GdkEventButton&> rbutton_up;
	sigc::signal<void, const GdkEventButton&> rbutton_click;
	sigc::signal<void, const GdkEventButton&> rbutton_double_click;
	sigc::signal<void, const GdkEventButton&> rbutton_triple_click;
	sigc::signal<void, const GdkEventMotion&> rbutton_start_drag;
	sigc::signal<void, const GdkEventMotion&> rbutton_drag;
	sigc::signal<void, const GdkEventButton&> rbutton_end_drag;

	sigc::signal<void, const GdkEventScroll&> scroll;
	sigc::signal<void, const GdkEventKey&> key_press;
	sigc::signal<void, const GdkEventKey&> key_release;
};

/////////////////////////////////////////////////////////////////////////////
// basic_input_model

basic_input_model::basic_input_model() :
	m_implementation(new implementation())
{
}

basic_input_model::~basic_input_model()
{
	delete m_implementation;
}

void basic_input_model::button_press_event(GdkEventButton* Event)
{
	// Maintain button state ...
	m_implementation->button_state[Event->button] = true;

	// Keep track of where the mouse pointer was, so we can detect drag operations ...
	m_implementation->start_mouse = k3d::point2(Event->x, Event->y);

	// Triple-click ...
	if(Event->type == GDK_3BUTTON_PRESS)
	{
		m_implementation->triple_click = true;
		switch(Event->button)
		{
			case 1:
				m_implementation->lbutton_triple_click.emit(*Event);
				break;
			case 2:
				m_implementation->mbutton_triple_click.emit(*Event);
				break;
			case 3:
				m_implementation->rbutton_triple_click.emit(*Event);
				break;
			default:
				break;
		}
		return;
	}

	// Double-click ...
	if(Event->type == GDK_2BUTTON_PRESS)
	{
		m_implementation->double_click = true;
		switch(Event->button)
		{
			case 1:
				m_implementation->lbutton_double_click.emit(*Event);
				break;
			case 2:
				m_implementation->mbutton_double_click.emit(*Event);
				break;
			case 3:
				m_implementation->rbutton_double_click.emit(*Event);
				break;
			default:
				break;
		}
		return;
	}

	// Send the low-level button-down event ...
	switch(Event->button)
	{
		case 1:
			m_implementation->lbutton_down.emit(*Event);
			break;
		case 2:
			m_implementation->mbutton_down.emit(*Event);
			break;
		case 3:
			m_implementation->rbutton_down.emit(*Event);
			break;
		default:
			break;
	}
}

void basic_input_model::button_release_event(GdkEventButton* Event)
{
	// Maintain button state ...
	m_implementation->button_state[Event->button] = false;

	// Send the low-level button-up event ...
	switch(Event->button)
	{
		case 1:
			m_implementation->lbutton_up.emit(*Event);
			break;
		case 2:
			m_implementation->mbutton_up.emit(*Event);
			break;
		case 3:
			m_implementation->rbutton_up.emit(*Event);
			break;
		default:
			break;
	}

	// If we were dragging, handle the drag-end event ...
	if(m_implementation->dragging == Event->button)
	{
		switch(m_implementation->dragging)
		{
			case 1:
				m_implementation->lbutton_end_drag.emit(*Event);
				break;
			case 2:
				m_implementation->mbutton_end_drag.emit(*Event);
				break;
			case 3:
				m_implementation->rbutton_end_drag.emit(*Event);
				break;
			default:
				break;
		}

		m_implementation->dragging = 0;
		return;
	}

	// Send the click event only if this isn't a double-click or a triple-click ...
	if(!m_implementation->double_click && !m_implementation->triple_click)
	{
		switch(Event->button)
		{
			case 1:
				m_implementation->lbutton_click.emit(*Event);
				break;
			case 2:
				m_implementation->mbutton_click.emit(*Event);
				break;
			case 3:
				m_implementation->rbutton_click.emit(*Event);
				break;
			default:
				break;
		}
	}

	m_implementation->double_click = false;
	m_implementation->triple_click = false;
}

void basic_input_model::motion_notify_event(GdkEventMotion* Event)
{
	// Always send mouse movement events ...
	m_implementation->mouse_move.emit(*Event);

	// If we're already dragging, send drag events ...
	if(m_implementation->dragging)
	{
		switch(m_implementation->dragging)
		{
			case 1:
				m_implementation->lbutton_drag.emit(*Event);
				break;
			case 2:
				m_implementation->mbutton_drag.emit(*Event);
				break;
			case 3:
				m_implementation->rbutton_drag.emit(*Event);
				break;
			default:
				break;
		}

		return;
	}

	// If the mouse pointer hasn't moved much lately, we're done ...
	const k3d::point2 mouse = k3d::point2(Event->x, Event->y);
	const k3d::vector2 delta = mouse - m_implementation->start_mouse;

	if(delta.length2() < 9)
		return;

	// Adjust the event structure so it reflects the original start point
	Event->x -= delta[0];
	Event->y -= delta[1];
	Event->x_root -= delta[0];
	Event->y_root -= delta[1];

	// Start dragging ...
	if(m_implementation->button(1))
	{
		m_implementation->dragging = 1;
		m_implementation->lbutton_start_drag.emit(*Event);
	}
	else if(m_implementation->button(2))
	{
		m_implementation->dragging = 2;
		m_implementation->mbutton_start_drag.emit(*Event);
	}
	else if(m_implementation->button(3))
	{
		m_implementation->dragging = 3;
		m_implementation->rbutton_start_drag.emit(*Event);
	}
}

void basic_input_model::scroll_event(GdkEventScroll* Event)
{
	m_implementation->scroll.emit(*Event);
}

void basic_input_model::key_press_event(GdkEventKey* Event)
{
	m_implementation->key_press.emit(*Event);
}

void basic_input_model::key_release_event(GdkEventKey* Event)
{
	m_implementation->key_release.emit(*Event);
}

sigc::connection basic_input_model::connect_mouse_move(const sigc::slot<void, const GdkEventMotion&>& Slot)
{
	return m_implementation->mouse_move.connect(Slot);
}

sigc::connection basic_input_model::connect_lbutton_down(const sigc::slot<void, const GdkEventButton&>& Slot)
{
	return m_implementation->lbutton_down.connect(Slot);
}

sigc::connection basic_input_model::connect_lbutton_up(const sigc::slot<void, const GdkEventButton&>& Slot)
{
	return m_implementation->lbutton_up.connect(Slot);
}

sigc::connection basic_input_model::connect_lbutton_click(const sigc::slot<void, const GdkEventButton&>& Slot)
{
	return m_implementation->lbutton_click.connect(Slot);
}

sigc::connection basic_input_model::connect_lbutton_double_click(const sigc::slot<void, const GdkEventButton&>& Slot)
{
	return m_implementation->lbutton_double_click.connect(Slot);
}

sigc::connection basic_input_model::connect_lbutton_triple_click(const sigc::slot<void, const GdkEventButton&>& Slot)
{
	return m_implementation->lbutton_triple_click.connect(Slot);
}

sigc::connection basic_input_model::connect_lbutton_start_drag(const sigc::slot<void, const GdkEventMotion&>& Slot)
{
	return m_implementation->lbutton_start_drag.connect(Slot);
}

sigc::connection basic_input_model::connect_lbutton_drag(const sigc::slot<void, const GdkEventMotion&>& Slot)
{
	return m_implementation->lbutton_drag.connect(Slot);
}

sigc::connection basic_input_model::connect_lbutton_end_drag(const sigc::slot<void, const GdkEventButton&>& Slot)
{
	return m_implementation->lbutton_end_drag.connect(Slot);
}

sigc::connection basic_input_model::connect_mbutton_down(const sigc::slot<void, const GdkEventButton&>& Slot)
{
	return m_implementation->mbutton_down.connect(Slot);
}

sigc::connection basic_input_model::connect_mbutton_up(const sigc::slot<void, const GdkEventButton&>& Slot)
{
	return m_implementation->mbutton_up.connect(Slot);
}

sigc::connection basic_input_model::connect_mbutton_click(const sigc::slot<void, const GdkEventButton&>& Slot)
{
	return m_implementation->mbutton_click.connect(Slot);
}

sigc::connection basic_input_model::connect_mbutton_double_click(const sigc::slot<void, const GdkEventButton&>& Slot)
{
	return m_implementation->mbutton_double_click.connect(Slot);
}

sigc::connection basic_input_model::connect_mbutton_triple_click(const sigc::slot<void, const GdkEventButton&>& Slot)
{
	return m_implementation->mbutton_triple_click.connect(Slot);
}

sigc::connection basic_input_model::connect_mbutton_start_drag(const sigc::slot<void, const GdkEventMotion&>& Slot)
{
	return m_implementation->mbutton_start_drag.connect(Slot);
}

sigc::connection basic_input_model::connect_mbutton_drag(const sigc::slot<void, const GdkEventMotion&>& Slot)
{
	return m_implementation->mbutton_drag.connect(Slot);
}

sigc::connection basic_input_model::connect_mbutton_end_drag(const sigc::slot<void, const GdkEventButton&>& Slot)
{
	return m_implementation->mbutton_end_drag.connect(Slot);
}

sigc::connection basic_input_model::connect_rbutton_down(const sigc::slot<void, const GdkEventButton&>& Slot)
{
	return m_implementation->rbutton_down.connect(Slot);
}

sigc::connection basic_input_model::connect_rbutton_up(const sigc::slot<void, const GdkEventButton&>& Slot)
{
	return m_implementation->rbutton_up.connect(Slot);
}

sigc::connection basic_input_model::connect_rbutton_click(const sigc::slot<void, const GdkEventButton&>& Slot)
{
	return m_implementation->rbutton_click.connect(Slot);
}

sigc::connection basic_input_model::connect_rbutton_double_click(const sigc::slot<void, const GdkEventButton&>& Slot)
{
	return m_implementation->rbutton_double_click.connect(Slot);
}

sigc::connection basic_input_model::connect_rbutton_triple_click(const sigc::slot<void, const GdkEventButton&>& Slot)
{
	return m_implementation->rbutton_triple_click.connect(Slot);
}

sigc::connection basic_input_model::connect_rbutton_start_drag(const sigc::slot<void, const GdkEventMotion&>& Slot)
{
	return m_implementation->rbutton_start_drag.connect(Slot);
}

sigc::connection basic_input_model::connect_rbutton_drag(const sigc::slot<void, const GdkEventMotion&>& Slot)
{
	return m_implementation->rbutton_drag.connect(Slot);
}

sigc::connection basic_input_model::connect_rbutton_end_drag(const sigc::slot<void, const GdkEventButton&>& Slot)
{
	return m_implementation->rbutton_end_drag.connect(Slot);
}

sigc::connection basic_input_model::connect_scroll(const sigc::slot<void, const GdkEventScroll&>& Slot)
{
	return m_implementation->scroll.connect(Slot);
}

sigc::connection basic_input_model::connect_key_press(const sigc::slot<void, const GdkEventKey&>& Slot)
{
	return m_implementation->key_press.connect(Slot);
}

sigc::connection basic_input_model::connect_key_release(const sigc::slot<void, const GdkEventKey&>& Slot)
{
	return m_implementation->key_release.connect(Slot);
}

} // namespace libk3dngui

