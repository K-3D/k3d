#ifndef K3DSDK_IENUMERATION_PROPERTY_H
#define K3DSDK_IENUMERATION_PROPERTY_H

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
		\brief Declares ienumeration_property, an abstract interface for discovering information about an enumerated property
		\author Tim Shead (tshead@k-3d.com)
*/

#include "iunknown.h"
#include "signal_system.h"
#include "types.h"

#include <vector>

namespace k3d
{

/// Abstract interface for discovering information about an enumerated property
class ienumeration_property :
	public virtual iunknown
{
public:
	/// Stores information about an individual enumeration value
	struct enumeration_value_t
	{
		enumeration_value_t(const k3d::string_t& Label, const k3d::string_t& Value, const k3d::string_t& Description) :
			label(Label),
			value(Value),
			description(Description)
		{
		}
		
		k3d::string_t label;
		k3d::string_t value;
		k3d::string_t description;
	};
	
	/// Defines a collection of enumeration values
	typedef std::vector<enumeration_value_t> enumeration_values_t;
	
	/// Returns the set of all valid values for this property
	virtual enumeration_values_t enumeration_values() = 0;
	/// Connects a slot to a signal that will be emitted whenever the set of enumeration values changes
	virtual sigc::connection connect_enumeration_values_changed(const sigc::slot<void>& Slot) = 0;

protected:
	ienumeration_property() {}
	ienumeration_property(const ienumeration_property&) {}
	ienumeration_property& operator = (const ienumeration_property&) { return *this; }
	virtual ~ienumeration_property() {}
};

} // namespace k3d

#endif // K3DSDK_IENUMERATION_PROPERTY_H

