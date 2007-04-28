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
		\author Romain Behar (romainbehar@yahoo.com)
*/

#include <gtkmm/widget.h>

#include "document_state.h"
#include "target.h"
#include "viewport.h"

#include <k3dsdk/algebra.h>
#include <k3dsdk/ibounded.h>
#include <k3dsdk/idrawable_gl.h>
#include <k3dsdk/transform.h>

namespace libk3dngui
{

namespace detail
{

/// Computes the average position of selected nodes, returns false when no selected node was found
bool selection_position(const k3d::nodes_t& Selection, k3d::point3& NewTarget)
{
	unsigned long count = 0;

	NewTarget = k3d::point3(0, 0, 0);
	for(k3d::nodes_t::const_iterator node = Selection.begin(); node != Selection.end(); ++node)
	{
		if(!dynamic_cast<k3d::gl::idrawable*>(*node))
			continue;

		++count;

		if(k3d::ibounded* bounded = dynamic_cast<k3d::ibounded*>(*node))
		{
			const k3d::bounding_box3 transformed_bbox = bounded->extents() * k3d::node_to_world_matrix(**node);
			NewTarget += transformed_bbox.center();
		}
		else
			NewTarget += k3d::world_position(**node);
	}

	if(!count)
		return false;

	NewTarget /= static_cast<double>(count);
	return true;
}

} // namespace detail

void aim_selection(document_state& DocumentState, viewport::control& Viewport)
{
	// Get the new target position in world coordinates
	k3d::point3 target;
	if(!detail::selection_position(DocumentState.selected_nodes(), target))
		return;

	const k3d::matrix4 view_matrix = Viewport.get_view_matrix();
	const k3d::vector3 look_vector = k3d::look_vector(view_matrix);
	const k3d::vector3 right_vector = k3d::right_vector(view_matrix);
	const k3d::point3 position = k3d::position(view_matrix);

	const k3d::vector3 new_look_vector = k3d::to_vector(target - position);
	const k3d::vector3 new_right_vector = new_look_vector ^ Viewport.get_up_axis();
	const k3d::vector3 new_up_vector = new_right_vector ^ new_look_vector;

	Viewport.set_view_matrix(k3d::view_matrix(new_look_vector, new_up_vector, position));
	Viewport.set_target(target);
}

void frame_selection(document_state& DocumentState, viewport::control& Viewport)
{
	// Get the new target position in world coordinates
	k3d::point3 target;
	if(!detail::selection_position(DocumentState.selected_nodes(), target))
		return;

	const k3d::matrix4 view_matrix = Viewport.get_view_matrix();
	const k3d::vector3 look_vector = k3d::look_vector(view_matrix);
	const k3d::vector3 up_vector = k3d::up_vector(view_matrix);
	const k3d::vector3 right_vector = k3d::right_vector(view_matrix);
	const k3d::point3 position = k3d::position(view_matrix);

	const k3d::vector3 delta = k3d::to_vector(target - Viewport.get_target());

	Viewport.set_view_matrix(k3d::view_matrix(look_vector, up_vector, position + delta));
	Viewport.set_target(target);
}

} // namespace libk3dngui

