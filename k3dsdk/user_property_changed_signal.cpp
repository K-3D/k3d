// K-3D
// Copyright (c) 1995-2005, Timothy M. Shead
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

#include "iproperty.h"
#include "iproperty_collection.h"
#include "iuser_property.h"
#include "user_property_changed_signal.h"

#include "log.h"
#include <iomanip>

namespace k3d
{

user_property_changed_signal::user_property_changed_signal(iproperty_collection& Collection) :
	m_collection(Collection)
{
	Collection.connect_properties_changed_signal(sigc::mem_fun(*this, &user_property_changed_signal::on_collection_changed));
	on_collection_changed(0);
}

sigc::connection user_property_changed_signal::connect(const sigc::slot<void, ihint*>& Slot)
{
	return m_changed_signal.connect(Slot);
}

void user_property_changed_signal::on_collection_changed(ihint*)
{
	for(connections_t::iterator connection = m_connections.begin(); connection != m_connections.end(); ++connection)
		connection->disconnect();
	m_connections.clear();

	const iproperty_collection::properties_t& properties = m_collection.properties();
	for(iproperty_collection::properties_t::const_iterator property = properties.begin(); property != properties.end(); ++property)
	{
		if(dynamic_cast<iuser_property*>(*property))
			m_connections.push_back((*property)->property_changed_signal().connect(m_changed_signal.make_slot()));
	}
}

} // namespace k3d

