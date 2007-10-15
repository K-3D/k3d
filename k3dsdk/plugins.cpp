// K3D
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
		\brief Implements plugin functions
		\author Tim Shead (tshead@k-3d.com)
*/

#include "inode_collection.h"
#include "inode.h"
#include "plugins.h"
#include "result.h"
#include "utility.h"

namespace k3d
{

iplugin_factory* plugin(const uuid& FactoryID)
{
	for(k3d::iplugin_factory_collection::factories_t::const_iterator factory = k3d::application().plugins().begin(); factory != k3d::application().plugins().end(); ++factory)
	{
		if((*factory)->factory_id() == FactoryID)
			return *factory;
	}

	return 0;
}

const iplugin_factory_collection::factories_t plugins(const std::string FactoryName)
{
	iplugin_factory_collection::factories_t factories;

	for(k3d::iplugin_factory_collection::factories_t::const_iterator factory = k3d::application().plugins().begin(); factory != k3d::application().plugins().end(); ++factory)
	{
		if((*factory)->name() == FactoryName)
			factories.insert(*factory);
	}

	return factories;
}

} // namespace k3d

