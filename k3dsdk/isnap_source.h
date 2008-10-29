#ifndef K3DSDK_ISNAP_SOURCE_H
#define K3DSDK_ISNAP_SOURCE_H

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
#include "point3.h"
#include "types.h"

#include <vector>

namespace k3d
{

class vector3;

/// Abstract interface for a "source" on a snappable object
class isnap_source :
	public virtual iunknown
{
public:
	virtual ~isnap_source() {}

	/// Defines a collection of "snap groups"
	typedef std::vector<string_t> groups_t;
	
	/// Returns a human-readable label that describes this source
	virtual const string_t label() = 0;
	/// Returns a collection of groups that this source is a member of
	virtual const groups_t groups() = 0;
	/// Returns the position of this source in local coordinates
	virtual const point3 source_position() = 0;
	/// Returns the (optional, could return false) orientation of this source in local coordinates
	virtual bool source_orientation(vector3& SourceLook, vector3& SourceUp) = 0;

protected:
	isnap_source() {}
	isnap_source(const isnap_source&) {}
	isnap_source& operator=(const isnap_source&) { return *this; }
};

} // namespace k3d

#endif // !K3DSDK_ISNAP_SOURCE_H

