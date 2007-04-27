#ifndef K3DSDK_PAINTER_SELECTION_STATE_GL_H
#define K3DSDK_PAINTER_SELECTION_STATE_GL_H

// K-3D
// Copyright (c) 1995-2006, Timothy M. Shead
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

#include "selection_state_gl.h"

namespace k3d
{

namespace gl
{

/// Used to pass selection state from a node to a mesh painter
class painter_selection_state :
	public selection_state
{
public:
	painter_selection_state(const selection_state& State) :
		selection_state(State)
	{
	}
};

} // namespace gl

} // namespace k3d

#endif // K3DSDK_PAINTER_SELECTION_STATE_GL_H

