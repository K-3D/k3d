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

#include "tutorial_message.h"
#include "utility.h"

#include <k3dsdk/result.h>
#include <k3dsdk/system.h>

namespace libk3dngui
{

////////////////////////////////////////////////////////////////////////////////
// tutorial_message::implementation

class tutorial_message::implementation
{
public:
	implementation() :
		m_run_state(IDLE)
	{
	}
	
	sigc::signal<void, const std::string&> show_message_signal;
	sigc::signal<void> move_pointer_signal;
	sigc::signal<void> wait_signal;
	sigc::signal<void> acknowledge_signal;
	sigc::signal<void> hide_messages_signal;

	enum
	{
		IDLE,
		RUN,
		QUIT,
		CONTINUE,
	} m_run_state;

};

/////////////////////////////////////////////////////////////////////////////////
// tutorial_message

tutorial_message& tutorial_message::instance()
{
	static tutorial_message g_instance;
	return g_instance;
}

tutorial_message::tutorial_message() :
	m_implementation(new implementation())
{
}

tutorial_message::~tutorial_message()
{
	delete m_implementation;
}

bool tutorial_message::show_message(const std::string& Message)
{
	// Note: we MUST set the run state before we emit any signals, because they might call continue_message() before it returns
	m_implementation->m_run_state = implementation::RUN;
	
	// Display the message ...
	m_implementation->show_message_signal.emit(Message);

	// Wait for the user to acknowledge ...
	m_implementation->wait_signal.emit();

	// Move the mouse pointer ...
	sigc::signal<void>::slot_list_type slots = m_implementation->move_pointer_signal.slots();
	if(slots.begin() != slots.end())
		slots.back()();
	
	while(m_implementation->m_run_state == implementation::RUN)
	{
		handle_pending_events();
		k3d::system::sleep(0.05);
	}
	const bool result = implementation::CONTINUE == m_implementation->m_run_state ? true : false;
	m_implementation->m_run_state = implementation::IDLE;

	m_implementation->acknowledge_signal.emit();

	return result;
}

void tutorial_message::hide_messages()
{
	if(m_implementation->m_run_state == implementation::RUN)
		m_implementation->m_run_state = implementation::QUIT;

	m_implementation->hide_messages_signal.emit();
}

void tutorial_message::cancel_message()
{
	m_implementation->m_run_state = implementation::QUIT;
	m_implementation->hide_messages_signal.emit();
}

void tutorial_message::continue_message()
{
	m_implementation->m_run_state = implementation::CONTINUE;
}

sigc::connection tutorial_message::connect_show_message_signal(const sigc::slot<void, const std::string&>& Slot)
{
	return m_implementation->show_message_signal.connect(Slot);
}

sigc::connection tutorial_message::connect_move_pointer_signal(const sigc::slot<void>& Slot)
{
	return m_implementation->move_pointer_signal.connect(Slot);
}

sigc::connection tutorial_message::connect_wait_signal(const sigc::slot<void>& Slot)
{
	return m_implementation->wait_signal.connect(Slot);
}

sigc::connection tutorial_message::connect_acknowledge_signal(const sigc::slot<void>& Slot)
{
	return m_implementation->acknowledge_signal.connect(Slot);
}

sigc::connection tutorial_message::connect_hide_messages_signal(const sigc::slot<void>& Slot)
{
	return m_implementation->hide_messages_signal.connect(Slot);
}

} // namespace libk3dui

