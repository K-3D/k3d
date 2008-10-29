#ifndef K3DSDK_INETWORK_RENDER_FARM_H
#define K3DSDK_INETWORK_RENDER_FARM_H

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
#include "types.h"

namespace k3d
{

class inetwork_render_job;
	
/// Abstract interface that encapsulates a "render farm" capable of executing render jobs
class inetwork_render_farm :
	public virtual iunknown
{
public:
	/// Creates a new job, in a "waiting" state
	virtual inetwork_render_job& create_job(const string_t& JobName) = 0;
	/// Tells the render farm to begin executing the given job
	virtual void start_job(inetwork_render_job& Job) = 0;

protected:
	inetwork_render_farm() {}
	inetwork_render_farm(const inetwork_render_farm&) {}
	inetwork_render_farm& operator=(const inetwork_render_farm&) { return *this; }
	virtual ~inetwork_render_farm() {}
};

} // namespace k3d

#endif // !K3DSDK_INETWORK_RENDER_FARM_H

