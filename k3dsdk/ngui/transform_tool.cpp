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

#include <gdkmm/cursor.h>
#include <gtkmm/widget.h>

#include <k3d-i18n-config.h>

#include "document_state.h"
#include "modifiers.h"
#include "icons.h"
#include "interactive.h"
#include "keyboard.h"
#include "selection_state.h"
#include "transform_tool.h"
#include "utility.h"
#include "viewport.h"

#include <k3dsdk/classes.h>
#include <k3dsdk/color.h>
#include <k3dsdk/plugins.h>
#include <k3dsdk/fstream.h>
#include <k3dsdk/icamera.h>
#include <k3dsdk/irenderable_gl.h>
#include <k3dsdk/ikeyframer.h>
#include <k3dsdk/imesh_source.h>
#include <k3dsdk/ipipeline.h>
#include <k3dsdk/iprojection.h>
#include <k3dsdk/itransform_sink.h>
#include <k3dsdk/itransform_source.h>
#include <k3dsdk/properties.h>
#include <k3dsdk/state_change_set.h>
#include <k3dsdk/transform.h>
#include <k3dsdk/xml.h>

#include <boost/lexical_cast.hpp>

#include <set>

namespace k3d
{

namespace ngui
{

namespace detail
{

double manipulators_scale(viewport::control& Viewport, const k3d::point3& Origin, const double Size)
{
	k3d::icamera* camera = Viewport.camera();
	return_val_if_fail(camera, 0);

	// Project unit axis on screen space
	const k3d::matrix4 screen_matrix = k3d::node_to_world_matrix(*Viewport.camera());
	const k3d::vector3 screen_parallel = screen_matrix * k3d::vector3(1, 0, 0);
	const k3d::point2 position = Viewport.project(Origin);
	const k3d::point2 x_axis = Viewport.project(Origin + screen_parallel);
	const double length = k3d::distance(position, x_axis);

	return_val_if_fail(length, 0);

	return Size / length;
}

bool is_front_facing(viewport::control& Viewport, const k3d::vector3& Normal, const k3d::point3& Origin, const k3d::matrix4& Orientation)
{
	return_val_if_fail(Viewport.gl_engine(), false);
	return_val_if_fail(Viewport.camera(), false);

	const k3d::matrix4 matrix = k3d::inverse(k3d::node_to_world_matrix(*Viewport.camera()));
	const k3d::point3 a = Origin + (Orientation * Normal);
	const k3d::point3 b = Origin + (Orientation * -Normal);
	return k3d::to_vector(matrix * a).length2() < k3d::to_vector(matrix * b).length2();
}

k3d::point3 get_selected_points(selection::mode SelectionMode, const k3d::mesh& Mesh, component_points_t& PointList)
{
	PointList.clear();
	k3d::point3 component_center(0, 0, 0);
	
	return_val_if_fail(Mesh.points, component_center);
	return_val_if_fail(Mesh.point_selection, component_center);
	
	const k3d::mesh::points_t& points = *Mesh.points;
	const k3d::mesh::selection_t& point_selection = *Mesh.point_selection;
	const size_t point_count = points.size();

	// Get selected points
	for (size_t point = 0; point != point_count; ++point)
	{
		if (point_selection[point])
		{
			PointList.push_back(point);

			component_center += to_vector(points[point]);
		}
	}

  assert_not_implemented();
  /*	
	if(Mesh.polyhedra)
	{
		// Get selected lines
		const k3d::mesh::polyhedra_t& polyhedra = *Mesh.polyhedra;
		return_val_if_fail(polyhedra.clockwise_edges, component_center);
		return_val_if_fail(polyhedra.edge_points, component_center);
		return_val_if_fail(polyhedra.edge_selection, component_center);
		const k3d::mesh::indices_t& clockwise_edges = *polyhedra.clockwise_edges;
		const k3d::mesh::indices_t& edge_points = *polyhedra.edge_points;
		const k3d::mesh::selection_t& edge_selection = *polyhedra.edge_selection;
		
		const size_t edge_count = edge_points.size();
		std::set<size_t> pointset; //ensure each point gets added only once
		for (size_t edge = 0; edge < edge_count; ++edge)
		{
			if (edge_selection[edge])
			{
				pointset.insert(edge_points[edge]);
				pointset.insert(edge_points[clockwise_edges[edge]]);
			}
		}
		
		// Selected faces ..
		return_val_if_fail(Mesh.polyhedra, component_center);
		return_val_if_fail(polyhedra.face_first_loops, component_center);
		return_val_if_fail(polyhedra.loop_first_edges, component_center);
		return_val_if_fail(polyhedra.face_selection, component_center);
		const k3d::mesh::indices_t& face_first_loops = *polyhedra.face_first_loops;
		const k3d::mesh::indices_t& loop_first_edges = *polyhedra.loop_first_edges;
		const k3d::mesh::selection_t& face_selection = *polyhedra.face_selection;
	
		const size_t face_count = face_first_loops.size();
		for(size_t face = 0; face != face_count; ++face)
		{
			if (face_selection[face])
			{
				const size_t first_edge = loop_first_edges[face_first_loops[face]];
				for(size_t edge = first_edge; ; )
				{
					pointset.insert(edge_points[edge]);
					edge = clockwise_edges[edge];
					if(edge == first_edge)
						break;
				}
			}
		}
		
		for (std::set<size_t>::const_iterator point = pointset.begin(); point != pointset.end(); ++ point)
		{
			PointList.push_back(*point);
			component_center += to_vector(points[*point]);
		}
	}
*/
	
	// Compute average position
	const double point_number = static_cast<double>(PointList.size());
	if(point_number)
		component_center /= point_number;

	return component_center;
}

} // namespace detail

	transform_tool::itarget::~itarget()
	{
		for(connections_t::iterator connection = m_connections.begin(); connection != m_connections.end(); ++connection)
		{
			connection->disconnect();
		}
	}

	k3d::matrix4 transform_tool::itarget::world_orientation()
	{
		if(LOCAL == current_system_type)
			return k3d::extract_rotation(k3d::node_to_world_matrix(*node));

		if(PARENT == current_system_type)
			return k3d::extract_rotation(k3d::parent_to_world_matrix(*node));

		return k3d::identity3();
	}

	void transform_tool::itarget::set_transform_modifier(k3d::inode* Modifier)
	{
		modifier = Modifier;
		m_connections.push_back(Modifier->deleted_signal().connect(sigc::mem_fun(*this, &itarget::reset_transform_modifier)));
	}

	void transform_tool::itarget::reset_transform_modifier()
	{
		modifier = 0;
	}

	void transform_tool::itarget::set_coordinate_system_change_matrices()
	{
		switch(current_system_type)
		{
			case GLOBAL:
				m_system_matrix = k3d::inverse(k3d::node_to_world_matrix(*node));
				m_system_matrix_inverse = k3d::node_to_world_matrix(*node);
				// Zero translation components
				m_system_matrix[0][3] = m_system_matrix[1][3] = m_system_matrix[2][3] = 0;
				m_system_matrix_inverse[0][3] = m_system_matrix_inverse[1][3] = m_system_matrix_inverse[2][3] = 0;
				break;
			case LOCAL:
				m_system_matrix = k3d::identity3();
				m_system_matrix_inverse = k3d::identity3();
				break;
			case PARENT:
				m_system_matrix = k3d::inverse(k3d::node_to_world_matrix(*node)) * k3d::parent_to_world_matrix(*node);
				m_system_matrix_inverse = k3d::inverse(m_system_matrix);
				// Zero translation components
				m_system_matrix[0][3] = m_system_matrix[1][3] = m_system_matrix[2][3] = 0;
				m_system_matrix_inverse[0][3] = m_system_matrix_inverse[1][3] = m_system_matrix_inverse[2][3] = 0;
				break;
			default:
				assert_not_reached();
		}
	}

	// transform_target implementation
	transform_tool::transform_target::transform_target(k3d::inode* Node)
	{
		node = Node;

		// Sanity check
		return_if_fail(node);
	}

	k3d::point3 transform_tool::transform_target::world_position()
	{
		return k3d::world_position(*node);
	}

	unsigned long transform_tool::transform_target::target_number()
	{
		// There's always one object to move
		return 1;
	}

	void transform_tool::transform_target::reset()
	{
	}

	void transform_tool::transform_target::start_move()
	{
		if(create_transform_modifier(k3d::classes::FrozenTransformation(), "Move "))
			assert_warning(k3d::property::set_internal_value(*modifier, "matrix", k3d::identity3()));

		// Setup matrices
		set_original_matrix();

		set_coordinate_system_change_matrices();
	}

	void transform_tool::transform_target::move(const k3d::vector3& Move)
	{
		if(!modifier)
			start_move();

		const k3d::matrix4 translation = k3d::translate3(m_system_matrix * Move);
		assert_warning(k3d::property::set_internal_value(*modifier, "matrix", m_original_matrix * translation));
	}

	void transform_tool::transform_target::start_rotation()
	{
		if(create_transform_modifier(k3d::classes::FrozenTransformation(), "Rotate "))
			assert_warning(k3d::property::set_internal_value(*modifier, "matrix", k3d::identity3()));

		// Setup matrices
		set_original_matrix();

		set_coordinate_system_change_matrices();
	}

	void transform_tool::transform_target::rotate(const k3d::matrix4& RotationMatrix, const k3d::point3& WorldCenter)
	{
		if(!modifier)
			start_rotation();
		
		k3d::vector3 translation = WorldCenter - m_original_matrix * k3d::point3(0,0,0);
		
		const k3d::matrix4 current_coordinate_system_rotation = m_system_matrix * k3d::translate3(translation) * RotationMatrix * k3d::translate3(-translation) * m_system_matrix_inverse;
		assert_warning(k3d::property::set_internal_value(*modifier, "matrix", m_original_matrix * current_coordinate_system_rotation));
	}

	void transform_tool::transform_target::start_scaling()
	{
		if(create_transform_modifier(k3d::classes::FrozenTransformation(), "Scale "))
			assert_warning(k3d::property::set_internal_value(*modifier, "matrix", k3d::identity3()));

		// Setup matrices
		set_original_matrix();

		set_coordinate_system_change_matrices();
	}

	void transform_tool::transform_target::scale(const k3d::point3& Scaling, const k3d::point3& WorldCenter)
	{
		if(!modifier)
			start_scaling();
		
		k3d::vector3 translation = WorldCenter - m_original_matrix * k3d::point3(0,0,0);

		const k3d::matrix4 current_coordinate_system_scaling = m_system_matrix * k3d::translate3(translation) * k3d::scale3(Scaling[0], Scaling[1], Scaling[2]) * k3d::translate3(-translation) * m_system_matrix_inverse;
		assert_warning(k3d::property::set_internal_value(*modifier, "matrix", m_original_matrix * current_coordinate_system_scaling));
	}

	void transform_tool::transform_target::end_drag_motion()
	{
	}

	bool transform_tool::transform_target::create_transform_modifier(const k3d::uuid& Class, const std::string& Name)
	{
		if(modifier)
			return false;

		return_val_if_fail(node, false);

		// Check for an existing transform modifier
		k3d::inode* upstream_node = 0;
		
		k3d::itransform_sink* const downstream_sink = dynamic_cast<k3d::itransform_sink*>(node);
		return_val_if_fail(downstream_sink, false);
		k3d::iproperty& downstream_input = downstream_sink->transform_sink_input();
		k3d::iproperty* upstream_output = node->document().pipeline().dependency(downstream_input);

		// Check upstream object
		if(upstream_output)
			upstream_node = upstream_output->property_node();
		
		/** \todo check for same name too */
		if(upstream_node && (Class == upstream_node->factory().factory_id()))
		{
			set_transform_modifier(upstream_node);
			return false;
		}
		else if (upstream_node && dynamic_cast<k3d::ikeyframer*>(upstream_node))
		{
			k3d::ikeyframer* keyframer = dynamic_cast<k3d::ikeyframer*>(upstream_node);
			k3d::iproperty& downstream_input2 = keyframer->input_property();
			upstream_output = node->document().pipeline().dependency(downstream_input2);
			if(upstream_output)
				upstream_node = upstream_output->property_node();
			if(upstream_node && (Class == upstream_node->factory().factory_id()))
			{
				set_transform_modifier(upstream_node);
				return false;
			}
			else
			{
				const std::string modifier_name = Name + node->name();
				modifier = k3d::plugin::create<k3d::inode>(Class, node->document(), modifier_name);
				return_val_if_fail(modifier, false);
			
				k3d::ipipeline::dependencies_t dependencies;
				dependencies.insert(std::make_pair(&dynamic_cast<k3d::itransform_sink*>(modifier)->transform_sink_input(), upstream_output));
				dependencies.insert(std::make_pair(&downstream_input2, &dynamic_cast<k3d::itransform_source*>(modifier)->transform_source_output()));
				node->document().pipeline().set_dependencies(dependencies);
			
				return true;
			}
		}	

		const std::string modifier_name = Name + node->name();
		set_transform_modifier(insert_transform_modifier(*node, Class, modifier_name));

		return true;
	}
	
	void transform_tool::transform_target::set_original_matrix()
	{
		return_if_fail(node && modifier);
		k3d::matrix4 node_input_matrix = boost::any_cast<k3d::matrix4>(k3d::property::pipeline_value(*node, "input_matrix"));
		k3d::matrix4 modifier_input_matrix = boost::any_cast<k3d::matrix4>(k3d::property::pipeline_value(*modifier, "input_matrix"));
		// Calculate the original matrix back from the current node matrix, in case it got changed by i.e. an animation track
		m_original_matrix = inverse(modifier_input_matrix) * node_input_matrix;
	}

	// mesh_target implementation
	transform_tool::mesh_target::mesh_target(document_state& DocumentState, k3d::inode* Node, k3d::iproperty& MeshSourceProperty) :
		m_document_state(DocumentState),
		mesh_source_property(MeshSourceProperty),
		component_center(k3d::point3(0, 0, 0)),
		m_mesh_changed(true),
		m_drag_mutex(false)
	{
		node = Node;

		// Sanity checks
		return_if_fail(node);

		// Connect to mesh_source change_signal to be acknowledged of changes
		m_mesh_change_signal = mesh_source_property.property_changed_signal().connect(sigc::mem_fun(*this, &transform_tool::mesh_target::mesh_changed));

		reset_selection();
	}

	k3d::point3 transform_tool::mesh_target::world_position()
	{
		if(!m_drag_mutex && m_mesh_changed)
			reset_selection();

		return k3d::node_to_world_matrix(*node) * component_center;
	}

	unsigned long transform_tool::mesh_target::target_number()
	{
		return selected_points.size();
	}

	void transform_tool::mesh_target::reset_selection()
	{
		if(m_drag_mutex)
			return;
		
		k3d::mesh* const mesh = boost::any_cast<k3d::mesh*>(mesh_source_property.property_internal_value());
		return_if_fail(mesh);

		// Get selection and save initial position
		component_center = detail::get_selected_points(m_document_state.selection_mode().internal_value(), *mesh, selected_points);

		m_mesh_changed = false;
	}

	void transform_tool::mesh_target::reset()
	{
		if(m_drag_mutex)
			return;

		// Reset position
		reset_selection();

		modifier = 0;
	}

	void transform_tool::mesh_target::start_move()
	{
		create_mesh_modifier("Move ");
		assert_warning(k3d::property::set_internal_value(*modifier, "center", k3d::point3(0.0,0.0,0.0))); // center not needed for translation
	}

	void transform_tool::mesh_target::move(const k3d::vector3& Move)
	{
		if(!modifier)
			start_move();

		m_drag_mutex = true;
		
		const k3d::matrix4 translation = k3d::translate3(m_system_matrix * Move);
		assert_warning(k3d::property::set_internal_value(*modifier, "matrix", m_original_matrix * translation));

		// Update manipulators position
		component_center = m_system_matrix * Move + m_origin;
	}

	void transform_tool::mesh_target::start_rotation()
	{
		create_mesh_modifier("Rotate ");
	}

	void transform_tool::mesh_target::rotate(const k3d::matrix4& RotationMatrix, const k3d::point3& WorldCenter)
	{
		if(!modifier)
			start_rotation();

		m_drag_mutex = true;
		
		const k3d::matrix4 current_coordinate_system_rotation = m_system_matrix * RotationMatrix * m_system_matrix_inverse;
		assert_warning(k3d::property::set_internal_value(*modifier, "center", k3d::inverse(k3d::node_to_world_matrix(*node)) * WorldCenter)); // set center first, so we can ride on the change signal of the matrix
		assert_warning(k3d::property::set_internal_value(*modifier, "matrix", m_original_matrix * current_coordinate_system_rotation));
	}

	void transform_tool::mesh_target::start_scaling()
	{
		create_mesh_modifier("Scale ");
	}

	void transform_tool::mesh_target::scale(const k3d::point3& Scaling, const k3d::point3& WorldCenter)
	{
		if(!modifier)
			start_scaling();

		m_drag_mutex = true;

		const k3d::matrix4 current_coordinate_system_scaling = m_system_matrix * k3d::scale3(Scaling[0], Scaling[1], Scaling[2]) * m_system_matrix_inverse;
		assert_warning(k3d::property::set_internal_value(*modifier, "center", k3d::inverse(k3d::node_to_world_matrix(*node))*WorldCenter)); // set center first, so we can ride on the change signal of the matrix
		assert_warning(k3d::property::set_internal_value(*modifier, "matrix", m_original_matrix * current_coordinate_system_scaling));
	}

	void transform_tool::mesh_target::end_drag_motion()
	{
		m_drag_mutex = false;
	}

	void transform_tool::mesh_target::create_mesh_modifier(const std::string& Name)
	{
		if (!modifier)
		{
			return_if_fail(node);

			// Get mesh to tweak
			k3d::mesh* const mesh = boost::any_cast<k3d::mesh*>(mesh_source_property.property_internal_value());
			return_if_fail(mesh);

			// Modify with TransformPoints
			const k3d::uuid transform_points(0xb9c5bae2, 0x01df44d4, 0x86c395e9, 0x34a11c31);

			// Check for an existing mesh modifier
			k3d::inode* upstream_node = upstream_mesh_modifier(*node);
			if(upstream_node && (transform_points == upstream_node->factory().factory_id()))
			{
				set_transform_modifier(upstream_node);
			}
			else
			{
				const std::string modifier_name = Name + node->name() + " components";
				set_transform_modifier(insert_mesh_modifier(*node, transform_points, modifier_name));
			}
		}

		// Update modifier's selected point list
		assert_warning(k3d::property::set_internal_value(*modifier, "selected_points", selected_points));
		
		m_origin = component_center;
		
		// Setup matrices
		m_original_matrix = boost::any_cast<k3d::matrix4>(k3d::property::pipeline_value(*modifier, "matrix"));
		set_coordinate_system_change_matrices();
	}


// transform_tool implementation

transform_tool::~transform_tool()
{
	for(connections_t::iterator connection = m_connections.begin(); connection != m_connections.end(); ++connection)
	{
		connection->disconnect();
	}
	clear_targets();
}
	
void transform_tool::lbutton_down(viewport::control& Viewport, const k3d::point2& Coordinates, const k3d::key_modifiers& Modifiers)
{
	// Return if an action is in progress
	if(MOTION_CLICK_DRAG == m_current_motion)
		return;

	assert_warning(MOTION_NONE == m_current_motion);

	// Init action
	m_mouse_down_content = NOTHING;

	// Find what's under the mouse pointer
	k3d::selection::records picked_selectables;
	pick_selectables(picked_selectables, Viewport, Coordinates);

	// Shift modifier starts ADD action
	if(Modifiers.shift())
	{
		lmb_down_add();
		return;
	}

	// Control modifier starts SUBTRACT action
	if(Modifiers.control())
	{
		lmb_down_subtract();
		return;
	}

	// Manipulator selection starts move
	detail::manipulators_t manipulators;
	for(k3d::selection::records::iterator record = picked_selectables.begin(); record != picked_selectables.end(); ++record)
	{
		for(k3d::selection::record::tokens_t::const_iterator token = record->tokens.begin(); token != record->tokens.end(); ++token)
		{
			if(token->type == k3d::selection::USER1)
				manipulators.push_back(manipulator_name(token->id));
		}
	}

	const std::string manipulator = get_manipulator(manipulators);
	if(manipulator != "")
	{
		lmb_down_manipulator(manipulator);
		return;
	}

	// If a node was hit ...
	if(k3d::selection::get_node(m_mouse_down_selection))
	{
		if(m_document_state.is_selected(m_mouse_down_selection))
			lmb_down_selected();
		else
			lmb_down_deselected();

		return;
	}

	lmb_down_nothing();
}

// LMB down actions
void transform_tool::lmb_down_add()
{
	k3d::start_state_change_set(m_document, K3D_CHANGE_SET_CONTEXT);
	m_mouse_down_content = SELECTION_ADD;
}

void transform_tool::lmb_down_subtract()
{
	k3d::start_state_change_set(m_document, K3D_CHANGE_SET_CONTEXT);
	m_mouse_down_content = SELECTION_SUBTRACT;
}

void transform_tool::lmb_down_manipulator(const std::string& ManipulatorName)
{
	k3d::start_state_change_set(m_document, K3D_CHANGE_SET_CONTEXT);

	set_manipulator(ManipulatorName);
	set_motion(MOTION_DRAG);

	m_mouse_down_content = SELECTED_OBJECT;
}

void transform_tool::lmb_down_selected()
{
	k3d::start_state_change_set(m_document, K3D_CHANGE_SET_CONTEXT);
	m_mouse_down_content = SELECTED_OBJECT;
}

void transform_tool::lmb_down_deselected()
{
	k3d::start_state_change_set(m_document, K3D_CHANGE_SET_CONTEXT);
	m_mouse_down_content = DESELECTED_OBJECT;

	// Deselect all
	selection::state(m_document_state.document()).deselect_all();
	// Select clicked object
	m_document_state.select(m_mouse_down_selection);
}

void transform_tool::lmb_down_nothing()
{
	k3d::start_state_change_set(m_document, K3D_CHANGE_SET_CONTEXT);
	m_mouse_down_content = NOTHING;
}

void transform_tool::lbutton_click(viewport::control& Viewport, const k3d::point2& Coordinates)
{
	if(MOTION_CLICK_DRAG == m_current_motion)
	{
		// Stop click-drag
		lmb_click_stop_motion();
		return;
	}

	switch(m_mouse_down_content)
	{
		case SELECTION_ADD:
			lmb_click_add(Viewport, Coordinates);
		break;
		case SELECTION_SUBTRACT:
			lmb_click_subtract(Viewport, Coordinates);
		break;
		case SELECTED_OBJECT:
			lmb_click_start_motion(Coordinates);
		break;
		case DESELECTED_OBJECT:
			lmb_click_replace(Viewport, Coordinates);
		break;
		case NOTHING:
			lmb_click_deselect_all();
		break;
		default:
			assert_not_reached();
	}
}

// LMB click actions
void transform_tool::lmb_click_add(viewport::control& Viewport, const k3d::point2& Coordinates)
{
	// Shift key modifier always adds to the selection
//	interactive::move_pointer(Viewport, Coordinates);
	m_document_state.select(Viewport.pick_object(Coordinates, m_document_state.pick_backfacing()));

	k3d::finish_state_change_set(m_document, "Selection add", K3D_CHANGE_SET_CONTEXT);

	tool_selection::redraw_all();
}

void transform_tool::lmb_click_subtract(viewport::control& Viewport, const k3d::point2& Coordinates)
{
	// Control key modifier always adds to the selection
//	interactive::move_pointer(Viewport, Coordinates);
	m_document_state.deselect(Viewport.pick_object(Coordinates, m_document_state.pick_backfacing()));

	k3d::finish_state_change_set(m_document, "Selection subtract", K3D_CHANGE_SET_CONTEXT);

	tool_selection::redraw_all();
}

void transform_tool::lmb_click_replace(viewport::control& Viewport, const k3d::point2& Coordinates)
{
//	interactive::move_pointer(Viewport, Coordinates);
	m_document_state.select(Viewport.pick_object(Coordinates, m_document_state.pick_backfacing()));

	k3d::finish_state_change_set(m_document, "Selection replace", K3D_CHANGE_SET_CONTEXT);

	tool_selection::redraw_all();
}

void transform_tool::lmb_click_start_motion(const k3d::point2& Coordinates)
{
	disconnect_navigation_input_model();

	set_motion(MOTION_CLICK_DRAG);
	begin_mouse_move(Coordinates);

	tool_selection::redraw_all();
}

void transform_tool::lmb_click_stop_motion()
{
	end_drag_motion();
	
	const std::string label = complete_mouse_move();
	k3d::finish_state_change_set(m_document, label, K3D_CHANGE_SET_CONTEXT);

	connect_navigation_input_model();

	tool_selection::redraw_all();
}

void transform_tool::lmb_click_deselect_all()
{
	// Deselect all
	selection::state(m_document_state.document()).deselect_all();

	k3d::finish_state_change_set(m_document, "Deselect all", K3D_CHANGE_SET_CONTEXT);

	tool_selection::redraw_all();
}

void transform_tool::lbutton_start_drag(viewport::control& Viewport, const k3d::point2& Coordinates)
{
	disconnect_navigation_input_model();

	switch(m_mouse_down_content)
	{
		case SELECTED_OBJECT:
		case DESELECTED_OBJECT:
			lmb_start_drag_start_motion(Coordinates);
		break;
		case SELECTION_ADD:
		case SELECTION_SUBTRACT:
		case NOTHING:
			lmb_start_drag_box_select(Viewport, Coordinates);
		break;
		default:
			assert_not_reached();
	}
}

// LMB start drag actions
void transform_tool::lmb_start_drag_start_motion(const k3d::point2& Coordinates)
{
	m_off_screen_offset = k3d::point2(0, 0);

	set_motion(MOTION_DRAG);
	begin_mouse_move(Coordinates);
}

void transform_tool::lmb_start_drag_box_select(viewport::control& Viewport, const k3d::point2& Coordinates)
{
	set_motion(MOTION_BOX_SELECT);
	m_box_selection = k3d::rectangle(Coordinates[0], Coordinates[0], Coordinates[1], Coordinates[1]);
	draw_rubber_band(Viewport);
}

void transform_tool::lmb_drag_box_select(viewport::control& Viewport, const k3d::point2& Coordinates)
{
	on_box_select_motion(Viewport, Coordinates);
}

void transform_tool::lbutton_end_drag(viewport::control& Viewport, const k3d::point2& Coordinates)
{
	if(MOTION_DRAG == m_current_motion)
		lmb_end_drag_stop_motion();
	else if(MOTION_BOX_SELECT == m_current_motion)
		lmb_end_drag_box_select(Viewport, Coordinates);

	connect_navigation_input_model();
}

// LMB end drag actions
void transform_tool::lmb_end_drag_stop_motion()
{
	end_drag_motion();
	
	const std::string label = complete_mouse_move();
	k3d::finish_state_change_set(m_document, label, K3D_CHANGE_SET_CONTEXT);

	tool_selection::redraw_all();
}

void transform_tool::lmb_end_drag_box_select(viewport::control& Viewport, const k3d::point2& Coordinates)
{
	draw_rubber_band(Viewport);

	on_box_select_objects(Viewport, Coordinates, m_box_selection);

	// Stop motion
	set_motion(MOTION_NONE);

	k3d::finish_state_change_set(m_document, "Box selection", K3D_CHANGE_SET_CONTEXT);

	tool_selection::redraw_all();
}

void transform_tool::mbutton_click(viewport::control& Viewport, const k3d::point2& Coordinates, const k3d::key_modifiers& Modifiers)
{
	// Motion mode
	if(MOTION_NONE != m_current_motion)
	{
		mmb_click_next_constraint(Viewport, Coordinates);
		return;
	}

	// We aren't moving
	if(Modifiers.control())
	{
		mmb_click_switch_coordinate_system();
	}
	else if(Modifiers.shift())
	{
		mmb_click_manipulators_next_selection();
	}
	else
	{
		mmb_click_toggle_manipulators_visibility();
	}
}

void transform_tool::mmb_click_toggle_manipulators_visibility()
{
	// Toggle the visible state of the manipulators
	m_visible_manipulators.set_value(!m_visible_manipulators.internal_value());

	tool_selection::redraw_all();
}

void transform_tool::mmb_click_manipulators_next_selection()
{
	// Show manipulators on the next selection
	m_current_target = m_targets.size() ? (m_current_target + 1) % m_targets.size() : 0;

	tool_selection::redraw_all();
}

void transform_tool::mmb_click_switch_coordinate_system()
{
	// Switch coordinate system between global and local modes
	switch(m_coordinate_system.internal_value())
	{
		case GLOBAL:
			set_coordinate_system(LOCAL);
			break;
		case LOCAL:
			set_coordinate_system(GLOBAL);
			break;
		default:
			break;
	}

	tool_selection::redraw_all();
}

void transform_tool::mmb_click_next_constraint(viewport::control& Viewport, const k3d::point2& Coordinates)
{
	// Set next constraint
	update_constraint(Viewport, Coordinates);

/*
	// Mouse warp
	const k3d::point2 coords = Viewport.project(world_position());

	int root_x = 0;
	int root_y = 0;
	Viewport.get_window()->get_origin(root_x, root_y);

	const k3d::point2 screen_coords = k3d::point2(coords[0] + root_x, coords[1] + root_y);

	// We temporarily disable motion, so warping the pointer doesn't cause unintended side effects
	const motion_t current_motion = m_current_motion;
	m_current_motion = MOTION_NONE;
	interactive::warp_pointer(screen_coords);
	handle_pending_events();

	// Acknowledge new mouse position
	m_current_constraint->begin_mouse_move(coords);

	m_current_motion = current_motion;
*/

	tool_selection::redraw_all();
}

void transform_tool::rbutton_click(const viewport::control& Viewport, const k3d::point2& Coordinates)
{
	if(MOTION_NONE == m_current_motion)
		rmb_click_selection_tool();
	else
		rmb_click_cancel_move();
}

// RMB click actions
void transform_tool::rmb_click_selection_tool()
{
	k3d::start_state_change_set(m_document, K3D_CHANGE_SET_CONTEXT);

	m_document_state.set_active_tool(m_document_state.selection_tool());

	k3d::finish_state_change_set(m_document, "Selection tool", K3D_CHANGE_SET_CONTEXT);

	tool_selection::redraw_all();
}

void transform_tool::rmb_click_cancel_move()
{
	cancel_mouse_move();

	tool_selection::redraw_all();
}

void transform_tool::cancel_mouse_move()
{
	// Stop motion
	set_motion(MOTION_NONE);

	end_drag_motion();
	
	// Undo changes
	k3d::cancel_state_change_set(m_document, K3D_CHANGE_SET_CONTEXT);

	// Reset targets
	for(targets_t::iterator target = m_targets.begin(); target != m_targets.end(); ++target)
		(*target)->reset();

	reset();

	tool_selection::redraw_all();
}

std::string transform_tool::complete_mouse_move()
{
	set_motion(MOTION_NONE);

	return get_constraint_name();
}

void transform_tool::set_motion(const motion_t Motion)
{
	m_current_motion = Motion;
}

/// Returns current target's world position
k3d::point3 transform_tool::world_position()
{
	if(target_number())
	{
		if(selection::NODES == m_document_state.selection_mode().internal_value())
		{
			m_current_target = m_current_target % m_targets.size();
			itarget* t = m_targets[m_current_target];
			return t->world_position();
		}
		else
		{
			k3d::point3 position(0, 0, 0);
			unsigned long count = 0;
			for(targets_t::iterator target = m_targets.begin(); target != m_targets.end(); ++target)
			{
				if((*target)->target_number())
				{
					position += to_vector((*target)->world_position());
					count++;
				}
			}

			position /= static_cast<double>(count);

			return position;
		}
	}

	return k3d::point3(0, 0, 0);
}

/// Returns current target's world orientation
k3d::matrix4 transform_tool::world_orientation()
{
	if(target_number())
	{
		m_current_target = m_current_target % m_targets.size();
		itarget* t = m_targets[m_current_target];

		return t->world_orientation();
	}

	return k3d::identity3();
}

void transform_tool::update_manipulators_scale(viewport::control& Viewport, const k3d::point3& Origin)
{
	k3d::icamera* camera = Viewport.camera();
	return_if_fail(camera);

	// Project unit axis on screen space
	const k3d::matrix4 screen_matrix = k3d::node_to_world_matrix(*Viewport.camera());
	const k3d::vector3 screen_parallel = screen_matrix * k3d::vector3(1, 0, 0);
	const k3d::point2 position = Viewport.project(Origin);
	const k3d::point2 x_axis = Viewport.project(Origin + screen_parallel);
	const double length = k3d::distance(position, x_axis);

	return_if_fail(length);
	m_manipulators_scale = m_manipulators_size / length;
}

bool transform_tool::front_facing(viewport::control& Viewport, const k3d::vector3& Normal, const k3d::point3& Origin)
{
	return_val_if_fail(Viewport.gl_engine(), false);
	return_val_if_fail(Viewport.camera(), false);

	const k3d::matrix4 matrix = k3d::inverse(k3d::node_to_world_matrix(*Viewport.camera()));
	const k3d::matrix4 orientation = world_orientation();
	const k3d::point3 a = Origin + (orientation * Normal);
	const k3d::point3 b = Origin + (orientation * -Normal);
	return k3d::to_vector(matrix * a).length2() < k3d::to_vector(matrix * b).length2();
}

void transform_tool::off_screen_warp(viewport::control& Viewport, k3d::point2& NewCoordinates)
{
	// Get mouse coordinates
	int x, y;
	Gdk::ModifierType modifiers;
	Gdk::Display::get_default()->get_pointer(x, y, modifiers);
	k3d::point2 mouse(x, y);

	// Check for screen warp
	bool screen_warp = false;
	const k3d::point2 previous_offset = m_off_screen_offset;

	// Wrap the mouse if it goes off the top-or-bottom of the screen ...
	const int screen_height = Gdk::Display::get_default()->get_default_screen()->get_height();
	if(y == 0)
	{
		mouse = k3d::point2(mouse[0], screen_height - 2);
		screen_warp = true;

		m_off_screen_offset[1] -= screen_height;
	}
	else if(screen_height - 1 == y)
	{
		mouse = k3d::point2(mouse[0], 1);
		screen_warp = true;

		m_off_screen_offset[1] += screen_height;
	}

	// Wrap the mouse if it goes off the left-or-right of the screen ...
	const int screen_width = Gdk::Display::get_default()->get_default_screen()->get_width();
	if(x == 0)
	{
		mouse = k3d::point2(screen_width - 2, mouse[1]);
		screen_warp = true;

		m_off_screen_offset[0] -= screen_width;
	}
	else if(screen_width - 1 == x)
	{
		mouse = k3d::point2(1, mouse[1]);
		screen_warp = true;

		m_off_screen_offset[0] += screen_width;
	}

	if(!screen_warp)
	{
		// No warp
		NewCoordinates += k3d::to_vector(m_off_screen_offset);
	}
	else
	{
		// Warp mouse pointer
		interactive::warp_pointer(mouse);

		// Set new position
		NewCoordinates += k3d::to_vector(previous_offset);
	}
}

void transform_tool::clear_targets()
{
	for(targets_t::iterator target = m_targets.begin(); target != m_targets.end(); ++target)
	{
		delete *target;
	}

	m_targets.clear();
}

/// Retrieves the current document selection, refreshing the target list
void transform_tool::get_current_selection()
{
	// Convert the current document selection into the set of targets to be moved interactively
	clear_targets();

	const k3d::nodes_t nodes = m_document_state.selected_nodes();

	if(selection::NODES == m_document_state.selection_mode().internal_value())
	{
		// Save transformable nodes as targets
		for(k3d::nodes_t::const_iterator node = nodes.begin(); node != nodes.end(); ++node)
		{
			if(!dynamic_cast<k3d::gl::irenderable*>(*node))
				continue;
			if(!dynamic_cast<k3d::itransform_sink*>(*node))
				continue;

			m_targets.push_back(new transform_target(*node));
			m_connections.push_back((*node)->deleted_signal().connect(sigc::mem_fun(*this, &transform_tool::target_list_changed)));
		}
	}
	else
	{
		// Component mode : save mesh nodes as targets
		for(k3d::nodes_t::const_iterator node = nodes.begin(); node != nodes.end(); ++node)
		{
			if(!dynamic_cast<k3d::gl::irenderable*>(*node))
				continue;

			// Get node's mesh
			k3d::imesh_source* const mesh_source = dynamic_cast<k3d::imesh_source*>(*node);
			if(!mesh_source)
				continue;

			// Get non-transformed mesh output
			k3d::iproperty& property = mesh_source->mesh_source_output();
			m_targets.push_back(new mesh_target(m_document_state, *node, property));

			m_connections.push_back((*node)->deleted_signal().connect(sigc::mem_fun(*this, &transform_tool::target_list_changed)));
		}
	}

	update_coordinate_system(0);
}

unsigned long transform_tool::target_number()
{
	unsigned long target_count = 0;
	for(targets_t::iterator target = m_targets.begin(); target != m_targets.end(); ++target)
		target_count += (*target)->target_number();

	return target_count;
}

void transform_tool::update_targets()
{
	// Update selection on new or deleted node(s)
	if(m_node_selection_changed)
	{
		get_current_selection();
		m_node_selection_changed = false;
	}
}

void transform_tool::start_move()
{
	for(targets_t::iterator target = m_targets.begin(); target != m_targets.end(); ++target)
		(*target)->start_move();
}

void transform_tool::move_targets(const k3d::vector3& Move)
{
	for(targets_t::iterator target = m_targets.begin(); target != m_targets.end(); ++target)
		(*target)->move(Move);

	tool_selection::redraw_all();
}

void transform_tool::start_rotation()
{
	for(targets_t::iterator target = m_targets.begin(); target != m_targets.end(); ++target)
		(*target)->start_rotation();
}

void transform_tool::rotate_targets(const k3d::matrix4& Rotation)
{
	if(!m_targets.size())
		return;

	for(targets_t::iterator target = m_targets.begin(); target != m_targets.end(); ++target)
		(*target)->rotate(Rotation, world_position());

	tool_selection::redraw_all();
}

void transform_tool::start_scaling()
{
	for(targets_t::iterator target = m_targets.begin(); target != m_targets.end(); ++target)
		(*target)->start_scaling();
}

void transform_tool::scale_targets(const k3d::point3& Scaling)
{
	if(!m_targets.size())
		return;

	for(targets_t::iterator target = m_targets.begin(); target != m_targets.end(); ++target)
		(*target)->scale(Scaling, world_position());

	tool_selection::redraw_all();
}

void transform_tool::end_drag_motion()
{
	for(targets_t::iterator target = m_targets.begin(); target != m_targets.end(); ++target)
		(*target)->end_drag_motion();
}

void transform_tool::set_coordinate_system(const coordinate_system_t CoordinateSystem)
{
	m_coordinate_system.set_value(CoordinateSystem);
}

/// Updates all targets : tells them what's the new coordinate system
void transform_tool::update_coordinate_system(k3d::iunknown*)
{
	switch(m_coordinate_system.internal_value())
	{
		case GLOBAL:
			for(targets_t::iterator target = m_targets.begin(); target != m_targets.end(); ++target)
				(*target)->set_global();
			break;
		case LOCAL:
			for(targets_t::iterator target = m_targets.begin(); target != m_targets.end(); ++target)
				(*target)->set_local();
			break;
		case PARENT:
			for(targets_t::iterator target = m_targets.begin(); target != m_targets.end(); ++target)
				(*target)->set_parent();
			break;
		default:
			assert_not_reached();
	}

	tool_selection::redraw_all();
}

} // namespace ngui

} // namespace k3d

