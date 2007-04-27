#ifndef K3DSDK_PERSISTENT_CONTAINER_H
#define K3DSDK_PERSISTENT_CONTAINER_H

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
		\brief Declares k3d::persistence::container and related classes used to automate serialization of data objects
		\author Tim Shead (tshead@k-3d.com)
*/

#include "ipersistent.h"
#include "ipersistent_container.h"

#include <map>

namespace k3d
{

/////////////////////////////////////////////////////////////////////////////
// persistent_container

/**
Mixin class that automates serialization of data objects - derive your parent object
from this one, and call the enable_serialization() method for each child you want
automatically serialized.
*/
class persistent_container :
	public ipersistent_container
{
public:
	void enable_serialization(const std::string& Name, ipersistent& Child);
	void enable_serialization(const named_children_t& Children);
	
	void disable_serialization(ipersistent& Child);
	void disable_serialization(const children_t& Children);

	const named_children_t& persistent_children();

protected:
	persistent_container();
	virtual ~persistent_container();

	void save(xml::element& Element, const ipersistent::save_context& Context);
	void load(xml::element& Element, const ipersistent::load_context& Context);

private:
	persistent_container(const persistent_container&);
	persistent_container& operator=(const persistent_container&);

	/// Contains a collection of data objects to be automatically serialized
	named_children_t m_children;
};

/// Convenience function that returns the set of user properties from within a container
const ipersistent_container::children_t user_properties(ipersistent_container& PersistentContainer);
	
} // namespace k3d

#endif // !K3DSDK_PERSISTENT_CONTAINER_H

