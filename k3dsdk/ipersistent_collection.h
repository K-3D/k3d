#ifndef K3DSDK_IPERSISTENT_COLLECTION_H
#define K3DSDK_IPERSISTENT_COLLECTION_H

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

#include <vector>

namespace k3d
{

class ipersistent;

/// Encapsulates a collection of named, serializable objects.
class ipersistent_collection :
	public virtual iunknown
{
public:
	/// Add a serializable object to the collection, specifying its name.  Note that names are constrained to be unique within the collection.
	virtual void enable_serialization(const string_t& Name, ipersistent& Object) = 0;
	/// Remove a serializable object from the collection.
	virtual void disable_serialization(ipersistent& Object) = 0;
	/// Returns the set of serializable objects and their names, in the order they were added.
	virtual const std::vector<std::pair<string_t, ipersistent*> > persistent_objects() = 0;

protected:
	ipersistent_collection() {}
	ipersistent_collection(const ipersistent_collection&) {}
	ipersistent_collection& operator=(const ipersistent_collection&) { return *this; }
	virtual ~ipersistent_collection() {}
};

} // namespace k3d

#endif // K3DSDK_IPERSISTENT_COLLECTION_H

