#ifndef K3DSDK_INETWORK_RENDER_JOB_H
#define K3DSDK_INETWORK_RENDER_JOB_H

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
*/

#include "iunknown.h"
#include <string>

namespace k3d
{

class inetwork_render_frame;
	
/// Abstract interface encapsulating a render job containing zero-to-many frames to be rendered
class inetwork_render_job :
	public virtual iunknown
{
public:
	/// Adds a new "frame" to the job, to be rendered when the job is run
	virtual inetwork_render_frame& create_frame(const std::string& FrameName) = 0;

protected:
	inetwork_render_job() {}
	inetwork_render_job(const inetwork_render_job&) {}
	inetwork_render_job& operator=(const inetwork_render_job&) { return *this; }
	virtual ~inetwork_render_job() {}
};

} // namespace k3d

#endif // K3DSDK_INETWORK_RENDER_JOB_H

