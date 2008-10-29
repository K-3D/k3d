#ifndef K3DSDK_IPLUGIN_FACTORY_COLLECTION_H
#define K3DSDK_IPLUGIN_FACTORY_COLLECTION_H

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
		\brief Declares iplugin_factory_collection, an abstract interface for a collection of plugin factory objects
		\author Tim Shead (tshead@k-3d.com)
*/

#include "iunknown.h"

#include <set>

namespace k3d
{

// Forward declarations
class iplugin_factory;

/// Abstract factory interface for plugin objects
class iplugin_factory_collection :
	public virtual iunknown
{
public:
	/// Defines a collection of plugin factories
	typedef std::set<iplugin_factory*> factories_t;
	/// Returns the set of available plugin factories
	virtual const factories_t& factories() = 0;

protected:
	iplugin_factory_collection() {}
	iplugin_factory_collection(const iplugin_factory_collection& RHS) {}
	iplugin_factory_collection& operator = (const iplugin_factory_collection& RHS) { return *this; }
	virtual ~iplugin_factory_collection() {}
};

} // namespace k3d

#endif // !K3DSDK_IPLUGIN_FACTORY_COLLECTION_H

