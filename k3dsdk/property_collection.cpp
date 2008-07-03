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

#include "iproperty.h"
#include "iuser_property.h"
#include "property_collection.h"
#include "result.h"

namespace k3d
{

/////////////////////////////////////////////////////////////////////////////
// property_collection

property_collection::property_collection()
{
}

property_collection::~property_collection()
{
	for(properties_t::iterator property = m_properties.begin(); property != m_properties.end(); ++property)
	{
		if(dynamic_cast<iuser_property*>(*property))
			delete *property;
	}
}

void property_collection::register_property(iproperty& Property)
{
	m_properties.erase(std::remove(m_properties.begin(), m_properties.end(), &Property), m_properties.end());
	m_properties.push_back(&Property);
	m_changed_signal.emit(0);
}

void property_collection::register_properties(const properties_t& Properties)
{
	for(properties_t::const_iterator property = Properties.begin(); property != Properties.end(); ++property)
	{
		m_properties.erase(std::remove(m_properties.begin(), m_properties.end(), *property), m_properties.end());
		m_properties.push_back(*property);
	}
	m_properties.erase(std::remove(m_properties.begin(), m_properties.end(), static_cast<iproperty*>(0)), m_properties.end());

	m_changed_signal.emit(0);
}

void property_collection::unregister_property(iproperty& Property)
{
	m_properties.erase(std::remove(m_properties.begin(), m_properties.end(), &Property), m_properties.end());
	m_changed_signal.emit(0);
}

void property_collection::unregister_properties(const properties_t& Properties)
{
	for(properties_t::const_iterator property = Properties.begin(); property != Properties.end(); ++property)
		m_properties.erase(std::remove(m_properties.begin(), m_properties.end(), *property), m_properties.end());

	m_changed_signal.emit(0);
}

const iproperty_collection::properties_t& property_collection::properties()
{
	return m_properties;
}

sigc::connection property_collection::connect_properties_changed_signal(const sigc::slot<void, ihint*>& Slot)
{
	return m_changed_signal.connect(Slot);
}

} // namespace k3d

