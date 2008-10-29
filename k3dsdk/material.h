#ifndef K3DSDK_MATERIAL_H
#define K3DSDK_MATERIAL_H

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

#include "classes.h"
#include "inode.h"
#include "iplugin_factory.h"
#include "iproperty_collection.h"
#include "properties.h"

namespace k3d
{

class iunknown;

namespace material
{

/// Given a generic material object, return a specific material type (could return NULL).
template<typename InterfaceT>
InterfaceT* lookup(iunknown* const Material)
{
	if(InterfaceT* const interface = dynamic_cast<InterfaceT*>(Material))
		return interface;

	if(inode* const node = dynamic_cast<inode*>(Material))
	{
		if(node->factory().factory_id() == classes::MultiMaterial())
		{
			if(iproperty_collection* const property_collection = dynamic_cast<iproperty_collection*>(Material))
			{
				const iproperty_collection::properties_t& properties = property_collection->properties();
				for(iproperty_collection::properties_t::const_iterator property = properties.begin(); property != properties.end(); ++property)
				{
					if((**property).property_type() == typeid(inode*))
					{
						if(InterfaceT* const interface = dynamic_cast<InterfaceT*>(k3d::property::pipeline_value<inode*>(**property)))
							return interface;
					}
				}
			}
		}
	}

	return 0;
}

} // namespace material

} // namespace k3d

#endif // !K3DSDK_MATERIAL_H

