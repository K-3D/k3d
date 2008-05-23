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

#include "ipersistent.h"
#include "ipersistent_collection.h"
#include "iproperty_collection.h"
#include "iuser_property.h"
#include "properties.h"
#include "result.h"
#include "user_properties.h"

namespace k3d
{

namespace user
{

////////////////////////////////////////////////////////////////////////////////////////////////////
// property_container

property_container::property_container(iunknown& Owner) :
	m_property_collection(dynamic_cast<iproperty_collection*>(&Owner)),
	m_persistent_collection(dynamic_cast<ipersistent_collection*>(&Owner))
{
	// Keep track of user properties ...
	if(m_property_collection)
		m_user_properties = property::user_properties(Owner);

	// Keep track of serialized properties ...
	if(m_persistent_collection)
	{
		const std::vector<std::pair<string_t, ipersistent*> > persistent_objects = m_persistent_collection->persistent_objects();
		for(uint_t i = 0; i != persistent_objects.size(); ++i)
		{
			if(dynamic_cast<iuser_property*>(persistent_objects[i].second))
			{
				m_persistent_property_names.push_back(persistent_objects[i].first);
				m_persistent_properties.push_back(persistent_objects[i].second);
			}
		}
	}
}

void property_container::restore_state()
{
	// Unregister existing user properties and disable their serialization
	if(m_property_collection)
		m_property_collection->unregister_properties(property::user_properties(*m_property_collection));

	if(m_persistent_collection)
	{
		const std::vector<std::pair<string_t, ipersistent*> > persistent_objects = m_persistent_collection->persistent_objects();
		for(uint_t i = 0; i != persistent_objects.size(); ++i)
		{
			if(dynamic_cast<iuser_property*>(persistent_objects[i].second))
				m_persistent_collection->disable_serialization(*persistent_objects[i].second);
		}
	}

	// Register our properties and enable their serialization
	if(m_property_collection)
		m_property_collection->register_properties(m_user_properties);

	if(m_persistent_collection)
	{
		for(uint_t i = 0; i != m_persistent_property_names.size(); ++i)
			m_persistent_collection->enable_serialization(m_persistent_property_names[i], *m_persistent_properties[i]);
	}
}

} // namespace user

} // namespace k3d

