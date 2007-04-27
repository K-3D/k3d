#ifndef K3DSDK_ISTATE_RECORDER_H
#define K3DSDK_ISTATE_RECORDER_H

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

#include "iunknown.h"
#include "signal_system.h"

#include <memory>
#include <string>
#include <vector>

namespace k3d
{

class state_change_set;
	
/// Abstract interface for an object that can store a hierarchical tree of state changes to the document
class istate_recorder :
	public virtual iunknown
{
public:
	/// Called by clients to register a change set for recording subsequent state changes (the recorder assumes responsibility for the lifetime of the changeset)
	virtual void start_recording(std::auto_ptr<state_change_set> ChangeSet, const char* const DebugLabel) = 0;
	/// Returns the current change set being recorded (if any - could return NULL)
	virtual state_change_set* current_change_set() = 0;
	/// Called by clients to stop recording a set of state changes (the recorder relinquishes responsibility for the lifetime of the returned changeset)
	virtual std::auto_ptr<state_change_set> stop_recording(const char* const DebugLabel) = 0;
	/// Called by clients once a set of changes is complete, to make them a part of the undo/redo tree (the recorder assumes responsibility for the lifetime of the change set)
	virtual void commit_change_set(std::auto_ptr<state_change_set> ChangeSet, const std::string& Label, const char* const DebugLabel) = 0;

	/// Defines a collection of "nodes" in the hierarchy of recorded state changes
	struct node;
	typedef std::vector<node*> nodes_t;

	/// Encapsulates a single "node" in the hierarchy of recorded state changes
	struct node
	{
		node(const std::string& Label, state_change_set* const ChangeSet, node* const Parent) : label(Label), change_set(ChangeSet), parent(Parent) { }

		/// Stores the human-readable label for this node
		const std::string label;
		/// Points to the change set owned by this node
		state_change_set* const change_set;
		/// Points to this node's parent (NULL for the root node)
		node* const parent;
		/// Points to this node's children
		nodes_t children;
	};

	/// Returns the root node(s) in the hierarchy of recorded state changes (could return an empty collection if no recording has taken place)
	virtual const nodes_t& root_nodes() = 0;
	/// Returns the current node - the node that will become the parent of any subsequently recorded nodes (could return NULL)
	virtual const node* current_node() = 0;
	/// Returns the newest (most-recently-created) node, which is typically the node that should be the "goal" of redo operations (could return NULL if nothing's been recorded yet)
	virtual const node* newest_node() = 0;
	/// Returns the most-recently-saved node in the hierarchy of recorded state changes (could return NULL if no save has taken place)
	virtual const node* last_saved_node() = 0;

	/// Called to set the current node
	virtual void set_current_node(const node* const Node) = 0;
	/// Called to mark the current node as saved
	virtual void mark_saved() = 0;

	/// Connects a slot that will be called when recording finishes
	virtual sigc::connection connect_recording_done_signal(const sigc::slot<void>& Slot) = 0;
	
	/// Connects a slot that will be called after a node is added to the hierarchy
	virtual sigc::connection connect_node_added_signal(const sigc::slot<void, const node*>& Slot) = 0;
	/// Connects a slot that will be called when the current node has changed
	virtual sigc::connection connect_current_node_changed_signal(const sigc::slot<void>& Slot) = 0;
	/// Connects a slot that will be called when the last saved node has changed
	virtual sigc::connection connect_last_saved_node_changed_signal(const sigc::slot<void>& Slot) = 0;

protected:
	istate_recorder() {}
	istate_recorder(const istate_recorder& RHS) {}
	istate_recorder& operator = (const istate_recorder& RHS) { return *this; }
	virtual ~istate_recorder() {}
};

} // namespace k3d

#endif // K3DSDK_ISTATE_RECORDER_H

