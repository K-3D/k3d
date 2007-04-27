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

#include "persistent_lookup.h"

namespace k3d
{

////////////////////////////////////////////////////////////////////////////////
// persistent_lookup

persistent_lookup::persistent_lookup()
{
	m_id_map[0] = 0;
	m_object_map[0] = 0;
}
	
const ipersistent_lookup::id_type persistent_lookup::lookup_id(iunknown* Object)
{
	if(!m_id_map.count(Object))
	{
		const id_type new_id = m_id_map.size() + 1;
		m_id_map[Object] = new_id;
		m_object_map[new_id] = Object;
	}
	
	return m_id_map[Object];
}

iunknown* persistent_lookup::lookup_object(const id_type ID)
{
	if(!m_object_map.count(ID))
		return 0;

	return m_object_map[ID];
}

void persistent_lookup::insert_lookup(const id_type ID, iunknown* Object)
{
	m_id_map[Object] = ID;
	m_object_map[ID] = Object;
}

} // namespace k3d

