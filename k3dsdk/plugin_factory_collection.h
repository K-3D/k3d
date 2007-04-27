#ifndef K3DSDK_PLUGIN_FACTORY_COLLECTION_H
#define K3DSDK_PLUGIN_FACTORY_COLLECTION_H

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
		\brief Declares plugin_factory_collection, an implementation of iplugin_factory_collection that can load plugin factories from shared libraries
		\author Tim Shead (tshead@k-3d.com)
*/

#include "iplugin_factory_collection.h"
#include "module.h"
#include "signal_system.h"

#include <string>

namespace k3d
{

namespace filesystem { class path; }

/////////////////////////////////////////////////////////////////////////////
// plugin_factory_collection

/// Provides an implementation of iplugin_factory_collection that can load plugin factories from shared libraries
class plugin_factory_collection :
	public iplugin_factory_collection
{
public:
	plugin_factory_collection();
	~plugin_factory_collection();

	enum load_proxy_t
	{
		LOAD_PROXIES,
		IGNORE_PROXIES
	};

	/// Connects a signal emitted to display human-readable progress messages while loading plugin modules
	sigc::connection connect_message_signal(const sigc::slot<void, const std::string&>& Slot);

	/// Binds a statically-linked plugin module
	void bind_module(const std::string& ModuleName, register_plugins_entry_point RegisterPlugins);
	/// Loads a single plugin module
	void load_module(const filesystem::path& Path, const load_proxy_t LoadProxies);
	/// Loads plugin modules from a directory, optionally descending recursively into subdirectories
	void load_modules(const filesystem::path& Path, const bool Recursive, const load_proxy_t LoadProxies);
	/// Loads plugin modules from zero-to-many directories, optionally descending recursively into each directory
	void load_modules(const std::string& Paths, const bool Recursive, const load_proxy_t LoadProxies);

	// iplugin_factory_collection implementation
	const factories_t& factories();

private:
	plugin_factory_collection(const plugin_factory_collection&);
	plugin_factory_collection& operator=(const plugin_factory_collection&);

	struct implementation;
	implementation* const m_implementation;
};

} // namespace k3d

#endif // !K3DSDK_PLUGIN_FACTORY_COLLECTION_H

