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
		\author Dan Erikson (derikson@montana.com)
*/

#include "idocument.h"
#include "istate_container.h"
#include "istate_recorder.h"
#include "result.h"
#include "state_change_set.h"
#include "utility.h"

#include <algorithm>
#include <functional>
#include <vector>

namespace k3d
{

/////////////////////////////////////////////////////////////////////////////
// state_change_set::implementation

class state_change_set::implementation
{
public:
	implementation()
	{
	}

	~implementation()
	{
		std::for_each(m_old_states.begin(), m_old_states.end(), delete_object());
		std::for_each(m_new_states.begin(), m_new_states.end(), delete_object());
	}

	typedef std::vector<istate_container*> state_collection_t;
	state_collection_t m_old_states;
	state_collection_t m_new_states;

	sigc::signal<void> m_undo_signal;
	sigc::signal<void> m_redo_signal;
};
	
/////////////////////////////////////////////////////////////////////////////
// state_change_set

state_change_set::state_change_set() :
	m_implementation(new implementation())
{
}

state_change_set::~state_change_set()
{
	delete m_implementation;
}

void state_change_set::record_old_state(istate_container* const OldState)
{
	return_if_fail(OldState);
	m_implementation->m_old_states.push_back(OldState);
}

void state_change_set::record_new_state(istate_container* const NewState)
{
	return_if_fail(NewState);
	m_implementation->m_new_states.push_back(NewState);
}

sigc::connection state_change_set::connect_undo_signal(const sigc::slot<void>& Slot)
{
	return m_implementation->m_undo_signal.connect(Slot);
}

sigc::connection state_change_set::connect_redo_signal(const sigc::slot<void>& Slot)
{
	return m_implementation->m_redo_signal.connect(Slot);
}

void state_change_set::undo()
{
	std::for_each(m_implementation->m_old_states.rbegin(), m_implementation->m_old_states.rend(), std::mem_fun(&istate_container::restore_state));
	m_implementation->m_undo_signal.emit();
}

void state_change_set::redo()
{
	std::for_each(m_implementation->m_new_states.begin(), m_implementation->m_new_states.end(), std::mem_fun(&istate_container::restore_state));
	m_implementation->m_redo_signal.emit();
}

const size_t state_change_set::undo_count() const
{
	return m_implementation->m_old_states.size();
}

const size_t state_change_set::redo_count() const
{
	return m_implementation->m_new_states.size();
}

/////////////////////////////////////////////////////////////////////////////
// create_state_change_set

std::auto_ptr<state_change_set> create_state_change_set(const char* const Context)
{
	return std::auto_ptr<state_change_set>(new state_change_set());
}

/////////////////////////////////////////////////////////////////////////////
// start_state_change_set

void start_state_change_set(idocument& Document, const char* const Context)
{
	Document.state_recorder().start_recording(create_state_change_set(Context), Context);
}

/////////////////////////////////////////////////////////////////////////////
// cancel_state_change_set

void cancel_state_change_set(idocument& Document, const char* const Context)
{
	// Tell the document to stop recording ...
	const std::auto_ptr<state_change_set> changeset(Document.state_recorder().stop_recording(Context));

	// Undo any changes up to this point, and let the leftover data get destroyed ...
	return_if_fail(changeset.get());
	changeset->undo();
}

/////////////////////////////////////////////////////////////////////////////
// finish_state_change_set

void finish_state_change_set(idocument& Document, const std::string& Label, const char* const Context)
{
	Document.state_recorder().commit_change_set(Document.state_recorder().stop_recording(Context), Label, Context);
}

/////////////////////////////////////////////////////////////////////////////
// record_state_change_set

record_state_change_set::record_state_change_set(idocument& Document, const std::string& Label, const char* const Context) :
	m_document(Document),
	m_label(Label),
	m_context(Context)
{
	// Sanity checks ...
	assert_warning(m_label.size());

	start_state_change_set(m_document, m_context);
}

record_state_change_set::~record_state_change_set()
{
	finish_state_change_set(m_document, m_label, m_context);
}

} //namespace k3d

