#ifndef K3DSDK_ISNAP_TARGET_H
#define K3DSDK_ISNAP_TARGET_H

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

#include "iunknown.h"
#include "types.h"

#include <vector>

namespace k3d
{

class vector3;
class point3;
	
/// Abstract interface for a "target" on a snappable object
class isnap_target :
	public virtual iunknown
{
public:
	virtual ~isnap_target() {}

	/// Defines a collection of "snap groups"
	typedef std::vector<string_t> groups_t;
	
	/// Returns a human-readable label that describes this target
	virtual const string_t label() = 0;
	/// Returns a collection of groups that this target is a member of
	virtual const groups_t groups() = 0;
	/// Given a set of local coordinates, returns the (optional, could return false) corresponding target position in local coordiantes
	virtual bool target_position(const point3& Position, point3& TargetPosition) = 0;
	/// Given a set of local coordinates, returns the (optional, could return false) corresponding target orientation in local coordinates
	virtual bool target_orientation(const point3& Position, vector3& TargetLook, vector3& TargetUp) = 0;

protected:
	isnap_target() {}
	isnap_target(const isnap_target&) {}
	isnap_target& operator=(const isnap_target&) { return *this; }
};

} // namespace k3d

#endif // K3DSDK_ISNAP_TARGET_H

