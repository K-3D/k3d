#ifndef K3DSDK_PLUGIN_FACTORY_H
#define K3DSDK_PLUGIN_FACTORY_H

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

#include "iplugin_factory.h"
#include "uuid.h"

#include <string>

namespace k3d
{

/////////////////////////////////////////////////////////////////////////////
// plugin_factory

/// Provides a boilerplate implementation of iplugin_factory
class plugin_factory :
	public iplugin_factory
{
public:
	plugin_factory(const uuid& FactoryID, const std::string& Name, const std::string& ShortDescription, const std::string& Categories, const quality_t Quality, const metadata_t& Metadata = metadata_t());

	const uuid& factory_id();
	const std::string name();
	const std::string short_description();
	const categories_t& categories();
	quality_t quality();
	const metadata_t& metadata();

private:
	const uuid m_factory_id;
	const std::string m_name;
	const std::string m_short_description;
	categories_t m_categories;
	const quality_t m_quality;
	const metadata_t m_metadata;
};

} // namespace k3d

#endif // !K3DSDK_PLUGIN_FACTORY_H

