#ifndef K3DSDK_SELECTION_STATE_GL_H
#define K3DSDK_SELECTION_STATE_GL_H

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
		select_point_groups(false),
		select_edges(false),
		select_faces(false),
		select_linear_curves(false),
		select_cubic_curves(false),
		select_nurbs_curves(false),
		select_bilinear_patches(false),
		select_bicubic_patches(false),
		select_nurbs_patches(false),
		select_blobbies(false),
		select_backfacing(false)
	{
	}

	bool exclude_unselected_nodes;
	bool select_points;
	bool select_point_groups;
	bool select_edges;
	bool select_faces;
	bool select_linear_curves;
	bool select_cubic_curves;
	bool select_nurbs_curves;
	bool select_bilinear_patches;
	bool select_bicubic_patches;
	bool select_nurbs_patches;
	bool select_blobbies;
	bool select_backfacing;
};

} // namespace gl

} // namespace k3d

#endif // K3DSDK_SELECTION_STATE_GL_H

