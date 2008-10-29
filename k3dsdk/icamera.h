#ifndef K3DSDK_ICAMERA_H
#define K3DSDK_ICAMERA_H

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
		\brief Declares k3d::icamera, an abstract interface for an object that can act as a camera (provide location and viewing frustum data)
		\author Tim Shead (tshead@k-3d.com)
*/

#include "iunknown.h"

namespace k3d
{

class icrop_window;
class iprojection;
class iproperty;
class itransform_source;

/// Abstract interface for objects that can host (provide position and projection information to) a viewport
class icamera :
	public virtual iunknown
{
public:
	/// Returns the required position data for this camera
	virtual itransform_source& transformation() = 0;
	/// Returns the required viewing projection for this camera
	virtual iprojection& projection() = 0;
	/// Returns the required crop-window for this camera
	virtual icrop_window& crop_window() = 0;
	/// Returns the transformable object that should be altered during interactive user navigation
	virtual itransform_source& navigation_target() = 0;
	/// Retuns the camera's world target, a point (in world coordinates) around which the viewport orbits
	virtual iproperty& world_target() = 0;

protected:
	icamera() {}
	icamera(const icamera&) {}
	icamera& operator=(const icamera&) { return *this; }
	virtual ~icamera() {}
};

} // namespace k3d

#endif // !K3DSDK_ICAMERA_H

