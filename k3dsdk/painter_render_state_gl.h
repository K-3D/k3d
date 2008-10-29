#ifndef K3DSDK_PAINTER_RENDER_STATE_GL_H
#define K3DSDK_PAINTER_RENDER_STATE_GL_H

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

#include "render_state_gl.h"

namespace k3d
{

namespace gl
{

/// Used to pass rendering state from a node to a mesh painter
class painter_render_state :
	public render_state
{
public:
	painter_render_state(const render_state& State, const matrix4& Matrix, const bool ShowComponentSelection) :
		render_state(State),
		matrix(Matrix),
		inside_out(k3d::inside_out(Matrix)),
		show_component_selection(ShowComponentSelection)
	{
	}

	/// Stores the complete world-to-object transform matrix for the mesh to be rendered
	matrix4 matrix;
	/// Set to true if the mesh transform matrix will turn it inside-out
	bool inside_out;
	/// Set to true if painters should highlight selected gprims
	bool show_component_selection;
};

} // namespace gl

} // namespace k3d

#endif // !K3DSDK_PAINTER_RENDER_STATE_GL_H

