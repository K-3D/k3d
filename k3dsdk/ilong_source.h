#ifndef K3DSDK_ILONG_SOURCE_H
#define K3DSDK_ILONG_SOURCE_H

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
		\brief Declares ilong_source, an interface for objects that can act as a source of long values
		\author Tim Shead (tshead@k-3d.com)
*/

#include "iunknown.h"

namespace k3d
{

class iproperty;

/// Abstract interface for objects that can act as a source of long values
class ilong_source :
	public virtual iunknown
{
public:
	virtual iproperty& long_source_output() = 0;

protected:
	ilong_source() {}
	ilong_source(const ilong_source&) {}
	ilong_source& operator=(const ilong_source&) { return *this; }
	virtual ~ilong_source() {}
};

} // namespace k3d

#endif // K3DSDK_ILONG_SOURCE_H

