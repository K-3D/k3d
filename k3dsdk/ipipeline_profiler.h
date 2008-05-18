#ifndef K3DSDK_IPIPELINE_PROFILER_H
#define K3DSDK_IPIPELINE_PROFILER_H

// K-3D
// Copyright (c) 1995-2008, Timothy M. Shead
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
#include "types.h"

namespace k3d
{

class inode;
	
/// Abstract interface for an object that collects and distributes profiling data for the K-3D visualization pipeline
class ipipeline_profiler :
	public virtual iunknown
{
public:
	/// Called by a node to indicate that it has begun processing the given task.  Note: it is critical that every call to start_execution() is balanced with a call to finish_execution().
	virtual void start_execution(inode& Node, const string_t& Task) = 0;
	/// Called by a node to indicate that it has finished processing the given task.  Note: it is critical that every call to finish_execution() matches a call to start_execution().
	virtual void finish_execution(inode& Node, const string_t& Task) = 0;

	/// Connects a slot that will be called to report the time in seconds that a node spent processing a given task
	virtual sigc::connection connect_node_execution_signal(const sigc::slot<void, inode&, const string_t&, double>& Slot) = 0;

	/// RAII helper class that records profile information for the current scope with return- and exception-safety
	class profile
	{
	public:
		profile(ipipeline_profiler& Profiler, inode& Node, const string_t& Task) :
			profiler(Profiler),
			node(Node),
			task(Task)
		{
			profiler.start_execution(node, task);
		}

		~profile()
		{
			profiler.finish_execution(node, task);
		}
		
	private:
		profile(const profile&);
		profile& operator=(const profile&);
		
		ipipeline_profiler& profiler;
		inode& node;
		const string_t task;
	};

protected:
	ipipeline_profiler() {}
	ipipeline_profiler(const ipipeline_profiler&) {}
	ipipeline_profiler& operator=(const ipipeline_profiler&) { return *this; }
	virtual ~ipipeline_profiler() {}
};

} // namespace k3d

#endif // !K3DSDK_IPIPELINE_PROFILER_H

