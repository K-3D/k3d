#ifndef K3DSDK_IPROPERTY_COLLECTION_H
#define K3DSDK_IPROPERTY_COLLECTION_H

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
		\brief Declares iproperty_collection, an abstract interface for wrapping a collection of iproperty objects
		\author Tim Shead (tshead@k-3d.com)
*/

#include "ihint.h"
#include "signal_system.h"

#include <vector>

namespace k3d
{

class iproperty;

/// Abstract interface for a collection of iproperty objects
class iproperty_collection :
	public virtual iunknown
{
public:
	/// Defines a collection of properties
	typedef std::vector<iproperty*> properties_t;

	/// Registers a property with the collection - note: the collection will assume responsibility for the lifetime of user properties only
	virtual void register_property(iproperty& Property) = 0;
	/// Registers a set of properties with the collection - note: the collection will assume responsibility for the lifetime of user properties only
	virtual void register_properties(const properties_t& Properties) = 0;
	
	/// Removes a property from the collection
	virtual void unregister_property(iproperty& Property) = 0;
	/// Removes a set of properties from the collection
	virtual void unregister_properties(const properties_t& Properties) = 0;

	/// Returns the set of all properties in the collection
	virtual const properties_t& properties() = 0;
	/// Connects a slot to a signal emitted whenever the collection changes - note: the signal is emitted when the collection changes (properties are registered and unregistered), not when individual property values are changed
	virtual sigc::connection connect_properties_changed_signal(const sigc::slot<void, ihint*>& Slot) = 0;

protected:
	iproperty_collection() {}
	iproperty_collection(const iproperty_collection& Other) : iunknown(Other) {}
	iproperty_collection& operator=(const iproperty_collection&) { return *this; }
	virtual ~iproperty_collection() {}
};

} // namespace k3d

#endif // !K3DSDK_IPROPERTY_COLLECTION_H

