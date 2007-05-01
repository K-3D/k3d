#ifndef K3DSDK_SELECTABLE_H
#define K3DSDK_SELECTABLE_H

// K-3D
// Copyright (c) 1995-2005, Timothy M. Shead
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

#include "iselectable.h"

namespace k3d
{

/// Provides a boilerplate implementation of k3d::iselectable
/** \deprecated Get rid of this once k3d::legacy::mesh is gone */
class selectable :
	public iselectable
{
public:
	selectable() :
		selection_weight(0.0)
	{
	}

	double get_selection_weight()
	{
		return selection_weight;
	}

	void set_selection_weight(const double Weight)
	{
		selection_weight = Weight;
	}

	double selection_weight;
};

} // namespace k3d

#endif // !K3DSDK_SELECTABLE_H


