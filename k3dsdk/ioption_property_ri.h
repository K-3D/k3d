#ifndef K3DSDK_IOPTION_PROPERTY_RI_H
#define K3DSDK_IOPTION_PROPERTY_RI_H

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
#include "types.h"

namespace k3d
{

namespace ri
{

/// Abstract interface for discovering information about a RenderMan option property 
class ioption_property :
	public virtual iunknown
{
public:
	virtual const string_t property_option_name() = 0;

protected:
	ioption_property() {}
	ioption_property(const ioption_property&) {}
	ioption_property& operator = (const ioption_property&) { return *this; }
	virtual ~ioption_property() {}
};

} // namespace ri

} // namespace k3d

#endif // !K3DSDK_IOPTION_PROPERTY_RI_H

