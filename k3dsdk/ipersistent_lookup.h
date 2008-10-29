#ifndef K3DSDK_IPERSISTENT_LOOKUP_H
#define K3DSDK_IPERSISTENT_LOOKUP_H

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
		\brief Declares ipersistent_lookup lookup, an abstract interface to assign/lookup unique identifiers for object serialization
		\author Tim Shead (tshead@k-3d.com)
*/

#include "iunknown.h"

namespace k3d
{

/// Abstract interface to assign / lookup unique identifiers for object serialization
class ipersistent_lookup :
	public virtual iunknown
{
public:
	/// Defines a unique identifier that can be serialized
	typedef unsigned long id_type;
	/// Returns a unique identifier for the given object that can be used in serialization
	virtual const id_type lookup_id(iunknown* Object) = 0;
	/// Given a unique identifier, returns the corresponding object (could return NULL)
	virtual iunknown* lookup_object(const id_type ID) = 0;
	

protected:
	ipersistent_lookup() {}
	ipersistent_lookup(const ipersistent_lookup&) {}
	ipersistent_lookup& operator=(const ipersistent_lookup&) { return *this; }
	virtual ~ipersistent_lookup() {}
};

} // namespace k3d

#endif // !K3DSDK_IPERSISTENT_LOOKUP_H

