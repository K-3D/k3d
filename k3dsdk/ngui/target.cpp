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
#include <k3dsdk/ngui/selection_state.h>
#include <k3dsdk/polyhedron.h>
#include <k3dsdk/properties.h>
#include <k3dsdk/transform.h>

#include <boost/scoped_ptr.hpp>

#include <set>

namespace k3d
{

namespace ngui
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
		const k3d::point3 transformed_point = m_matrix * Point;
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

/// Traverse selected mesh points
template<typename visitor_t>
void traverse_selected_points(const k3d::mesh& Mesh, visitor_t& Visitor)
{
	for (k3d::uint_t point = 0; point != Mesh.points->size(); ++point)
	{
		if (Mesh.point_selection->at(point))
		{
			Visitor(point, Mesh.points->at(point));
		}
	}
}

/// For each selected edge, visit the start and end point (multiple visits per point possible!)
template<typename visitor_t>
void traverse_selected_edge_points(const k3d::mesh& Mesh, visitor_t& Visitor)
{
/*
	boost::scoped_ptr<k3d::polyhedron::const_primitive> polyhedron(k3d::polyhedron::validate(Mesh));
	return_if_fail(polyhedron);
	const k3d::mesh::points_t& points = *Mesh.points;
	const k3d::mesh::indices_t& edge_points = *Mesh.polyhedra->edge_points;
	const k3d::mesh::indices_t& clockwise_edges = *Mesh.polyhedra->clockwise_edges;
	const k3d::mesh::selection_t& edge_selection = *Mesh.polyhedra->edge_selection;
	for (k3d::uint_t edge = 0; edge != edge_points.size(); ++edge)
	{
		if (edge_selection[edge])
		{
			Visitor(edge_points[edge], points[edge_points[edge]]);
			Visitor(edge_points[clockwise_edges[edge]], points[edge_points[clockwise_edges[edge]]]);
		}
	}
*/
}

// For each selected face, visit all of its points (multiple visits per point possible!)
template<typename visitor_t>
void traverse_selected_face_points(const k3d::mesh& Mesh, visitor_t& Visitor)
{
/*
	boost::scoped_ptr<k3d::polyhedron::const_primitive> polyhedron(k3d::polyhedron::validate(Mesh));
	return_if_fail(polyhedron);
	const k3d::mesh::points_t& points = *Mesh.points;
	const k3d::mesh::indices_t& face_first_loops = *Mesh.polyhedra->face_first_loops;
	const k3d::mesh::counts_t& face_loop_counts = *Mesh.polyhedra->face_loop_counts;
	const k3d::mesh::indices_t& loop_first_edges = *Mesh.polyhedra->loop_first_edges;
	const k3d::mesh::indices_t& edge_points = *Mesh.polyhedra->edge_points;
	const k3d::mesh::indices_t& clockwise_edges = *Mesh.polyhedra->clockwise_edges;
	const k3d::mesh::selection_t& face_selection = *Mesh.polyhedra->face_selection;
	for(k3d::uint_t face = 0; face != face_first_loops.size(); ++face)
	{
		if (!face_selection[face])
			continue;
		
		const k3d::uint_t loop_begin = face_first_loops[face];
		const k3d::uint_t loop_end = loop_begin + face_loop_counts[face];
		for(k3d::uint_t loop = loop_begin; loop != loop_end; ++loop)
		{
			const k3d::uint_t first_edge = loop_first_edges[loop];
			for(k3d::uint_t edge = first_edge; ; )
			{
				Visitor(edge_points[edge], points[edge_points[edge]]);

				edge = clockwise_edges[edge];
				if(edge == first_edge)
					break;
			}
		}
	}
*/
}

/// Computes the average position of selected nodes, returns false when no selected node was found.
bool selection_position(const selection::mode& SelectionMode, const k3d::nodes_t& Selection, k3d::bounding_box3& BoundingBox, k3d::mesh::points_t& Points)
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

		if(SelectionMode == selection::NODES)
		{
			if(k3d::ibounded* bounded = dynamic_cast<k3d::ibounded*>(*node))
			{
				BoundingBox.insert(transformation * bounded->extents());
			}
			else
			{
				BoundingBox.insert(k3d::world_position(**node));
			}
		}
		if(!(*node)->factory().implements(typeid(k3d::imesh_source)))
			continue;
		const k3d::mesh* mesh = k3d::property::pipeline_value<k3d::mesh*>(dynamic_cast<k3d::imesh_source*>(*node)->mesh_source_output());
		if(!mesh)
			continue;
		if(!mesh->points)
			continue;
		if(SelectionMode == selection::NODES)
		{
			const k3d::mesh::points_t& points = *mesh->points;
			for (k3d::uint_t point = 0; point != points.size(); ++point)
				Points.push_back(transformation * points[point]);
		}
		if(SelectionMode == selection::POINTS)
		{
			traverse_selected_points(*mesh, visitor);
		}
		if(SelectionMode == selection::SPLIT_EDGES)
		{
			traverse_selected_edge_points(*mesh, visitor);
		}
		if(SelectionMode == selection::UNIFORM)
		{
			traverse_selected_face_points(*mesh, visitor);
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
	if(!detail::selection_position(selection::state(DocumentState.document()).current_mode(), selection::state(DocumentState.document()).selected_nodes(), bbox, points))
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
	if(!detail::selection_position(selection::state(DocumentState.document()).current_mode(), selection::state(DocumentState.document()).selected_nodes(), bbox, points))
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
			k3d::vector4(right_vector[0], right_vector[1], right_vector[2], -right_vector*k3d::to_vector(pan_translation + position)),
			k3d::vector4(look_vector[0], look_vector[1], look_vector[2], -look_vector*k3d::to_vector(pan_translation + position)),
			k3d::vector4(up_vector[0], up_vector[1], up_vector[2], -up_vector*k3d::to_vector(pan_translation + position)),
			k3d::vector4(0,0,0,1));
	
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

} // namespace ngui

} // namespace k3d

