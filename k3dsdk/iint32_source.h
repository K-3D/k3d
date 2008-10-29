#ifndef K3DSDK_IINT32_SOURCE_H
#define K3DSDK_IINT32_SOURCE_H

// K-3D
// Copyright (c) 1995-2008, Timothy M. Shead
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
// License aint32 with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

/** \file
	\author Tim Shead (tshead@k-3d.com)
*/

#include "iunknown.h"

namespace k3d
{

class iproperty;

/// Abstract interface for objects that can act as a source of k3d::int32_t values
class iint32_source :
	public virtual iunknown
{
public:
	virtual iproperty& int32_source_output() = 0;

protected:
	iint32_source() {}
	iint32_source(const iint32_source&) {}
	iint32_source& operator=(const iint32_source&) { return *this; }
	virtual ~iint32_source() {}
};

} // namespace k3d

#endif // !K3DSDK_IINT32_SOURCE_H

