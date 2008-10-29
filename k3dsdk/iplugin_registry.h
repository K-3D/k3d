#ifndef K3DSDK_IPLUGIN_REGISTRY_H
#define K3DSDK_IPLUGIN_REGISTRY_H

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
		\brief Declares iplugin_registry, an abstract interface passed to plugin module entry points so they may register their plugin factories
		\author Tim Shead (tshead@k-3d.com)
*/

#include "iunknown.h"

namespace k3d
{

// Forward declarations ...
class iplugin_factory;

/**
\brief Abstract interface passed to plugin module entry points so they may register their plugin factories
\note
Your K-3D plugin module should export a function named "register_k3d_plugins" with the module_entry_point signature, which will
be called at program startup.  In the implementation of "register_k3d_plugins", call register_factory() once for every plugin type your module provides.
*/
class iplugin_registry :
	public virtual iunknown
{
public:
	/// Called to register a new plugin factory object
	virtual void register_factory(iplugin_factory& Factory) = 0;

protected:
	iplugin_registry() {}
	iplugin_registry(const iplugin_registry&) {}
	iplugin_registry& operator=(const iplugin_registry&) { return *this; }
	virtual ~iplugin_registry() {}
};

} // namespace k3d

#endif // !K3DSDK_IPLUGIN_REGISTRY_H
