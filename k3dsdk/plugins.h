#ifndef K3DSDK_PLUGINS_H
#define K3DSDK_PLUGINS_H

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
		\brief Declares plugin functions
		\author Tim Shead (tshead@k-3d.com)
*/

#include "application.h"
#include "iapplication.h"
#include "iplugin_factory.h"
#include "iplugin_factory_collection.h"

#include <typeinfo>

namespace k3d
{

// Forward declarations
class inode;
class uuid;

/// Convenience typedef
typedef iplugin_factory_collection::factories_t factories_t;

/// Returns the plugin that implements a specific class_id (could return NULL)
iplugin_factory* plugin(const uuid& ClassID);

/// Returns the set of plugins that match the given plugin name (could potentially return more than one result - you should be prepared to handle this)
const factories_t plugins(const std::string PluginName);

/// Returns the set of plugins that implement a specific interface type
template<typename interface_t>
const factories_t plugins()
{
	factories_t factories;
	for(factories_t::const_iterator factory = application().plugins().begin(); factory != application().plugins().end(); ++factory)
		if((*factory)->implements(typeid(interface_t)))
			factories.insert(*factory);

	return factories;
}

} // namespace k3d

#endif // !K3DSDK_PLUGINS_H

