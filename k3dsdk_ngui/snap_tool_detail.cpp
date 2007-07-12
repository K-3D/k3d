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

#include "document_state.h"
#include "modifiers.h"
#include "icons.h"
#include "interactive.h"
#include "keyboard.h"
#include "snap_tool_detail.h"
#include "tutorial_message.h"
#include "utility.h"
#include "viewport.h"

#include <k3dsdk/classes.h>
#include <k3dsdk/color.h>
#include <k3dsdk/fstream.h>
#include <k3dsdk/geometry.h>
#include <k3dsdk/gl.h>
#include <k3d-i18n-config.h>
#include <k3dsdk/icamera.h>
#include <k3dsdk/idrawable_gl.h>
#include <k3dsdk/imesh_source.h>
#include <k3dsdk/iprojection.h>
#include <k3dsdk/isnap_source.h>
#include <k3dsdk/isnap_target.h>
#include <k3dsdk/isnappable.h>
#include <k3dsdk/itransform_sink.h>
#include <k3dsdk/property.h>
#include <k3dsdk/state_change_set.h>
#include <k3dsdk/transform.h>
#include <k3dsdk/xml.h>

namespace libk3dngui
{

//////////////////////////////////////////////////////////////////////////////
// get_snap_source

k3d::isnap_source* get_snap_source(k3d::iunknown* Snappable)
{
	k3d::isnappable* const snappable = dynamic_cast<k3d::isnappable*>(Snappable);
	if(!snappable)
		return 0;

	const k3d::isnappable::snap_sources_t sources = snappable->snap_sources();
	if(sources.empty())
		return 0;

	return sources[0];
}

const k3d::isnappable::snap_targets_t get_snap_targets(k3d::isnappable* const Target, k3d::isnap_target* const SnapTarget)
{
	if(!Target)
		return k3d::isnappable::snap_targets_t();

	if(SnapTarget)
		return k3d::isnappable::snap_targets_t(1, SnapTarget);

	return Target->snap_targets();
}

///////////////////////////////////////////////////////////////////////////////////
// snap_tool_detail::transform_target

snap_tool_detail::transform_target::transform_target(k3d::inode* Node)
{
	node = Node;

	// Sanity check
	return_if_fail(node);
}

unsigned long snap_tool_detail::transform_target::target_number()
{
	return 1;
}

void snap_tool_detail::transform_target::reset(k3d::iunknown*)
{
}

void snap_tool_detail::transform_target::start_transform()
{
	if(create_transform_modifier("Snap "))
		assert_warning(k3d::set_value(*modifier, "matrix", k3d::identity3D()));

	m_origin = k3d::point3();
//	m_world_position = k3d::point3();

	// Save orientation matrices
	switch(current_system_type)
	{
		case GLOBAL:
			m_system_orientation = k3d::inverse(k3d::node_to_world_matrix(*node));
			break;
		case LOCAL:
			m_system_orientation = k3d::identity3D();
			break;
		case PARENT:
			m_system_orientation = k3d::inverse(k3d::node_to_world_matrix(*node)) * k3d::parent_to_world_matrix(*node);
			break;
		default:
			assert_not_reached();
	}
}

const k3d::matrix4 upstream_matrix(k3d::inode& Node)
{
	if(k3d::itransform_sink* const downstream_sink = dynamic_cast<k3d::itransform_sink*>(&Node))
	{
		if(k3d::iproperty* const upstream_output = Node.document().dag().dependency(downstream_sink->transform_sink_input()))
			return boost::any_cast<k3d::matrix4>(upstream_output->property_value());
	}

	return k3d::identity3D();
}

const k3d::matrix4 snap_tool_detail::transform_target::snap(k3d::isnappable* const Target, k3d::isnap_target* const SnapTarget, const double SnapDistance, const bool SnapOrientation, const bool MatchGroups, const k3d::matrix4& Transformation)
{
	k3d::isnap_source* const snap_source = get_snap_source(node);
	if(!snap_source)
		return Transformation;

	const k3d::isnappable::snap_targets_t snap_targets = get_snap_targets(Target, SnapTarget);
	if(snap_targets.empty())
		return Transformation;

	const k3d::matrix4 target_matrix = k3d::node_to_world_matrix(*Target);

	// Convert the source position to target coordinates
	const k3d::point3 source_position = k3d::inverse(target_matrix) * Transformation * snap_source->source_position();

	// Find the best (closest) snap target and its position
	k3d::isnap_target* snap_target = 0;
	k3d::point3 target_position;
	double target_distance = std::numeric_limits<double>::max();

	for(k3d::isnappable::snap_targets_t::const_iterator i = snap_targets.begin(); i != snap_targets.end(); ++i)
	{
		if(MatchGroups)
		{
			const k3d::isnap_source::groups_t source_groups = snap_source->groups();
			const k3d::isnap_target::groups_t target_groups = (*i)->groups();

			if(source_groups.size() && target_groups.end() ==
				std::find_first_of(target_groups.begin(), target_groups.end(), source_groups.begin(), source_groups.end()))
			{
				continue;
			}
		}

		// Get the target position in target coordinates
		k3d::point3 position;
		if((*i)->target_position(source_position, position))
		{
			const double distance = k3d::distance(position, source_position);
			if(distance < target_distance)
			{
				snap_target = *i;
				target_position = position;
				target_distance = distance;
			}
		}
	}

	if(!snap_target)
		return Transformation;

	if(target_distance > SnapDistance)
		return Transformation;

	// Optionally handle snap orientation
	if(SnapOrientation)
	{
		// If there's a source orientation, convert it to target coordinates
		k3d::vector3 source_look;
		k3d::vector3 source_up;
		if(snap_source->source_orientation(source_look, source_up))
		{
			source_look = k3d::inverse(target_matrix) * Transformation * source_look;
			source_up = k3d::inverse(target_matrix) * Transformation * source_up;

			// Get the target orientation in target coordinates
			k3d::vector3 target_look;
			k3d::vector3 target_up;
			if(snap_target->target_orientation(source_position, target_look, target_up))
			{
				k3d::matrix4 rotation1 = k3d::rotation3D(k3d::angle_axis(
					std::acos(k3d::normalize(source_look) * k3d::normalize(target_look)),
					source_look ^ target_look));

				source_up = rotation1 * source_up;

				k3d::matrix4 rotation2 = k3d::rotation3D(k3d::angle_axis(
					std::acos(k3d::normalize(source_up) * k3d::normalize(target_up)),
					source_up ^ target_up));

				return k3d::translation3D(target_position - source_position) * Transformation * rotation2 * rotation1;
			}
		}
	}

	// Just snap the position
	return k3d::translation3D(target_position - source_position) * Transformation;
}

void snap_tool_detail::transform_target::transform(k3d::isnappable* const Target, k3d::isnap_target* const SnapTarget, const double SnapDistance, const bool SnapOrientation, const bool MatchGroups, const k3d::matrix4& Transform)
{
	if(!modifier)
		start_transform();

	const k3d::matrix4 snap_matrix = snap(Target, SnapTarget, SnapDistance, SnapOrientation, MatchGroups, Transform);

	assert_warning(k3d::set_value(*modifier, "matrix", k3d::inverse(upstream_matrix(*modifier)) * snap_matrix));
}

bool snap_tool_detail::transform_target::create_transform_modifier(const std::string& Name)
{
	if(modifier)
		return false;

	return_val_if_fail(node, false);

	// Check for an existing transform modifier
	k3d::inode* upstream_node = upstream_transform_modifier(*node);
	/** \todo check for same name too */
	if(upstream_node && (k3d::classes::FrozenTransformation() == upstream_node->factory().class_id()))
	{
		set_transform_modifier(upstream_node);
		return false;
	}

	const std::string modifier_name = Name + node->name();
	set_transform_modifier(insert_transform_modifier(*node, k3d::classes::FrozenTransformation(), modifier_name));

	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////
// snap_tool_detail::mesh_target

snap_tool_detail::mesh_target::mesh_target(document_state& DocumentState, k3d::inode* Node, k3d::iproperty& MeshSourceProperty) :
	m_document_state(DocumentState),
	mesh_source_property(MeshSourceProperty),
	component_center(k3d::point3(0, 0, 0)),
	m_tweaks_mutex(false)
{
	node = Node;

	// Sanity checks
	return_if_fail(node);

	reset_selection();
}

unsigned long snap_tool_detail::mesh_target::target_number()
{
	return selected_points.size();
}

void snap_tool_detail::mesh_target::reset_selection()
{
k3d::log() << debug << K3D_CHANGE_SET_CONTEXT << std::endl;
	k3d::mesh* const mesh = boost::any_cast<k3d::mesh*>(mesh_source_property.property_value());
	return_if_fail(mesh);

	// Get selection and save initial position
	component_center = detail::get_selected_points(m_document_state.selection_mode().value(), *mesh, selected_points);
}

void snap_tool_detail::mesh_target::reset(k3d::iunknown*)
{
	if(m_tweaks_mutex)
		return;

	// Reset position
	reset_selection();

	modifier = 0;
}

void snap_tool_detail::mesh_target::init_transformation()
{
	assert_not_implemented();
/*
	// Save initial positions
	k3d::mesh* const mesh = boost::any_cast<k3d::mesh*>(mesh_source_property.property_value());
	return_if_fail(mesh);
	for(detail::component_points_t::iterator point = selected_points.begin(); point != selected_points.end(); ++point)
	{
		const unsigned long index = point->index;
		point->initial_position = mesh->points[index]->position;
		point->tweak_value = tweaks[index];
	}

	// Save orientation matrices
	switch(current_system_type)
	{
		case GLOBAL:
			m_system_orientation = k3d::inverse(k3d::node_to_world_matrix(*node));
			m_system_orientation_inverse = k3d::node_to_world_matrix(*node);
			// Zero translation components
			m_system_orientation[0][3] = m_system_orientation[1][3] = m_system_orientation[2][3] = 0;
			m_system_orientation_inverse[0][3] = m_system_orientation_inverse[1][3] = m_system_orientation_inverse[2][3] = 0;
			break;
		case LOCAL:
			m_system_orientation = k3d::identity3D();
			m_system_orientation_inverse = k3d::identity3D();
			break;
		case PARENT:
			m_system_orientation = k3d::inverse(k3d::node_to_world_matrix(*node)) * k3d::parent_to_world_matrix(*node);
			m_system_orientation_inverse = k3d::inverse(m_system_orientation);
			// Zero translation components
			m_system_orientation[0][3] = m_system_orientation[1][3] = m_system_orientation[2][3] = 0;
			m_system_orientation_inverse[0][3] = m_system_orientation_inverse[1][3] = m_system_orientation_inverse[2][3] = 0;
			break;
		default:
			assert_not_reached();
	}
*/
}

void snap_tool_detail::mesh_target::start_transform()
{
	assert_not_implemented();
/*
	create_mesh_modifier("Move ");

	init_transformation();

	m_origin = component_center;
*/
}

void snap_tool_detail::mesh_target::transform(k3d::isnappable* const Target, k3d::isnap_target* const SnapTarget, const double SnapDistance, const bool SnapOrientation, const bool MatchGroups, const k3d::matrix4& Transform)
{
	assert_not_implemented();
/*
	if(SnapTarget)
		k3d::log() << warning << "mesh snapping not implemented" << std::endl;

	if(!modifier)
		start_move();

	// Tweak points
	for(detail::component_points_t::const_iterator point = selected_points.begin(); point != selected_points.end(); ++point)
	{
		const k3d::point3 position = point->initial_position;
		const k3d::point3 new_position = m_system_orientation * Move + position;

		tweaks[point->index] = (new_position - position) + point->tweak_value;
	}

	// Update manipulators position
	component_center = m_system_orientation * Move + m_origin;

	update_mesh_modifier();
*/
}

void snap_tool_detail::mesh_target::create_mesh_modifier(const std::string& Name)
{
	assert_not_implemented();
/*
	//if(modifier)
	//	return;

	return_if_fail(node);

	// Get mesh to tweak
	k3d::mesh* const mesh = boost::any_cast<k3d::mesh*>(mesh_source_property.property_value());
	return_if_fail(mesh);

	// Modify with TweakPoints
	const k3d::uuid tweak_points(0xed302b87, 0x49bf4fe6, 0x99064963, 0x17ec12d9);

	// Check for an existing mesh modifier
	k3d::inode* upstream_node = upstream_mesh_modifier(*node);
	if(upstream_node && (tweak_points == upstream_node->factory().class_id()))
	{
		set_transform_modifier(upstream_node);

		// Init tweaks
		tweaks = boost::any_cast<tweaks_t>(k3d::get_value(*modifier, "tweaks"));
		tweaks.resize(mesh->points.size(), k3d::point3(0, 0, 0));

		return;
	}

	// Create a new TweakPoints modifier
	const std::string modifier_name = Name + node->name() + " components";
	set_transform_modifier(insert_mesh_modifier(*node, tweak_points, modifier_name));

	// Initialize new TweakPoints modifier
	tweaks.clear();
	tweaks.resize(mesh->points.size(), k3d::point3(0, 0, 0));
	update_mesh_modifier();

	// Connect to change_signal to be acknowledged of external changes such as document Undo/Redo
	k3d::iproperty* property = get_property(*modifier, "tweaks");
	return_if_fail(property);
	m_modifier_change_signal = property->property_changed_signal().connect(sigc::mem_fun(*this, &snap_tool_detail::mesh_target::reset));
*/
}

void snap_tool_detail::mesh_target::update_mesh_modifier()
{
	m_tweaks_mutex = true;
	assert_warning(k3d::set_value(*modifier, "tweaks", tweaks));
	m_tweaks_mutex = false;
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// snap_tool_detail

void snap_tool_detail::lbutton_down(viewport::control& Viewport, const k3d::point2& Coordinates, const k3d::key_modifiers& Modifiers)
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
	detail::abstract_tool::manipulators_t manipulators;
	for(k3d::selection::records::iterator record = picked_selectables.begin(); record != picked_selectables.end(); ++record)
	{
		for(k3d::selection::record::tokens_t::const_iterator token = record->tokens.begin(); token != record->tokens.end(); ++token)
		{
			if(token->type == k3d::selection::USER1)
				manipulators.push_back(m_abstract_tool.manipulator_name(token->id));
		}
	}

	const std::string manipulator = m_abstract_tool.get_manipulator(manipulators);
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
void snap_tool_detail::lmb_down_add()
{
	k3d::start_state_change_set(m_document, K3D_CHANGE_SET_CONTEXT);
	m_tutorial_action = "lmb_down_add";

	m_mouse_down_content = SELECTION_ADD;
}

void snap_tool_detail::lmb_down_subtract()
{
	k3d::start_state_change_set(m_document, K3D_CHANGE_SET_CONTEXT);
	m_tutorial_action = "lmb_down_subtract";

	m_mouse_down_content = SELECTION_SUBTRACT;
}

void snap_tool_detail::lmb_down_manipulator(const std::string& ManipulatorName)
{
	k3d::start_state_change_set(m_document, K3D_CHANGE_SET_CONTEXT);
	m_tutorial_action = "lmb_down_manipulator_" + ManipulatorName;

	m_abstract_tool.set_manipulator(ManipulatorName);
	set_motion(MOTION_DRAG);

	m_mouse_down_content = SELECTED_OBJECT;
}

void snap_tool_detail::lmb_down_selected()
{
	k3d::start_state_change_set(m_document, K3D_CHANGE_SET_CONTEXT);
	m_tutorial_action = "lmb_down_selected";

	m_mouse_down_content = SELECTED_OBJECT;
}

void snap_tool_detail::lmb_down_deselected()
{
	k3d::start_state_change_set(m_document, K3D_CHANGE_SET_CONTEXT);
	m_tutorial_action = "lmb_down_deselected";

	m_mouse_down_content = DESELECTED_OBJECT;

	// Deselect all
	m_document_state.deselect_all();
	// Select clicked object
	m_document_state.select(m_mouse_down_selection);
}

void snap_tool_detail::lmb_down_nothing()
{
	k3d::start_state_change_set(m_document, K3D_CHANGE_SET_CONTEXT);
	m_tutorial_action = "lmb_down_nothing";

	m_mouse_down_content = NOTHING;
}

void snap_tool_detail::lbutton_click(const viewport::control& Viewport, const k3d::point2& Coordinates)
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
			lmb_click_add();
		break;
		case SELECTION_SUBTRACT:
			lmb_click_subtract();
		break;
		case SELECTED_OBJECT:
			lmb_click_start_motion(Coordinates);
		break;
		case DESELECTED_OBJECT:
			lmb_click_replace();
		break;
		case NOTHING:
			lmb_click_deselect_all();
		break;
		default:
			assert_not_reached();
	}
}

// LMB click actions
void snap_tool_detail::lmb_click_add()
{
	m_tutorial_action = "lmb_click_add";

	// Shift key modifier always adds to the selection
	if(k3d::selection::get_node(m_mouse_down_selection))
		m_document_state.select(m_mouse_down_selection);

	k3d::finish_state_change_set(m_document, "Selection add", K3D_CHANGE_SET_CONTEXT);

	redraw_all();
}

void snap_tool_detail::lmb_click_subtract()
{
	m_tutorial_action = "lmb_click_subtract";

	// Control key modifier always adds to the selection
	if(k3d::selection::get_node(m_mouse_down_selection))
		m_document_state.deselect(m_mouse_down_selection);

	k3d::finish_state_change_set(m_document, "Selection subtract", K3D_CHANGE_SET_CONTEXT);

	redraw_all();
}

void snap_tool_detail::lmb_click_replace()
{
	m_tutorial_action = "lmb_click_replace";

	// Replace selection
	m_document_state.deselect_all();
	if(k3d::selection::get_node(m_mouse_down_selection))
		m_document_state.select(m_mouse_down_selection);

	k3d::finish_state_change_set(m_document, "Selection replace", K3D_CHANGE_SET_CONTEXT);

	redraw_all();
}

void snap_tool_detail::lmb_click_start_motion(const k3d::point2& Coordinates)
{
	m_tutorial_action = "lmb_click_start_motion";

	disconnect_navigation_input_model();

	set_motion(MOTION_CLICK_DRAG);
	m_abstract_tool.begin_mouse_move(Coordinates);

	redraw_all();
}

void snap_tool_detail::lmb_click_stop_motion()
{
	m_tutorial_action = "lmb_click_stop_motion";

	const std::string label = complete_mouse_move();
	k3d::finish_state_change_set(m_document, label, K3D_CHANGE_SET_CONTEXT);

	connect_navigation_input_model();

	redraw_all();
}

void snap_tool_detail::lmb_click_deselect_all()
{
	m_tutorial_action = "lmb_click_deselect_all";

	// Deselect all
	m_document_state.deselect_all();

	k3d::finish_state_change_set(m_document, "Deselect all", K3D_CHANGE_SET_CONTEXT);

	redraw_all();
}

void snap_tool_detail::lbutton_start_drag(viewport::control& Viewport, const k3d::point2& Coordinates)
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
void snap_tool_detail::lmb_start_drag_start_motion(const k3d::point2& Coordinates)
{
	m_tutorial_action = "lmb_start_drag_start_motion";

	m_off_screen_offset = k3d::point2(0, 0);

	set_motion(MOTION_DRAG);
	m_abstract_tool.begin_mouse_move(Coordinates);
}

void snap_tool_detail::lmb_start_drag_box_select(viewport::control& Viewport, const k3d::point2& Coordinates)
{
	m_tutorial_action = "lmb_start_drag_box_select";

	set_motion(MOTION_BOX_SELECT);
	m_box_selection = k3d::rectangle(Coordinates[0], Coordinates[0], Coordinates[1], Coordinates[1]);
	draw_rubber_band(Viewport);
}

void snap_tool_detail::lmb_drag_box_select(viewport::control& Viewport, const k3d::point2& Coordinates)
{
	m_tutorial_action = "lmb_drag_box_select";

	on_box_select_motion(Viewport, Coordinates);
}

void snap_tool_detail::lbutton_end_drag(viewport::control& Viewport, const k3d::point2& Coordinates)
{
	if(MOTION_DRAG == m_current_motion)
		lmb_end_drag_stop_motion();
	else if(MOTION_BOX_SELECT == m_current_motion)
		lmb_end_drag_box_select(Viewport, Coordinates);

	connect_navigation_input_model();
}

// LMB end drag actions
void snap_tool_detail::lmb_end_drag_stop_motion()
{
	m_tutorial_action = "lmb_end_drag_stop_motion";

	const std::string label = complete_mouse_move();
	k3d::finish_state_change_set(m_document, label, K3D_CHANGE_SET_CONTEXT);

	redraw_all();
}

void snap_tool_detail::lmb_end_drag_box_select(viewport::control& Viewport, const k3d::point2& Coordinates)
{
	m_tutorial_action = "lmb_end_drag_box_select";

	draw_rubber_band(Viewport);

	on_box_select_objects(Viewport, Coordinates, m_box_selection);

	// Stop motion
	set_motion(MOTION_NONE);

	k3d::finish_state_change_set(m_document, "Box selection", K3D_CHANGE_SET_CONTEXT);

	redraw_all();
}

void snap_tool_detail::mbutton_click(viewport::control& Viewport, const k3d::point2& Coordinates, const k3d::key_modifiers& Modifiers)
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

void snap_tool_detail::mmb_click_toggle_manipulators_visibility()
{
	m_tutorial_action = "mmb_click_toggle_manipulators_visibility";

	// Toggle the visible state of the manipulators
	m_visible_manipulators.set_value(!m_visible_manipulators.value());

	redraw_all();
}

void snap_tool_detail::mmb_click_manipulators_next_selection()
{
	m_tutorial_action = "mmb_click_manipulators_next_selection";

	// Show manipulators on the next selection
	m_current_target = m_targets.size() ? (m_current_target + 1) % m_targets.size() : 0;

	redraw_all();
}

void snap_tool_detail::mmb_click_switch_coordinate_system()
{
	m_tutorial_action = "mmb_click_switch_coordinate_system";

	// Switch coordinate system between global and local modes
	switch(m_coordinate_system.value())
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

	redraw_all();
}

void snap_tool_detail::mmb_click_next_constraint(viewport::control& Viewport, const k3d::point2& Coordinates)
{
	m_tutorial_action = "mmb_click_next_constraint";

	// Set next constraint
	m_abstract_tool.update_constraint(Viewport, Coordinates);

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

	redraw_all();
}

void snap_tool_detail::rbutton_click(const viewport::control& Viewport, const k3d::point2& Coordinates)
{
	if(MOTION_NONE == m_current_motion)
		rmb_click_selection_tool();
	else
		rmb_click_cancel_move();
}

// RMB click actions
void snap_tool_detail::rmb_click_selection_tool()
{
	k3d::start_state_change_set(m_document, K3D_CHANGE_SET_CONTEXT);
	m_tutorial_action = "rmb_click_selection_tool";

	m_document_state.set_active_tool(m_document_state.selection_tool());

	k3d::finish_state_change_set(m_document, "Selection tool", K3D_CHANGE_SET_CONTEXT);

	redraw_all();
}

void snap_tool_detail::rmb_click_cancel_move()
{
	m_tutorial_action = "rmb_click_cancel_move";

	cancel_mouse_move();

	redraw_all();
}

void snap_tool_detail::cancel_mouse_move()
{
	// Stop motion
	set_motion(MOTION_NONE);

	// Undo changes
	k3d::cancel_state_change_set(m_document, K3D_CHANGE_SET_CONTEXT);

	// Reset targets
	for(targets_t::iterator target = m_targets.begin(); target != m_targets.end(); ++target)
		(*target)->reset(0);

	m_abstract_tool.reset();

	redraw_all();
}

std::string snap_tool_detail::complete_mouse_move()
{
	set_motion(MOTION_NONE);

	return m_abstract_tool.get_constraint_name();
}

void snap_tool_detail::set_motion(const motion_t Motion)
{
	m_current_motion = Motion;
}

void snap_tool_detail::update_manipulators_scale(viewport::control& Viewport, const k3d::point3& Origin)
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

bool snap_tool_detail::front_facing(viewport::control& Viewport, const k3d::vector3& Normal, const k3d::point3& Origin)
{
	return_val_if_fail(Viewport.gl_engine(), false);
	return_val_if_fail(Viewport.camera(), false);

	const k3d::matrix4 matrix = k3d::inverse(k3d::node_to_world_matrix(*Viewport.camera()));
	const k3d::matrix4 orientation = k3d::identity3D();
	const k3d::point3 a = Origin + (orientation * Normal);
	const k3d::point3 b = Origin + (orientation * -Normal);
	return k3d::to_vector(matrix * a).length2() < k3d::to_vector(matrix * b).length2();
}

void snap_tool_detail::off_screen_warp(viewport::control& Viewport, k3d::point2& NewCoordinates)
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
		NewCoordinates += m_off_screen_offset;
	}
	else
	{
		// Warp mouse pointer
		interactive::warp_pointer(mouse);

		// Set new position
		NewCoordinates += previous_offset;
	}
}

void snap_tool_detail::clear_targets()
{
	for(targets_t::iterator target = m_targets.begin(); target != m_targets.end(); ++target)
	{
		delete *target;
	}

	m_targets.clear();
}

/// Retrieves the current document selection, refreshing the target list
void snap_tool_detail::get_current_selection()
{
	// Convert the current document selection into the set of targets to be moved interactively
	clear_targets();

	const k3d::nodes_t nodes = m_document_state.selected_nodes();

	if(SELECT_NODES == m_document_state.selection_mode().value())
	{
		// Save transformable nodes as targets
		for(k3d::nodes_t::const_iterator node = nodes.begin(); node != nodes.end(); ++node)
		{
			if(!dynamic_cast<k3d::gl::idrawable*>(*node))
				continue;
			if(!dynamic_cast<k3d::itransform_sink*>(*node))
				continue;

			m_targets.push_back(new transform_target(*node));
			(*node)->deleted_signal().connect(sigc::mem_fun(*this, &snap_tool_detail::target_deleted));
		}
	}
	else
	{
		// Component mode : save mesh nodes as targets
		for(k3d::nodes_t::const_iterator node = nodes.begin(); node != nodes.end(); ++node)
		{
			if(!dynamic_cast<k3d::gl::idrawable*>(*node))
				continue;

			// Get node's mesh
			k3d::imesh_source* const mesh_source = dynamic_cast<k3d::imesh_source*>(*node);
			if(!mesh_source)
				continue;

			// Get non-transformed mesh output
			k3d::iproperty& property = mesh_source->mesh_source_output();
			m_targets.push_back(new mesh_target(m_document_state, *node, property));

			(*node)->deleted_signal().connect(sigc::mem_fun(*this, &snap_tool_detail::target_deleted));
		}
	}

	update_coordinate_system(0);
}

unsigned long snap_tool_detail::target_number()
{
	unsigned long target_count = 0;
	for(targets_t::iterator target = m_targets.begin(); target != m_targets.end(); ++target)
		target_count += (*target)->target_number();

	return target_count;
}

void snap_tool_detail::update_targets()
{
	// Update target list when one of them was deleted
	if(m_deleted_target)
	{
		get_current_selection();
		m_deleted_target = false;
	}
}

void snap_tool_detail::start_transform()
{
	for(targets_t::iterator target = m_targets.begin(); target != m_targets.end(); ++target)
		(*target)->start_transform();
}

void snap_tool_detail::transform_targets(k3d::isnappable* const Target, k3d::isnap_target* const SnapTarget, const double SnapDistance, const bool SnapOrientation, const bool MatchGroups, const k3d::matrix4& Transform)
{
	for(targets_t::iterator target = m_targets.begin(); target != m_targets.end(); ++target)
		(*target)->transform(Target, SnapTarget, SnapDistance, SnapOrientation, MatchGroups, Transform);

	redraw_all();
}

void snap_tool_detail::set_coordinate_system(const coordinate_system_t CoordinateSystem)
{
	m_coordinate_system.set_value(CoordinateSystem);
}

/// Updates all targets : tells them what's the new coordinate system
void snap_tool_detail::update_coordinate_system(k3d::iunknown*)
{
	switch(m_coordinate_system.value())
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

	redraw_all();
}

} // namespace libk3dngui

