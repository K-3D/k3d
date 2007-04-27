#ifndef K3DSDK_PERSISTENT_LOOKUP_H
#define K3DSDK_PERSISTENT_LOOKUP_H

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

#include "ipersistent_lookup.h"
#include <map>

namespace k3d
{

/// Concrete implementation of ipersistent_lookup
class persistent_lookup :
	public ipersistent_lookup
{
public:
	persistent_lookup();

	const id_type lookup_id(iunknown* Object);
	iunknown* lookup_object(const id_type ID);

	void insert_lookup(const id_type ID, iunknown* Object);
	
private:
	std::map<iunknown*, id_type> m_id_map;
	std::map<id_type, iunknown*> m_object_map;
};

} // namespace k3d

#endif // K3DSDK_PERSISTENT_LOOKUP_H

