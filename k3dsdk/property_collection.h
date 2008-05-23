#ifndef K3DSDK_PROPERTY_COLLECTION_H
#define K3DSDK_PROPERTY_COLLECTION_H

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

#include "iproperty_collection.h"

namespace k3d
{

/////////////////////////////////////////////////////////////////////////////
// property_collection

/// Provides a default implementation of iproperty_collection
class property_collection :
	public iproperty_collection
{
public:
	property_collection();
	virtual ~property_collection();

	void register_property(iproperty& Property);
	void register_properties(const properties_t& Properties);
	void unregister_property(iproperty& Property);
	void unregister_properties(const properties_t& Properties);
	const properties_t& properties();
	sigc::connection connect_properties_changed_signal(const sigc::slot<void, iunknown*>& Slot);

private:
	/// Contains the collection of all properties
	properties_t m_properties;
	/// Change-notification signal for the collection
	sigc::signal<void, iunknown*> m_changed_signal;
};

} // namespace k3d

#endif // K3DSDK_PROPERTY_COLLECTION_H

