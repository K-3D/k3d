#ifndef K3DSDK_IWATCHED_PATH_PROPERTY_H
#define K3DSDK_IWATCHED_PATH_PROPERTY_H

// K-3D
// Copyright (c) 1995-2009, Timothy M. Shead
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
		\brief Declares iwatched_path_property, an abstract interface for discovering information about a property that stores filesystem paths
		\author Tim Shead (tshead@k-3d.com)
		\author Bart Janssens (bart.janssens@lid.kviv.be)
*/

#include "iunknown.h"
#include "signal_system.h"

#include <string>
#include <vector>

namespace k3d
{

/// Abstract interface for properties that represent external filesystem resources (typically used to alter presentation)
class iwatched_path_property :
	public virtual iunknown
{
public:
	/// Set if the associated path should be watched
	virtual void watch(const bool_t Watch) = 0;
	
	/// Returns true if the associated path is being watched, false otherwise
	virtual const bool_t is_watched() const = 0;
	
protected:
	iwatched_path_property() {}
	iwatched_path_property(const iwatched_path_property&) {}
	iwatched_path_property& operator=(const iwatched_path_property&) { return *this; }
	virtual ~iwatched_path_property() {}
};

} // namespace k3d

#endif // !K3DSDK_IWATCHED_PATH_PROPERTY_H

