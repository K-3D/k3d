#ifndef K3DSDK_IPROPERTY_GROUPS_H
#define K3DSDK_IPROPERTY_GROUPS_H

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
#include "types.h"

#include <vector>

namespace k3d
{

/// Provides an abstract interface grouping a collection of iproperty objects (for UI purposes)
class iproperty_group_collection :
	public virtual iunknown
{
public:
	/// Defines a grouped collection of properties
	struct group
	{
		group()
		{
		}
		
		group(const string_t& Name) :
			name(Name)
		{
		}
		
		group(const string_t& Name, const iproperty_collection::properties_t& Properties) :
			name(Name),
			properties(Properties)
		{
		}
		
		/// Stores a human-readable name for the group
		string_t name;

		/// Stores references to the properties within the group
		iproperty_collection::properties_t properties;
	};
	
	/// Defines a collection of property groups
	typedef std::vector<group> groups_t;
	/// Returns the set of property groups within the object
	virtual const groups_t property_groups() = 0;

protected:
	iproperty_group_collection() {}
	iproperty_group_collection(const iproperty_group_collection&) {}
	iproperty_group_collection& operator=(const iproperty_group_collection&) { return *this; }
	virtual ~iproperty_group_collection() {}
};

} // namespace k3d


#endif // K3DSDK_IPROPERTY_GROUPS_H
