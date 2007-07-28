#ifndef K3DSDK_IDOCUMENT_H
#define K3DSDK_IDOCUMENT_H

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
		\brief Declares idocument, an abstract interface for an open K-3D document
		\author Tim Shead (tshead@k-3d.com)
*/

#include "iunknown.h"
#include "signal_accumulators.h"
#include "signal_system.h"
#include "uuid.h"

#include <map>
#include <string>

namespace k3d
{

class ipipeline;
class inode_collection;
class inode_name_map;
class ipersistent;
class ipipeline_profiler;
class iproperty;
class istate_recorder;

/// Abstract interface for an open K-3D document
class idocument :
	public virtual iunknown
{
public:
	/// Returns the collection of K-3D objects within this document
	virtual inode_collection& nodes() = 0;
	/// Returns the directed acyclic graph of dependencies between object properties within this document
	virtual ipipeline& pipeline() = 0;
	/// Returns an object that can be used to collect and distribute profiling data for the visualization pipeline
	virtual ipipeline_profiler& pipeline_profiler() = 0;
	/// Returns the istate_recorder interface for this document
	virtual istate_recorder& state_recorder() = 0;

	/// Returns an interface that provides a mapping of nodes to unique names
	virtual inode_name_map& unique_node_names() = 0;
	
	/// Returns a property that will store the document filepath (could be empty)
	virtual iproperty& path() = 0;
	/// Returns a property that will store the document title (could be empty string)
	virtual iproperty& title() = 0;

	/// Registers a handler to be used for serializing per-plugin-type data in a document
	virtual void enable_plugin_serialization(const uuid& ClassID, ipersistent& Handler) = 0;
	/// Defines a collection of plugin serialization handlers
	typedef std::map<uuid, ipersistent*> plugin_serialization_handlers_t;
	virtual const plugin_serialization_handlers_t& plugin_serialization_handlers() = 0;

	/// Defines a signal emitted when the document closes
	typedef sigc::signal<void> close_signal_t;
	virtual close_signal_t& close_signal() = 0;

protected:
	idocument() {}
	idocument(const idocument&) {}
	idocument& operator=(const idocument&) { return *this; }
	virtual ~idocument() {}
};

} // namespace k3d

#endif // K3DSDK_IDOCUMENT_H

