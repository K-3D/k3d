#ifndef K3DSDK_ITRANSFORM_SINK_H
#define K3DSDK_ITRANSFORM_SINK_H

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
		\brief Declares itransform_sink, an interface for objects that can act as consumers of transform data
		\author Tim Shead (tshead@k-3d.com)
*/

#include "iunknown.h"

namespace k3d
{

// Forward declarations
class iproperty;

/// Abstract interface for objects that can act as consumers of transformation data
class itransform_sink :
	public virtual iunknown
{
public:
	virtual iproperty& transform_sink_input() = 0;

protected:
	itransform_sink() {}
	itransform_sink(const itransform_sink& Other) : iunknown(Other) {}
	itransform_sink& operator=(const itransform_sink&) { return *this; }
	virtual ~itransform_sink() {}
};

} // namespace k3d

#endif // !K3DSDK_ITRANSFORM_SINK_H

