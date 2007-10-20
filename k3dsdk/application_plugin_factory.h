#ifndef K3DSDK_APPLICATION_PLUGIN_FACTORY_H
#define K3DSDK_APPLICATION_PLUGIN_FACTORY_H

// K-3D
// Copyright (c) 1995-2007, Timothy M. Shead
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

#include "iapplication_plugin_factory.h"
#include "interface_list.h"
#include "plugin_factory.h"

namespace k3d
{

/////////////////////////////////////////////////////////////////////////////
// application_plugin_factory

template<typename plugin_t, typename interface_list = null_interface>
class application_plugin_factory :
	public plugin_factory,
	public iapplication_plugin_factory
{
public:
	application_plugin_factory(const uuid& FactoryID, const std::string& Name, const std::string& ShortDescription, const std::string& Categories, const quality_t Quality = STABLE, const metadata_t& Metadata = metadata_t()) :
		plugin_factory(FactoryID, Name, ShortDescription, Categories, Quality, Metadata)
	{
	}

	iunknown* create_plugin()
	{
		return new plugin_t();
	}

	bool implements(const std::type_info& InterfaceType)
	{
		return implements_interface<interface_list>()(InterfaceType);
	}

	const interfaces_t interfaces()
	{
		interfaces_t results;
		get_interfaces<interface_list>()(results);
		return results;
	}
};

} // namespace k3d

#endif // !K3DSDK_APPLICATION_PLUGIN_FACTORY_H

