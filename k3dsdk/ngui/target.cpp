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
#include <k3dsdk/icamera.h>
#include <k3dsdk/imesh_source.h>
#include <k3dsdk/iprojection.h>
#include <k3dsdk/irenderable_gl.h>
#include <k3dsdk/mesh.h>
#include <k3dsdk/mesh_operations.h>
#include <k3dsdk/properties.h>
#include <k3dsdk/transform.h>

#include <set>

namespace libk3dngui
{

namespace detail
{

/// Expands a bounding box around the visited points
class point_visitor
{
public:
	point_visitor(k3d::bounding_box3& BBox, k3d::mesh::points_t& Points, const k3d::matrix4& Matrix) : m_bbox(BBox), m_points(Points), m_matrix(Matrix) {}
	void operator()(k3d::uint_t PointIndex, const k3d::point3& Point)
	{
		const k3d::point3 transformed_point = Point * m_matrix;
		m_bbox.insert(transformed_point);
		if (m_inserted_points.insert(PointIndex).second)
		{
			m_points.push_back(transformed_point);
		}
	}
private:
	k3d::bounding_box3& m_bbox;
	k3d::mesh::points_t& m_points;
	const k3d::matrix4& m_matrix;
	std::set<k3d::uint_t> m_inserted_points;
};

/// Computes the average position of selected nodes, returns false when no selected node was found.
bool selection_position(const selection_mode_t& SelectionMode, const k3d::nodes_t& Selection, k3d::bounding_box3& BoundingBox, k3d::mesh::points_t& Points)
{
	if (Selection.empty())
		k3d::log() << debug << "Empty node selection!" << std::endl;
	
	// Expand the bounding box around the selection
	for(k3d::nodes_t::const_iterator node = Selection.begin(); node != Selection.end(); ++node)
	{
		if(!dynamic_cast<k3d::gl::irenderable*>(*node))
			continue;
		
		const k3d::matrix4 transformation = k3d::node_to_world_matrix(**node);
		point_visitor visitor(BoundingBox, Points, transformation);

		if (SelectionMode == SELECT_NODES)
		{
			if(k3d::ibounded* bounded = dynamic_cast<k3d::ibounded*>(*node))
			{
				BoundingBox.insert(bounded->extents() * transformation);
			}
			else
			{
				BoundingBox.insert(k3d::world_position(**node));
			}
		}
		if (!(*node)->factory().implements(typeid(k3d::imesh_source)))
			continue;
		const k3d::mesh* mesh = k3d::property::pipeline_value<k3d::mesh*>(dynamic_cast<k3d::imesh_source*>(*node)->mesh_source_output());
		if (!mesh)
			continue;
		if (SelectionMode == SELECT_NODES)
		{
			const k3d::mesh::points_t& points = *mesh->points;
			for (k3d::uint_t point = 0; point != points.size(); ++point)
				Points.push_back(points[point] * transformation);
		}
		if (SelectionMode == SELECT_POINTS)
		{
			k3d::traverse_selected_points(*mesh, visitor);
		}
		if (SelectionMode == SELECT_LINES)
		{
			k3d::traverse_selected_edge_points(*mesh, visitor);
		}
		if (SelectionMode == SELECT_FACES)
		{
			k3d::traverse_selected_face_points(*mesh, visitor);
		}
	}
	
	// Nothing was selected...
	if (BoundingBox.empty())
	{
		k3d::log() << debug << "Empty bbox" << std::endl;
		return false;
	}
	
	if (Points.empty())
	{
		Points.push_back(k3d::point3(BoundingBox.nx, BoundingBox.ny, BoundingBox.nz));
		Points.push_back(k3d::point3(BoundingBox.px, BoundingBox.ny, BoundingBox.nz));
		Points.push_back(k3d::point3(BoundingBox.px, BoundingBox.py, BoundingBox.nz));
		Points.push_back(k3d::point3(BoundingBox.nx, BoundingBox.py, BoundingBox.nz));
		Points.push_back(k3d::point3(BoundingBox.nx, BoundingBox.ny, BoundingBox.pz));
		Points.push_back(k3d::point3(BoundingBox.px, BoundingBox.ny, BoundingBox.pz));
		Points.push_back(k3d::point3(BoundingBox.nx, BoundingBox.py, BoundingBox.pz));
		Points.push_back(k3d::point3(BoundingBox.px, BoundingBox.py, BoundingBox.pz));
	}

	return true;
}

} // namespace detail

void aim_selection(document_state& DocumentState, viewport::control& Viewport)
{
	// Get the bounding box of the current selection
	k3d::bounding_box3 bbox;
	k3d::mesh::points_t points;
	if(!detail::selection_position(DocumentState.selection_mode().internal_value(), DocumentState.selected_nodes(), bbox, points))
		return;
	
	k3d::point3 target = bbox.center();

	const k3d::matrix4 view_matrix = Viewport.get_view_matrix();
	const k3d::vector3 look_vector = k3d::look_vector(view_matrix);
	const k3d::vector3 right_vector = k3d::right_vector(view_matrix);
	const k3d::point3 position = k3d::position(view_matrix);

	const k3d::vector3 new_look_vector = target - position;
	const k3d::vector3 new_right_vector = new_look_vector ^ Viewport.get_up_axis();
	const k3d::vector3 new_up_vector = new_right_vector ^ new_look_vector;

	Viewport.set_view_matrix(k3d::view_matrix(new_look_vector, new_up_vector, position));
	Viewport.set_target(bbox.center());
}

void frame_selection(document_state& DocumentState, viewport::control& Viewport)
{
	// Get the bounding box of the current selection
	k3d::bounding_box3 bbox;
	k3d::mesh::points_t points;
	if(!detail::selection_position(DocumentState.selection_mode().internal_value(), DocumentState.selected_nodes(), bbox, points))
		return;
	
	k3d::point3 target = bbox.center();

	const k3d::matrix4 view_matrix = Viewport.get_view_matrix();
	const k3d::vector3 look_vector = k3d::look_vector(view_matrix);
	const k3d::vector3 up_vector = k3d::up_vector(view_matrix);
	const k3d::vector3 right_vector = k3d::right_vector(view_matrix);
	const k3d::point3 position = k3d::position(view_matrix);

	k3d::point3 old_target = Viewport.get_target();
	
	// translation to center
	const k3d::vector3 pan_translation = target - old_target;
	
	// Get the zoom factor by comparing view area and bounding box
	return_if_fail(Viewport.camera());
	k3d::iprojection* camera_projection = &(Viewport.camera()->projection());
	double near = 1;
	double left = 0;
	double right = 0;
	double top = 0;
	double bottom = 0;
	if (k3d::iorthographic* projection = dynamic_cast<k3d::iorthographic*>(camera_projection))
	{
		left = k3d::property::pipeline_value<double>(projection->left());
		right = k3d::property::pipeline_value<double>(projection->right());
		top = k3d::property::pipeline_value<double>(projection->top());
		bottom = k3d::property::pipeline_value<double>(projection->bottom());
	}
	else if (k3d::iperspective* projection = dynamic_cast<k3d::iperspective*>(camera_projection))
	{
		near = k3d::property::pipeline_value<double>(projection->near());
		left = k3d::property::pipeline_value<double>(projection->left());
		right = k3d::property::pipeline_value<double>(projection->right());
		top = k3d::property::pipeline_value<double>(projection->top());
		bottom = k3d::property::pipeline_value<double>(projection->bottom());
	}
	else
	{
		assert_not_reached();
	}
	
	// Transformation to camera coordinates (x = right, y = look, z = up)
	k3d::matrix4 view_transformation(
			k3d::point4(right_vector[0], right_vector[1], right_vector[2], -right_vector*k3d::to_vector(pan_translation + position)),
			k3d::point4(look_vector[0], look_vector[1], look_vector[2], -look_vector*k3d::to_vector(pan_translation + position)),
			k3d::point4(up_vector[0], up_vector[1], up_vector[2], -up_vector*k3d::to_vector(pan_translation + position)),
			k3d::point4(0,0,0,1));
	
	// Find the point with the largest viewing angle
	double factor = std::numeric_limits<double>::max();
	for (k3d::uint_t point = 0; point != points.size(); ++point)
	{
		k3d::point3 p = view_transformation * points[point];
		points[point] = p;
		double x = p[0];
		double y = p[1];
		double z = p[2];
		double testfactor = x < 0 ? y - (x * near / left) : y - (x * near / right);
		factor = testfactor < factor ? testfactor : factor;
		testfactor = z < 0 ? y - (z * near / bottom) : y - (z * near / top);
		factor = testfactor < factor ? testfactor : factor;
	}
	
	// Check and correct if the zoom factor found makes the selection intersect the near plane
	for (k3d::uint_t point = 0; point != points.size(); ++point)
	{
		double y = points[point][1];
		if ((y - factor) < near)
		{
			factor = y - near;
		}
	}
	
	const k3d::vector3 dolly_translation = factor * look_vector; 

	Viewport.set_view_matrix(k3d::view_matrix(look_vector, up_vector, position + pan_translation + dolly_translation));
	Viewport.set_target(target);
}

} // namespace libk3dngui

