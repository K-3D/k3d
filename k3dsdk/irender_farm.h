#ifndef K3DSDK_IRENDER_FARM_H
#define K3DSDK_IRENDER_FARM_H

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

class irender_job;
	
/// Abstract interface that encapsulates a "render farm" capable of executing render jobs
class irender_farm :
	public virtual iunknown
{
public:
	/// Creates a new job, in a "waiting" state
	virtual irender_job& create_job(const std::string& JobName) = 0;
	/// Tells the render farm to begin executing the given job
	virtual void start_job(irender_job& Job) = 0;

protected:
	irender_farm() {}
	irender_farm(const irender_farm&) {}
	irender_farm& operator=(const irender_farm&) { return *this; }
	virtual ~irender_farm() {}
};

} // namespace k3d

#endif // K3DSDK_IRENDER_FARM_H

