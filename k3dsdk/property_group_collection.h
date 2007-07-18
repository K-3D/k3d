#ifndef K3DSDK_PROPERTY_GROUP_COLLECTION_H
#define K3DSDK_PROPERTY_GROUP_COLLECTION_H

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
		\brief Declares commandnode, which provides a default implementation of icommand_node
		\author Tim Shead (tshead@k-3d.com)
*/

#include "iproperty_group_collection.h"

namespace k3d
{

// Forward declarations
class iproperty;

/////////////////////////////////////////////////////////////////////////////
// property_group_collection

/// Provides a default implementation of iproperty_group_collection
class property_group_collection :
	public iproperty_group_collection
{
public:
	property_group_collection();
	virtual ~property_group_collection();

	// iproperty_group_collection implementation ...
	const groups_t property_groups();

protected:
	/// Clears the collection, removing all groups
	void clear();
	/// Stores a new group
	void register_property_group(const group& Group);
	/// Removes the group(s) named Name
	void unregister_property_group(const std::string& Name);

private:
	/// Contains the collection of property groups
	groups_t m_groups;
};

} // namespace k3d

#endif // K3DSDK_PROPERTY_GROUP_COLLECTION_H

