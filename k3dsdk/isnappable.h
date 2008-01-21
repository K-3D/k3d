#ifndef K3DSDK_ISNAPPABLE_H
#define K3DSDK_ISNAPPABLE_H

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
		\brief Declares isnappable, an abstract interface implemented by objects to which other objects can be interactively "snapped"
		\author Tim Shead (tshead@k-3d.com)
*/

#include "iunknown.h"
#include <vector>

namespace k3d
{

class isnap_source;
class isnap_target;
	
/// Abstract interface implemented by objects to which other objects can be interactively "snapped"
class isnappable :
	public virtual iunknown
{
public:
	virtual ~isnappable() {}

	/// Defines a collection of "snap sources" that describe what parts of this object can be snapped
	typedef std::vector<isnap_source*> snap_sources_t;
	virtual const snap_sources_t snap_sources() = 0;
	
	/// Defines a collection of "snap targets" that describe what parts of this object can be snapped-to
	typedef std::vector<isnap_target*> snap_targets_t;
	virtual const snap_targets_t snap_targets() = 0;

	/// Adds a new snap source to the object (which takes control of the lifetime of the new source)
	virtual void add_snap_source(isnap_source* const SnapSource) = 0;
	/// Adds a new snap target to the object (which takes control of the lifetime of the new target)
	virtual void add_snap_target(isnap_target* const SnapTarget) = 0;
	
protected:
	isnappable() {}
	isnappable(const isnappable&) {}
	isnappable& operator=(const isnappable&) { return *this; }
};

} // namespace k3d

#endif // K3DSDK_ISNAPPABLE_H

