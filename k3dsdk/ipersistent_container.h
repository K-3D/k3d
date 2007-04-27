#ifndef K3DSDK_IPERSISTENT_CONTAINER_H
#define K3DSDK_IPERSISTENT_CONTAINER_H

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
		\author Tim Shead (tshead@k-3d.com)
*/

#include "iunknown.h"

#include <map>
#include <string>
#include <vector>

namespace k3d
{

class ipersistent;

/// Abstract interface implemented by objects that can automatically serialize their "children"
class ipersistent_container :
	public virtual iunknown
{
public:
	/// Defines a collection of persistent data objects
	typedef std::vector<ipersistent*> children_t;
	/// Defines a collection of persistent data objects with names
	typedef std::map<std::string, ipersistent*> named_children_t;
	
	virtual void enable_serialization(const std::string& Name, ipersistent& Child) = 0;
	virtual void enable_serialization(const named_children_t& Children) = 0;
	
	virtual void disable_serialization(ipersistent& Child) = 0;
	virtual void disable_serialization(const children_t& Children) = 0;

	virtual const named_children_t& persistent_children() = 0;
	
protected:
	ipersistent_container() {}
	ipersistent_container(const ipersistent_container&) {}
	ipersistent_container& operator=(const ipersistent_container&) { return *this; }
	virtual ~ipersistent_container() {}
};

} // namespace k3d

#endif // K3DSDK_IPERSISTENT_CONTAINER_H

