#ifndef K3DSDK_IMATERIAL_SINK_H
#define K3DSDK_IMATERIAL_SINK_H

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

#include "iunknown.h"

namespace k3d
{

class iproperty;

/// Abstract interface for objects that can "consume" an imaterial
class imaterial_sink :
	public virtual iunknown
{
public:
	virtual iproperty& material_sink_input() = 0;

protected:
	imaterial_sink() {}
	imaterial_sink(const imaterial_sink&) {}
	imaterial_sink& operator=(const imaterial_sink&) { return *this; }
	virtual ~imaterial_sink() {}
};

} // namespace k3d

#endif // !K3DSDK_IMATERIAL_SINK_H

