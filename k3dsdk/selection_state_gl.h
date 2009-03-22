#ifndef K3DSDK_SELECTION_STATE_GL_H
#define K3DSDK_SELECTION_STATE_GL_H

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
// License along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

/** \file
	\author Tim Shead (tshead@k-3d.com)
*/

#include "types.h"

namespace k3d
{

namespace gl
{

/// Used to pass selection bitmask for any combination of primitives
class selection_state
{
public:
	selection_state() :
		exclude_unselected_nodes(false),
		select_points(false),
		select_faces(false),
		select_nurbs_patches(false),
		select_backfacing(false),
		select_uniform(false),
		select_split_edges(false)
	{
	}

	bool_t exclude_unselected_nodes;
	bool_t select_points;
	bool_t select_faces;
	bool_t select_nurbs_patches;
	bool_t select_backfacing;
	bool_t select_uniform;
	bool_t select_split_edges;
};

} // namespace gl

} // namespace k3d

#endif // !K3DSDK_SELECTION_STATE_GL_H

