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

#include "persistent_container.h"
#include "property_collection.h"
#include "user_properties.h"

namespace k3d
{

namespace user
{

////////////////////////////////////////////////////////////////////////////////////////////////////
// property_container

property_container::property_container(iunknown& Owner) :
	m_property_collection(dynamic_cast<iproperty_collection*>(&Owner)),
	m_persistent_container(dynamic_cast<ipersistent_container*>(&Owner))
{
	// Keep track of every user property in the collection ...
	if(m_property_collection)
		m_user_properties = user_properties(*m_property_collection);

	// Keep track of named properties that are serializable ...
	if(m_persistent_container)
	{
		const ipersistent_container::named_children_t& persistent_children = m_persistent_container->persistent_children();
		for(ipersistent_container::named_children_t::const_iterator child = persistent_children.begin(); child != persistent_children.end(); ++child)
		{
			if(dynamic_cast<iuser_property*>(child->second))
				m_persistent_properties.insert(*child);
		}
	}
}

void property_container::restore_state()
{
	// Unregister existing user properties and disable their serialization
	if(m_property_collection)
		m_property_collection->unregister_properties(user_properties(*m_property_collection));
	if(m_persistent_container)
		m_persistent_container->disable_serialization(user_properties(*m_persistent_container));

	// Register our properties and enable their serialization
	if(m_property_collection)
		m_property_collection->register_properties(m_user_properties);
	if(m_persistent_container)
		m_persistent_container->enable_serialization(m_persistent_properties);
}

} // namespace user

} // namespace k3d

