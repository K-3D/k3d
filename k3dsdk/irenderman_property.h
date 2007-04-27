#ifndef K3DSDK_IRENDERMAN_PROPERTY_H
#define K3DSDK_IRENDERMAN_PROPERTY_H

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

#include <iosfwd>
#include <string>
#include <vector>

namespace k3d
{

/// Abstract interface for discovering information about a RenderMan property (one that has an attribute or option name)
class irenderman_property :
	public virtual iunknown
{
public:
	typedef enum
	{
		ATTRIBUTE,
		OPTION
	} parameter_type_t;

	virtual const parameter_type_t property_parameter_list_type() = 0;
	virtual const std::string property_parameter_list_name() = 0;

protected:
	irenderman_property() {}
	irenderman_property(const irenderman_property&) {}
	irenderman_property& operator = (const irenderman_property&) { return *this; }
	virtual ~irenderman_property() {}
};

} // namespace k3d

#endif // K3DSDK_IRENDERMAN_PROPERTY_H

