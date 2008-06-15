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

#include "high_res_timer.h"
#include "log.h"
#include "pipeline_profiler.h"

#include <iomanip>
#include <stack>

namespace k3d
{

/////////////////////////////////////////////////////////////////////
// pipeline_profiler::implementation

class pipeline_profiler::implementation
{
public:
	sigc::signal<void, inode&, const string_t&, double> node_execution_signal;
	std::stack<timer> timers;
	std::stack<double> adjustments;
};

/////////////////////////////////////////////////////////////////////
// pipeline_profile

pipeline_profiler::pipeline_profiler() :
	m_implementation(new implementation())
{
}

pipeline_profiler::~pipeline_profiler()
{
	delete m_implementation;
}

void pipeline_profiler::start_execution(inode& Node, const string_t& Task)
{
	m_implementation->timers.push(timer());
	m_implementation->adjustments.push(0.0);
}

/**
 * Start a pipeline profiler entry with an adjustment.
 * The adjustment value effectively moves the start forward in time.
 */
void pipeline_profiler::start_execution(inode& Node, const string_t& Task, const double Adjustment)
{
	m_implementation->timers.push(timer());
	m_implementation->adjustments.push(Adjustment);
}

void pipeline_profiler::finish_execution(inode& Node, const string_t& Task)
{
	return_if_fail(m_implementation->timers.size());

	const double elapsed = m_implementation->timers.top().elapsed();
	const double adjustment = m_implementation->adjustments.top();
	m_implementation->node_execution_signal.emit(Node, Task, elapsed - adjustment);

	m_implementation->timers.pop();
	m_implementation->adjustments.pop();

	if(m_implementation->adjustments.size())
		m_implementation->adjustments.top() += elapsed;
}

/**
 * Manually add a timing entry
 */
void pipeline_profiler::add_timing_entry(inode& Node, const string_t& Task, const double TimingValue)
{
	m_implementation->node_execution_signal.emit(Node, Task, TimingValue);
}

sigc::connection pipeline_profiler::connect_node_execution_signal(const sigc::slot<void, inode&, const string_t&, double>& Slot)
{
	return m_implementation->node_execution_signal.connect(Slot);
}

} // namespace k3d

