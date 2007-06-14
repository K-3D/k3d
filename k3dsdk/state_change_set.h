#ifndef K3DSDK_STATE_CHANGE_SET_H
#define K3DSDK_STATE_CHANGE_SET_H

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

#include "signal_system.h"

#include <memory>
#include <string>

namespace k3d
{

class idocument;
class istate_container;

/// Stores an atomic set of state changes that can be undone / redone
class state_change_set
{
public:
	state_change_set();
	~state_change_set();

	/// Records an original state that should be restored as part of an "undo" operation (assumes ownership of the given state container)
	void record_old_state(k3d::istate_container* const OldState);
	/// Records a new state that should be restored as part of a "redo" operation (assumes ownership of the given state container)
	void record_new_state(k3d::istate_container* const NewState);

	/// Connects a slot that will be called if this change set is undone
	sigc::connection connect_undo_signal(const sigc::slot<void>& Slot);
	/// Connects a slot that will be called if this change set is redone
	sigc::connection connect_redo_signal(const sigc::slot<void>& Slot);

	/// Restores original states and emits the undo_signal
	void undo();
	/// Restores new states and emits the redo signal
	void redo();

	/// Returns the number of stored undo state containers (mainly for debugging)
	const size_t undo_count() const;
	/// Returns the number of stored redo state containers (mainly for debugging)
	const size_t redo_count() const;
	
private:
	state_change_set(const state_change_set&);
	state_change_set& operator=(const state_change_set&);

	class implementation;
	implementation* const m_implementation;
};

#define K3D_CHANGE_SET_CONTEXT_STRINGIZE2(x) #x
#define K3D_CHANGE_SET_CONTEXT_STRINGIZE(x) K3D_CHANGE_SET_CONTEXT_STRINGIZE2(x)
#define K3D_CHANGE_SET_CONTEXT __FILE__ " (" K3D_CHANGE_SET_CONTEXT_STRINGIZE(__LINE__) ")"

/// Factory function for creating standard state change set objects
std::auto_ptr<state_change_set> create_state_change_set(const char* const Context);
/// Convenience function that starts recording a state change set for undo/redo purposes
void start_state_change_set(idocument& Document, const char* const Context);
/// Convenience function that cancels recording the current state change set
void cancel_state_change_set(idocument& Document, const char* const Context);
/// Convenience function that finishes recording a state change set for undo/redo purposes
void finish_state_change_set(idocument& Document, const std::string& Label, const char* const Context);

/** \brief RAII helper class that provides  a return- and exception-safe way to record state changes for UNDO purposes
	\note You should prefer to use this class whenever possible, instead of start_state_change_set() / finish_state_change_set()
*/
class record_state_change_set
{
public:
	record_state_change_set(idocument& Document, const std::string& Label, const char* const Context);
	~record_state_change_set();

private:
	idocument& m_document;
	const std::string m_label;
	const char* const m_context;
};

} // namespace k3d

#endif // !K3DSDK_STATE_CHANGE_SET_H

