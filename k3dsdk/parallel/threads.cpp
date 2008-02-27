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

#include "threads.h"

#include <k3d-parallel-config.h>

#ifdef K3D_BUILD_PARALLEL
#include <tbb/task_scheduler_init.h>
#endif // K3D_BUILD_PARALLEL

namespace k3d
{

namespace parallel
{

#ifdef K3D_BUILD_PARALLEL

static ::tbb::task_scheduler_init g_scheduler(::tbb::task_scheduler_init::automatic);

void set_thread_count(int32_t Count)
{
	g_scheduler.terminate();

	if(Count == automatic)
		g_scheduler.initialize(::tbb::task_scheduler_init::automatic);
	else
		g_scheduler.initialize(Count);
}

#else // K3D_BUILD_PARALLEL

void set_thread_count(uint32_t Count)
{
}

#endif // !K3D_BUILD_PARALLEL

} // namespace parallel

} // namespace k3d

